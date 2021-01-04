#include "tilemap.h"

using namespace Zen;

Tilemap::Tilemap() {

}

Tilemap::Tilemap(int tile_width, int tile_height, int columns, int rows) {
    m_tile_width = tile_width;
    m_tile_height = tile_height;
    m_columns = columns;
    m_rows = rows;
    m_grid = std::shared_ptr<Subtexture[]>(new Subtexture[columns * rows]);
}

int Tilemap::tile_width() const {
    return m_tile_width;
}

int Tilemap::tile_height() const {
    return m_tile_height;
}

int Tilemap::columns() const {
    return m_columns;
}

int Tilemap::rows() const {
    return m_rows;
}

void Tilemap::set_cell(int x, int y, const Subtexture *tex) {
    BLAH_ASSERT(x >= 0 && y >= 0 && x < m_columns && y < m_rows, "Tilemap indices out of bounds!");

    if (tex) {
        m_grid[x + y * m_columns] = *tex;
    } else {
        m_grid[x + y * m_columns].texture.reset();
    }
}

void Tilemap::set_cells(int x, int y, int w, int h, const Subtexture *tex) {
    for (int ix = x; ix < x + w; ix++) {
        for (int iy = y; iy < y + h; iy++) {
            set_cell(ix, iy, tex);
        }
    }
}

void Tilemap::render(Batch &batch) {
    for (int x = 0; x < m_columns; x++) {
        for (int y = 0; y < m_rows; y++) {
            if (m_grid[x + y * m_columns].texture) {
                batch.tex(m_grid[x + y * m_columns], Vec2(x * m_tile_width, y * m_tile_height));
            }
        }
    }
}
