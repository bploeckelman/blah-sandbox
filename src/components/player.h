#pragma once
#include <blah.h>
#include "../world.h"

using namespace Blah;

namespace Zen {

    // technically PlayerController would be more accurate
    class Player : public Component {
    public:
        VirtualStick input_move;
        VirtualButton input_jump;

        Player();

        void update() override;

    private:
        int m_facing = 1;
        float m_jump_timer = 0;
        float m_on_ground = false;

    };

}
