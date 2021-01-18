#include "game.h"
#include "masks.h"
#include "content.h"
#include "factory.h"
#include "components/player.h"
#include "components/tilemap.h"
#include "components/mover.h"

using namespace Zen;

namespace {
    constexpr float transition_duration = 0.5f;
}

void Game::load_room(Point cell, bool is_reload) {
    const Image* grid = Content::find_room(cell);
    BLAH_ASSERT(grid != nullptr, "Room doesn't exist!");
    room = cell;

    // get room offset
    auto offset = Point(cell.x * width, cell.y * height);

    // get the tilesets
    auto castle = Content::find_tileset("castle");
    auto grass = Content::find_tileset("grass");
    auto plants = Content::find_tileset("plants");
    auto back = Content::find_tileset("back");
    auto jumpthru = Content::find_tileset("jumpthru");

    // add a floor
    auto floor = world.add_entity(offset);
    auto tilemap = floor->add(Tilemap(8, 8, columns, rows));
    auto solids = floor->add(Collider::make_grid(8, 40, 23));
    solids->mask = Mask::solid;

    // loop over the room grid
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            Point world_position = offset + Point(x * tile_width, y * tile_height) + Point(tile_width / 2, tile_height);
            Color col = grid->pixels[x + y * columns];
            uint32_t rgb = ((uint32_t)col.r << 16)
                         | ((uint32_t)col.g << 8)
                         | ((uint32_t)col.b << 0);
            switch (rgb) {
                // black does nothing
                case 0x000000: default: break;

                // castle is white
                case 0xffffff: {
                    tilemap->set_cell(x, y, &castle->random_tile());
                    solids->set_cell(x, y, true);
                } break;

                // background is purpleish
                case 0x45283c: {
                    tilemap->set_cell(x, y, &back->random_tile());
                } break;

                // orange is jumpthru platforms
                case 0xdf7126: {
                    tilemap->set_cell(x, y, &jumpthru->random_tile());
                    auto jumpthru_en = world.add_entity(offset + Point(x * tile_width, y * tile_height));
                    auto jumpthru_col = jumpthru_en->add(Collider::make_rect(RectI(0, 0, 8, 4)));
                    jumpthru_col->mask = Mask::jumpthru;
                } break;

                // grass is pale green
                case 0x8f974a: {
                    tilemap->set_cell(x, y, &grass->random_tile());
                    solids->set_cell(x, y, true);
                } break;

                // plants (not solid) are dark green
                case 0x4b692f: {
                    tilemap->set_cell(x, y, &plants->random_tile());
                } break;

                // doors are grayish
                case 0xcbdbfc: {
                    Factory::door(&world, world_position);
                } break;

                // player is green (only create if it doesn't already exist)
                case 0x6abe30: {
                    if (!world.first<Player>()) {
                        // TODO: this is janked if is_reload and player starts in a tight area
//                        Factory::player(&world, world_position + (is_reload ? Point(0, -16) : Point::zero));
                        Factory::player(&world, world_position);
                    }
                } break;

                // bramble is pink
                case 0xd77bba: {
                    Factory::bramble(&world, world_position);
                } break;

                // spitter is red
                case 0xac3232: {
                    Factory::spitter(&world, world_position);
                } break;

                // mosquito is yellow
                case 0xfbf236: {
                    Factory::mosquito(&world, world_position - Point(0, tile_height / 2));
                } break;

                // blob is dark blue
                case 0x3f3f74: {
                    Factory::blob(&world, world_position);
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
    m_frame_by_frame = false;

    // camera setup
    load_room(Point(0, 0));
    camera = Vec2(room.x * width, room.y * height);
}

void Game::shutdown() {
    // unload assets
    Content::unload();
}

void Game::update() {
    // quick exit
    if (Input::pressed(Key::Escape)) {
        App::exit();
    }

    // toggle collider render
    if (Input::pressed(Key::F1)) {
        m_draw_colliders = !m_draw_colliders;
    }

    // if flag is enabled, press F12 to progress a frame at a time
    if (m_frame_by_frame && !Input::pressed(Key::F12)) {
        return;
    }

    // reload current room
    if (Input::pressed(Key::F2)) {
        // not transitioning
        m_transition = false;
        // destroy all entities
        world.clear();
        // reload room
        load_room(room);
    }

    // normal update
    if (!m_transition) {
        world.update();

        auto player = world.first<Player>();
        if (player) {
            auto pos = player->entity()->position;
            auto bounds = RectI(room.x * width, room.y * height, width, height);
            if (!bounds.contains(pos)) {
                // target room
                Point next_room = Point(pos.x / width, pos.y / height);
                if (pos.x < 0) next_room.x--;
                if (pos.y < 0) next_room.y--;

                // see if room exists and player isn't dead
                auto player_is_alive = player->health > 0;
                auto next_room_exists = Content::find_room(next_room);
                auto next_room_is_forward = next_room.x >= room.x;
                if (player_is_alive && next_room_exists && next_room_is_forward) {
                    Time::pause_for(0.1f);

                    // transition to next room
                    m_transition = true;
                    m_next_ease = 0;
                    m_next_room = next_room;
                    m_last_room = room;

                    // store entities from the previous room
                    m_last_entities.clear();
                    Entity* e = world.first_entity();
                    while (e) {
                        m_last_entities.push_back(e);
                        e = e->next();
                    }

                    // load contents of the next room
                    load_room(next_room);
                } else {
                    // no next room, keep player in this room
                    player->entity()->position = Point(
                            Calc::clamp_int(pos.x, bounds.x, bounds.x + bounds.w),
                            Calc::clamp_int(pos.y, bounds.y, bounds.y + bounds.h + 100));

                    // reload if they fell out the bottom
                    if (player->entity()->position.y > bounds.y + bounds.h + 64) {
                        world.clear();
                        load_room(room, true);
                    }
                }
            }

            // death ... delete everything except the player
            // then the player falls out of the room causing the room to reload
            if (player->health <= 0) {
                auto e = world.first_entity();
                while (e) {
                    auto next = e->next();
                    if (!e->get<Player>()) {
                        world.destroy_entity(e);
                    }
                    e = next;
                }
            }
        }
    }
    // room transition
    else {
        // increment ease
        m_next_ease = Calc::approach(m_next_ease, 1.0f, Time::delta / transition_duration);

        // get last & next camera positions
        auto last_cam = Vec2(m_last_room.x * width, m_last_room.y * height);
        auto next_cam = Vec2(m_next_room.x * width, m_next_room.y * height);

        // lerp camera position for room transition
        camera = last_cam + (next_cam - last_cam) * Ease::cube_in_out(m_next_ease);

        // finish transition
        if (m_next_ease >= 1.0f) {
            // boost player up if moving to a higher room
            if (m_next_room.y < m_last_room.y) {
                auto player = world.first<Player>();
                if (player) {
                    player->get<Mover>()->speed = Vec2(0, -150);
                }
            }

            // delete old objects (except player)
            for (auto& it : m_last_entities) {
                if (it->get<Player>()) continue;
                world.destroy_entity(it);
            }

            Time::pause_for(0.1f);
            m_transition = false;
        }
    }
}

void Game::render() {
    // draw gameplay stuff
    {
        buffer->clear(0x150e22);

        // push camera offset
        batch.push_matrix(Mat3x2::create_translation(-camera));
        {
            // draw gameplay objects
            world.render(batch);

            // draw debug colliders
            if (m_draw_colliders) {
                auto collider = world.first<Collider>();
                while (collider) {
                    collider->render(batch);
                    collider = (Collider *) collider->next();
                }
            }
        }
        // end camera offset
        batch.pop_matrix();

        // hacky start / end screen text
        if (room == Point(0, 0)) {
            auto w = Content::font.width_of(title);
            auto pos = Point((width - w) / 2, 20);
            batch.str(Content::font, title, pos + Point(0, 1), Color::black);
            batch.str(Content::font, title, pos, Color::white);

            w = Content::font.width_of(controls);
            pos = Point((width - w) / 2, 40);
            batch.str(Content::font, controls, pos, Color::white * 0.25f);
        }

        // draw health
        auto player = world.first<Player>();
        if (player) {
            auto heart = Content::find_sprite("heart");
            auto full = heart->get_animation("full");
            auto empty = heart->get_animation("empty");

            auto pos = Point(0, height - 16);
            batch.rect(Rect(pos.x, pos.y + 7, 40, 4), Color::black);

            for (int i = 0; i < Player::max_health; i++) {
                if (player->health >= i + 1) {
                    batch.tex(full->frames[0].image, pos, Color::red);
                } else {
                    batch.tex(empty->frames[0].image, pos);
                }
                pos.x += 12;
            }
        }

        // draw to gameplay buffer
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
