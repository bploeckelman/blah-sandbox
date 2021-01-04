#include "player.h"
#include "mover.h"

using namespace Zen;

Player::Player() {
    btn_jump = VirtualButton()
            .press_buffer(0.15f)
            .add_key(Key::X)
            .add_button(0, Button::A);
}
void Player::update() {
    auto mover = get<Mover>();

    btn_jump.update();
    if (btn_jump.pressed() && mover->on_ground()) {
        // perform jomp
        mover->speed.y = -100;
    }
}

