#include "sprite.h"

using namespace Zen;

const Sprite::Animation* Sprite::get_animation(const String& name) const {
    for (auto& it : animations) {
        if (it.name == name) {
            return &it;
        }
    }
    return nullptr;
}
