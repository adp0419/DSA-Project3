#include "Graph.h"

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
