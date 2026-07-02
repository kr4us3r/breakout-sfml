#ifndef BREAKOUT_PLATFORM
#define BREAKOUT_PLATFORM

#include <SFML/Graphics.hpp>

class Platform {
    sf::RectangleShape platform;
    sf::RectangleShape glow;

public:
    float speed = 0.f;          // current velocity (px per frame-equivalent)
    float target_speed = 0.f;   // desired velocity from input
    float max_speed = 16.f;     // max movement speed
    float acceleration = 0.8f;  // how fast we approach target_speed
    bool charged = false;

    Platform(const sf::Vector2f pos, const sf::Vector2f dimensions) {
        platform.setSize(dimensions);
        platform.setPosition(pos);
        platform.setFillColor(sf::Color(80, 140, 255));
        platform.setOutlineThickness(-2.f);
        platform.setOutlineColor(sf::Color(180, 220, 255));

        glow.setSize({dimensions.x + 12.f, dimensions.y + 12.f});
        glow.setPosition({pos.x - 6.f, pos.y - 6.f});
        glow.setFillColor(sf::Color(80, 140, 255, 40));
    }

    const sf::RectangleShape& getShape() const {
        return platform;
    }

    const sf::RectangleShape& getGlow() const {
        return glow;
    }

    const sf::Vector2f getPosition() const {
        return platform.getPosition();
    }

    void setPosition(sf::Vector2f pos) {
        platform.setPosition(pos);
        glow.setPosition({pos.x - 6.f, pos.y - 6.f});
    }

    // Smoothly accelerate toward target_speed, then move
    void update() {
        speed += (target_speed - speed) * acceleration;
        // dead-zone to zero
        if (std::abs(speed) < 0.05f) speed = 0.f;

        sf::Vector2f offset = {speed, 0.f};
        platform.move(offset);
        glow.move(offset);
    }

    void charge() {
        platform.setFillColor(sf::Color(255, 220, 80));
        platform.setOutlineColor(sf::Color::White);
        glow.setFillColor(sf::Color(255, 200, 60, 80));
        charged = true;
    }

    void discharge() {
        platform.setFillColor(sf::Color(80, 140, 255));
        platform.setOutlineColor(sf::Color(180, 220, 255));
        glow.setFillColor(sf::Color(80, 140, 255, 40));
        charged = false;
    }
};

#endif // BREAKOUT_PLATFORM