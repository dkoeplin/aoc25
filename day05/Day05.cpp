#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>

#include "nvl/data/List.h"
#include "nvl/geo/Volume.h"
#include "nvl/geo/Tuple.h"
#include "nvl/geo/RTree.h"
#include "nvl/geo/Util.h"
#include "nvl/macros/Pure.h"

using namespace nvl;

struct Data {
    explicit Data(const std::string &filename);
    pure bool is_fresh(const I64 id) const {
        const Pos<1> pos (id);
        return ranges.range().exists([&](const Box<1> &range){ return range.contains(pos); });
    }
    pure I64 part1() const {
        return std::ranges::count_if(ids, [&](const I64 id){ return is_fresh(id); });
    }
    pure I64 part2() const {
        RTree<1, Box<1>> tree;
        tree.insert(ranges.range());
        I64 total = 0;
        for (const auto &component : tree.components()) {
            total += bounding_box<1, I64, Box<1>>(component.values()).shape()[0];
        }
        return total;
    }
    List<Box<1>> ranges;
    List<I64> ids;
};
Data::Data(const std::string &filename) {
    static const std::regex regex("([0-9]+)-([0-9]+)");
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_match(line, match, regex)) {
            ranges.emplace_back(Pos<1>{std::stoll(match[1].str())}, Pos<1>{std::stoll(match[2].str()) + 1});
        } else if (!line.empty()) {
            ids.push_back(std::stoll(line));
        }
    }
}

int main() {
    const Data data ("../data/05/full");
    std::cout << "Part 1: " << data.part1() << std::endl;
    std::cout << "Part 2: " << data.part2() << std::endl;
}
