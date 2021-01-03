#include <blah.h>

#include "game.h"

using namespace Blah;
using namespace Zen;

namespace {

    const int window_width = 1280;
    const int window_height = 720;

    Game game;

    void startup() {
        game.startup();
    }

    void shutdown() {
        game.shutdown();
    }

    void update() {
        game.update();
    }

    void render() {
        game.render();
    }

}

// ----------------------------------------------------------------------------

int main() {
    Config config;
    config.name = "Blah Sandbox";
    config.width = window_width;
    config.height = window_height;

    config.on_startup = startup;
    config.on_shutdown = shutdown;
    config.on_update = update;
    config.on_render = render;

    App::run(&config);
}
