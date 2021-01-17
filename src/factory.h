#pragma once
#include <blah.h>
#include "world.h"

using namespace Blah;

namespace Zen {

    namespace Factory {
        Entity* player(World* world, Point position);
        Entity* bramble(World* world, Point position);
        Entity* pop(World* world, Point position);
        Entity* spitter(World* world, Point position);
        Entity* bullet(World* world, Point position, int direction);
        Entity* mosquito(World* world, Point position);
        Entity* door(World* world, Point position);
    }

}
