#include "factory.h"
#include "masks.h"
#include "components/animator.h"
#include "components/collider.h"
#include "components/mover.h"
#include "components/player.h"
#include "components/enemy.h"
#include "components/hurtable.h"
#include "components/timer.h"

using namespace Zen;

Entity* Factory::player(World* world, Point position) {
    auto en = world->add_entity(position);

    auto anim = en->add(Animator("player"));
    anim->play("idle");
    anim->depth = -10;

    auto hitbox = en->add(Collider::make_rect(RectI(-4, -14, 8, 14)));

    auto mover = en->add(Mover());
    mover->collider = hitbox;

    en->add(Player());

    return en;
}

Entity* Factory::bramble(World* world, Point position) {
    auto en = world->add_entity(position);

    auto anim = en->add(Animator("bramble"));
    anim->play("idle");
    anim->depth = -5;

    auto hitbox = en->add(Collider::make_rect(RectI(-4, -8, 8, 8)));
    hitbox->mask = Mask::enemy;

    auto hurtable = en->add(Hurtable());
    hurtable->hurt_by = Mask::player_attack;
    hurtable->collider = hitbox;
    hurtable->on_hurt = [](Hurtable* self) {
        Time::pause_for(0.1f);
        pop(self->world(), self->entity()->position + Point(0, -4));
        self->entity()->destroy();
    };

    return en;
}

Entity* Factory::pop(World* world, Point position) {
    auto en = world->add_entity(position);

    auto anim = en->add(Animator("pop"));
    anim->play("pop");
    anim->depth = -20;

    auto timer = en->add(Timer(anim->animation()->duration(), [](Timer* self) {
        self->entity()->destroy();
    }));

    return en;
}

Entity* Factory::spitter(World* world, Point position) {
    auto en = world->add_entity(position);
    en->add(Enemy());

    auto anim = en->add(Animator("spitter"));
    anim->play("idle");
    anim->depth = -5;

    auto hitbox = en->add(Collider::make_rect(RectI(-8, -12, 13, 12)));
    hitbox->mask = Mask::enemy;

    auto hurtable = en->add(Hurtable());
    hurtable->hurt_by = Mask::player_attack;
    hurtable->collider = hitbox;
    hurtable->on_hurt = [health = 1](Hurtable* self) mutable {
        health--;
        self->flicker_timer = 1;
        if (health <= 0) {
            Time::pause_for(0.1f);
            pop(self->world(), self->entity()->position + Point(0, -4));
            self->entity()->destroy();
        }
    };

    en->add(Timer(1.0f, [](Timer* self) {
        bullet(self->world(), self->entity()->position + Point(-8, -8), -1);

        self->get<Animator>()->play("shoot");
        self->entity()->add(Timer(0.4f, [](Timer* self) { self->get<Animator>()->play("idle"); }));
        self->start(3.0f);
    }));

    return en;
}

Entity* Factory::bullet(World* world, Point position, int direction) {
    auto en = world->add_entity(position);

    auto anim = en->add(Animator("bullet"));
    anim->play("idle");
    anim->depth = -5;

    auto hitbox = en->add(Collider::make_rect(RectI(-4, -4, 8, 8)));
    hitbox->mask = Mask::enemy;

    auto mover = en->add(Mover());
    mover->collider = hitbox;
    mover->speed = Vec2(direction * 40, 0);
    mover->gravity = 130;
    mover->on_hit_x = [](Mover* self) { self->destroy(); };
    mover->on_hit_y = [](Mover *self) {
        self->speed.y = -60;
    };

    auto hurtable = en->add(Hurtable());
    hurtable->hurt_by = Mask::player_attack;
    hurtable->collider = hitbox;
    hurtable->on_hurt = [](Hurtable* self) {
        Time::pause_for(0.1f);
        pop(self->world(), self->entity()->position + Point(0, -4));
        self->entity()->destroy();
    };

    // start warning flicker
    en->add(Timer(2.5f, [](Timer* self) {
        self->get<Hurtable>()->flicker_timer = 100;
    }));

    // suicide
    en->add(Timer(3.0f, [](Timer* self) {
        self->entity()->destroy();
    }));

    return en;
}

namespace {

    class MosquitoBehavior : public Component {
    public:
        int health = 2;
        float timer = 0;

        void update() override {
            auto mover = get<Mover>();
            auto player = world()->first<Player>();
            if (player) {
                auto diff = player->entity()->position.x - entity()->position.x;
                auto dist = Calc::abs(diff);

                if (dist < 100) {
                    mover->speed.x += Calc::sign(diff) * 100 * Time::delta;
                } else {
                    mover->speed.x = Calc::approach(mover->speed.x, 0, 100 * Time::delta);
                }

                if (Calc::abs(mover->speed.x) > 50) {
                    mover->speed.x = Calc::approach(mover->speed.x, Calc::sign(mover->speed.x) * 50, 800 * Time::delta);
                }

                mover->speed.y = Calc::sin(timer * 4) * 10;
            }

            timer += Time::delta * 4;
        }

        void hurt() {
            health--;
            if (health <= 0) {
                Factory::pop(world(), entity()->position);
                entity()->destroy();
            } else {
                auto mover = get<Mover>();
                auto player = world()->first<Player>();
                auto sign = Calc::sign(player->entity()->position.x - entity()->position.x);
                mover->speed.x = -sign * 140;
            }
        }
    };

}

Entity *Factory::mosquito(World *world, Point position) {
    auto en = world->add_entity(position);
    en->add(MosquitoBehavior());
    en->add(Enemy());
    en->add(Mover());

    auto anim = en->add(Animator("mosquito"));
    anim->play("idle");
    anim->depth = -5;

    auto hitbox = en->add(Collider::make_rect(RectI(-7, -4, 15, 8)));
    hitbox->mask = Mask::enemy;

    auto hurtable = en->add(Hurtable());
    hurtable->hurt_by = Mask::player_attack;
    hurtable->collider = hitbox;
    hurtable->on_hurt = [](Hurtable *self) { self->get<MosquitoBehavior>()->hurt(); };

    return en;
}

Entity *Factory::door(World *world, Point position) {
    auto en = world->add_entity(position);

    auto anim = en->add(Animator("door"));
    anim->play("idle");
    anim->depth = -1;

    auto hitbox = en->add(Collider::make_rect(RectI(-6, -16, 12, 16)));
    hitbox->mask = Mask::solid;

    // check if all enemies are dead
    en->add(Timer(0.25f, [](Timer* self) {
        if (!self->world()->first<Enemy>()) {
            Factory::pop(self->world(), self->entity()->position + Point(0, -8));
            self->entity()->destroy();
        } else {
            self->start(0.25f);
        }
    }));

    return en;
}

Entity* Factory::blob(World* world, Point position) {
    auto en = world->add_entity(position);
    en->add(Enemy());

    auto anim = en->add(Animator("blob"));
    anim->play("idle");
    anim->depth = -5;

    auto hitbox = en->add(Collider::make_rect(RectI(-7, -13, 14, 13)));
    hitbox->mask = Mask::enemy;

    auto mover = en->add(Mover());
    mover->collider = hitbox;
    mover->gravity = 300;
    mover->friction = 400;
    mover->on_hit_y = [](Mover* self) {
        self->get<Animator>()->play("idle");
        self->stop_y();
    };

    // jump timer
    en->add(Timer(2.0f, [](Timer* self) {
        auto mover = self->get<Mover>();
        if (!mover->on_ground()) {
            self->start(0.05f);
        } else {
            self->start(2.0f);

            self->get<Animator>()->play("jump");
            mover->speed.y = -90;

            auto player = self->world()->first<Player>();
            if (player) {
                auto dir = Calc::sign(player->entity()->position.x - self->entity()->position.x);
                if (dir == 0) {
                    dir = 1;
                }
                self->get<Animator>()->scale = Vec2(dir, 1);

                mover->speed.x = dir * 40;
            }
        }
    }));

    auto hurtable = en->add(Hurtable());
    hurtable->hurt_by = Mask::player_attack;
    hurtable->collider = hitbox;
    hurtable->on_hurt = [health = 3](Hurtable *self) mutable {
        auto player = self->world()->first<Player>();
        if (player) {
            auto mover = self->get<Mover>();
            mover->speed.x = Calc::sign(self->entity()->position.x - player->entity()->position.x) * 80;
        }

        health--;
        if (health <= 0) {
            Factory::pop(self->world(), self->entity()->position + Point(0, -4));
            self->entity()->destroy();
        }
    };

    return en;
}
