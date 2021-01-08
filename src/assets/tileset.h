#pragma once
#include <blah.h>

using namespace Blah;

namespace Zen {

    struct Tileset {

        static constexpr int max_columns = 16;
        static constexpr int max_rows = 16;

        String name;
        int columns = 0;
        int rows = 0;
        Subtexture tiles[max_columns * max_rows];

        const Subtexture& random_tile() const;
    };

}
