#ifndef BREAKOUT_BRICK
#define BREAKOUT_BRICK

#include <array>
#include <SFML/Graphics.hpp>

class Brick {
    sf::RectangleShape brick;
    sf::RectangleShape glow;
    static constexpr size_t N = 3;
    static constexpr std::array<sf::Color, N> fill_pool = {
        sf::Color(100, 200, 255),   // 1 HP — cyan
        sf::Color(255, 200, 80),    // 2 HP — gold
        sf::Color(255, 100, 100)    // 3 HP — red
    };
    static constexpr std::array<sf::Color, N> glow_pool = {
        sf::Color(100, 200, 255, 50),
        sf::Color(255, 200, 80, 50),
        sf::Color(255, 100, 100, 50)
    };

public:
    size_t health_points;
    size_t max_health;
    bool destroyed = false;

    Brick(const sf::Vector2f pos, const sf::Vector2f dimensions, size_t hp) {
        brick.setSize(dimensions);
        brick.setPosition(pos);
        brick.setOutlineThickness(-1.f);
        brick.setOutlineColor(sf::Color(255, 255, 255, 100));
        health_points = hp;
        max_health = hp;
        brick.setFillColor(fill_pool[std::min(hp, N) - 1]);

        glow.setSize({dimensions.x + 8.f, dimensions.y + 8.f});
        glow.setPosition({pos.x - 4.f, pos.y - 4.f});
        glow.setFillColor(glow_pool[std::min(hp, N) - 1]);
    }

    const sf::RectangleShape& getShape() const {
        return brick;
    }

    const sf::RectangleShape& getGlow() const {
        return glow;
    }

    const sf::Vector2f getPosition() const {
        return brick.getPosition();
    }

    sf::Color getColor() const {
        return fill_pool[std::min(health_points, N) - 1];
    }

    void takeDamage() {
        --health_points;
        if (health_points == 0) {
            brick.setFillColor(sf::Color::Transparent);
            glow.setFillColor(sf::Color::Transparent);
            destroyed = true;
        } else {
            brick.setFillColor(fill_pool[std::min(health_points, N) - 1]);
            glow.setFillColor(glow_pool[std::min(health_points, N) - 1]);
        }
    }
};

#endif // BREAKOUT_BRICK