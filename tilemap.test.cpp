import<iostream>;
import tilemap;
import<filesystem>;
import vec2;
#include "ut.hpp"

int main() {
    using namespace boost::ut;
    std::filesystem::remove_all("tilemap");
    Tilemap tilemap;
    "empty tilemap"_test = [&] {
        should("create tilemap directory") = [] {
            expect(std::filesystem::is_directory("tilemap"));
        };
        should("return Terrain::None") = [&] {
            expect(tilemap.getTile(vec2<int>(420, 69)) == Terrain::None);
        };
    };
    "after reveal"_test = [&] {
        vec2<int> const position = vec2<int>(-32, 900);
        tilemap.reveal(position);
        should("not return Terrain::None there") = [&] {
            expect(tilemap.getTile(position) != Terrain::None);
        };
        should("return Terrain::None 11 hexes away") = [&] {
            expect(tilemap.getTile(position + vec2<int>(11, -11)) == Terrain::None);
        };
    };
}
