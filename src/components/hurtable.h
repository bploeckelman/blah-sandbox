#pragma once
#include <blah.h>
#include "collider.h"
#include "../world.h"
#include <functional>

using namespace Blah;

namespace Zen {

    class Hurtable : public Component {
    public:
        float stun_timer = 0;
        float flicker_timer = 0;
        Collider* collider = nullptr;
        uint32_t hurt_by = 0;
        std::function<void(Hurtable* self)> on_hurt;

        void update() override;
    };

}
