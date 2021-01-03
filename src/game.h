#pragma once
#include <blah.h>

#include "world.h"

using namespace Blah;

namespace Zen {

    class Game {
    public:
        World world;
        FrameBufferRef buffer;
        Batch batch;

        void startup();
        void shutdown();
        void update();
        void render();
    private:
        void load_map();
    };

}
