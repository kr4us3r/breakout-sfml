#ifndef BREAKOUT_PARTICLES
#define BREAKOUT_PARTICLES

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cmath>

class Particle {
    sf::CircleShape shape;
    float life = 0.f;
    float max_life = 1.f;

public:
    sf::Vector2f velocity;

    Particle() {
        shape.setFillColor(sf::Color::Transparent);
    }

    void init(const sf::Vector2f& pos, const sf::Vector2f& vel, float radius, sf::Color color, float lifespan) {
        shape.setRadius(radius);
        shape.setOrigin({radius, radius});
        shape.setPosition(pos);
        shape.setFillColor(color);
        velocity = vel;
        life = lifespan;
        max_life = lifespan;
    }

    void update(float dt) {
        shape.move(velocity * dt);
        life -= dt;
        if (life <= 0.f) {
            shape.setFillColor(sf::Color::Transparent);
        }
    }

    [[nodiscard]] bool isDead() const { return life <= 0.f; }

    const sf::CircleShape& getShape() const { return shape; }
};

// Pool-based particle system — avoids per-frame allocation
class ParticleSystem {
    static constexpr size_t POOL_SIZE = 500;
    Particle pool[POOL_SIZE];
    size_t next_index = 0;

    size_t nextSlot() {
        size_t idx = next_index;
        next_index = (next_index + 1) % POOL_SIZE;
        return idx;
    }

public:
    void emit(const sf::Vector2f& pos, const sf::Vector2f& base_vel,
              unsigned count, sf::Color color, float spread,
              float speed_min, float speed_max,
              float radius_min, float radius_max,
              float life_min, float life_max) {
        for (unsigned i = 0; i < count; ++i) {
            float angle = (std::rand() % 1000 / 1000.f) * 2.f * 3.14159265f;
            float speed = speed_min + (std::rand() % 1000 / 1000.f) * (speed_max - speed_min);
            sf::Vector2f vel(base_vel.x + std::cos(angle) * speed,
                             base_vel.y + std::sin(angle) * speed);
            float radius = radius_min + (std::rand() % 1000 / 1000.f) * (radius_max - radius_min);
            float life = life_min + (std::rand() % 1000 / 1000.f) * (life_max - life_min);

            // Fade alpha based on life proportion
            sf::Color c = color;

            pool[nextSlot()].init(pos, vel, radius, c, life);
        }
    }

    void update(float dt) {
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            if (!pool[i].isDead()) {
                pool[i].update(dt);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            if (!pool[i].isDead()) {
                window.draw(pool[i].getShape());
            }
        }
    }
};

#endif // BREAKOUT_PARTICLES