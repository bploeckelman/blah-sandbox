#pragma once
#include <blah.h>
#include "collider.h"
#include "../world.h"
#include <functional>

using namespace Blah;

namespace Zen {

    class Mover : public Component {
    private:
        Vec2 m_remainder;

    public:
        Collider* collider = nullptr;
        Vec2 speed;
        float gravity = 0;
        float friction = 0;
        std::function<void(Mover*)> on_hit_x;
        std::function<void(Mover*)> on_hit_y;

        bool move_x(int amount);
        bool move_y(int amount);

        void stop_x();
        void stop_y();
        void stop();

        bool on_ground(int dist = 1) const;

        void update() override;

    };

}


