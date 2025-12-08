#include <fstream>
#include <iostream>
#include <regex>

#include "nvl/data/List.h"
#include "nvl/data/UnionFind.h"
#include "nvl/geo/Tuple.h"

using namespace nvl;

struct Pair {
    Pos<3> a;
    Pos<3> b;
    double dist;
};

std::vector<Pair> distance_pairs(const List<Pos<3>> &points) {
    std::vector<Pair> pairs;
    for (U64 i = 0; i < points.size(); ++i) {
        for (U64 j = i + 1; j < points.size(); ++j) {
            pairs.emplace_back(points[i], points[j], points[i].dist(points[j]));
        }
    }
    std::ranges::sort(pairs, [](const Pair &a, const Pair &b){ return a.dist < b.dist; });
    return pairs;
}

List<Pos<3>> parse_points(const std::string &filename) {
    static const std::regex regex("([0-9]+),([0-9]+),([0-9]+)");
    List<Pos<3>> points;
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_match(line, match, regex)) {
            points.emplace_back(std::stoll(match[1].str()), std::stoll(match[2].str()), std::stoll(match[3].str()));
        }
    }
    return points;
}

U64 topk_product(const Range<Set<Pos<3>>> &circuits) {
    std::priority_queue<U64, std::vector<U64>, std::greater<>> topk;
    for (const auto &circuit : circuits) {
        topk.push(circuit.size());
        if (topk.size() > 3) {
            topk.pop();
        }
    }
    U64 total = 1;
    while (!topk.empty()) {
        total *= topk.top();
        topk.pop();
    }
    return total;
}

void merge(const List<Pos<3>> &points, const std::vector<Pair> &pairs, U64 n) {
    UnionFind<Pos<3>> circuits;
    for (auto point : points) {
        circuits.add(point);
    }
    U64 i = 0;
    U64 num_sets = 1;
    while (num_sets > 1 || i < n) {
        const auto &pair = pairs[i];
        circuits.add(pair.a, pair.b);
        num_sets = circuits.num_sets();
        if (i == n - 1) {
            std::cout << "Part 1: " << topk_product(circuits.sets()) << std::endl;
        }
        if (num_sets == 1) {
            std::cout << "Part 2: " << pair.a << ", " << pair.b << ": " << pair.a[0] * pair.b[0] << std::endl;
        }
        i += 1;
    }
}

int main() {
    const auto points = parse_points("../data/08/full");
    const auto pairs = distance_pairs(points);
    merge(points, pairs, 1000);
}
