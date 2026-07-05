#ifndef BREAKOUT_SOUND
#define BREAKOUT_SOUND

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <random>
#include <cstddef>

#include "audio_data.hpp"

// A music track embedded in the executable.
using EmbeddedTrack = std::pair<const void*, std::size_t>;

// Unified audio manager. Streams music from the embedded byte arrays via
// sf::Music::openFromMemory() (low RAM), and plays short sound effects via
// sf::SoundBuffer::loadFromMemory() + a pool of sf::Sound voices.
//
// IMPORTANT design note: each sf::SoundBuffer is heap-allocated (unique_ptr)
// so its address is stable. sf::Sound voices hold a raw pointer to their
// buffer, so the buffer must never move/relocate after the voices are
// created. Moving an Effect that holds the buffer by value would invalidate
// those pointers - the previous bug that caused SFX to play silently.
class SoundManager {
    static constexpr std::size_t voices_per_effect = 4;

    struct SfxEffect {
        std::unique_ptr<sf::SoundBuffer> buffer;  // stable address
        std::vector<std::unique_ptr<sf::Sound>> voices;
        std::size_t next = 0;
    };

    // --- Music (streamed) ---
    std::unique_ptr<sf::Music> music_;
    std::vector<EmbeddedTrack> playlist_;
    std::mt19937 rng_;
    int last_index_ = -1;

    // --- SFX (in-RAM buffers) ---
    std::unordered_map<std::string, SfxEffect> sfx_;

    // --- volumes ---
    float music_volume_ = 30.f;
    float sfx_volume_ = 60.f;

    void playRandomTrack() {
        if (playlist_.empty()) return;

        int index = last_index_;
        if (playlist_.size() > 1) {
            std::uniform_int_distribution<int> dist(
                0, static_cast<int>(playlist_.size()) - 1);
            do {
                index = dist(rng_);
            } while (index == last_index_);
        } else {
            index = 0;
        }

        const auto& [data, size] = playlist_[index];
        music_ = std::make_unique<sf::Music>();
        if (!music_->openFromMemory(data, size)) {
            music_.reset();
            return;
        }

        music_->setLooping(false);
        music_->setVolume(music_volume_);
        music_->play();
        last_index_ = index;
    }

public:
    SoundManager() : rng_(std::random_device{}()) {}

    // ---------------- Music ----------------

    void setPlaylist(std::vector<EmbeddedTrack> tracks) {
        playlist_ = std::move(tracks);
        last_index_ = -1;
        playRandomTrack();
    }

    void updateMusic() {
        if (music_ && music_->getStatus() == sf::SoundSource::Status::Stopped) {
            playRandomTrack();
        }
    }

    void stopMusic() {
        if (music_) music_->stop();
    }

    void setMusicVolume(float volume) {
        music_volume_ = volume;
        if (music_) music_->setVolume(volume);
    }

    // ---------------- SFX ----------------

    // Load a single effect from an embedded byte array.
    void loadSfx(const std::string& name, const void* data, std::size_t size) {
        auto buffer = std::make_unique<sf::SoundBuffer>();
        if (!buffer->loadFromMemory(data, size)) return;

        SfxEffect e;
        e.buffer = std::move(buffer);  // address now stable
        e.voices.reserve(voices_per_effect);
        for (std::size_t i = 0; i < voices_per_effect; ++i) {
            auto v = std::make_unique<sf::Sound>(*e.buffer);
            v->setVolume(sfx_volume_);
            e.voices.push_back(std::move(v));
        }
        sfx_.emplace(name, std::move(e));
    }

    // Convenience: load all the embedded SFX at once.
    void loadAllSfx() {
        loadSfx("platform_hit", sfx_platform_hit_data,
                static_cast<std::size_t>(sfx_platform_hit_size));
        loadSfx("brick_hit", sfx_brick_hit_data,
                static_cast<std::size_t>(sfx_brick_hit_size));
        loadSfx("wall_hit", sfx_wall_hit_data,
                static_cast<std::size_t>(sfx_wall_hit_size));
    }

    void playSfx(const std::string& name) {
        auto it = sfx_.find(name);
        if (it == sfx_.end()) return;
        SfxEffect& e = it->second;
        // Round-robin voices so overlapping hits don't cut each other off.
        sf::Sound& v = *e.voices[e.next];
        v.stop();
        v.play();
        e.next = (e.next + 1) % voices_per_effect;
    }

    void setSfxVolume(float volume) {
        sfx_volume_ = volume;
        for (auto& [_, e] : sfx_) {
            for (auto& v : e.voices) v->setVolume(volume);
        }
    }

    void stopAllSfx() {
        for (auto& [_, e] : sfx_) {
            for (auto& v : e.voices) v->stop();
        }
    }
};

#endif // BREAKOUT_SOUND