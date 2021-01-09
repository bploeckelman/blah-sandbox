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

            float duration() const {
                float d = 0;
                for (auto& it : frames) {
                    d += it.duration;
                }
                return d;
            }
        };

        String name;
        Vec2 origin;
        Vector<Animation> animations;

        const Animation* get_animation(const String& name) const;
    };

}
