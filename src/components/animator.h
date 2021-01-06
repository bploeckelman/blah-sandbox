#pragma once
#include <blah.h>
#include "../world.h"
#include "../assets/sprite.h"

using namespace Blah;

namespace Zen {
    class Animator : public Component {
    private:
        const Sprite* m_sprite = nullptr;
        int m_facing = 1;
        int m_animation_index = 0;
        int m_frame_index = 0;
        float m_frame_counter = 0;

    public:
        Animator() = default;
        Animator(const String& sprite);

        void play(const String& animation);

        void update() override;
        void render(Batch& batch) override;

    private:
        bool in_valid_state() const;
    };
}