#include <fstream>
#include <iostream>
#include <regex>

#include "nvl/data/List.h"
#include "nvl/data/Tensor.h"
#include "nvl/macros/Assert.h"

using namespace nvl;

struct Gift {
    explicit Gift(const Tensor<2, char> &t) : pts(t) {}

    explicit Gift(const List<std::string> &lines) {
        pts = matrix_from_lines(lines);
        for (const auto index : pts.indices()) {
            filled += pts[index] == '#';
        }
    }
    Tensor<2, char> pts;
    I64 filled = 0;
};

struct Tree {
    explicit Tree(Pos<2> shape, const List<I64> &amt) : shape(shape), amount(amt) {}
    pure bool can_fit(const List<Gift> &gifts) const;

    Pos<2> shape = Pos<2>::fill(0);
    List<I64> amount;
};

bool Tree::can_fit(const List<Gift> &gifts) const {
    I64 open = shape.product();
    I64 filled = 0;
    I64 total_gifts = 0;
    for (U64 i = 0; i < gifts.size(); ++i) {
        total_gifts += amount[i];
        filled += amount[i] * gifts[i].filled;
    }
    const I64 squares = (shape / Pos<2>(3, 3)).product();
    // Trivially false if the number of required squares is more than the available space
    if (filled > open) {
        std::cout << "Trivially false (" << shape << ", open: " << open << ", filled: " << filled << ")" << std::endl;
        return false;
    }
    if (squares >= total_gifts) {
        std::cout << "Trivially true (" << shape << ", 3x3 squares: " << squares << ", required: " << total_gifts << ")" << std::endl;
        return true;
    }
    ASSERT(false, "Too lazy to implement: " << shape << ": " << amount);
}

struct Input {
    List<Gift> gifts;
    List<Tree> trees;
};

Gift parse_gift(std::ifstream &file) {
    std::string line;
    List<std::string> lines;
    while (std::getline(file, line) && !line.empty()) {
        lines.push_back(line);
    }
    return Gift(lines);
}

Input parse_input(const std::string &filename) {
    static const std::regex kGift("[0-9]+:");
    static const std::regex kShape("([0-9]+)x([0-9]+)");
    static const std::regex kAmount ("[0-9]+");
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    Input input;
    while (std::getline(file, line)) {
        if (std::regex_match(line, kGift)) {
            input.gifts.push_back(parse_gift(file));
        } else {
            std::string::const_iterator iter (line.cbegin());
            std::regex_search(iter, line.cend(), match, kShape);
            const Pos<2> shape {std::stoll(match[1].str()), std::stoll(match[2].str())};
            List<I64> amount;
            iter = match.suffix().first;
            while (iter != line.cend() && std::regex_search(iter, line.cend(), match, kAmount)) {
                amount.push_back(std::stoll(match[0].str()));
                iter = match.suffix().first;
            }
            input.trees.emplace_back(shape, amount);
        }
    }
    return input;
}

int main() {
    auto input = parse_input("../data/12/full");
    I64 part1 = 0;
    for (U64 i = 0; i < input.trees.size(); ++i) {
        std::cout << i << ": ";
        part1 += input.trees[i].can_fit(input.gifts);
    }
    std::cout << "Part 1: " << part1 << std::endl;
    return 0;
}
