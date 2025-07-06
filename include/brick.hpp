#ifndef BREAKOUT_BRICK
#define BREAKOUT_BRICK

#include <array>
#include <SFML/Graphics.hpp>

class Brick {
    sf::RectangleShape brick;
    static constexpr size_t N = 3;
    static constexpr std::array<sf::Color, N> color_pool = {sf::Color::White, sf::Color::Yellow, sf::Color::Red};

public:
    sf::Vector2f position;
    size_t health_points;
    bool destroyed = false;
    
    Brick(const sf::Vector2f pos, const sf::Vector2f dimensions, size_t hp) {
        brick.setSize(dimensions);
        brick.setPosition(pos);
        health_points = hp;
        brick.setFillColor(color_pool[std::min(hp, N)-1]);
    }

    const sf::RectangleShape& getShape() const {
        return brick;
    }

    const sf::Vector2f getPosition() const {
        return brick.getPosition();
    }

    void takeDamage() {
        --health_points;
        if (health_points == 0) {
            brick.setFillColor(sf::Color::Transparent);
            destroyed = true;
        } else {
            brick.setFillColor(color_pool[std::min(health_points, N)-1]);
        }
    }
};

#endif // BREAKOUT_BRICK