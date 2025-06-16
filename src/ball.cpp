#include <ball.hpp>

Ball::Ball(const sf::Vector2f pos, const float radius) {
    ball.setRadius(radius);
    ball.setOrigin({radius, radius});
    ball.setPosition(pos);
    ball.setFillColor(sf::Color::Red);
}

const sf::CircleShape& Ball::getShape() {
    return ball;
}

void Ball::move() {
    ball.move(velocity);
}

void Ball::displace(sf::Vector2f offset) {
    ball.move(offset);
}

void Ball::setPosition(sf::Vector2f pos) {
    ball.setPosition(pos);
}