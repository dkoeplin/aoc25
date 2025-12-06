#include <fstream>
#include <iostream>
#include <string>

#include "nvl/macros/Aliases.h"

struct ArgMax {
    I64 value;
    I64 index;
};
ArgMax argmax(const std::string &line, I64 start, I64 end) {
    I64 max = -1;
    I64 idx = 0;
    for (I64 i = start; i < end; ++i) {
        I64 v = static_cast<I64>(line[i]) - '0';
        if (v > max) {
            idx = i;
            max = v;
        }
    }
    return {max, idx};
}

I64 max_joltage(const std::string &line, I64 len) {
    I64 result = 0;
    I64 start = 0;
    for (I64 i = 0; i < len; ++i) {
        auto [c, idx] = argmax(line, start, line.size() - len + 1 + i);
        result += (c * std::pow(10, len - i - 1));
        start = idx + 1;
    }
    return result;
}

int main() {
    std::ifstream file("../data/03/full");
    std::string line;
    I64 part1 = 0, part2 = 0;
    while (std::getline(file, line)) {
        part1 += max_joltage(line, 2);
        part2 += max_joltage(line, 12);
    }
    std::cout << "Part 1: " << part1 << std::endl;
    std::cout << "Part 2: " << part2 << std::endl;
}