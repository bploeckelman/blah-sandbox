#include "mover.h"
#include "../masks.h"

using namespace Zen;

bool Mover::move_x(int amount) {
    if (collider) {
        int sign = Calc::sign(amount);

        while (amount != 0) {
            if (collider->check(Mask::solid, Point(sign, 0))) {
                if (on_hit_x) {
                    on_hit_x(this);
                } else {
                    stop_x();
                }
                return true;
            }

            amount -= sign;
            entity()->position.x += sign;
        }
    } else {
        entity()->position.x += amount;
    }

    return false;
}

bool Mover::move_y(int amount) {
    if (collider) {
        int sign = Calc::sign(amount);

        while (amount != 0) {
            if (collider->check(Mask::solid, Point(0, sign))) {
                if (on_hit_y) {
                    on_hit_y(this);
                } else {
                    stop_y();
                }
                return true;
            }

            amount -= sign;
            entity()->position.y += sign;
        }
    } else {
        entity()->position.y += amount;
    }

    return false;
}

void Mover::stop_x() {
    speed.x = 0;
    m_remainder.x = 0;
}

void Mover::stop_y() {
    speed.y = 0;
    m_remainder.y = 0;
}

void Mover::stop() {
    speed.x = 0;
    speed.y = 0;
    m_remainder.x = 0;
    m_remainder.y = 0;
}

bool Mover::on_ground(int dist) const{
    if (!collider) {
        return false;
    }

    return collider->check(Mask::solid, Point(0, dist));
}

void Mover::update() {
    // apply gravity
    if (gravity != 0 && (!collider || !collider->check(Mask::solid, Point(0, 1)))) {
        speed.y += gravity * Time::delta;
    }

    // get the amount we should move, including remainder from previous frame
    Vec2 total = m_remainder + speed * Time::delta;

    // round to integer values since we only move in pixels at a time
    Point to_move = Point((int) total.x, (int) total.y);

    // store the remainder floating values
    m_remainder.x = total.x - to_move.x;
    m_remainder.y = total.y - to_move.y;

    // move by integer values
    move_x(to_move.x);
    move_y(to_move.y);
}

