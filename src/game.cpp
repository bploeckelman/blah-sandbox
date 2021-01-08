#include "game.h"
#include "masks.h"
#include "content.h"
#include "factory.h"
#include "components/collider.h"
#include "components/tilemap.h"

using namespace Zen;


void Game::load_map() {
    world.clear();

    // add a player
    Factory::player(&world, Point(width / 2, height - 32));

    // get the castle tileset for now
    auto castle = Content::find_tileset("castle");

    // add a floor
    auto floor = world.add_entity();
    auto tm = floor->add(Tilemap(8, 8, columns, rows));
    tm->set_cells(0, rows - 3, columns, 3, &castle->tiles[0]);
    tm->set_cells(0, rows - 5, 8, 2, &castle->tiles[0]);

    auto c2 = floor->add(Collider::make_grid(8, columns, rows));
    c2->set_cells(0, rows - 3, columns, 3, true);
    c2->set_cells(0, rows - 5, 8, 2, true);
    c2->mask = Mask::solid;
}

void Game::startup() {
    // load assets
    Content::load();

    // create framebuffer for the game
    buffer = FrameBuffer::create(width, height);

    // set batch to use nearest filtering
    batch.default_sampler = TextureSampler(TextureFilter::Nearest);

    // set flags
    m_draw_colliders = false;

    // load the world
    load_map();
}

void Game::shutdown() {
    // unload assets
    Content::unload();
}

void Game::update() {
    if (Input::pressed(Key::Escape)) {
        App::exit();
    }

    if (Input::pressed(Key::F1)) {
        m_draw_colliders = !m_draw_colliders;
    }

    if (Input::pressed(Key::F2)) {
        load_map();
    }

    world.update();
}

void Game::render() {
    // draw gameplay stuff
    {
        buffer->clear(0x150e22);

        world.render(batch);

        if (m_draw_colliders) {
            auto collider = world.first<Collider>();
            while (collider) {
                collider->render(batch);
                collider = (Collider *) collider->next();
            }
        }

        batch.render(buffer);
        batch.clear();
    }

    // draw buffer to the screen
    {
        float scale = Calc::min(
                App::backbuffer->width()  / (float) buffer->width(),
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
