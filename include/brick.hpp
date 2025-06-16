#ifndef BREAKOUT_BRICK
#define BREAKOUT_BRICK

#include <SFML/Graphics.hpp>

class Brick {
    sf::RectangleShape brick;
    unsigned hp = 1;
public:
    sf::Vector2f position;
    Brick(const sf::Vector2f, const sf::Vector2f);
    const sf::RectangleShape& getShape();
};

#endif // BREAKOUT_BRICK