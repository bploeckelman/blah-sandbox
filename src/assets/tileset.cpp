#include "tileset.h"

using namespace Zen;

const Subtexture& Tileset::random_tile() const {
    int i = Calc::rand_int(columns * rows);
    return tiles[i];
}
