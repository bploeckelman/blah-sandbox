#pragma once
#include <blah.h>
#include "collider.h"
#include "../world.h"

using namespace Blah;

namespace Zen {

    // technically PlayerController would be more accurate
    class Player : public Component {
    public:
        static constexpr int st_normal = 0;
        static constexpr int st_attack = 1;
        static constexpr int st_hurt   = 2;
        static constexpr int st_start  = 3;

        int health = 3;

        VirtualStick input_move;
        VirtualButton input_jump;
        VirtualButton input_attack;

        Player();

        void update() override;

    private:
        int m_state = st_start;
        int m_facing = 1;
        float m_jump_timer = 0;
        float m_attack_timer = 0;
        float m_hurt_timer = 0;
        float m_invincible_timer = 0;
        float m_start_timer = 0;
        float m_on_ground = false;
        Collider* m_attack_collider = nullptr;

    };

}
