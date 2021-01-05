#pragma once
#include <blah.h>

#include "world.h"

using namespace Blah;

namespace Zen {

    class Game {
    public:
        static constexpr int width = 240;
        static constexpr int height = 135;
        static constexpr int tile_width = 8;
        static constexpr int tile_height = 8;
        static constexpr int columns = width / tile_width;
        static constexpr int rows = height / tile_height + 1;

        World world;
        FrameBufferRef buffer;
        Batch batch;

        void load_map();
        void startup();
        void shutdown();
        void update();
        void render();

    private:
        bool m_draw_colliders;
    };

}
