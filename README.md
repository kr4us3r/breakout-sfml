# Breakout

A modern take on Breakout, built in C++ with SFML 3.0. Features procedurally shattering bricks, realistic ball behavior, and two game modes.

---

## Highlights

### Crumbling bricks
Every brick is procedurally decomposed into a unique **Voronoi diagram** of polygon fragments - a different shatter pattern each game. When the ball hits, the nearest fragments physically break off.

### Realistic physics
- The ball reflects off the surfaces at the angle a real ball would, with restitution and friction coupling to paddle motion.
- Air drag bleeds energy over time, which feeds the de-launch grab mechanic.

### Burning ball
When the ball exceeds a speed threshold, it catches fire emitting a dynamic flame trail. The burn also lets it punch through bricks without deflecting.

### Charged paddle
Press Space while the ball is in play to charge the paddle. The next contact gives the ball a velocity boost.

---

## Controls

| Key       | Action |
|-----------|--------|
| **← / →** | Move paddle |
| **Space** | Launch ball / Charge & discharge paddle |
| **↑ / ↓** | Navigate menu |
| **Esc**   | Pause to menu / Quit |

---

## Building from Source

### Prerequisites
- **MinGW-w64 GCC 15+** (or any C++20-capable compiler)
- **SFML 3.0**
- **FreeType** (bundled with SFML static libs)

### Build (Windows / MinGW)
```bash
g++ -O3 -std=c++20 -static -DSFML_STATIC \
    -Iinclude -I<path-to-sfml>/include \
    -L<path-to-sfml>/lib \
    src/*.cpp -o breakout.exe \
    -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
    -lfreetype -lopengl32 -lgdi32 -lwinmm -mwindows
```

---

## 📜 License
This project is open source.

## 🙏 Credits
Built with [SFML](https://www.sfml-dev.org/) and GLM 5.2.
