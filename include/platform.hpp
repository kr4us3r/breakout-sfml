#ifndef BREAKOUT_PLATFORM
#define BREAKOUT_PLATFORM

#include <SFML/Graphics.hpp>

class Platform {
    sf::RectangleShape platform;

public:
    float speed = 0.f;
    bool charged = false;

    Platform(const sf::Vector2f pos, const sf::Vector2f dimensions) {
        platform.setSize(dimensions);
        platform.setPosition(pos);
        platform.setFillColor(sf::Color::Blue);
        platform.setOutlineThickness(-1.f);
        platform.setOutlineColor(sf::Color::White);
    }

    const sf::RectangleShape& getShape() {
        return platform;
    }

    const sf::Vector2f getPosition() {
        return platform.getPosition();
    }

    void move(const float offset) {
        speed = offset;
        platform.move({offset, 0.f});
    }

    void charge() {
        platform.setFillColor(sf::Color::Yellow);
        charged = true;
    }

    void discharge() {
        platform.setFillColor(sf::Color::Blue);
        charged = false;
    }
};

#endif // BREAKOUT_PLATFORM