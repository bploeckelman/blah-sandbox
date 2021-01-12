#include "player.h"
#include "mover.h"
#include "animator.h"
#include "../masks.h"

using namespace Zen;

namespace {

    constexpr float max_ground_speed = 60;
    constexpr float max_air_speed = 70;
    constexpr float ground_accel = 500;
    constexpr float air_accel = 20;
    constexpr float friction = 800;
    constexpr float hurt_friction = 200;
    constexpr float gravity = 450;
    constexpr float jump_force = -105;
    constexpr float jump_time = 0.18f;
    constexpr float hurt_duration = 0.5f;
    constexpr float invincible_duration = 1.5f;

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
    auto hitbox = get<Collider>();
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
            m_attack_timer = 0;

            if (!m_attack_collider) {
                m_attack_collider = entity()->add(Collider::make_rect(RectI()));
                m_attack_collider->mask = Mask::player_attack;
            }

            if (m_on_ground) {
                mover->stop_x();
            }
        }
    }
    // ATTACK STATE
    else if (m_state == st_attack) {
        anim->play("attack");
        m_attack_timer += Time::delta;

        // setup hitbox (depends on where in the attack animation we currently are)
        // assumes right facing, if left facing, it flips the hitbox afterwards
        // NOTE: timer threshold values are hardcoded here, they come from frame durations of each anim frame
        // NOTE: rects are built based with x,y being offsets from pivot point specified in aseprite
        if (m_attack_timer < 0.05f) {
            m_attack_collider->set_rect(RectI(-11, -17, 7, 7));
        }
        else if (m_attack_timer < 0.1f) {
            m_attack_collider->set_rect(RectI(-13, -19, 9, 9));
        }
        else if (m_attack_timer < 0.12f) {
            m_attack_collider->set_rect(RectI(-13, -19, 8, 8));
        }
        else if (m_attack_timer < 0.14f) {
            m_attack_collider->set_rect(RectI(-4, -15, 8, 8));
        }
        else if (m_attack_timer < 0.49f) {
            m_attack_collider->set_rect(RectI(-1, -13, 18, 10));
        }
        else if (m_attack_timer < 0.54f) {
            m_attack_collider->set_rect(RectI(-1, -13, 15, 10));
        }
        // done with attack anim, destroy attack collider
        else if (m_attack_collider) {
            m_attack_collider->destroy();
            m_attack_collider = nullptr;
        }

        // flip hitbox if you're facing left
        if (m_facing < 0 && m_attack_collider) {
            auto rect = m_attack_collider->get_rect();
            rect.x = -(rect.x + rect.w);
            m_attack_collider->set_rect(rect);
        }

        // end the attack
        if (m_attack_timer >= anim->animation()->duration()) {
            anim->play("idle");
            m_state = st_normal;
        }
    }
    // HURT STATE
    else if (m_state == st_hurt) {
        m_hurt_timer -= Time::delta;
        if (m_hurt_timer <= 0) {
            m_state = st_normal;
        }

        // friction
        if (m_on_ground) {
            mover->speed.x = Calc::approach(mover->speed.x, 0, hurt_friction * Time::delta);
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

    // invincible timer
    if (m_state != st_hurt && m_invincible_timer > 0) {
        // flicker animation
        if (Time::on_interval(0.05f)) {
            anim->visible = !anim->visible;
        }

        m_invincible_timer -= Time::delta;
        if (m_invincible_timer <= 0) {
            anim->visible = true;
        }
    }

    // gravity
    if (!m_on_ground) {
        // make gravity more 'hovery' when in the air
        float grav = gravity;
        if (m_state == st_normal && Calc::abs(mover->speed.y) < 20 && input_jump.down()) {
            grav *= 0.4f;
        }

        mover->speed.y += grav * Time::delta;
    }

    // hurt check (could be done with hurtable component)
    if (m_invincible_timer <= 0 && hitbox->check(Mask::enemy)) {
        Time::pause_for(0.1f);
        anim->play("hurt");

        if (m_attack_collider) {
            m_attack_collider->destroy();
            m_attack_collider = nullptr;
        }

        // for now bounce back is always the reverse direction player is facing
        mover->speed = Vec2(-m_facing * 100, -80);

        health -= 1;
        m_hurt_timer = hurt_duration;
        m_invincible_timer = invincible_duration;
        m_state = st_hurt;
    }
}

