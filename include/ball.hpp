#ifndef BREAKOUT_BALL
#define BREAKOUT_BALL

#include <SFML/Graphics.hpp>

class Ball {
    sf::CircleShape ball;
    
public:
    sf::Vector2f velocity = {0.f, 0.f};

    Ball(const sf::Vector2f, const float);
    const sf::CircleShape& getShape();
    void move();
    void displace(sf::Vector2f);
    void setPosition(sf::Vector2f);
};

#endif // BREAKOUT_BALL