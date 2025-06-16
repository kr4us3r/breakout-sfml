#include <platform.hpp>

Platform::Platform(const sf::Vector2f pos, const sf::Vector2f dimensions) {
    platform.setSize(dimensions);
    platform.setPosition(pos);
    platform.setFillColor(sf::Color::Blue);
    platform.setOutlineThickness(-1.f);
    platform.setOutlineColor(sf::Color::White);
}

const sf::RectangleShape& Platform::getShape() {
    return platform;
}

void Platform::move(const float offset) {
    speed = offset;
    platform.move({offset, 0.f});
}

void Platform::charge() {
    platform.setFillColor(sf::Color::Yellow);
    charged = true;
}

void Platform::discharge() {
    platform.setFillColor(sf::Color::Blue);
    charged = false;
}
