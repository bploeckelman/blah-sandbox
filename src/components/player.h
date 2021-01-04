#pragma once
#include <blah.h>
#include "../world.h"

using namespace Blah;

namespace Zen {

    // technically PlayerController would be more accurate
    class Player : public Component {
    public:
        VirtualButton btn_jump;

        Player();

        void update() override;

    private:

    };

}
