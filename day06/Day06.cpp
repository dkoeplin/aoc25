#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>

#include "nvl/data/List.h"
#include "nvl/data/Tensor.h"
#include "nvl/macros/Pure.h"

using namespace nvl;

I64 operation(const List<I64> &numbers, char op) {
    const bool sum = op == '+';
    I64 result = sum ? 0 : 1;
    for (I64 num : numbers) {
        result = sum ? result + num : result * num;
    }
    return result;
}

struct Worksheet {
    explicit Worksheet(const std::string &filename);
    pure U64 grand_total() const {
        U64 total = 0;
        for (U64 i = 0; i < operations.size(); ++i) {
            List<I64> entries;
            for (const auto &line : lines) {
                entries.push_back(line[i]);
            }
            total += operation(entries, operations[i]);
        }
        return total;
    }
    List<List<I64>> lines;
    List<char> operations;
};

Worksheet::Worksheet(const std::string &filename) {
    static const std::regex num("[0-9]+");
    static const std::regex op("\\*|\\+");
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        List<I64> list;
        std::string::const_iterator iter (line.cbegin());
        while (std::regex_search(iter, line.cend(), match, num)) {
            list.push_back(std::stoll(match[0].str()));
            iter = match.suffix().first;
        }
        if (!list.empty()) {
            lines.push_back(list);
        }
        while (std::regex_search(iter, line.cend(), match, op)) {
            operations.push_back(match[0].str()[0]);
            iter = match.suffix().first;
        }
    }
}

Maybe<I64> column_num(const Tensor<2, char> &m, I64 col) {
    const I64 rows = m.shape()[0];
    I64 num = 0;
    I64 pow = 0;
    bool empty = true;
    for (I64 j = rows - 2; j >= 0; --j) {
        if (char c = m[Pos<2>(j, col)]; c != ' ') {
            empty = false;
            num += (static_cast<I64>(c - '0') * static_cast<I64>(std::pow(10, pow)));
            pow += 1;
        }
    }
    return SomeIf(num, !empty);
}

I64 part2(const std::string &filename) {
    const Tensor<2, char> m = matrix_from_file(filename, ' ');
    const I64 rows = m.shape()[0];
    const I64 cols = m.shape()[1];
    I64 total = 0;
    I64 i = 0;
    while (i < cols) {
        const char op = m[Pos<2>(rows - 1, i)];
        List<I64> nums;
        bool done = false;
        do {
            if (auto num = column_num(m, i)) {
                nums.push_back(*num);
            } else {
                done = true;
            }
            i += 1;
        } while (!done && i < cols);
        total += operation(nums, op);
    }
    return total;
}

int main() {
    const std::string filename = "../data/06/full";
    const Worksheet worksheet (filename);
    std::cout << "Part 1: " << worksheet.grand_total() << std::endl;
    std::cout << "Part 2: " << part2(filename) << std::endl;
}
