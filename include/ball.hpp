#ifndef BREAKOUT_BALL
#define BREAKOUT_BALL

#include <SFML/Graphics.hpp>

class Ball {
    sf::CircleShape ball;
    sf::Vector2f prev_position;

public:
    sf::Vector2f velocity = {0.f, 0.f};
    float radius;

    Ball(const sf::Vector2f pos, const float r) : radius(r) {
        ball.setRadius(r);
        ball.setOrigin({r, r});
        ball.setPosition(pos);
        ball.setFillColor(sf::Color(255, 240, 200));
        prev_position = pos;
    }

    const sf::CircleShape& getShape() const {
        return ball;
    }

    const sf::Vector2f getPosition() const {
        return ball.getPosition();
    }

    const sf::Vector2f getPrevPosition() const {
        return prev_position;
    }

    void setPosition(sf::Vector2f pos) {
        ball.setPosition(pos);
        prev_position = pos;
    }

    void savePosition() {
        prev_position = ball.getPosition();
    }

    void move() {
        ball.move(velocity);
    }

    void displace(sf::Vector2f offset) {
        ball.move(offset);
    }

    sf::Vector2f lerpPosition(float t) const {
        return prev_position + (ball.getPosition() - prev_position) * t;
    }
};

#endif // BREAKOUT_BALL