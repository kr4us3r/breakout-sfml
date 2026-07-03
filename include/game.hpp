#ifndef BREAKOUT_GAME
#define BREAKOUT_GAME

#include <vector>
#include <string>
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
    static constexpr unsigned num_bricks_y = 8u;
    static constexpr float brick_width = 85.f;
    static constexpr float brick_height = 22.f;
    static constexpr float brick_gap = 12.f;

    static constexpr float platform_x_initial = (static_cast<float>(window_width) - platform_width) / 2;
    static constexpr float platform_y_initial = static_cast<float>(window_height - 40u);

    static constexpr float ball_radius = 7.f;
    static constexpr float ball_x_initial = static_cast<float>(window_width / 2);
    static constexpr float ball_y_initial = platform_y_initial - ball_radius;

    // physics constants (scale slightly with level)
    static constexpr float bounce_coeff = -0.9f;
    static constexpr float base_fall_accel = 0.05f;
    float fall_accel = base_fall_accel;
    static constexpr float platform_charge_boost = 7.f;
    static constexpr float base_launch_speed = 12.f;
    float initial_launch_speed = base_launch_speed;
    static constexpr float max_ball_speed = 25.f;
    static constexpr float burn_speed_threshold = 16.f;

    // realistic platform/wall physics
    static constexpr float platform_restitution = 0.92f;
    static constexpr float platform_friction = 0.16f;
    static constexpr float wall_restitution = 0.88f;
    static constexpr float air_drag = 0.04f;
    static constexpr float relaunch_threshold = 4.5f;
    static constexpr float grab_vertical_range = 10.f;

    // CCD — max swept sub-steps (adaptive, based on velocity)
    static constexpr unsigned ccd_max_steps = 16u;
    static constexpr float ccd_step_size = ball_radius * 0.5f;

    // lives
    static constexpr unsigned max_lives = 3u;

    // still ball's position relative to leftmost platform corner
    float static_ball_x_displacement = platform_width / 2;

    // game mode + state
    enum class Mode { STORY, ENDLESS };
    enum class State { MENU, PLAYING, LEVEL_CLEAR, DEAD, WIN };
    Mode mode = Mode::STORY;
    State state = State::MENU;
    unsigned menu_selection = 0;          // 0 = Story, 1 = Endless
    unsigned current_level = 1u;
    static constexpr unsigned endless_cap_level = 12u;  // difficulty caps here
    float level_clear_timer = 0.f;
    std::string status_line;              // shown on LEVEL_CLEAR / WIN / DEAD

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
    void renderLevelClearScreen();
    void moveBall(float dt);
    void spawnLevel(unsigned level, Mode mode);
    void applyDifficulty(unsigned level);
    void resetBall();
    void startStory();
    void startEndless();
    void advanceLevel();
    void handleWallCollision();
    void handlePlatformCollision();
    void handleBrickCollision();
    void updateParticles(float dt);
    bool checkWin() const;

public:
    static constexpr unsigned story_level_count = 5u;
    Game();
    void run();
};

#endif // BREAKOUT_GAME