#include "player.h"
#include "mover.h"
#include "animator.h"

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
            .add_key(Key::Space)
            .add_button(0, Button::A);

    input_attack = VirtualButton()
            .press_buffer(0.15f)
            .add_key(Key::Z)
            .add_button(0, Button::X);
}
void Player::update() {
    input_move.update();
    input_jump.update();
    input_attack.update();

    auto mover = get<Mover>();
    auto anim = get<Animator>();
    auto was_on_ground = m_on_ground;
    m_on_ground = mover->on_ground();
    int input = input_move.value_i().x;

    // sprite stuff
    {
        // land squish
        if (!was_on_ground && m_on_ground) {
            anim->scale = Vec2(m_facing * 1.5f, 0.7f);
        }

        // lerp scale back to one
        anim->scale = Calc::approach(anim->scale, Vec2(m_facing, 1.0f), Time::delta * 4);

        // set facing
        anim->scale.x = Calc::abs(anim->scale.x) * m_facing;
    }

    // NORMAL STATE
    if (m_state == st_normal) {
        // current animation
        if (m_on_ground) {
            if (input != 0) {
                anim->play("run");
            } else {
                anim->play("idle");
            }
        } else {
            anim->play("jump");
        }

        // horizontal movement
        {
            // acceleration
            mover->speed.x += input * (m_on_ground ? ground_accel : air_accel) * Time::delta;

            // max speed
            auto maxspd = (m_on_ground ? max_ground_speed : max_air_speed);
            if (Calc::abs(mover->speed.x) > maxspd) {
                mover->speed.x = Calc::approach(
                        mover->speed.x,
                        Calc::sign(mover->speed.x) * maxspd,
                        2000 * Time::delta);
            }

            // friction
            if (input == 0 && m_on_ground) {
                mover->speed.x = Calc::approach(mover->speed.x, 0, friction * Time::delta);
            }

            // facing direction
            if (input != 0 && m_on_ground) {
                m_facing = input;
            }
        }

        // invoke jumping
        {
            // do the jump
            if (input_jump.pressed() && m_on_ground) {
                input_jump.clear_press_buffer();

                // squoosh on jomp
                anim->scale = Vec2(m_facing * 0.65f, 1.4f);

                // tweak horizontal movement when jumping
                mover->speed.x = input * max_air_speed;
                m_jump_timer = jump_time;
            }
        }

        // begin attacking
        if (input_attack.pressed()) {
            input_attack.clear_press_buffer();

            m_state = st_attack;
            m_attack_timer = anim->sprite()->get_animation("attack")->duration();

            if (m_on_ground) {
                mover->stop_x();
            }
        }
    }
    // ATTACK STATE
    else if (m_state == st_attack) {
        anim->play("attack");
        m_attack_timer -= Time::delta;

        if (m_attack_timer <= 0) {
            anim->play("idle");
            m_state = st_normal;
        }
    }

    // variable jumping based on how long the button is held down
    if (m_jump_timer > 0) {
        m_jump_timer -= Time::delta;

        mover->speed.y = jump_force;

        if (!input_jump.down()) {
            m_jump_timer = 0;
        }
    }

    // gravity
    if (!m_on_ground) {
        // make gravity more 'hovery' when in the air
        float grav = gravity;
        if (Calc::abs(mover->speed.y) < 20 && input_jump.down()) {
            grav *= 0.4f;
        }

        mover->speed.y += grav * Time::delta;
    }
}

