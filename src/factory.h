#pragma once
#include <blah.h>
#include "world.h"

using namespace Blah;

namespace Zen {

    namespace Factory {
        Entity* player(World* world, Point position);
        Entity* bramble(World* world, Point position);
    }

}
