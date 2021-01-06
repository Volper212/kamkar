#include "kamkar.h"
#include <stdio.h>

#pragma warning(disable : 26812)

constexpr uint treeSizeLog2 = 5;
constexpr uint treeSize = 1 << treeSizeLog2;

constexpr uint chunkSizeLog2 = 6;
constexpr uint chunkSize = 1 << chunkSizeLog2;

struct Chunk {
    Terrain tiles[chunkSize][chunkSize];
};

constexpr uint maxDepth = 4;
constexpr uint tilemapHalfSize = 1 << (treeSizeLog2 * maxDepth);

constexpr uint tilemapTotalSize = 1 << (treeSizeLog2 * maxDepth + chunkSizeLog2);
static_assert(tilemapTotalSize, "tilemap size exceeds 32 bit unsigned integer limit");

struct Tree {
    Tree *subtrees[treeSize][treeSize];
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
    Chunk *chunk = allocate<Chunk>();
    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            const vec2 pixel = hexToPixel(position * chunkSize + ivec2{ x, y });
            chunk->tiles[x][y] = terrainFromNoise(fnlGetNoise2D(fnlState, pixel.x, pixel.y));
        }
    }
    return chunk;
};

Tilemap *createTilemap() {
    Tilemap *tilemap = allocate<Tilemap>();
    zerofill(tilemap);
    tilemap->fnlState = fnlCreateState();
    tilemap->fnlState.fractal_type = FNL_FRACTAL_FBM;
    
    return tilemap;
};

Terrain getTile(Tilemap *tilemap, ivec2 position) {
    // using >> and & because / and % are fucked up at negative values
    const ivec2 chunkPosition = position >> chunkSizeLog2;

    if (tilemap->chunkCache == nullptr || tilemap->cachePosition != chunkPosition) {
        Tree *tree = &tilemap->tree;

        for (int currentDepth = maxDepth; currentDepth >= 0; --currentDepth) {
            const ivec2 treePosition = (chunkPosition >> (currentDepth * treeSizeLog2)) & (treeSize - 1);
            Tree *&currentTree = tree->subtrees[treePosition.x][treePosition.y];
            if (currentTree == nullptr) {
                if (currentDepth > 0) {
                    currentTree = allocate<Tree>();
                    zerofill(currentTree);
                } else {
                    currentTree = reinterpret_cast<Tree *>(generateChunk(chunkPosition, &tilemap->fnlState));
                }
            }
            tree = currentTree;
        }

        tilemap->chunkCache = reinterpret_cast<Chunk *>(tree);
        tilemap->cachePosition = chunkPosition;
    }

    const ivec2 inChunkPosition = position & (chunkSize - 1);
    return tilemap->chunkCache->tiles[inChunkPosition.x][inChunkPosition.y];
}