#include <regex>
#include <fstream>
#include <iostream>

#include "nvl/data/Set.h"
#include "nvl/macros/Aliases.h"
#include "nvl/math/Bitwise.h"

/// Returns the number of decimal digits in [x].
I64 num_digits(const I64 x) { return std::ceil(std::log10(x + 1)); }

/// Returns the integer resulting from concatenating [pattern] multiple times to form [total_digits] digits.
I64 repeat(const I64 pattern, const I64 total_digits) {
    I64 total = 0;
    const I64 pattern_digits = num_digits(pattern);
    for (I64 p = pattern_digits; p <= total_digits; p += pattern_digits) {
        total += pattern * std::pow(10, p - pattern_digits);
    }
    return total;
}

struct Range {
    std::pair<I64, I64> sum_repeated_nums() const {
        I64 part1 = 0, part2 = 0;
        nvl::Set<I64> counted;
        const I64 min_digits = num_digits(min);
        const I64 max_digits = num_digits(max);
        const I64 min_pattern_digits = 1;
        const I64 max_pattern_digits = std::max<I64>(1, std::floor(max_digits / 2));
        // Reverse iteration for part 1 to find the longest repeated string first.
        for (I64 digits = max_pattern_digits; digits >= min_pattern_digits; --digits) {
            I64 start = min / std::pow(10, min_digits - digits);
            start = min_digits < max_digits ? std::pow(10, digits - 1) : start;
            I64 end = max / std::pow(10, min_digits - digits);
            end = num_digits(end) > digits ? std::pow(10, digits) - 1 : end;
            const I64 start_digits = std::max<I64>(digits*2, nvl::ceil_div(min_digits, digits)*digits);
            for (I64 pattern = start; pattern <= end; ++pattern) {
                for (I64 total_digits = start_digits; total_digits <= max_digits; total_digits += digits) {
                    const I64 repeated = repeat(pattern, total_digits);
                    if (contains(repeated) && !counted.has(repeated)) {
                        part1 += total_digits == digits*2 ? repeated : 0;
                        part2 += repeated;
                        counted.insert(repeated);
                    }
                }
            }
        }
        return {part1, part2};
    }
    bool contains(const I64 x) const { return x >= min && x <= max; }
    explicit Range(I64 min, I64 max) : min(min), max(max) {}
    I64 min;
    I64 max;
};

int main() {
    const std::regex num("([0-9]+)\\-([0-9]+)");
    std::ifstream file("../data/02/full");
    std::string line;
    std::smatch match;
    I64 part1 = 0, part2 = 0;
    while (std::getline(file, line)) {
        std::string::const_iterator iter (line.cbegin());
        while (std::regex_search(iter, line.cend(), match, num)) {
            const Range range(std::stoll(match[1].str()), std::stoll(match[2].str()));
            auto [p1, p2] = range.sum_repeated_nums();
            part1 += p1;
            part2 += p2;
            iter = match.suffix().first;
        }
    }
    std::cout << "Part 1: " << part1 << std::endl;
    std::cout << "Part 2: " << part2 << std::endl;
    return 0;
}