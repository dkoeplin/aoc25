#include <fstream>
#include <iostream>

#include "nvl/data/Ref.h"
#include "nvl/data/Map.h"
#include "nvl/data/Tensor.h"

using namespace nvl;

struct Step {
    static constexpr Pos<2> kLeft {0, -1};
    static constexpr Pos<2> kRight {0, 1};
    static constexpr Pos<2> kDown {1, 0};

    explicit Step(Ref<Tensor<2, char>> map) : map(map) {}

    static Step first(const Ref<Tensor<2, char>> map) {
        Step first (map);
        const auto start = map->index_where([](char c){ return c == 'S'; }).value_or(Pos<2>{-1, -1});
        first.add_beam_if_valid(start, 1);
        return first;
    }
    pure Step next() const;
    void add_beam_if_valid(const Pos<2> &next, const I64 m) {
        const I64 n = beams.get_or(next, 0) + m;
        beams[next] = n;
    }
    void add_beam(const Pos<2> &next, const I64 m) {
        if (map->get_or(next, 'X') == '^') {
            splits += 1;
            add_beam_if_valid(next + kLeft, m);
            add_beam_if_valid(next + kRight, m);
        } else {
            add_beam_if_valid(next, m);
        }
    }
    pure I64 num_paths() const {
        I64 n = 0;
        for (auto v : beams.values()) {
            n += v;
        }
        return n;
    }
    Ref<Tensor<2, char>> map;
    Map<Pos<2>, I64> beams; // Map from current beam position to number of unique paths to this position
    I64 splits = 0;
};
Step Step::next() const {
    Step step (map);
    step.splits = splits;
    for (const auto &[prev, m] : beams) {
        step.add_beam(prev + kDown, m);
    }
    return step;
}

int main() {
    auto m = matrix_from_file("../data/07/full");
    Step current = Step::first(m);
    for (I64 i = 0; i < m.shape()[0]; ++i) {
        current = current.next();
    }
    std::cout << "Part 1: " << current.splits << std::endl;
    std::cout << "Part 2: " << current.num_paths() << std::endl;
}
