#pragma once
#include <blah.h>
#include "collider.h"
#include "../world.h"
#include <functional>

using namespace Blah;

namespace Zen {

    class Hurtable : public Component {
    private:
        float m_cooldown_timer = 0;

    public:
        Collider* collider = nullptr;
        uint32_t hurt_by = 0;
        float cooldown = 1.0f;
        std::function<void(Hurtable* self)> on_hurt;

        void update() override;
    };

}
