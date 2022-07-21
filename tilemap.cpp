export module tilemap;
import<filesystem>;
import vec2;

export enum struct Terrain : char { None, NotNone };

export struct Tilemap {
    bool isRevealed = false;
    vec2<int> revealed;

    Tilemap() {
        std::filesystem::create_directory("tilemap");
    }

    Terrain getTile(vec2<int> position) {
        return isRevealed && position == revealed ? Terrain::NotNone : Terrain::None;
    }

    void reveal(vec2<int> position) {
        isRevealed = true;
        revealed = position;
    }
};
