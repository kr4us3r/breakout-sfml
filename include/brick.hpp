#ifndef BREAKOUT_BRICK
#define BREAKOUT_BRICK

#include <SFML/Graphics.hpp>

class Brick {
    sf::RectangleShape brick;

public:
    sf::Vector2f position;
    bool destroyed = false;
    
    Brick(const sf::Vector2f pos, const sf::Vector2f dimensions) {
        brick.setSize(dimensions);
        brick.setPosition(pos);
        brick.setFillColor(sf::Color::Green);
    }

    const sf::RectangleShape& getShape() const {
        return brick;
    }

    const sf::Vector2f getPosition() const {
        return brick.getPosition();
    }

    void destroy() {
        brick.setFillColor(sf::Color::Transparent);
        destroyed = true;
    }
};

#endif // BREAKOUT_BRICK