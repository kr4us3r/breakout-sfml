#ifndef BREAKOUT_GAME
#define BREAKOUT_GAME

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <platform.hpp>
#include <ball.hpp>
#include <brick.hpp>
#include <particles.hpp>

class Game {
    // geometry constants
    static constexpr unsigned window_width = 1280u;
    static constexpr unsigned window_height = 800u;

    static constexpr float platform_width = 200.f;
    static constexpr float platform_height = 15.f;

    static constexpr unsigned num_bricks_x = 12u;
    static constexpr unsigned num_bricks_y = 6u;
    static constexpr float brick_width = 85.f;
    static constexpr float brick_height = 22.f;
    static constexpr float brick_gap = 12.f;

    static constexpr float platform_x_initial = (static_cast<float>(window_width) - platform_width) / 2;
    static constexpr float platform_y_initial = static_cast<float>(window_height - 40u);

    static constexpr float ball_radius = 7.f;
    static constexpr float ball_x_initial = static_cast<float>(window_width / 2);
    static constexpr float ball_y_initial = platform_y_initial - ball_radius;

    // physics constants
    static constexpr float bounce_coeff = -0.9f;
    static constexpr float fall_accel = 0.05f;
    static constexpr float platform_charge_boost = 7.f;
    static constexpr float initial_launch_speed = 12.f;
    static constexpr float max_ball_speed = 25.f;
    static constexpr float burn_speed_threshold = 16.f;

    // CCD — max swept sub-steps (adaptive, based on velocity)
    static constexpr unsigned ccd_max_steps = 16u;
    static constexpr float ccd_step_size = ball_radius * 0.5f;

    // lives
    static constexpr unsigned max_lives = 3u;

    // still ball's position relative to leftmost platform corner
    float static_ball_x_displacement = platform_width / 2;

    // game state
    enum class State { MENU, PLAYING, DEAD, WIN };
    State state = State::MENU;

    bool ball_launched = false;
    unsigned lives = max_lives;
    unsigned score = 0;
    unsigned best_score = 0;
    float death_timer = 0.f;

    // timing
    sf::Clock clock;

    sf::RenderWindow window;
    sf::Image img;
    sf::Texture texture;
    sf::Sprite background;

    sf::Font font;

    Platform platform;
    Ball ball;
    std::vector<Brick> bricks;
    ParticleSystem particles;

    void render();
    void renderHearts();
    void renderHUD();
    void renderMenu();
    void renderDeathScreen();
    void renderWinScreen();
    void moveBall(float dt);
    void spawnBricks();
    void resetBall();
    void startNewGame();
    void handleWallCollision();
    void handlePlatformCollision();
    void handleBrickCollision();
    void updateParticles(float dt);
    bool checkWin() const;

public:
    Game();
    void run();
};

#endif // BREAKOUT_GAME