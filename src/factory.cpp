#include "factory.h"
#include "components/animator.h"
#include "components/collider.h"
#include "components/mover.h"
#include "components/player.h"

using namespace Zen;

Entity* Factory::player(World* world, Point position) {
    auto en = world->add_entity(position);

    auto an = en->add(Animator("player"));
    an->play("idle");
    an->depth = -10;

    auto hitbox = en->add(Collider::make_rect(RectI(-4, -8, 8, 8)));

    auto mover = en->add(Mover());
    mover->collider = hitbox;

    en->add(Player());

    return en;
}
