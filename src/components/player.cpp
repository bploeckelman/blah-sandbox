#include "player.h"
#include "mover.h"

using namespace Zen;

namespace {

    constexpr float max_ground_speed = 60;
    constexpr float max_air_speed = 70;
    constexpr float ground_accel = 500;
    constexpr float air_accel = 20;
    constexpr float friction = 800;
    constexpr float gravity = 450;
    constexpr float jump_force = -105;
    constexpr float jump_time = 0.18f;

}

Player::Player() {
    input_move = VirtualStick()
            .add_keys(Key::Left, Key::Right, Key::Up, Key::Down)
            .add_buttons(0, Button::Left, Button::Right, Button::Up, Button::Down)
            .add_axes(0, Axis::LeftX, Axis::LeftY, 0.2);

    input_jump = VirtualButton()
            .press_buffer(0.15f)
            .add_key(Key::X)
            .add_button(0, Button::A);
}
void Player::update() {
    input_move.update();
    input_jump.update();

    auto mover = get<Mover>();
    auto on_ground = mover->on_ground();
    int input = input_move.value_i().x;

    // horizontal movement
    {
        // acceleration
        mover->speed.x += input * (on_ground ? ground_accel : air_accel) * Time::delta;

        // max speed
        auto maxspd = (on_ground ? max_ground_speed : max_air_speed);
        if (Calc::abs(mover->speed.x) > maxspd) {
            mover->speed.x = Calc::approach(
                    mover->speed.x,
                    Calc::sign(mover->speed.x) * maxspd,
                    2000 * Time::delta);
        }

        // friction
        if (input == 0 && on_ground) {
            mover->speed.x = Calc::approach(mover->speed.x, 0, friction * Time::delta);
        }
    }

    // gravity
    if (!on_ground) {
        // make gravity more 'hovery' when in the air
        float grav = gravity;
        if (Calc::abs(mover->speed.y) < 20 && input_jump.down()) {
            grav *= 0.4f;
        }

        mover->speed.y += grav * Time::delta;
    }

    // jomping
    {
        // do the jomp
        if (input_jump.pressed() && on_ground) {
            // tweak horizontal movement when jumping
            mover->speed.x = input * max_air_speed;
            m_jump_timer = jump_time;
        }

        // variable jumping based on how long the button is held down
        if (m_jump_timer > 0) {
            m_jump_timer -= Time::delta;

            mover->speed.y = jump_force;

            if (!input_jump.down()) {
                m_jump_timer = 0;
            }
        }
    }
}
