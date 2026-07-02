#include <game.hpp>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <fstream>
#include <cstdint>

// ----------------------------------------------------------------------
// A parametric heart curve
// ----------------------------------------------------------------------
static void drawHeart(sf::RenderWindow& win, float cx, float cy, float size, sf::Color color) {
    const int points = 40;
    sf::ConvexShape heart;
    heart.setPointCount(points);
    float scale = size / 32.f;

    for (int i = 0; i < points; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(points) * 2.f * 3.14159265f;
        float hx = 16.f * std::pow(std::sin(t), 3.f);
        float hy = 13.f * std::cos(t) - 5.f * std::cos(2.f * t)
                   - 2.f * std::cos(3.f * t) - std::cos(4.f * t);
        heart.setPoint(i, {cx + hx * scale, cy - hy * scale});
    }
    heart.setFillColor(color);
    win.draw(heart);
}

Game::Game() : window(sf::VideoMode({window_width, window_height}), "breakout",
                      sf::Style::Titlebar | sf::Style::Close),
               img({window_width, window_height}, sf::Color(15, 10, 35)),
               texture(img),
               background(texture),
               platform({platform_x_initial, platform_y_initial}, {platform_width, platform_height}),
               ball({ball_x_initial, ball_y_initial}, ball_radius) {
    window.setFramerateLimit(75u);
    window.setMouseCursorVisible(false);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        static_cast<void>(font.openFromFile("C:/Windows/Fonts/times.ttf"));
    }

    std::ifstream score_file("bestscore.txt");
    if (score_file.is_open()) {
        score_file >> best_score;
        score_file.close();
    }
}

// ----------------------------------------------------------------------
// Rendering
// ----------------------------------------------------------------------
void Game::render() {
    window.clear(sf::Color(15, 10, 35));
    window.draw(background);

    if (state == State::MENU) {
        renderMenu();
        window.display();
        return;
    }

    if (state == State::DEAD) {
        for (const Brick& brick : bricks) {
            if (!brick.destroyed) window.draw(brick.getGlow());
        }
        window.draw(platform.getGlow());
        for (const Brick& brick : bricks) {
            if (!brick.destroyed) window.draw(brick.getShape());
        }
        window.draw(platform.getShape());
        window.draw(ball.getShape());
        particles.draw(window);

        renderDeathScreen();
        window.display();
        return;
    }

    if (state == State::WIN) {
        for (const Brick& brick : bricks) {
            if (!brick.destroyed) window.draw(brick.getGlow());
        }
        window.draw(platform.getGlow());
        window.draw(platform.getShape());
        window.draw(ball.getShape());
        particles.draw(window);

        renderWinScreen();
        window.display();
        return;
    }

    // --- PLAYING state ---
    for (const Brick& brick : bricks) {
        if (!brick.destroyed) window.draw(brick.getGlow());
    }
    window.draw(platform.getGlow());

    window.draw(platform.getShape());
    window.draw(ball.getShape());
    for (const Brick& brick : bricks) {
        if (!brick.destroyed) window.draw(brick.getShape());
    }

    particles.draw(window);
    renderHUD();

    window.display();
}

void Game::renderHearts() {
    float hx = 20.f;
    float hy = 18.f;
    float hsize = 26.f;
    for (unsigned i = 0; i < max_lives; ++i) {
        if (i < lives) {
            drawHeart(window, hx + i * (hsize + 6.f), hy, hsize, sf::Color(220, 60, 80));
        } else {
            drawHeart(window, hx + i * (hsize + 6.f), hy, hsize, sf::Color(60, 40, 50));
        }
    }
}

void Game::renderHUD() {
    renderHearts();

    sf::Text score_text(font);
    score_text.setString("Score: " + std::to_string(score));
    score_text.setCharacterSize(22);
    score_text.setFillColor(sf::Color(200, 200, 220));
    score_text.setPosition({static_cast<float>(window_width) - 200.f, 12.f});
    window.draw(score_text);

    sf::Text best_text(font);
    best_text.setString("Best: " + std::to_string(best_score));
    best_text.setCharacterSize(18);
    best_text.setFillColor(sf::Color(140, 140, 170));
    best_text.setPosition({static_cast<float>(window_width) / 2.f - 40.f, 14.f});
    window.draw(best_text);
}

void Game::renderMenu() {
    sf::Text title(font);
    title.setString("BREAKOUT");
    title.setCharacterSize(90);
    title.setFillColor(sf::Color(255, 200, 100));
    title.setStyle(sf::Text::Bold);
    auto tb = title.getLocalBounds();
    title.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    title.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f - 120.f});
    window.draw(title);

    sf::Text sub(font);
    sub.setString("Press SPACE to Start");
    sub.setCharacterSize(32);
    sub.setFillColor(sf::Color(180, 180, 220));
    auto sb = sub.getLocalBounds();
    sub.setOrigin({sb.position.x + sb.size.x / 2.f, sb.position.y + sb.size.y / 2.f});
    sub.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 20.f});
    window.draw(sub);

    sf::Text ctrl(font);
    ctrl.setString("Left/Right to move    Space to launch & charge");
    ctrl.setCharacterSize(18);
    ctrl.setFillColor(sf::Color(120, 120, 150));
    auto cb = ctrl.getLocalBounds();
    ctrl.setOrigin({cb.position.x + cb.size.x / 2.f, cb.position.y + cb.size.y / 2.f});
    ctrl.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 100.f});
    window.draw(ctrl);

    if (best_score > 0) {
        sf::Text bs(font);
        bs.setString("Best Score: " + std::to_string(best_score));
        bs.setCharacterSize(24);
        bs.setFillColor(sf::Color(255, 180, 80));
        auto bb = bs.getLocalBounds();
        bs.setOrigin({bb.position.x + bb.size.x / 2.f, bb.position.y + bb.size.y / 2.f});
        bs.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 160.f});
        window.draw(bs);
    }
}

// ----------------------------------------------------------------------
// "YOU DIED" — Dark Souls style animated death screen
// ----------------------------------------------------------------------
void Game::renderDeathScreen() {
    constexpr float anim_duration = 2.5f;
    float progress = std::min(death_timer / anim_duration, 1.f);

    std::uint8_t overlay_alpha = static_cast<std::uint8_t>(255.f * progress);
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window_width), static_cast<float>(window_height)));
    overlay.setFillColor(sf::Color(0, 0, 0, overlay_alpha));
    window.draw(overlay);

    float min_size = 20.f;
    float max_size = 120.f;
    float text_size = min_size + (max_size - min_size) * progress;

    sf::Text died(font);
    died.setString("YOU DIED");
    died.setCharacterSize(static_cast<unsigned>(text_size));
    died.setFillColor(sf::Color(140, 20, 20, static_cast<std::uint8_t>(255.f * progress)));
    died.setStyle(sf::Text::Bold);
    auto db = died.getLocalBounds();
    died.setOrigin({db.position.x + db.size.x / 2.f, db.position.y + db.size.y / 2.f});
    died.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f - 40.f});
    window.draw(died);

    if (progress >= 1.f) {
        sf::Text dscore(font);
        dscore.setString("Score: " + std::to_string(score) + "    Best: " + std::to_string(best_score));
        dscore.setCharacterSize(28);
        dscore.setFillColor(sf::Color(160, 160, 180));
        auto dsb = dscore.getLocalBounds();
        dscore.setOrigin({dsb.position.x + dsb.size.x / 2.f, dsb.position.y + dsb.size.y / 2.f});
        dscore.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 80.f});
        window.draw(dscore);

        sf::Text prompt(font);
        prompt.setString("Press SPACE to return to Menu");
        prompt.setCharacterSize(22);
        prompt.setFillColor(sf::Color(120, 120, 140));
        auto pb = prompt.getLocalBounds();
        prompt.setOrigin({pb.position.x + pb.size.x / 2.f, pb.position.y + pb.size.y / 2.f});
        prompt.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 140.f});
        window.draw(prompt);
    }
}

void Game::renderWinScreen() {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window_width), static_cast<float>(window_height)));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    sf::Text won(font);
    won.setString("VICTORY!");
    won.setCharacterSize(110);
    won.setFillColor(sf::Color(255, 200, 60));
    won.setStyle(sf::Text::Bold);
    auto wb = won.getLocalBounds();
    won.setOrigin({wb.position.x + wb.size.x / 2.f, wb.position.y + wb.size.y / 2.f});
    won.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f - 40.f});
    window.draw(won);

    sf::Text ws(font);
    ws.setString("Score: " + std::to_string(score) + "    Best: " + std::to_string(best_score));
    ws.setCharacterSize(28);
    ws.setFillColor(sf::Color(180, 180, 200));
    auto wsb = ws.getLocalBounds();
    ws.setOrigin({wsb.position.x + wsb.size.x / 2.f, wsb.position.y + wsb.size.y / 2.f});
    ws.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 80.f});
    window.draw(ws);

    sf::Text prompt(font);
    prompt.setString("Press SPACE to return to Menu");
    prompt.setCharacterSize(22);
    prompt.setFillColor(sf::Color(120, 120, 140));
    auto pb = prompt.getLocalBounds();
    prompt.setOrigin({pb.position.x + pb.size.x / 2.f, pb.position.y + pb.size.y / 2.f});
    prompt.setPosition({static_cast<float>(window_width) / 2.f, static_cast<float>(window_height) / 2.f + 140.f});
    window.draw(prompt);
}

// ----------------------------------------------------------------------
// Ball movement with FULL SWEPT CCD
// ----------------------------------------------------------------------
void Game::moveBall(float /*dt*/) {
    if (!ball_launched) return;

    ball.velocity.y += fall_accel;

    // Clamp speed
    float speed_sq = ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y;
    if (speed_sq > max_ball_speed * max_ball_speed) {
        float speed = std::sqrt(speed_sq);
        ball.velocity *= max_ball_speed / speed;
    }

    // Calculate adaptive substeps
    float speed = std::sqrt(ball.velocity.x * ball.velocity.x +
                            ball.velocity.y * ball.velocity.y);
    unsigned steps = static_cast<unsigned>(speed / ccd_step_size) + 1u;
    if (steps > ccd_max_steps) steps = ccd_max_steps;
    if (steps < 1u) steps = 1u;

    // Burning effect — when ball is fast, emit fire trail particles
    if (speed > burn_speed_threshold) {
        float burn_intensity = (speed - burn_speed_threshold) / (max_ball_speed - burn_speed_threshold);
        unsigned burn_count = static_cast<unsigned>(5.f + burn_intensity * 10.f);
        sf::Color fire_color(255, static_cast<std::uint8_t>(150 + 105 * (1.f - burn_intensity)), 50);
        particles.emit(ball.getPosition(), {-ball.velocity.x * 0.1f, -ball.velocity.y * 0.1f},
                       burn_count, fire_color, 0,
                       20, 80, 2.f, 5.f, 0.3f, 0.6f, -50.f, 0.93f);
    }

    // Swept CCD
    for (unsigned i = 0; i < steps; ++i) {
        sf::Vector2f step = ball.velocity / static_cast<float>(steps);
        ball.displace(step);

        handleWallCollision();
        handleBrickCollision();
        handlePlatformCollision();

        if (!ball_launched) break;
        if (state != State::PLAYING) break;
    }
}

// ----------------------------------------------------------------------
// Spawn bricks
// ----------------------------------------------------------------------
void Game::spawnBricks() {
    bricks.clear();
    float total_width = num_bricks_x * brick_width + (num_bricks_x - 1) * brick_gap;
    float offset_x = (static_cast<float>(window_width) - total_width) / 2.f;
    float offset_y = 60.f;

    for (unsigned y = 0; y < num_bricks_y; ++y) {
        for (unsigned x = 0; x < num_bricks_x; ++x) {
            if (std::rand() % 100 < 30) continue;

            bricks.emplace_back(Brick(
                {offset_x + static_cast<float>(x) * (brick_width + brick_gap),
                 offset_y + static_cast<float>(y) * (brick_height + brick_gap)},
                {brick_width, brick_height},
                static_cast<size_t>(std::rand() % 3 + 1)));
        }
    }
}

void Game::resetBall() {
    ball_launched = false;
    ball.velocity = {0.f, 0.f};
    static_ball_x_displacement = platform_width / 2.f;
    sf::Vector2f plat_pos = platform.getPosition();
    ball.setPosition({plat_pos.x + static_ball_x_displacement, ball_y_initial});
}

void Game::startNewGame() {
    lives = max_lives;
    score = 0;
    state = State::PLAYING;
    platform.setPosition({platform_x_initial, platform_y_initial});
    spawnBricks();
    resetBall();
}

bool Game::checkWin() const {
    for (const Brick& b : bricks) {
        if (!b.destroyed) return false;
    }
    return true;
}

// ----------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------
void Game::run() {
    clock.restart();

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    if (state == State::MENU) {
                        startNewGame();
                    } else if (state == State::DEAD) {
                        if (death_timer >= 2.5f) state = State::MENU;
                    } else if (state == State::WIN) {
                        state = State::MENU;
                    } else if (state == State::PLAYING) {
                        if (!ball_launched) {
                            ball_launched = true;
                            ball.velocity.y -= initial_launch_speed;
                        } else {
                            if (platform.charged) {
                                platform.discharge();
                            } else {
                                platform.charge();
                            }
                        }
                    }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    if (state == State::PLAYING) {
                        state = State::MENU;
                    } else if (state == State::MENU) {
                        window.close();
                    }
                }
            }
        }

        if (state == State::PLAYING) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
                platform.target_speed = -platform.max_speed;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
                platform.target_speed = platform.max_speed;
            } else {
                platform.target_speed = 0.f;
            }
            platform.update();

            sf::Vector2f ppos = platform.getPosition();
            if (ppos.x < 0) {
                platform.setPosition({0.f, ppos.y});
                if (platform.speed < 0) platform.speed = 0.f;
            }
            if (ppos.x > static_cast<float>(window_width) - platform_width) {
                platform.setPosition({static_cast<float>(window_width) - platform_width, ppos.y});
                if (platform.speed > 0) platform.speed = 0.f;
            }

            if (!ball_launched) {
                sf::Vector2f plat_pos = platform.getPosition();
                ball.setPosition({plat_pos.x + static_ball_x_displacement, ball_y_initial});
            }

            moveBall(dt);

            if (checkWin()) {
                if (score > best_score) {
                    best_score = score;
                    std::ofstream of("bestscore.txt");
                    if (of.is_open()) {
                        of << best_score;
                        of.close();
                    }
                }
                state = State::WIN;
            }
        } else if (state == State::DEAD) {
            death_timer += dt;
        }

        updateParticles(dt);
        render();
    }
}

// ----------------------------------------------------------------------
// Wall collisions
// ----------------------------------------------------------------------
void Game::handleWallCollision() {
    const sf::Vector2f ball_pos = ball.getPosition();

    if (ball_pos.y - ball_radius <= 0) {
        ball.displace({0.f, ball_radius - ball_pos.y});
        ball.velocity.y *= bounce_coeff;
        particles.emit(ball_pos, {0.f, 0.f}, 15, sf::Color(180, 200, 255), 0,
                       60, 250, 2.f, 4.f, 0.3f, 0.6f, 200.f, 0.95f);
    }
    if (ball_pos.x - ball_radius <= 0) {
        ball.displace({ball_radius - ball_pos.x, 0.f});
        ball.velocity.x *= bounce_coeff;
        particles.emit(ball_pos, {0.f, 0.f}, 15, sf::Color(180, 200, 255), 0,
                       60, 250, 2.f, 4.f, 0.3f, 0.6f, 200.f, 0.95f);
    }
    if (ball_pos.x + ball_radius >= static_cast<float>(window_width)) {
        ball.displace({static_cast<float>(window_width) - ball_pos.x - ball_radius, 0.f});
        ball.velocity.x *= bounce_coeff;
        particles.emit(ball_pos, {0.f, 0.f}, 15, sf::Color(180, 200, 255), 0,
                       60, 250, 2.f, 4.f, 0.3f, 0.6f, 200.f, 0.95f);
    }

    // Bottom — ball lost
    if (ball_pos.y - ball_radius > static_cast<float>(window_height)) {
        particles.emit(ball_pos, {0.f, -200.f}, 50, sf::Color(255, 100, 100), 0,
                       150, 500, 2.f, 7.f, 0.5f, 1.2f, 200.f, 0.96f);
        particles.emit(ball_pos, {0.f, -200.f}, 30, sf::Color(255, 200, 100), 0,
                       100, 400, 1.5f, 5.f, 0.4f, 1.0f, 200.f, 0.96f);

        --lives;
        if (lives == 0) {
            if (score > best_score) {
                best_score = score;
                std::ofstream of("bestscore.txt");
                if (of.is_open()) {
                    of << best_score;
                    of.close();
                }
            }
            state = State::DEAD;
            death_timer = 0.f;
        } else {
            resetBall();
        }
    }
}

// ----------------------------------------------------------------------
// Platform collision
// ----------------------------------------------------------------------
void Game::handlePlatformCollision() {
    if (!ball_launched) return;

    const sf::Vector2f ball_pos = ball.getPosition();
    const sf::Vector2f platform_pos = platform.getPosition();

    float closest_x = std::clamp(ball_pos.x, platform_pos.x, platform_pos.x + platform_width);
    float closest_y = std::clamp(ball_pos.y, platform_pos.y, platform_pos.y + platform_height);
    float dx = ball_pos.x - closest_x;
    float dy = ball_pos.y - closest_y;
    float dist_sq = dx * dx + dy * dy;

    if (dist_sq > ball_radius * ball_radius) return;

    if (ball.velocity.y <= 0.3f && ball.velocity.y > 0.f) {
        ball_launched = false;
        static_ball_x_displacement = ball_pos.x - platform_pos.x;
        return;
    }

    ball.displace({0.f, platform_pos.y - ball_radius - ball_pos.y});

    float hit_offset = (ball_pos.x - platform_pos.x) / platform_width;
    float angle_factor = (hit_offset - 0.5f) * 2.f;

    float speed = std::sqrt(ball.velocity.x * ball.velocity.x +
                            ball.velocity.y * ball.velocity.y);
    speed = std::max(speed, 6.f);

    ball.velocity.x = angle_factor * speed * 0.7f + platform.speed * 0.3f;
    ball.velocity.y = -std::abs(ball.velocity.y) * 0.9f;
    if (ball.velocity.y > -3.f) ball.velocity.y = -3.f;

    if (platform.charged) {
        ball.velocity.y -= platform_charge_boost;
        particles.emit(ball_pos, {0.f, 0.f}, 40, sf::Color(255, 220, 80), 0,
                       100, 450, 2.f, 6.f, 0.4f, 1.0f, 150.f, 0.96f);
        particles.emit(ball_pos, {0.f, 0.f}, 20, sf::Color(255, 255, 200), 0,
                       150, 500, 1.5f, 4.f, 0.3f, 0.6f, 100.f, 0.95f);
        platform.discharge();
    } else {
        particles.emitDirected(ball_pos, {0.f, -1.f}, 18, sf::Color(100, 180, 255),
                               60, 250, 1.0f,
                               1.5f, 4.f, 0.3f, 0.6f, 150.f, 0.95f);
    }
}

// ----------------------------------------------------------------------
// Brick collision
// ----------------------------------------------------------------------
void Game::handleBrickCollision() {
    const sf::Vector2f ball_pos = ball.getPosition();

    float speed = std::sqrt(ball.velocity.x * ball.velocity.x +
                            ball.velocity.y * ball.velocity.y);
    bool burning = speed > burn_speed_threshold;

    for (Brick& brick : bricks) {
        if (brick.destroyed) continue;

        const sf::Vector2f brick_pos = brick.getPosition();

        float closest_x = std::clamp(ball_pos.x, brick_pos.x, brick_pos.x + brick_width);
        float closest_y = std::clamp(ball_pos.y, brick_pos.y, brick_pos.y + brick_height);
        float dx = ball_pos.x - closest_x;
        float dy = ball_pos.y - closest_y;
        float dist_sq = dx * dx + dy * dy;

        if (dist_sq <= ball_radius * ball_radius) {
            float dist = std::sqrt(dist_sq);
            sf::Vector2f contact_pos = ball_pos;

            if (dist > 0.001f) {
                float nx = dx / dist;
                float ny = dy / dist;
                contact_pos = ball_pos - sf::Vector2f{nx * ball_radius, ny * ball_radius};

                if (!burning) {
                    float penetration = ball_radius - dist;
                    ball.displace({nx * penetration, ny * penetration});

                    float dot = ball.velocity.x * nx + ball.velocity.y * ny;
                    if (dot < 0.f) {
                        ball.velocity.x -= 2.f * dot * nx;
                        ball.velocity.y -= 2.f * dot * ny;
                        ball.velocity *= -bounce_coeff;
                    }
                }
            } else {
                if (!burning) {
                    ball.velocity.y *= bounce_coeff;
                    ball.displace({0.f, -ball_radius});
                }
            }

            brick.takeDamage();
            score += brick.destroyed ? 100 : 25;

            sf::Color pcolor = brick.getColor();
            if (brick.destroyed) {
                particles.emit(contact_pos, {0.f, 0.f}, 40, pcolor, 0,
                               120, 400, 2.f, 6.f, 0.4f, 0.9f, 200.f, 0.96f);
                particles.emit(contact_pos, {0.f, 0.f}, 15,
                               sf::Color(255, 255, 255), 0,
                               50, 200, 1.5f, 3.f, 0.2f, 0.4f, 0.f, 0.92f);
                if (burning) {
                    particles.emit(contact_pos, {ball.velocity.x * 0.2f, ball.velocity.y * 0.2f},
                                   20, sf::Color(255, 150, 50), 0,
                                   100, 350, 2.f, 5.f, 0.3f, 0.7f, 100.f, 0.95f);
                }
            } else {
                particles.emitDirected(contact_pos, {dx, dy}, 18, pcolor,
                                       80, 300, 1.2f,
                                       1.5f, 4.f, 0.3f, 0.6f, 150.f, 0.95f);
            }

            if (!burning) break;
        }
    }
}

void Game::updateParticles(float dt) {
    particles.update(dt);
}