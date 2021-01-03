#pragma once
#include <blah.h>
#include "collider.h"
#include "../world.h"

using namespace Blah;

namespace Zen {

    class Mover : public Component {
    private:
        Vec2 m_remainder;

    public:
        Collider* collider;
        Vec2 speed;

        bool move_x(int amount);
        bool move_y(int amount);

        void stop_x();
        void stop_y();
        void stop();

        void update() override;

    };

}


