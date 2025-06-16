#include <brick.hpp>

Brick::Brick(const sf::Vector2f pos, const sf::Vector2f dimensions) {
    brick.setSize(dimensions);
    brick.setPosition(pos);
    brick.setFillColor(sf::Color::Green);
}

const sf::RectangleShape& Brick::getShape() {
    return brick;
}