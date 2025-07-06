#include <game.hpp>
#include <algorithm>
#include <ctime>

Game::Game() : window(sf::VideoMode({window_width, window_height}), "breakout",
                      sf::Style::Titlebar | sf::Style::Close),
               img({window_width, window_height}, sf::Color::Black),
               texture(img),
               background(texture),
               platform({platform_x_initial, platform_y_initial}, {platform_width, platform_height}),
               ball({ball_x_initial, ball_y_initial}, ball_radius) {
    window.setFramerateLimit(75u);
    window.setMouseCursorVisible(false);
    spawnBricks();
    std::srand(std::time({}));
}

void Game::render() {
    window.clear();
    window.draw(background);
    window.draw(platform.getShape());
    window.draw(ball.getShape());
    for (const Brick& brick : bricks) {
        window.draw(brick.getShape());
    }
    window.display();
}

void Game::moveBall() {
    if (ball_launched) {
        ball.velocity.y += fall_accel;
        ball.move();
    }
}

void Game::spawnBricks() {
    for (unsigned y = 0; y < num_bricks_y; ++y) {
        for (unsigned x = 0; x < num_bricks_x; ++x) {
            bricks.emplace_back(Brick({spacing + (brick_width+spacing)*x,
                                       spacing + (brick_height+spacing)*y},
                                       {brick_width, brick_height},
                                        std::rand() % 3 + 1));
        }
    }
}

void Game::run() {
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
                    if (platform.getPosition().x >= 0) {
                        platform.move(-platform_speed);
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    if (platform.getPosition().x <= window_width - platform_width) {
                        platform.move(platform_speed);
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    if (!ball_launched) {
                        ball_launched = true;
                        ball.velocity.y -= 12.f; // the initial speed is higher than the boost
                    } else {
                        platform.charged ? platform.discharge() : platform.charge();
                    }
                }
            }
            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                if (keyReleased->scancode == sf::Keyboard::Scancode::Left ||
                    keyReleased->scancode == sf::Keyboard::Scancode::Right)
                    platform.speed = 0.f;
            }
        }
        handlePlatformCollision();
        handleWallCollision();
        handleBrickCollision();
        moveBall();
        render();
    }
}

void Game::handleWallCollision() {
    const sf::Vector2f ball_pos = ball.getPosition();

    if (float displacement = (ball_pos.y - ball_radius); displacement <= 0) {
        ball.displace({0.f, -displacement});
        ball.velocity.y *= bounce_coeff;
    }
    if (float displacement = ball_pos.x - ball_radius; displacement <= 0) {
        ball.displace({-displacement, 0.f});
        ball.velocity.x *= bounce_coeff;
    }
    else if (float displacement = (window_width - ball_pos.x - ball_radius); displacement <= 0) {
        ball.displace({displacement, 0.f});
        ball.velocity.x *= bounce_coeff;
    }
}

void Game::handlePlatformCollision() {
    const sf::Vector2f ball_pos = ball.getPosition();
    const sf::Vector2f platform_pos = platform.getPosition();
    
    if (ball_launched) {
        // check if platform hit, push back when dug into it; reverse the velocity
        if ((ball_pos.x >= platform_pos.x - ball_radius && ball_pos.x <= platform_pos.x + platform_width) &&
            (ball_pos.y >= ball_y_initial && ball_pos.y <= ball_y_initial + platform_height)) {

            // de-launch the ball in case of slowing too greatly
            if (ball.velocity.y <= 0.3f && ball.velocity.y > 0) {
                ball_launched = false;
                static_ball_x_displacement = ball_pos.x - platform_pos.x;
            }

            ball.displace({0.f, ball_y_initial - ball_pos.y});
            
            ball.velocity.x += platform.speed * 0.05;
            ball.velocity.y *= bounce_coeff;

            // pump up the little guy if charged
            if (platform.charged) {
                ball.velocity.y -= platform_charge_boost;
                platform.discharge();
            }
        }
    }
    else {
        ball.setPosition({platform_pos.x + static_ball_x_displacement, ball_y_initial});
    }
}

void Game::handleBrickCollision() { 
    const sf::Vector2f ball_pos = ball.getPosition();

    for (Brick& brick : bricks) {
        if (brick.destroyed) continue;

        const sf::Vector2f brick_pos = brick.getPosition();

        float closest_x = std::clamp(ball_pos.x, brick_pos.x, brick_pos.x + brick_width);
        float closest_y = std::clamp(ball_pos.y, brick_pos.y, brick_pos.y + brick_height);

        float dist_x = ball_pos.x - closest_x;
        float dist_y = ball_pos.y - closest_y;
        float dist_squared = dist_x * dist_x + dist_y * dist_y;
        float radius_squared = ball_radius * ball_radius;

        if (dist_squared <= radius_squared) {
            ball.displace({-dist_x, dist_y});
            ball.velocity.y *= bounce_coeff;

            brick.takeDamage();
            break;
        }
    }
}