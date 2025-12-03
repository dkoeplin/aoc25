#include <fstream>
#include <iostream>
#include <regex>

#include "nvl/macros/Aliases.h"
#include "nvl/geo/Dir.h"

struct State {
    void spin(const I64 spin) {
        const auto prev = pos;
        part2 += std::abs(spin) / 100; // Full rotations
        const auto spinr = spin % 100; // Partial rotation
        pos = (prev + spinr) % 100;
        part2 += (spin < 0 && pos < 0 && prev > 0) ||
                 (spin > 0 && pos > 0 && pos < prev) || pos == 0 ? 1 : 0;
        pos = (pos + 100) % 100;
        part1 += pos == 0 ? 1 : 0;
    }
    I64 pos = 50;
    I64 part1 = 0;
    I64 part2 = 0;
};

std::pair<I64, I64> dial(const std::string &filename) {
    static const std::regex regex("(L|R)([0-9]+)");
    std::ifstream file(filename);
    State state;
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_match(line, match, regex)) {
            auto dir = match[1].str()[0] == 'L' ? nvl::Dir::Neg : nvl::Dir::Pos;
            state.spin(dir * std::stoi(match[2].str()));
        }
    }
    return {state.part1, state.part2};
}

int main() {
    auto [part1, part2] = dial("../data/01/full");
    std::cout << "Part 1: " << part1 << std::endl;
    std::cout << "Part 2: " << part2 << std::endl;
    return 0;
}