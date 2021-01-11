#include "kamkar.h"

#pragma warning(disable : 26812)

constexpr uint treeSizeLog2 = 5;
constexpr uint treeSize = 1 << treeSizeLog2;

constexpr uint chunkSizeLog2 = 6;
constexpr uint chunkSize = 1 << chunkSizeLog2;

struct Chunk {
    Tile tiles[chunkSize][chunkSize];
};

constexpr uint maxDepth = 4;
constexpr uint tilemapHalfSize = 1 << (treeSizeLog2 * maxDepth);

constexpr uint tilemapTotalSize = 1 << (treeSizeLog2 * maxDepth + chunkSizeLog2);
static_assert(tilemapTotalSize, "tilemap size exceeds 32 bit unsigned integer limit");

struct Tree {
    void *nodes[treeSize][treeSize];
};

struct Tilemap {
    Tree tree;
    fnl_state fnlState;
    Chunk *chunkCache;
    ivec2 cachePosition;
};

template <uchar N>
static Terrain splitFloat(float input, const float (&splitPoints)[N]) {
    for (uchar i = 0; i < N; ++i) {
        if (input < splitPoints[i]) {
            return Terrain(i);
        }
    }
    return Terrain(N);
}

static Terrain terrainFromNoise(float noise) {
    static const float splitPoints[] = { 0.0f, 0.2f, 0.7f };
    return Terrain(splitFloat(noise, splitPoints) + 1);
}

static vec2 hexToPixel(ivec2 hex) {
    return { sqrt3 * hex.x + sqrt3 / 2 * hex.y, hexHeight * hex.y };
}

static Chunk *generateChunk(ivec2 position, fnl_state *fnlState) {
    Chunk *chunk = ecalloc<Chunk>(1);
    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            const vec2 pixel = hexToPixel(position * chunkSize + ivec2{ x, y });
            chunk->tiles[x][y].terrain = terrainFromNoise(fnlGetNoise2D(fnlState, pixel.x, pixel.y));
        }
    }
    return chunk;
};

Tilemap *createTilemap() {
    Tilemap *const tilemap = ecalloc<Tilemap>(1);
    tilemap->fnlState = fnlCreateState();
    tilemap->fnlState.fractal_type = FNL_FRACTAL_FBM;
    return tilemap;
};

Tile *getTile(Tilemap *tilemap, ivec2 position) {
    // using >> and & because / and % are fucked up at negative values
    const ivec2 chunkPosition = position >> chunkSizeLog2;

    if (tilemap->chunkCache == nullptr || tilemap->cachePosition != chunkPosition) {
        void *node = &tilemap->tree;

        for (int currentDepth = maxDepth; currentDepth >= 0; --currentDepth) {
            const ivec2 treePosition = (chunkPosition >> (currentDepth * treeSizeLog2)) & (treeSize - 1);
            void *&subnode = static_cast<Tree *>(node)->nodes[treePosition.x][treePosition.y];
            if (subnode == nullptr) {
                if (currentDepth > 0) {
                    subnode = ecalloc<Tree>(1);
                } else {
                    subnode = generateChunk(chunkPosition, &tilemap->fnlState);
                }
            }
            node = subnode;
        }

        tilemap->chunkCache = static_cast<Chunk *>(node);
        tilemap->cachePosition = chunkPosition;
    }

    const ivec2 inChunkPosition = position & (chunkSize - 1);
    return &tilemap->chunkCache->tiles[inChunkPosition.x][inChunkPosition.y];
}
