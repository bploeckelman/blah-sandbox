#pragma once
#include <blah.h>

using namespace Blah;

namespace Zen {
    struct Sprite {
        struct Frame {
            Subtexture image;
            float duration;
        };

        struct Animation {
            String name;
            Vector<Frame> frames;
        };

        String name;
        Vec2 origin;
        Vector<Animation> animations;
    };
}
