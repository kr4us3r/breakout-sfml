#ifndef BREAKOUT_PLATFORM
#define BREAKOUT_PLATFORM

#include <SFML/Graphics.hpp>

class Platform {
    sf::RectangleShape platform;

public:
    float speed = 0.f;
    bool charged = false;

    Platform(const sf::Vector2f, const sf::Vector2f);
    const sf::RectangleShape& getShape();
    void move(const float);
    void charge();
    void discharge();
};

#endif // BREAKOUT_PLATFORM