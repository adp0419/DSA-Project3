#pragma once
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Edge {
    int toId;
    int weight;
    bool isOpen;
};

class Graph {
private:
    unordered_map<int, vector<Edge>> adjacencyList;
    unordered_map<int, string> locationNames;

public:
    void addEdge(int fromId, int toId, int weight);
    void setLocationName(int locationId, const string &name);
    bool hasLocation(int locationId);
    bool hasEdge(int fromId, int toId);
    bool isEdgeOpen(int fromId, int toId);
    void toggleEdge(int fromId, int toId);
    bool isConnected(int startId, int endId);
    void findShortestPaths(int startId, unordered_map<int, int> &distances, unordered_map<int, int> &parents);
    int findMinimumSpanningTreeCost(const set<int> &locationIds);
    string getLocationName(int locationId);
    vector<Edge> getNeighbors(int locationId);
};
