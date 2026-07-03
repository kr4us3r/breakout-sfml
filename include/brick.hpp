#ifndef BREAKOUT_BRICK
#define BREAKOUT_BRICK

#include <array>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <SFML/Graphics.hpp>

// ----------------------------------------------------------------------
// Small math helpers for Voronoi cell generation (ODR-safe inline)
// ----------------------------------------------------------------------
namespace brick_detail {
    inline float dot(sf::Vector2f a, sf::Vector2f b) {
        return a.x * b.x + a.y * b.y;
    }

    inline sf::Color scaleColor(sf::Color c, float k) {
        auto clamp = [](int v) {
            return static_cast<std::uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v));
        };
        return sf::Color(clamp(static_cast<int>(c.r * k)),
                         clamp(static_cast<int>(c.g * k)),
                         clamp(static_cast<int>(c.b * k)),
                         c.a);
    }

    // Sutherland–Hodgman clip to the half-plane that contains `keep`.
    // Boundary is the perpendicular bisector of (keep, against).
    inline std::vector<sf::Vector2f> clipBisector(std::vector<sf::Vector2f> poly,
                                                  sf::Vector2f keep,
                                                  sf::Vector2f against) {
        if (poly.empty()) return poly;
        sf::Vector2f mid = (keep + against) * 0.5f;
        sf::Vector2f n = against - keep;   // points toward `against`
        auto inside = [&](sf::Vector2f p) { return dot(p - mid, n) <= 0.f; };

        std::vector<sf::Vector2f> out;
        const std::size_t m = poly.size();
        for (std::size_t i = 0; i < m; ++i) {
            sf::Vector2f cur = poly[i];
            sf::Vector2f prv = poly[(i + m - 1) % m];
            bool curIn = inside(cur);
            bool prvIn = inside(prv);
            if (curIn) {
                if (!prvIn) {
                    sf::Vector2f d = cur - prv;
                    float denom = dot(d, n);
                    if (std::abs(denom) > 1e-6f) {
                        float t = dot(mid - prv, n) / denom;
                        out.push_back(prv + d * t);
                    }
                }
                out.push_back(cur);
            } else if (prvIn) {
                sf::Vector2f d = cur - prv;
                float denom = dot(d, n);
                if (std::abs(denom) > 1e-6f) {
                    float t = dot(mid - prv, n) / denom;
                    out.push_back(prv + d * t);
                }
            }
        }
        return out;
    }
}

// ======================================================================
// Brick — a rectangular block composed of Voronoi fragments that
// physically crumble away when damaged.
// ======================================================================
class Brick {
    sf::RectangleShape glow;
    sf::Vector2f position;
    sf::Vector2f size;

    static constexpr std::size_t N = 6;
    static constexpr std::array<sf::Color, N> fill_pool = {
        sf::Color(100, 200, 255),   // 1 HP — cyan
        sf::Color(120, 230, 140),   // 2 HP — green
        sf::Color(255, 220, 90),    // 3 HP — gold
        sf::Color(255, 160, 70),    // 4 HP — orange
        sf::Color(255, 90, 90),     // 5 HP — red
        sf::Color(190, 110, 240)    // 6 HP — purple
    };
    static constexpr std::array<sf::Color, N> glow_pool = {
        sf::Color(100, 200, 255, 50),
        sf::Color(120, 230, 140, 50),
        sf::Color(255, 220, 90, 50),
        sf::Color(255, 160, 70, 50),
        sf::Color(255, 90, 90, 50),
        sf::Color(190, 110, 240, 50)
    };

    static constexpr unsigned fragments_per_brick = 9u;
    static constexpr float debris_life = 1.5f;
    static constexpr float debris_gravity = 1000.f;

    // A polygon fragment in local brick coordinates (origin = brick top-left).
    struct Fragment {
        std::vector<sf::Vector2f> points;
        sf::Vector2f centroid;
        float brightness = 1.f;
        sf::Color color;
    };
    std::vector<Fragment> fragments;   // currently attached

    // A detached, falling piece.
    struct Debris {
        std::vector<sf::Vector2f> points;   // recentered about centroid
        sf::Color color;
        sf::Color outline;
        sf::Vector2f position;
        sf::Vector2f velocity;
        float angle = 0.f;
        float spin = 0.f;
        float life = debris_life;
    };
    std::vector<Debris> debris;

    void generateFragments(sf::Color tier) {
        fragments.clear();

        // Random per-brick, per-game seed.
        unsigned seed = static_cast<unsigned>(std::rand())
                      ^ static_cast<unsigned>(position.x * 73856093u)
                      ^ static_cast<unsigned>(position.y * 19349663u);
        std::mt19937 rng(seed);
        const float margin = 4.f;
        std::uniform_real_distribution<float> ufx(margin, size.x - margin);
        std::uniform_real_distribution<float> ufy(margin, size.y - margin);
        std::uniform_real_distribution<float> ubr(0.82f, 1.12f);

        std::vector<sf::Vector2f> seeds(fragments_per_brick);
        for (auto& s : seeds) s = sf::Vector2f(ufx(rng), ufy(rng));

        for (std::size_t i = 0; i < fragments_per_brick; ++i) {
            std::vector<sf::Vector2f> cell = {
                {0.f, 0.f}, {size.x, 0.f}, {size.x, size.y}, {0.f, size.y}
            };
            for (std::size_t j = 0; j < fragments_per_brick && cell.size() >= 3; ++j) {
                if (i == j) continue;
                cell = brick_detail::clipBisector(cell, seeds[i], seeds[j]);
            }
            if (cell.size() < 3) continue;

            Fragment f;
            f.points = cell;
            sf::Vector2f c{0.f, 0.f};
            for (auto& p : cell) c += p;
            c /= static_cast<float>(cell.size());
            f.centroid = c;
            f.brightness = ubr(rng);
            fragments.push_back(f);
        }

        if (fragments.empty()) {
            Fragment f;
            f.points = {{0.f, 0.f}, {size.x, 0.f}, {size.x, size.y}, {0.f, size.y}};
            f.centroid = size * 0.5f;
            f.brightness = 1.f;
            fragments.push_back(f);
        }
        recolorFragments(tier);
    }

    void recolorFragments(sf::Color tier) {
        for (auto& f : fragments)
            f.color = brick_detail::scaleColor(tier, f.brightness);
    }

    // Move the `count` fragments nearest to `hitPos` (or all of them) into
    // the falling-debris list.
    void detachFragments(sf::Vector2f hitPos, std::size_t count, bool all) {
        if (fragments.empty()) return;

        sf::Vector2f localHit = hitPos - position;
        std::vector<std::size_t> idx(fragments.size());
        for (std::size_t i = 0; i < fragments.size(); ++i) idx[i] = i;
        std::sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) {
            sf::Vector2f da = fragments[a].centroid - localHit;
            sf::Vector2f db = fragments[b].centroid - localHit;
            return brick_detail::dot(da, da) < brick_detail::dot(db, db);
        });

        std::size_t n = all ? fragments.size() : std::min(count, fragments.size());
        std::vector<bool> detachFlag(fragments.size(), false);
        for (std::size_t i = 0; i < n; ++i) detachFlag[idx[i]] = true;

        auto frand = [](float lo, float hi) {
            return lo + (std::rand() / static_cast<float>(RAND_MAX)) * (hi - lo);
        };

        std::vector<Fragment> remaining;
        remaining.reserve(fragments.size() - n);
        for (std::size_t i = 0; i < fragments.size(); ++i) {
            const Fragment& f = fragments[i];
            if (!detachFlag[i]) { remaining.push_back(f); continue; }

            Debris d;
            d.points.reserve(f.points.size());
            for (auto& p : f.points) d.points.push_back(p - f.centroid);
            d.color = f.color;
            d.outline = brick_detail::scaleColor(f.color, 0.5f);
            d.position = position + f.centroid;

            sf::Vector2f dir = f.centroid - localHit;
            float len = std::sqrt(brick_detail::dot(dir, dir));
            if (len < 0.001f) dir = sf::Vector2f(0.f, -1.f);
            else dir /= len;

            d.velocity = dir * frand(60.f, 220.f);
            d.velocity.y += frand(-140.f, -20.f);   // upward burst
            d.spin = frand(-220.f, 220.f);
            d.angle = 0.f;
            d.life = debris_life;
            debris.push_back(std::move(d));
        }
        fragments = std::move(remaining);
    }

public:
    std::size_t health_points;
    std::size_t max_health;
    bool destroyed = false;

    Brick(const sf::Vector2f pos, const sf::Vector2f dimensions, std::size_t hp)
        : position(pos), size(dimensions) {
        health_points = hp;
        max_health = hp;

        glow.setSize({dimensions.x + 8.f, dimensions.y + 8.f});
        glow.setPosition({pos.x - 4.f, pos.y - 4.f});
        std::size_t tier = std::min(hp, N) - 1;
        glow.setFillColor(glow_pool[tier]);

        generateFragments(fill_pool[tier]);
    }

    const sf::RectangleShape& getGlow() const { return glow; }

    void drawBody(sf::RenderWindow& window) const {
        if (!destroyed) {
            for (const auto& f : fragments) {
                sf::ConvexShape s;
                s.setPointCount(f.points.size());
                for (std::size_t i = 0; i < f.points.size(); ++i)
                    s.setPoint(i, f.points[i]);
                s.setPosition(position);
                s.setFillColor(f.color);
                s.setOutlineThickness(-1.f);
                s.setOutlineColor(sf::Color(15, 10, 30, 200));
                window.draw(s);
            }
        }

        for (const auto& d : debris) {
            float r = d.life / debris_life;
            if (r < 0.f) r = 0.f;
            sf::Color c = d.color;
            c.a = static_cast<std::uint8_t>(255.f * r);
            sf::Color oc = d.outline;
            oc.a = static_cast<std::uint8_t>(200.f * r);

            sf::ConvexShape s;
            s.setPointCount(d.points.size());
            for (std::size_t i = 0; i < d.points.size(); ++i)
                s.setPoint(i, d.points[i]);
            s.setPosition(d.position);
            s.setRotation(sf::degrees(d.angle));
            s.setFillColor(c);
            s.setOutlineThickness(-1.f);
            s.setOutlineColor(oc);
            window.draw(s);
        }
    }

    void update(float dt) {
        for (auto& d : debris) {
            d.velocity.y += debris_gravity * dt;
            d.velocity.x *= std::exp(-0.8f * dt);
            d.position += d.velocity * dt;
            d.angle += d.spin * dt;
            d.life -= dt;
        }
        debris.erase(std::remove_if(debris.begin(), debris.end(),
            [](const Debris& d) { return d.life <= 0.f; }), debris.end());
    }

    const sf::Vector2f getPosition() const { return position; }

    sf::Color getColor() const {
        return fill_pool[std::min(health_points, N) - 1];
    }

    void takeDamage(sf::Vector2f hitPos) {
        if (destroyed || health_points == 0) return;
        --health_points;
        if (health_points == 0) {
            detachFragments(hitPos, 0, true);
            glow.setFillColor(sf::Color::Transparent);
            destroyed = true;
        } else {
            std::size_t count = 1u + static_cast<std::size_t>(std::rand() % 2u);
            detachFragments(hitPos, count, false);
            recolorFragments(fill_pool[std::min(health_points, N) - 1]);
        }
    }
};

#endif // BREAKOUT_BRICK