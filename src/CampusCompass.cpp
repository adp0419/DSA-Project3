#include "CampusCompass.h"

#include <fstream>
#include <sstream>
#include <string>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize your object
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    // return boolean based on whether parsing was successful or not
    bool edgesLoaded = loadEdges(edges_filepath);
    bool classesLoaded = loadClasses(classes_filepath);
    return edgesLoaded && classesLoaded;
}

bool CampusCompass::ParseCommand(const string &command) {
    // do whatever regex you need to parse validity
    // hint: return a boolean for validation when testing. For example:
    bool is_valid = true; // replace with your actual validity checking

    return is_valid;
}

bool CampusCompass::loadEdges(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    string line;
    getline(file, line);
    while (getline(file, line)) {
        vector<string> fields = splitLine(line);
        if (fields.size() < 5) {
            continue;
        }
        int fromId = stoi(fields[0]);
        int toId = stoi(fields[1]);
        int weight = stoi(fields[4]);
        campusGraph.setLocationName(fromId, fields[2]);
        campusGraph.setLocationName(toId, fields[3]);
        campusGraph.addEdge(fromId, toId, weight);
    }
    return true;
}

bool CampusCompass::loadClasses(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    string line;
    getline(file, line);
    while (getline(file, line)) {
        vector<string> fields = splitLine(line);
        if (fields.size() < 4) {
            continue;
        }
        ClassInfo info;
        info.locationId = stoi(fields[1]);
        info.startMinutes = timeToMinutes(fields[2]);
        info.endMinutes = timeToMinutes(fields[3]);
        classes[fields[0]] = info;
    }
    return true;
}

vector<string> CampusCompass::splitLine(const string &line) {
    string cleanLine = line;
    // Windows line endings leave a '\r' at the end
    if (!cleanLine.empty() && cleanLine.back() == '\r') {
        cleanLine.pop_back();
    }

    vector<string> fields;
    stringstream stream(cleanLine);
    string field;
    while (getline(stream, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

int CampusCompass::timeToMinutes(const string &time) {
    int colonIndex = time.find(':');
    int hours = stoi(time.substr(0, colonIndex));
    int minutes = stoi(time.substr(colonIndex + 1));
    return hours * 60 + minutes;
}
