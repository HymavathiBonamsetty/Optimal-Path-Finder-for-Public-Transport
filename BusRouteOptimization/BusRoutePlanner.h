#ifndef BUS_ROUTE_PLANNER_H
#define BUS_ROUTE_PLANNER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <limits>

struct Edge {
    int destination;
    double weight;
    std::string busLine;
    Edge(int dest, double w, const std::string& line = "") : destination(dest), weight(w), busLine(line) {}
};

class BusRoutePlanner {
private:
    std::vector<std::vector<Edge>> adjList;
    std::map<std::string, int> stopNameToId;
    std::vector<std::string> stopIdToName;
    int numStops;

    int getStopId(const std::string& stopName);

public:
    BusRoutePlanner();
    void addStop(const std::string& stopName);
    void addRoute(const std::string& stopA_Name, const std::string& stopB_Name,
                  double weight, const std::string& busLine = "", bool bidirectional = true);
    void removeRoute(const std::string& stopA_Name, const std::string& stopB_Name,
                     const std::string& busLine = "", bool bidirectional = true);

    std::vector<std::string> getShortestPathDijkstra(const std::string& startStopName, const std::string& endStopName);
    int findMinTransfersBFS(const std::string& startStopName, const std::string& endStopName);
    double getPathDistance(const std::vector<std::string>& path);
    std::vector<std::string> getAllStops();

    void loadRoutesFromCSV(const std::string& filename);
};

#endif // BUS_ROUTE_PLANNER_H
