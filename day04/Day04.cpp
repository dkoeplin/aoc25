#include <iostream>

#include "nvl/data/Set.h"
#include "nvl/data/Tensor.h"
#include "nvl/geo/Tuple.h"
#include "nvl/geo/Volume.h"
#include "nvl/macros/Aliases.h"

using namespace nvl;

struct Round {
    Round() = default;
    Round(const Tensor<2, char> &map, const Range<Pos<2>> &indices);
    List<Pos<2>> removed;
    List<Pos<2>> remain;
};
Round::Round(const Tensor<2, char> &map, const Range<Pos<2>> &indices) {
    static constexpr Pos<2> zero {0, 0};
    static constexpr Box<2> dirs ({-1, -1}, {2, 2});
    for (auto index : indices) {
        if (map[index] == '@') {
            I64 rolls = 0;
            for (auto delta : dirs.indices()) {
                rolls += (delta != zero && map.get_or(index + delta, '.') == '@') ? 1 : 0;
            }
            auto &list = rolls < 4 ? removed : remain;
            list.push_back(index);
        }
    }
}

int main() {
    auto m = matrix_from_file("../data/04/full");
    bool first = true;
    Round part;
    I64 removed = 0;
    do {
        part = Round(m, first ? m.indices() : part.remain.range());
        removed += part.removed.size();
        for (const auto index : part.removed) {
            m[index] = '.';
        }
        if (first) {
            std::cout << "Part 1: " << part.removed.size() << std::endl;
            first = false;
        }
    } while (!part.removed.empty());
    std::cout << "Part 2: " << removed << std::endl;
}
