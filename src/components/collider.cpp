#include "collider.h"

using namespace Blah;
using namespace Zen;

Collider::Collider() {
    visible = false;
    active = false;
}

Collider Collider::make_rect(const RectI &rect) {
    Collider collider;
    collider.m_shape = Shape::Rect;
    collider.m_rect = rect;
    return collider;
}

Collider Collider::make_grid(int tile_size, int columns, int rows) {
    Collider collider;
    collider.m_shape = Shape::Grid;
    collider.m_grid.tile_size = tile_size;
    collider.m_grid.columns = columns;
    collider.m_grid.rows = rows;
    collider.m_grid.cells = std::shared_ptr<bool[]>(new bool[columns * rows]);
    return collider;
}

Collider::Shape Collider::shape() const {
    return m_shape;
}

RectI Collider::get_rect() const {
    BLAH_ASSERT(m_shape == Shape::Rect, "Collider is not a Rectangle!");
    return m_rect;
}

void Collider::set_rect(const RectI& value) {
    BLAH_ASSERT(m_shape == Shape::Rect, "Collider is not a Rectangle!");
    m_rect = value;
}

bool Collider::get_cell(int x, int y) const {
    BLAH_ASSERT(m_shape == Shape::Grid, "Collider is not a Grid!");
    BLAH_ASSERT(x >= 0 && y >= 0 && x < m_grid.columns && y < m_grid.rows, "Cell is out of bounds!");

    return m_grid.cells[x + y * m_grid.columns];
}

void Collider::set_cell(int x, int y, bool value) {
    BLAH_ASSERT(m_shape == Shape::Grid, "Collider is not a Grid!");
    BLAH_ASSERT(x >= 0 && y >= 0 && x < m_grid.columns && y < m_grid.rows, "Cell is out of bounds!");

    m_grid.cells[x + y * m_grid.columns] = value;
}

bool Collider::check(uint32_t mask, Point offset) const {
    auto other = world()->first<Collider>();
    while (other) {
        if (other != this
         && (other->mask & mask) == mask
         && overlaps(other, offset)) {
            return true;
        }

        other = (Collider*) other->next();
    }

    return false;
}

bool Collider::overlaps(const Collider *other, Point offset) const {
    if (m_shape == Shape::Rect) {
        if (other->m_shape == Shape::Rect) {
            return rect_to_rect(this, other, offset);
        } else if (other->m_shape == Shape::Grid) {
            return rect_to_grid(this, other, offset);
        }
    } else if (m_shape == Shape::Grid) {
        if (other->m_shape == Shape::Rect) {
            return rect_to_grid(other, this, offset);
        } else if (other->m_shape == Shape::Grid) {
            BLAH_ASSERT(false, "Grid->Grid overap checks not supported!");
        }
    }

    return false;
}

void Collider::render(Batch& batch) {
    static const Color color = Color::red;

    batch.push_matrix(Mat3x2::create_translation(entity()->position));

    if (m_shape == Shape::Rect) {
        batch.rect_line(m_rect, 1, color);
    } else if (m_shape == Shape::Grid) {
        for (int x = 0; x < m_grid.columns; x++) {
            for (int y = 0; y < m_grid.rows; y++) {
                if (!m_grid.cells[x + y * m_grid.columns])  continue;

                RectI rect = RectI(x * m_grid.tile_size, y * m_grid.tile_size, m_grid.tile_size, m_grid.tile_size);
                batch.rect_line(rect, 1, color);
            }
        }
    }

    batch.pop_matrix();
}

bool Collider::rect_to_rect(const Collider *a, const Collider *b, Point offset) {
    RectI ar = a->m_rect + a->entity()->position + offset;
    RectI br = b->m_rect + b->entity()->position;

    return ar.overlaps(br);
}

bool Collider::rect_to_grid(const Collider *a, const Collider *b, Point offset) {
    // TODO
    return false;
}
