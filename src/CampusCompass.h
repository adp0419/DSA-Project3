#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Graph.h"

using namespace std;

struct ClassInfo {
    int locationId;
    int startMinutes;
    int endMinutes;
};

class CampusCompass {
private:
    // Think about what member variables you need to initialize
    // perhaps some graph representation?
    Graph campusGraph;
    unordered_map<string, ClassInfo> classes;

    bool loadEdges(const string &filepath);
    bool loadClasses(const string &filepath);
    vector<string> splitLine(const string &line);
    int timeToMinutes(const string &time);
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);
};
