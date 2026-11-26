#include "Graph.h"

#include <functional>
#include <queue>
#include <set>
#include <utility>

using namespace std;

void Graph::addEdge(int fromId, int toId, int weight) {
    if (hasEdge(fromId, toId)) {
        return;
    }
    Edge forwardEdge = {toId, weight, true};
    Edge backwardEdge = {fromId, weight, true};
    adjacencyList[fromId].push_back(forwardEdge);
    adjacencyList[toId].push_back(backwardEdge);
}

void Graph::setLocationName(int locationId, const string &name) {
    locationNames[locationId] = name;
}

bool Graph::hasLocation(int locationId) {
    return adjacencyList.count(locationId) > 0;
}

bool Graph::hasEdge(int fromId, int toId) {
    if (!hasLocation(fromId)) {
        return false;
    }
    for (int i = 0; i < (int)adjacencyList[fromId].size(); i++) {
        if (adjacencyList[fromId][i].toId == toId) {
            return true;
        }
    }
    return false;
}

bool Graph::isEdgeOpen(int fromId, int toId) {
    if (!hasLocation(fromId)) {
        return false;
    }
    for (int i = 0; i < (int)adjacencyList[fromId].size(); i++) {
        if (adjacencyList[fromId][i].toId == toId) {
            return adjacencyList[fromId][i].isOpen;
        }
    }
    return false;
}

void Graph::toggleEdge(int fromId, int toId) {
    if (!hasEdge(fromId, toId)) {
        return;
    }
    for (int i = 0; i < (int)adjacencyList[fromId].size(); i++) {
        if (adjacencyList[fromId][i].toId == toId) {
            adjacencyList[fromId][i].isOpen = !adjacencyList[fromId][i].isOpen;
        }
    }
    for (int i = 0; i < (int)adjacencyList[toId].size(); i++) {
        if (adjacencyList[toId][i].toId == fromId) {
            adjacencyList[toId][i].isOpen = !adjacencyList[toId][i].isOpen;
        }
    }
}

bool Graph::isConnected(int startId, int endId) {
    if (!hasLocation(startId) || !hasLocation(endId)) {
        return false;
    }

    set<int> visited;
    queue<int> toVisit;
    visited.insert(startId);
    toVisit.push(startId);

    while (!toVisit.empty()) {
        int currentId = toVisit.front();
        toVisit.pop();
        if (currentId == endId) {
            return true;
        }
        for (int i = 0; i < (int)adjacencyList[currentId].size(); i++) {
            Edge edge = adjacencyList[currentId][i];
            if (edge.isOpen && visited.count(edge.toId) == 0) {
                visited.insert(edge.toId);
                toVisit.push(edge.toId);
            }
        }
    }
    return false;
}

void Graph::findShortestPaths(int startId, unordered_map<int, int> &distances, unordered_map<int, int> &parents) {
    distances.clear();
    parents.clear();
    if (!hasLocation(startId)) {
        return;
    }

    // each pair is (distance, locationId) so the smallest distance comes out first
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> toVisit;
    set<int> finished;
    distances[startId] = 0;
    toVisit.push(make_pair(0, startId));

    while (!toVisit.empty()) {
        int currentDistance = toVisit.top().first;
        int currentId = toVisit.top().second;
        toVisit.pop();
        if (finished.count(currentId) > 0) {
            continue;
        }
        finished.insert(currentId);

        for (int i = 0; i < (int)adjacencyList[currentId].size(); i++) {
            Edge edge = adjacencyList[currentId][i];
            if (!edge.isOpen) {
                continue;
            }
            int newDistance = currentDistance + edge.weight;
            if (distances.count(edge.toId) == 0 || newDistance < distances[edge.toId]) {
                distances[edge.toId] = newDistance;
                parents[edge.toId] = currentId;
                toVisit.push(make_pair(newDistance, edge.toId));
            }
        }
    }
}

string Graph::getLocationName(int locationId) {
    if (locationNames.count(locationId) == 0) {
        return "";
    }
    return locationNames[locationId];
}

vector<Edge> Graph::getNeighbors(int locationId) {
    if (!hasLocation(locationId)) {
        return vector<Edge>();
    }
    return adjacencyList[locationId];
}
