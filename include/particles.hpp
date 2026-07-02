#ifndef BREAKOUT_PARTICLES
#define BREAKOUT_PARTICLES

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cmath>
#include <cstdint>

// ======================================================================
// Advanced Particle — supports alpha fade, size shrink, gravity, drag
// ======================================================================
class Particle {
    sf::CircleShape shape;
    float life = 0.f;
    float max_life = 1.f;
    float initial_radius = 1.f;
    float drag = 0.98f;
    float gravity = 0.f;
    sf::Color base_color;
    bool fading = true;
    bool shrinking = true;

public:
    sf::Vector2f velocity;

    Particle() {
        shape.setFillColor(sf::Color::Transparent);
    }

    void init(const sf::Vector2f& pos, const sf::Vector2f& vel, float r,
              sf::Color color, float lifespan, float grav = 0.f, float d = 0.98f) {
        shape.setRadius(r);
        shape.setOrigin({r, r});
        shape.setPosition(pos);
        shape.setFillColor(color);
        velocity = vel;
        life = lifespan;
        max_life = lifespan;
        initial_radius = r;
        base_color = color;
        gravity = grav;
        drag = d;
        fading = true;
        shrinking = true;
    }

    void update(float dt) {
        // Apply gravity
        velocity.y += gravity * dt;

        // Apply drag (frame-rate independent-ish)
        velocity *= drag;

        shape.move(velocity * dt);
        life -= dt;

        if (life <= 0.f) {
            shape.setFillColor(sf::Color::Transparent);
            return;
        }

        float ratio = life / max_life;  // 1 = just born, 0 = dying

        // Fade alpha
        if (fading) {
            sf::Color c = base_color;
            c.a = static_cast<std::uint8_t>(static_cast<float>(c.a) * ratio);
            shape.setFillColor(c);
        }

        // Shrink radius
        if (shrinking) {
            float r = initial_radius * ratio;
            if (r < 0.5f) r = 0.5f;
            shape.setRadius(r);
            shape.setOrigin({r, r});
        }
    }

    [[nodiscard]] bool isDead() const { return life <= 0.f; }

    const sf::CircleShape& getShape() const { return shape; }
};

// ======================================================================
// Pool-based particle system — avoids per-frame allocation
// ======================================================================
class ParticleSystem {
    static constexpr size_t POOL_SIZE = 1200;
    Particle pool[POOL_SIZE];
    size_t next_index = 0;

    size_t nextSlot() {
        size_t idx = next_index;
        next_index = (next_index + 1) % POOL_SIZE;
        return idx;
    }

public:
    // Emit particles with full control over appearance + physics
    void emit(const sf::Vector2f& pos, const sf::Vector2f& base_vel,
              unsigned count, sf::Color color, float /*spread*/,
              float speed_min, float speed_max,
              float radius_min, float radius_max,
              float life_min, float life_max,
              float gravity = 0.f, float drag = 0.97f) {
        for (unsigned i = 0; i < count; ++i) {
            float r01 = std::rand() % 1000 / 1000.f;
            float angle = r01 * 2.f * 3.14159265f;
            float speed = speed_min + (std::rand() % 1000 / 1000.f) * (speed_max - speed_min);
            sf::Vector2f vel(base_vel.x + std::cos(angle) * speed,
                             base_vel.y + std::sin(angle) * speed);
            float radius = radius_min + (std::rand() % 1000 / 1000.f) * (radius_max - radius_min);
            float life = life_min + (std::rand() % 1000 / 1000.f) * (life_max - life_min);

            pool[nextSlot()].init(pos, vel, radius, color, life, gravity, drag);
        }
    }

    // Emit in a directed cone (useful for directional sparks)
    void emitDirected(const sf::Vector2f& pos, const sf::Vector2f& direction,
                      unsigned count, sf::Color color,
                      float speed_min, float speed_max,
                      float cone_half_angle,
                      float radius_min, float radius_max,
                      float life_min, float life_max,
                      float gravity = 0.f, float drag = 0.97f) {
        float base_angle = std::atan2(direction.y, direction.x);
        for (unsigned i = 0; i < count; ++i) {
            float offset = (std::rand() % 1000 / 1000.f - 0.5f) * 2.f * cone_half_angle;
            float angle = base_angle + offset;
            float speed = speed_min + (std::rand() % 1000 / 1000.f) * (speed_max - speed_min);
            sf::Vector2f vel(std::cos(angle) * speed, std::sin(angle) * speed);
            float radius = radius_min + (std::rand() % 1000 / 1000.f) * (radius_max - radius_min);
            float life = life_min + (std::rand() % 1000 / 1000.f) * (life_max - life_min);

            pool[nextSlot()].init(pos, vel, radius, color, life, gravity, drag);
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