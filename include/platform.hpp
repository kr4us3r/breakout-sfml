#ifndef BREAKOUT_PLATFORM
#define BREAKOUT_PLATFORM

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <cstdint>

// ======================================================================
// Platform — a metallic grey paddle that can hold the ball with a
// crackling electric arc, and reflects the ball using realistic
// (normal/tangential) physics rather than arcade angle-steering.
// ======================================================================
class Platform {
    sf::Vector2f position;
    sf::Vector2f size;
    sf::RectangleShape glow;
    bool charged = false;

    static sf::Color lerp(sf::Color a, sf::Color b, float t) {
        return sf::Color(
            static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<std::uint8_t>(a.b + (b.b - a.b) * t),
            static_cast<std::uint8_t>(a.a + (b.a - a.a) * t));
    }

public:
    float speed = 0.f;          // current velocity (px per frame-equivalent)
    float target_speed = 0.f;   // desired velocity from input
    float max_speed = 16.f;     // max movement speed
    float acceleration = 0.8f;  // how fast we approach target_speed

    Platform(const sf::Vector2f pos, const sf::Vector2f dimensions)
        : position(pos), size(dimensions) {
        glow.setSize({dimensions.x + 16.f, dimensions.y + 18.f});
        glow.setPosition({pos.x - 8.f, pos.y - 9.f});
        glow.setFillColor(sf::Color(130, 150, 180, 30));
    }

    bool isCharged() const { return charged; }
    const sf::RectangleShape& getGlow() const { return glow; }

    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }

    void setPosition(sf::Vector2f pos) {
        position = pos;
        glow.setPosition({pos.x - 8.f, pos.y - 9.f});
    }

    // Smoothly accelerate toward target_speed, then move
    void update() {
        speed += (target_speed - speed) * acceleration;
        if (std::abs(speed) < 0.05f) speed = 0.f;

        position.x += speed;
        glow.move({speed, 0.f});
    }

    void charge() {
        charged = true;
        glow.setFillColor(sf::Color(255, 200, 70, 85));
    }

    void discharge() {
        charged = false;
        glow.setFillColor(sf::Color(130, 150, 180, 30));
    }

    // ------------------------------------------------------------------
    // Pretty metallic body: dark edge + vertical gradient + highlights
    // ------------------------------------------------------------------
    void drawBody(sf::RenderWindow& window) const {
        const sf::Color c_top      = charged ? sf::Color(255, 235, 160) : sf::Color(214, 219, 228);
        const sf::Color c_mid      = charged ? sf::Color(240, 190, 80)  : sf::Color(150, 156, 168);
        const sf::Color c_bot      = charged ? sf::Color(170, 120, 30)  : sf::Color(82, 87, 100);
        const sf::Color c_edge     = charged ? sf::Color(90, 60, 10)    : sf::Color(40, 44, 54);
        const sf::Color c_highlight= charged ? sf::Color(255, 250, 210) : sf::Color(245, 248, 255);
        const sf::Color c_energy   = charged ? sf::Color(255, 220, 120) : sf::Color(110, 190, 255);

        // outer dark edge
        sf::RectangleShape edge(size);
        edge.setPosition(position);
        edge.setFillColor(c_edge);
        window.draw(edge);

        // vertical gradient via stacked slices
        const int slices = 7;
        const float pad = 1.5f;
        const float bw = size.x - pad * 2.f;
        const float bh = size.y - pad * 2.f;
        const float sh = bh / static_cast<float>(slices);
        for (int i = 0; i < slices; ++i) {
            float t0 = static_cast<float>(i) / static_cast<float>(slices);
            sf::Color c0 = (t0 < 0.5f) ? lerp(c_top, c_mid, t0 * 2.f)
                                       : lerp(c_mid, c_bot, (t0 - 0.5f) * 2.f);
            sf::RectangleShape s({bw, sh + 1.f});
            s.setPosition({position.x + pad, position.y + pad + bh * t0});
            s.setFillColor(c0);
            window.draw(s);
        }

        // bright top highlight
        sf::RectangleShape hl({bw, 2.f});
        hl.setPosition({position.x + pad, position.y + pad});
        hl.setFillColor(c_highlight);
        window.draw(hl);

        // energy strip near the top
        sf::RectangleShape energy({bw - 6.f, 2.f});
        energy.setPosition({position.x + pad + 3.f, position.y + pad + 4.f});
        energy.setFillColor(sf::Color(c_energy.r, c_energy.g, c_energy.b, 210));
        window.draw(energy);

        // bottom shadow line
        sf::RectangleShape shline({bw, 2.f});
        shline.setPosition({position.x + pad, position.y + size.y - pad - 2.f});
        shline.setFillColor(sf::Color(c_edge.r, c_edge.g, c_edge.b, 210));
        window.draw(shline);
    }

    // ------------------------------------------------------------------
    // Electric arc from the platform's top center to the ball — drawn
    // as several jittered polylines to look like crackling lightning.
    // ------------------------------------------------------------------
    void drawArc(sf::RenderWindow& window, sf::Vector2f ball_pos) const {
        const sf::Vector2f from = {position.x + size.x * 0.5f, position.y};
        const sf::Vector2f to = ball_pos;
        sf::Vector2f d = to - from;
        float len = std::sqrt(d.x * d.x + d.y * d.y);
        sf::Vector2f perp = (len > 0.001f) ? sf::Vector2f{-d.y / len, d.x / len}
                                           : sf::Vector2f{0.f, 0.f};

        auto buildArc = [&](sf::Color color, float amp) {
            const int segs = 9;
            sf::VertexArray arc(sf::PrimitiveType::LineStrip, static_cast<std::size_t>(segs + 1));
            for (int i = 0; i <= segs; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(segs);
                sf::Vector2f p = from * (1.f - t) + to * t;
                if (i != 0 && i != segs) {
                    float j = (std::rand() / static_cast<float>(RAND_MAX) - 0.5f) * amp;
                    p += perp * j;
                }
                arc[static_cast<std::size_t>(i)].position = p;
                arc[static_cast<std::size_t>(i)].color = color;
            }
            window.draw(arc);
        };

        const sf::Color base = charged ? sf::Color(255, 220, 120) : sf::Color(120, 200, 255);
        buildArc(sf::Color(base.r, base.g, base.b, 60),  20.f); // soft halo
        buildArc(sf::Color(base.r, base.g, base.b, 130), 13.f);
        buildArc(sf::Color(230, 245, 255, 235),          7.f);  // bright core
    }
};

#endif // BREAKOUT_PLATFORM