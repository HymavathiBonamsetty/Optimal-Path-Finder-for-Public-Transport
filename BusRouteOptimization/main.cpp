#include "BusRoutePlanner.h"
#include <iostream>

int main(int argc, char* argv[]) {
    BusRoutePlanner planner;
    planner.loadRoutesFromCSV("busroutes.csv");

    if (argc == 2 && std::string(argv[1]) == "cities") {
        auto cities = planner.getAllStops();
        for (auto& c : cities) std::cout << c << "\n";
        return 0;
    }

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source> <destination>\n";
        return 1;
    }

    std::string src = argv[1];
    std::string dst = argv[2];

    auto path = planner.getShortestPathDijkstra(src, dst);
    if (path.empty()) { std::cout << "No path found\n"; return 0; }

    std::cout << "Path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i];
        if (i+1 < path.size()) std::cout << " -> ";
    }
    std::cout << "\n";

    std::cout << "Total Distance: " << planner.getPathDistance(path) << "\n";
    std::cout << "Min Transfers: " << planner.findMinTransfersBFS(src, dst) << "\n";

    return 0;
}
