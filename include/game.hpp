#ifndef BREAKOUT_GAME
#define BREAKOUT_GAME

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <platform.hpp>
#include <ball.hpp>
#include <brick.hpp>

class Game {
    // geometry constants
    static constexpr unsigned window_width = 1280u;
    static constexpr unsigned window_height = 800u;

    static constexpr float platform_width = 200.f;
    static constexpr float platform_height = 15.f;

    static constexpr unsigned num_bricks_x = 15u;
    static constexpr unsigned num_bricks_y = 5u;
    static constexpr float brick_width = 70.f;
    static constexpr float brick_height = 30.f;

    static constexpr float platform_x_initial = (static_cast<float>(window_width) - platform_width) / 2;
    static constexpr float platform_y_initial = static_cast<float>(window_height - 40u);

    static constexpr float ball_radius = 10.f;
    static constexpr float ball_x_initial = static_cast<float>(window_width / 2);
    static constexpr float ball_y_initial = platform_y_initial - ball_radius;

    static constexpr float spacing = static_cast<float>(window_width - (num_bricks_x * brick_width)) / (num_bricks_x + 1);
    
    // physics constants
    static constexpr float bounce_coeff = -0.9f;
    static constexpr float fall_accel = 0.05f;
    static constexpr float platform_speed = 50.f;
    static constexpr float platform_charge_boost = 7.f;

    // still ball's position relative to the leftmost platform's corner
    float static_ball_x_displacement = platform_width / 2;

    // flags
    bool ball_launched = false;

    sf::RenderWindow window;
    sf::Image img;
    sf::Texture texture;
    sf::Sprite background;

    Platform platform;
    Ball ball;
    std::vector<Brick> bricks;

    void render();
    void moveBall();
    void spawnBricks();
    void handleWallCollision();
    void handlePlatformCollision();
    void handleBrickCollision();

public:
    Game();
    void run();
};

#endif // BREAKOUT_GAME