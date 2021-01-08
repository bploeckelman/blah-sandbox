#include "game.h"
#include "masks.h"
#include "content.h"
#include "factory.h"
#include "components/collider.h"
#include "components/tilemap.h"

using namespace Zen;


void Game::load_room(Point cell) {
    const Image* grid = Content::find_room(cell);
    BLAH_ASSERT(grid != nullptr, "Room doesn't exist!");
    room = cell;

    // destroy all entities
    world.clear();

    // get the castle tileset for now
    auto castle = Content::find_tileset("castle");

    // add a floor
    auto floor = world.add_entity();
    auto tilemap = floor->add(Tilemap(8, 8, columns, rows));
    auto solids = floor->add(Collider::make_grid(8, 40, 23));
    solids->mask = Mask::solid;

    // loop over the room grid
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            Color col = grid->pixels[x + y * columns];
            uint32_t rgb = ((uint32_t)col.r << 16)
                         | ((uint32_t)col.g << 8)
                         | ((uint32_t)col.b << 0);
            switch (rgb) {
                // black does nothing
                case 0x000000: break;
                // white is solids
                case 0xffffff: {
                    tilemap->set_cell(x, y, &castle->random_tile());
                    solids->set_cell(x, y, true);
                } break;
                // green is player
                case 0x6abe30: {
                    Factory::player(&world, Point(x * tile_width + tile_width / 2, (y + 1) * tile_height));
                } break;
            }
        }
    }
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
    load_room(Point(0, 0));
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
        load_room(room);
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
