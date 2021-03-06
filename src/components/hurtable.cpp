#include "hurtable.h"

using namespace Zen;

void Hurtable::update() {
    if (collider && on_hurt && stun_timer <= 0) {
        if (collider->check(hurt_by)) {
            Time::pause_for(0.1f);
            stun_timer = 0.5f;
            flicker_timer = 0.5f;
            on_hurt(this);
        }
    }

    stun_timer -= Time::delta;

    if (flicker_timer > 0) {
        flicker_timer -= Time::delta;

        if (Time::on_interval(0.05f)) {
            entity()->visible = !entity()->visible;
        }
        if (flicker_timer <= 0) {
            entity()->visible = true;
        }
    }
}
