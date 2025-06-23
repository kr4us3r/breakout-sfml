#ifndef BREAKOUT_BALL
#define BREAKOUT_BALL

#include <SFML/Graphics.hpp>

class Ball {
    sf::CircleShape ball;
    
public:
    sf::Vector2f velocity = {0.f, 0.f};

    Ball(const sf::Vector2f pos, const float radius) {
        ball.setRadius(radius);
        ball.setOrigin({radius, radius});
        ball.setPosition(pos);
        ball.setFillColor(sf::Color::Red);
    }

    const sf::CircleShape& getShape() {
        return ball;
    }

    const sf::Vector2f getPosition() {
        return ball.getPosition();
    }

    void setPosition(sf::Vector2f pos) {
        ball.setPosition(pos);
    }

    void move() {
        ball.move(velocity);
    }

    void displace(sf::Vector2f offset) {
        ball.move(offset);
    }
};

#endif // BREAKOUT_BALL