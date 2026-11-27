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
    } else if (words[0] == "insert") {
        is_valid = insertStudent(words);
    } else if (words[0] == "remove") {
        is_valid = removeStudent(words);
    } else if (words[0] == "dropClass") {
        is_valid = dropClass(words);
    } else if (words[0] == "replaceClass") {
        is_valid = replaceClass(words);
    } else if (words[0] == "removeClass") {
        is_valid = removeClass(words);
    } else if (words[0] == "toggleEdgesClosure") {
        is_valid = toggleEdgesClosure(words);
    } else if (words[0] == "checkEdgeStatus") {
        is_valid = checkEdgeStatus(words);
    } else if (words[0] == "isConnected") {
        is_valid = isConnected(words);
    } else if (words[0] == "printShortestEdges") {
        is_valid = printShortestEdges(words);
    } else if (words[0] == "printStudentZone") {
        is_valid = printStudentZone(words);
    } else {
        is_valid = false;
    }

    if (!is_valid) {
        cout << "unsuccessful" << endl;
    }
    return is_valid;
}

bool CampusCompass::insertStudent(const vector<string> &words) {
    if (words.size() < 5) {
        return false;
    }

    string quotedName = words[1];
    if (quotedName.size() < 3 || quotedName.front() != '"' || quotedName.back() != '"') {
        return false;
    }
    string name = quotedName.substr(1, quotedName.size() - 2);
    string studentId = words[2];
    string residenceText = words[3];
    string classCountText = words[4];

    if (!isValidName(name) || !isValidStudentId(studentId) || students.count(studentId) > 0) {
        return false;
    }
    if (!isNumber(residenceText) || !isNumber(classCountText) || classCountText.size() > 1) {
        return false;
    }

    int classCount = stoi(classCountText);
    if (classCount < 1 || classCount > 6 || (int)words.size() != 5 + classCount) {
        return false;
    }

    Student newStudent;
    newStudent.name = name;
    newStudent.studentId = studentId;
    newStudent.residenceId = stoi(residenceText);
    for (int i = 5; i < (int)words.size(); i++) {
        if (!isValidClassCode(words[i]) || newStudent.classCodes.count(words[i]) > 0) {
            return false;
        }
        newStudent.classCodes.insert(words[i]);
    }

    students[studentId] = newStudent;
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::removeStudent(const vector<string> &words) {
    if (words.size() != 2 || students.count(words[1]) == 0) {
        return false;
    }
    students.erase(words[1]);
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::dropClass(const vector<string> &words) {
    if (words.size() != 3) {
        return false;
    }
    string studentId = words[1];
    string classCode = words[2];
    if (students.count(studentId) == 0 || !isValidClassCode(classCode)) {
        return false;
    }

    Student &student = students[studentId];
    if (student.classCodes.count(classCode) == 0) {
        return false;
    }
    student.classCodes.erase(classCode);
    if (student.classCodes.empty()) {
        students.erase(studentId);
    }
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::replaceClass(const vector<string> &words) {
    if (words.size() != 4) {
        return false;
    }
    string studentId = words[1];
    string oldClassCode = words[2];
    string newClassCode = words[3];
    if (students.count(studentId) == 0 || !isValidClassCode(newClassCode)) {
        return false;
    }

    Student &student = students[studentId];
    if (student.classCodes.count(oldClassCode) == 0 || student.classCodes.count(newClassCode) > 0) {
        return false;
    }
    student.classCodes.erase(oldClassCode);
    student.classCodes.insert(newClassCode);
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::removeClass(const vector<string> &words) {
    if (words.size() != 2 || !isValidClassCode(words[1])) {
        return false;
    }
    string classCode = words[1];

    int droppedCount = 0;
    vector<string> emptyStudentIds;
    for (auto &entry : students) {
        Student &student = entry.second;
        if (student.classCodes.count(classCode) > 0) {
            student.classCodes.erase(classCode);
            droppedCount++;
            if (student.classCodes.empty()) {
                emptyStudentIds.push_back(student.studentId);
            }
        }
    }
    for (int i = 0; i < (int)emptyStudentIds.size(); i++) {
        students.erase(emptyStudentIds[i]);
    }

    if (droppedCount == 0) {
        return false;
    }
    cout << droppedCount << endl;
    return true;
}

bool CampusCompass::toggleEdgesClosure(const vector<string> &words) {
    if (words.size() < 2 || !isNumber(words[1]) || words[1].size() > 9) {
        return false;
    }
    int edgeCount = stoi(words[1]);
    if ((int)words.size() != 2 + edgeCount * 2) {
        return false;
    }
    for (int i = 2; i < (int)words.size(); i++) {
        if (!isNumber(words[i]) || words[i].size() > 9) {
            return false;
        }
    }

    for (int i = 2; i < (int)words.size(); i += 2) {
        int fromId = stoi(words[i]);
        int toId = stoi(words[i + 1]);
        campusGraph.toggleEdge(fromId, toId);
    }
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::checkEdgeStatus(const vector<string> &words) {
    if (words.size() != 3 || !isNumber(words[1]) || !isNumber(words[2])) {
        return false;
    }
    if (words[1].size() > 9 || words[2].size() > 9) {
        return false;
    }
    int fromId = stoi(words[1]);
    int toId = stoi(words[2]);

    if (!campusGraph.hasEdge(fromId, toId)) {
        cout << "DNE" << endl;
    } else if (campusGraph.isEdgeOpen(fromId, toId)) {
        cout << "open" << endl;
    } else {
        cout << "closed" << endl;
    }
    return true;
}

bool CampusCompass::isConnected(const vector<string> &words) {
    if (words.size() != 3 || !isNumber(words[1]) || !isNumber(words[2])) {
        return false;
    }
    if (words[1].size() > 9 || words[2].size() > 9) {
        return false;
    }
    int startId = stoi(words[1]);
    int endId = stoi(words[2]);

    if (campusGraph.isConnected(startId, endId)) {
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
    return true;
}

bool CampusCompass::printShortestEdges(const vector<string> &words) {
    if (words.size() != 2 || students.count(words[1]) == 0) {
        return false;
    }
    Student &student = students[words[1]];

    unordered_map<int, int> distances;
    unordered_map<int, int> parents;
    campusGraph.findShortestPaths(student.residenceId, distances, parents);

    cout << "Time For Shortest Edges: " << student.name << endl;
    for (const string &classCode : student.classCodes) {
        int classLocationId = classes[classCode].locationId;
        int travelTime = -1;
        if (distances.count(classLocationId) > 0) {
            travelTime = distances[classLocationId];
        }
        cout << classCode << ": " << travelTime << endl;
    }
    return true;
}

bool CampusCompass::printStudentZone(const vector<string> &words) {
    if (words.size() != 2 || students.count(words[1]) == 0) {
        return false;
    }
    Student &student = students[words[1]];

    unordered_map<int, int> distances;
    unordered_map<int, int> parents;
    campusGraph.findShortestPaths(student.residenceId, distances, parents);

    set<int> zoneLocationIds;
    zoneLocationIds.insert(student.residenceId);
    for (const string &classCode : student.classCodes) {
        int currentId = classes[classCode].locationId;
        if (distances.count(currentId) == 0) {
            continue;
        }
        while (currentId != student.residenceId) {
            zoneLocationIds.insert(currentId);
            currentId = parents[currentId];
        }
    }

    int zoneCost = campusGraph.findMinimumSpanningTreeCost(zoneLocationIds);
    cout << "Student Zone Cost For " << student.name << ": " << zoneCost << endl;
    return true;
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
