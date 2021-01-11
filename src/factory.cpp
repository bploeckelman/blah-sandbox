#include "factory.h"
#include "masks.h"
#include "components/animator.h"
#include "components/collider.h"
#include "components/mover.h"
#include "components/player.h"
#include "components/hurtable.h"

using namespace Zen;

Entity* Factory::player(World* world, Point position) {
    auto en = world->add_entity(position);

    auto an = en->add(Animator("player"));
    an->play("idle");
    an->depth = -10;

    auto hitbox = en->add(Collider::make_rect(RectI(-4, -16, 8, 16)));

    auto mover = en->add(Mover());
    mover->collider = hitbox;

    en->add(Player());

    return en;
}

Entity* Factory::bramble(World* world, Point position) {
    auto en = world->add_entity(position);

    auto an = en->add(Animator("bramble"));
    an->play("idle");
    an->depth = -5;

    auto hitbox = en->add(Collider::make_rect(RectI(-4, -8, 8, 8)));
    hitbox->mask = Mask::enemy;

    auto hurtable = en->add(Hurtable());
    hurtable->hurt_by = Mask::player_attack;
    hurtable->collider = hitbox;
    hurtable->on_hurt = [](Hurtable* self) {
        self->entity()->destroy();
    };

    return en;
}
