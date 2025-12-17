#include <fstream>
#include <iostream>
#include <regex>

#include "nvl/data/List.h"
#include "nvl/geo/RTree.h"
#include "nvl/geo/Tuple.h"
#include "nvl/geo/Volume.h"
#include "nvl/time/Duration.h"

using namespace nvl;

List<Pos<2>> parse_points(const std::string &filename) {
    static const std::regex regex("([0-9]+),([0-9]+)");
    List<Pos<2>> points;
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_match(line, match, regex)) {
            points.emplace_back(std::stoll(match[1].str()), std::stoll(match[2].str()));
        }
    }
    return points;
}

I64 part1(const List<Pos<2>> &points) {
    I64 max_area = 0;
    for (U64 i = 0; i < points.size(); ++i) {
        for (U64 j = i + 1; j < points.size(); ++j) {
            max_area = std::max(max_area, Box<2>::inclusive(points[i], points[j]).shape().product());
        }
    }
    return max_area;
}

Pos<2> transposed(const Pos<2> &dir) {
    Pos<2> t = dir;
    std::swap(t[0], t[1]);
    return abs(t);
}

struct Points {
    Set<Pos<2>> inside;
    Set<Pos<2>> outside;
};

struct Tiles {
    enum Kind {kInternal, kEdge};
    Tiles(const Box<2> &box, const Kind kind) : box(box), kind(kind) {}
    pure Box<2> bbox() const { return box; }
    Box<2> box;
    Kind kind;
};

struct Move {
    Ref<Tiles> collision;
    Pos<2> pt;
};
Maybe<Move> move(const RTree<2, Tiles> &grid, const Pos<2> &initial, const Pos<2> &dir) {
    const Pos<2> t = transposed(dir);
    const bool left = dir.min() < 0;
    const auto end = left ? grid.bbox().min : grid.bbox().end;
    const Line line (real(initial), real(end * dir + initial * t));
    const auto intersect = grid.first_where(line, [](const Intersect<2> &i){ return i.dist; });
    return intersect ? Some(Move{.collision = intersect->item, .pt = floor(intersect->pt) - dir}) : None;
}

/// The area [box] is enclosed within the borders in [grid] if there is no path which exists from a point
/// on its surface to the outside of the grid.
bool fully_within(Points &cache, RTree<2, Tiles> &grid, const Box<2> &box) {
    static constexpr Pos<2> dirs[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    List<Pos<2>> frontier { box.min };
    Set<Pos<2>> visited { box.min };
    Maybe<bool> found_path_to_outside = None;
    while (!frontier.empty() && !found_path_to_outside.has_value()) {
        const Pos<2> current = frontier.back();
        frontier.pop_back();
        if (cache.outside.has(current)) {
            found_path_to_outside = true;
        } else if (cache.inside.has(current)) {
            found_path_to_outside = false;
        } else {
            for (auto dir : dirs) {
                if (auto next = move(grid, current, dir)) {
                    if (!visited.has(next->pt)) {
                        frontier.push_back(next->pt);
                        visited.insert(next->pt);
                    }
                } else {
                    found_path_to_outside = true;
                }
            }
        }
    }
    const bool outside = found_path_to_outside.value_or(false);
    auto &set = outside ? cache.outside : cache.inside;
    set.insert(visited);
    return !outside;
}

/// Adds all non-overlapping parts of the area [box] to grid if no part is outside the existing filled area.
/// Returns true if the area was added, false otherwise.
bool add_if_contained_in(Points &cache, RTree<2, Tiles> &grid, const Box<2> &box) {
    const Set<Ref<Tiles>> intersect = grid[box];
    const auto remaining = box.diff(intersect.values());
    // Check if all non-intersecting parts of the box are fully inside the existing lines.
    const bool inside = remaining.range().all([&](const Box<2> &remain) { return fully_within(cache, grid, remain); });
    return inside;
}

I64 part2(const List<Pos<2>> &points) {
    RTree<2, Tiles> grid;
    for (U64 i = 0; i < points.size(); ++i) {
        grid.emplace(Box<2>::inclusive(points[i], points[(i + 1) % points.size()]), Tiles::kEdge);
    }

    I64 max_area = 0;
    Points cache;
    for (U64 i = 0; i < points.size(); ++i) {
        for (U64 j = i + 1; j < points.size(); ++j) {
            const auto box = Box<2>::inclusive(points[i], points[j]);
            const auto area = box.shape().product();
            // Only consider the next box if we already know the area is larger than the current record
            if (area > max_area && add_if_contained_in(cache, grid, box)) {
                max_area = std::max(max_area, area);
                std::cout << points[i] << "::" << points[j] << ": " << area << std::endl;
                std::cout << "  cache(inside: " << cache.inside.size() << ", outside: " << cache.outside.size() << ")" << std::endl;
            }
        }
    }
    return max_area;
}

int main() {
    const auto points = parse_points("../data/09/full");
    std::cout << "Part 1: " << part1(points) << std::endl;
    auto start = Clock::now();
    auto p2 = part2(points);
    auto end = Clock::now();
    std::cout << "Part 2: " << p2 << " (" << Duration(end - start) << ")" <<  std::endl;
}