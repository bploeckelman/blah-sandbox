#pragma once
#include <blah.h>
#include "../world.h"
#include "../assets/sprite.h"

using namespace Blah;

namespace Zen {
    class Animator : public Component {
    private:
        const Sprite* m_sprite = nullptr;
        int m_animation_index = 0;
        int m_frame_index = 0;
        float m_frame_counter = 0;

    public:
        Vec2 scale = Vec2::one;

        Animator() = default;
        Animator(const String& sprite);

        const Sprite* sprite() const;

        void play(const String& animation, bool restart = false);

        void update() override;
        void render(Batch& batch) override;

    private:
        bool in_valid_state() const;
    };
}