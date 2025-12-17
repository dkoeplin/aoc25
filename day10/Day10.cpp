#include <fstream>
#include <iostream>
#include <queue>
#include <ranges>
#include <regex>
#include <sstream>

#include "nvl/data/List.h"
#include "nvl/data/Map.h"
#include "nvl/data/Maybe.h"
#include "nvl/data/Set.h"
#include "nvl/data/SipHash.h"
#include "nvl/data/Tensor.h"
#include "nvl/macros/Aliases.h"

#include "z3++.h"

using namespace nvl;

using Lights = List<I64>;
using Button = List<I64>;

template <>
struct std::hash<Lights> {
    U64 operator()(const Lights &lights) const noexcept { return sip_hash(lights.range()); }
};

Lights toggle(const Lights &lights, const Button &button) {
    Lights result = lights;
    for (const I64 index : button) {
        result[index] = !result[index];
    }
    return result;
}

struct Machine {
    Machine() = default;
    Lights target;
    List<Button> buttons;
    List<I64> joltage;
};

List<I64> parse_ints(const std::string &str, const char delim = ',') {
    List<I64> ints;
    std::stringstream ss (str);
    std::string item;
    while (std::getline(ss, item, delim)) {
        ints.push_back(std::stoll(item));
    }
    return ints;
}

List<Machine> parse(const std::string &filename) {
    static const std::regex lights("\\[([^\\]]+)\\]");
    static const std::regex button("\\(([^\\)]+)\\)");
    static const std::regex joltage("\\{([^\\}]+)\\}");
    List<Machine> machines;
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        Machine &machine = machines.emplace_back();
        std::string::const_iterator iter (line.cbegin());
        bool done = false;
        while (iter != line.cend() && !done) {
            if (machine.target.empty() && std::regex_search(iter, line.cend(), match, lights)) {
                for (auto c : match[1].str()) {
                    machine.target.push_back(c == '#' ? 1 : 0);
                }
                iter = match.suffix().first;
            } else if (std::regex_search(iter, line.cend(), match, button)) {
                machine.buttons.emplace_back(parse_ints(match[1].str()));
                iter = match.suffix().first;
            } else if (std::regex_search(iter, line.cend(), match, joltage)) {
                machine.joltage = parse_ints(match[1].str());
                done = true;
            }
        }
    }
    return machines;
}

struct Dijkstra {
    struct Pair {
        bool operator<(const Pair &rhs) const noexcept { return cost > rhs.cost; }
        Lights lights;
        U64 cost = 0;
    };

    explicit Dijkstra(const Machine &machine) : ending(machine.target) {
        starting = Lights(ending.size(), 0);

        Set<Lights> visited;
        std::priority_queue<Pair> queue;
        dist[starting] = 0;
        queue.emplace(starting, 0);

        while (!queue.empty()) {
            const auto [current, ignored_cost] = queue.top();
            queue.pop();
            if (!visited.has(current)) {
                visited.insert(current);
                for (const Button &button : machine.buttons) {
                    const Lights next = toggle(current, button);
                    const I64 alt = dist[current] + 1; // one button press
                    const I64 prev_dist = dist.get_or(next, INT64_MAX);
                    if (alt < prev_dist) {
                        dist[next] = alt;
                        prev[next] = {current};
                        queue.emplace(next, alt);
                    } else if (alt == prev_dist) {
                        prev[next].push_back(current);
                    }
                }
            }
        }
        if (auto best = dist.get(ending))
            best_cost = *best;
    }

    Map<Lights, I64> dist;
    Map<Lights, List<Lights>> prev;
    Maybe<I64> best_cost = None;
    Lights starting;
    Lights ending;
};

// This could also be solved with z3, like below.
I64 part1(const List<Machine> &machines) {
    I64 part1 = 0;
    for (const auto &machine : machines) {
        Dijkstra solution (machine);
        part1 += solution.best_cost.value_or(0);
    }
    return part1;
}

// Each "joltage" is a function of some of the button presses.
// This can just be expressed as a system of linear equations, which can be minimized with e.g. z3.
I64 part2(const List<Machine> &machines) {
    I64 part2 = 0;
    for (const auto &machine : machines) {
        const I64 B = static_cast<I64>(machine.buttons.size());
        const I64 N = static_cast<I64>(machine.joltage.size());

        z3::context ctx;
        z3::optimize solver (ctx);
        std::vector<z3::expr> buttons;
        std::optional<z3::expr> sum;
        for (I64 b = 0; b < B; ++b) {
            std::string name = "B" + std::to_string(b);
            auto button = ctx.int_const(name.c_str());
            buttons.push_back(button);
            sum = sum ? *sum + button : button;
            solver.add(button >= 0);
        }

        std::vector<std::optional<z3::expr>> lights (N, None);
        for (I64 b = 0; b < B; ++b) {
            const z3::expr &button = buttons[b];
            for (I64 l : machine.buttons[b]) {
                std::optional<z3::expr> &light = lights[l];
                light = light ? *light + button : button;
            }
        }
        for (I64 l = 0; l < N; ++l) {
            if (const auto &light = lights[l]) {
                auto expr = *light == static_cast<int>(machine.joltage[l]);
                solver.add(expr);
            }
        }
        auto h = solver.minimize(*sum);
        (void)solver.check();
        part2 += solver.lower(h).as_int64();
    }
    return part2;
}

int main() {
    const List<Machine> machines = parse("../data/10/full");
    std::cout << "Part 1: " << part1(machines) << std::endl;
    std::cout << "Part 2: " << part2(machines) << std::endl;
}
