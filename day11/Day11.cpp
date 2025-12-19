#include <fstream>
#include <iostream>
#include <regex>

#include "nvl/data/List.h"
#include "nvl/data/Map.h"
#include "nvl/data/Set.h"
#include "nvl/data/SipHash.h"

using namespace nvl;

template <>
struct std::hash<Set<std::string>> {
    pure size_t operator()(const Set<std::string> &set) const noexcept { return sip_hash(set.values()); }
};

Map<std::string, List<std::string>> parse_nodes(const std::string &filename) {
    static const std::regex kNodePattern ("[a-z]+");
    Map<std::string, List<std::string>> nodes;
    std::ifstream file(filename);
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        std::string::const_iterator iter (line.cbegin());
        List<std::string> *node = nullptr;
        while (iter != line.cend() && std::regex_search(iter, line.cend(), match, kNodePattern)) {
            if (!node) {
                node = &nodes[match[0].str()];
            } else {
                node->push_back(match[0].str());
            }
            iter = match.suffix().first;
        }
    }
    return nodes;
}

I64 num_paths(Map<std::string, List<std::string>> &nodes, const std::string &start,
              const Set<std::string> &requirements) {
    List<std::string> worklist {start};
    Map<std::string, Map<Set<std::string>, I64>> visited;
    visited["out"][{}] = 1;
    while (!worklist.empty()) {
        const auto &current = worklist.back();
        auto iter = visited.find(current);
        if (iter == visited.end()) {
            worklist.append(nodes[current]);
            visited[current] = {};
        } else if (iter->second.empty()) {
            const bool is_requirement = requirements.has(current);
            for (const auto &edge : nodes[current]) {
                for (const auto &[cat, paths] : visited[edge]) {
                    Set<std::string> cat2 = cat;
                    if (is_requirement) cat2.insert(current);
                    if (auto iter2 = iter->second.find(cat2); iter2 == iter->second.end()) {
                        iter->second[cat2] = 0;
                    }
                    iter->second[cat2] += paths;
                }
            }
            worklist.pop_back();
        } else {
            worklist.pop_back();
        }
    }
    return visited[start][requirements];
}

int main() {
    auto nodes = parse_nodes("../data/11/full");
    std::cout << "Part 1: " << num_paths(nodes, "you", {}) << std::endl;
    std::cout << "Part 2: " << num_paths(nodes, "svr", {"dac", "fft"}) << std::endl;
    return 0;
}
