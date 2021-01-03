#include "game.h"

using namespace Zen;

void Game::startup() {
    // TODO: load assets

    // create framebuffer for the game
    buffer = FrameBuffer::create(320, 180);

    // set batch to use nearest filtering
    batch.default_sampler = TextureSampler(TextureFilter::Nearest);

    // TODO: set flags (eg. draw colliders)

    load_map();
}

void Game::load_map() {
    world.clear();

    // TODO: add a test player
}

void Game::shutdown() {

}

void Game::update() {
    if (Input::pressed(Key::Escape)) {
        App::exit();
    }

    // TODO: toggle flag
//    if (Input::pressed(Key::F1)) {
//        draw_colliders = !draw_colliders;
//    }

    if (Input::pressed(Key::F2)) {
        load_map();
    }

    world.update();
}

void Game::render() {
    // draw gameplay stuff
    {
        buffer->clear(0x4488aa);

        world.render(batch);

//        if (draw_colliders) {
//            auto collider = m_world.first<Collider>();
//            while (collider) {
//                collider->render(batch);
//                collider = (Collider *) collider->m_next();
//            }
//        }
    }

    // draw buffer to the screen
    {
        float scale = Calc::min(
                App::backbuffer->width() / (float) buffer->width(),
                App::backbuffer->height() / (float) buffer->height()
        );

        Vec2 screen_center = Vec2(App::backbuffer->width(), App::backbuffer->height()) / 2;
        Vec2 buffer_center = Vec2(buffer->width(), buffer->height()) / 2;

        App::backbuffer->clear(Color::black);
        batch.push_matrix(Mat3x2::create_transform(screen_center, buffer_center, Vec2::one * scale, 0));
        batch.tex(buffer->attachment(0), Vec2::zero, Color::white);
        batch.pop_matrix();
        batch.render(App::backbuffer);
        batch.clear();
    }
}
