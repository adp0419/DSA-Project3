#include "CampusCompass.h"

#include <cctype>
#include <fstream>
#include <iostream>
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

    vector<string> words;
    if (!splitCommand(command, words) || words.empty()) {
        is_valid = false;
    } else {
        is_valid = false;
    }

    if (!is_valid) {
        cout << "unsuccessful" << endl;
    }
    return is_valid;
}

bool CampusCompass::splitCommand(const string &command, vector<string> &words) {
    string currentWord = "";
    bool insideQuotes = false;

    for (int i = 0; i < (int)command.size(); i++) {
        char letter = command[i];
        if (letter == '"') {
            currentWord += letter;
            insideQuotes = !insideQuotes;
        } else if (letter == ' ' && !insideQuotes) {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord = "";
            }
        } else if (letter != '\r' && letter != '\n') {
            currentWord += letter;
        }
    }
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }

    return !insideQuotes;
}

bool CampusCompass::isNumber(const string &text) {
    if (text.empty()) {
        return false;
    }
    for (int i = 0; i < (int)text.size(); i++) {
        if (!isdigit(text[i])) {
            return false;
        }
    }
    return true;
}

bool CampusCompass::isValidName(const string &name) {
    if (name.empty()) {
        return false;
    }
    for (int i = 0; i < (int)name.size(); i++) {
        if (!isalpha(name[i]) && name[i] != ' ') {
            return false;
        }
    }
    return true;
}

bool CampusCompass::isValidStudentId(const string &studentId) {
    return studentId.size() == 8 && isNumber(studentId);
}

bool CampusCompass::isValidClassCode(const string &classCode) {
    if (classCode.size() != 7) {
        return false;
    }
    for (int i = 0; i < 3; i++) {
        if (classCode[i] < 'A' || classCode[i] > 'Z') {
            return false;
        }
    }
    for (int i = 3; i < 7; i++) {
        if (!isdigit(classCode[i])) {
            return false;
        }
    }
    return classes.count(classCode) > 0;
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
