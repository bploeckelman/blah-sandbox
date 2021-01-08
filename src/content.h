#pragma once
#include <blah.h>

#include "assets/sprite.h"
#include "assets/tileset.h"

using namespace Blah;

namespace Zen {

    struct Sprite;
    struct Tileset;

    class Content {
    public:
        static SpriteFont font;

        static FilePath path();
        static void load();
        static void unload();
        static TextureRef atlas();

        static const Sprite* find_sprite(const char* name);
        static const Tileset* find_tileset(const char* name);
        static const Image* find_room(const Point& cell);
    };

}
