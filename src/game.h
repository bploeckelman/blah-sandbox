#pragma once
#include <blah.h>

using namespace Blah;

namespace Zen {

    class Game {
    public:
//        World world;
        FrameBufferRef buffer;
//        Batch batch;

        void startup();
        void shutdown();
        void update();
        void render();
    };

}
