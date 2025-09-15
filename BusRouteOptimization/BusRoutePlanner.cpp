#include "BusRoutePlanner.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <limits>
#include <iostream>

// Trim whitespace helper
std::string trim(const std::string& str) {
    const char* ws = " \t\r\n";
    size_t start = str.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(ws);
    return str.substr(start, end - start + 1);
}

// Constructor
BusRoutePlanner::BusRoutePlanner() : numStops(0) {}

// Get or create stop ID
int BusRoutePlanner::getStopId(const std::string& stopName) {
    auto it = stopNameToId.find(stopName);
    if (it == stopNameToId.end()) {
        stopNameToId[stopName] = numStops;
        stopIdToName.push_back(stopName);
        adjList.resize(numStops + 1);
        numStops++;
        return numStops - 1;
    }
    return it->second;
}

// Add stop
void BusRoutePlanner::addStop(const std::string& stopName) {
    getStopId(stopName);
}

// Add route
void BusRoutePlanner::addRoute(const std::string& stopA_Name, const std::string& stopB_Name,
                               double weight, const std::string& busLine, bool bidirectional) {
    int u = getStopId(stopA_Name);
    int v = getStopId(stopB_Name);
    if ((int)adjList.size() < numStops) adjList.resize(numStops);
    adjList[u].push_back(Edge(v, weight, busLine));
    if (bidirectional) adjList[v].push_back(Edge(u, weight, busLine));
}

// Load routes from CSV
void BusRoutePlanner::loadRoutesFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << "\n";
        return;
    }

    std::string line;
    bool headerSkipped = false;
    while (std::getline(file, line)) {
        if (!headerSkipped) { headerSkipped = true; continue; } // Skip header

        std::stringstream ss(line);
        std::string src, dst, distStr;
        if (!std::getline(ss, src, ',')) continue;
        if (!std::getline(ss, dst, ',')) continue;
        if (!std::getline(ss, distStr, ',')) continue;

        src = trim(src); dst = trim(dst); distStr = trim(distStr);
        if (src.empty() || dst.empty() || distStr.empty()) continue;

        try {
            double dist = std::stod(distStr);
            addRoute(src, dst, dist, "BusLine", true);
        } catch (...) {
            std::cerr << "Skipping invalid line: " << line << "\n";
        }
    }
    file.close();
}

// Get all stops
std::vector<std::string> BusRoutePlanner::getAllStops() {
    return stopIdToName;
}

// Dijkstra shortest path
std::vector<std::string> BusRoutePlanner::getShortestPathDijkstra(const std::string& startStopName,
                                                                  const std::string& endStopName) {
    std::vector<std::string> path;
    if (stopNameToId.find(startStopName) == stopNameToId.end() ||
        stopNameToId.find(endStopName) == stopNameToId.end()) return path;

    int startNode = stopNameToId[startStopName];
    int endNode = stopNameToId[endStopName];

    std::vector<double> dist(numStops, std::numeric_limits<double>::infinity());
    std::vector<int> parent(numStops, -1);

    using DijkstraNode = std::pair<double, int>;
    std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<DijkstraNode>> pq;

    dist[startNode] = 0;
    pq.push({0, startNode});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == endNode) break;

        for (const auto& edge : adjList[u]) {
            int v = edge.destination;
            double w = edge.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[endNode] == std::numeric_limits<double>::infinity()) return path;

    int cur = endNode;
    while (cur != -1) {
        path.push_back(stopIdToName[cur]);
        cur = parent[cur];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// Distance of path
double BusRoutePlanner::getPathDistance(const std::vector<std::string>& path) {
    double total = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        int u = stopNameToId[path[i]];
        int v = stopNameToId[path[i+1]];
        for (auto& e : adjList[u]) {
            if (e.destination == v) { total += e.weight; break; }
        }
    }
    return total;
}

// BFS for min transfers
int BusRoutePlanner::findMinTransfersBFS(const std::string& startStopName, const std::string& endStopName) {
    if (stopNameToId.find(startStopName) == stopNameToId.end() ||
        stopNameToId.find(endStopName) == stopNameToId.end()) return -1;

    int startNode = stopNameToId[startStopName];
    int endNode = stopNameToId[endStopName];

    std::vector<int> dist(numStops, -1);
    std::queue<int> q;
    dist[startNode] = 0;
    q.push(startNode);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == endNode) break;

        for (auto& edge : adjList[u]) {
            int v = edge.destination;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist[endNode];
}
