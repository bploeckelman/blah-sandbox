#include "game.h"
#include "masks.h"
#include "content.h"
#include "components/animator.h"
#include "components/collider.h"
#include "components/mover.h"

using namespace Zen;


void Game::load_map() {
    world.clear();

    // add a test entity
    auto en = world.add_entity(Point(100, 60));
    auto an = en->add(Animator("player"));
    auto col = en->add(Collider::make_rect(RectI(-4, -8, 8, 8)));
    auto mover = en->add(Mover());

    mover->collider = col;
    mover->speed = Vec2(5, 20);
    an->play("idle");

    auto floor = world.add_entity(Point(50, 100));
    auto c2 = floor->add(Collider::make_rect(RectI(0, 0, 100, 16)));
    c2->mask = Mask::solid;

    m_draw_colliders = true;
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

    auto en = world.first_entity();
    if (!en->get<Collider>()->check(Mask::solid, Point(0, 1))) {
        en->position.y += 1;
    }

    world.update();
}

void Game::render() {
    // draw gameplay stuff
    {
        buffer->clear(0x4488aa);

        world.render(batch);

        if (m_draw_colliders) {
            auto collider = world.first<Collider>();
            while (collider) {
                collider->render(batch);
                collider = (Collider *) collider->next();
            }
        }

        // test some stuffffff
        batch.tex(Content::atlas(), Vec2::zero, Color::white);
//        batch.str(Content::font, "Hello World", Vec2(32, 160), Color::white);

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
