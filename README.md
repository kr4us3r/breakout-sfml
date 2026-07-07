# Breakout

A modern take on Breakout, built in C++ with SFML 3.0. Features procedurally shattering bricks, realistic ball behavior, and two game modes.

---

## Highlights

### Crumbling bricks
Every brick is procedurally decomposed into a unique Voronoi diagram of polygon fragments - a different shatter pattern each game. When the ball hits, the nearest fragments physically break off.

### Realistic physics
- The ball reflects off the surfaces at the angle a real ball would, with restitution and friction coupling to paddle motion.
- Air drag bleeds energy over time, which feeds the de-launch grab mechanic.

### Burning ball
When the ball exceeds a speed threshold, it catches fire emitting a dynamic flame trail. The burn also lets it punch through bricks without deflecting.

### Charged paddle
Press Space while the ball is in play to charge the paddle. The next contact gives the ball a velocity boost.

---

## Controls

| Input           | Action |
|-----------------|--------|
| **← / →** or **Mouse** | Move paddle |
| **Space / Click** | Launch ball / Charge & discharge paddle / Select menu item |
| **↑ / ↓**       | Navigate menu |
| **Esc**         | Pause game / Resume / Return to menu / Quit |

---

## Building from Source

### Prerequisites
- **MinGW-w64 GCC 15+** (or any C++20-capable compiler)
- **SFML 3.0**
- **FreeType** (bundled with SFML static libs)

### Audio assets (music + SFX)

Both the music tracks (in `assets/music/`) and the sound effects are embedded into the
executable as C++ byte arrays via `tools/embed_audio.py`, so the resulting
`.exe` is fully self-contained.

If you add, replace, or remove any audio asset, regenerate the embedded
sources:

```bash
# Re-embed all audio (music + SFX) into C++ sources
python tools/embed_audio.py
```

This regenerates `include/audio_data.hpp` and `src/audio_data.cpp`, which are
picked up automatically by the build (the latter is compiled alongside the
other translation units under `src/`).

### Build (Windows / MinGW)
```bash
# 1. Embed the audio assets into C++ sources (run once, or whenever assets change)
python tools/embed_audio.py

# 2. Compile
g++ -O3 -std=c++20 -static -DSFML_STATIC \
    -Iinclude -I<path-to-sfml>/include \
    -L<path-to-sfml>/lib \
    src/*.cpp -o breakout.exe \
    -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s \
    -lvorbisenc -lvorbisfile -lvorbis -logg -lFLAC \
    -lfreetype -lopengl32 -lgdi32 -lwinmm -mwindows
```

---

## License
This project is open source.

## Credits
Built with [SFML](https://www.sfml-dev.org/) and GLM 5.2.