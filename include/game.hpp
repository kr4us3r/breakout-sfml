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
    const unsigned window_width = 1280u;
    const unsigned window_height = 800u;

    const float platform_width = 200.f;
    const float platform_height = 15.f;

    const float brick_width = 30.f;
    const float brick_height = 15.f;

    const float platform_x_initial = (static_cast<float>(window_width) - platform_width) / 2;
    const float platform_y_initial = static_cast<float>(window_height - 40u);

    const float ball_radius = 10.f;
    const float ball_x_initial = static_cast<float>(window_width / 2);
    const float ball_y_initial = platform_y_initial - ball_radius;

    // still ball's position relative to the leftmost platform's corner
    float static_ball_x_displacement = platform_width / 2;

    // physics constants
    const float bounce_coeff = -0.6f;
    const float fall_accel = 0.06f;
    const float platform_speed = 50.f;
    const float platform_charge_boost = 15.f;

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
    void detectCollision();

public:
    Game();
    void run();
};

#endif // BREAKOUT_GAME