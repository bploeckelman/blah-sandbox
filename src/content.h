#pragma once
#include <blah.h>

#include "assets/sprite.h"

using namespace Blah;

namespace Zen {

    struct Sprite;

    class Content {
    public:
        static SpriteFont font;

        static FilePath path();
        static void load();
        static void unload();
        static TextureRef atlas();

        static const Sprite* find_sprite(const char* name);
    };

}
