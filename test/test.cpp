#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// change if you choose to use a different header name
#include "CampusCompass.h"

using namespace std;

string runCommands(CampusCompass &compass, const vector<string> &commands) {
    stringstream capturedOutput;
    streambuf *originalOutput = cout.rdbuf(capturedOutput.rdbuf());
    for (int i = 0; i < (int)commands.size(); i++) {
        compass.ParseCommand(commands[i]);
    }
    cout.rdbuf(originalOutput);
    return capturedOutput.str();
}

TEST_CASE("Incorrect commands are rejected", "[invalid]") {
    CampusCompass compass;
    REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));

    vector<string> badCommands = {
        "insert \"A11y\" 45679999 1 1 COP3530",
        "insert \"Brian\" 4567999 1 1 COP3530",
        "insert \"Brian\" 45679999 1 1 cop3530",
        "insert \"Brian\" 45679999 1 2 COP3530",
        "insert \"Brian\" 45679999 1 7 COP3530 COP3502 COP3503 COP3504 MAC2311 MAC2312 MAC2313",
        "insert Brian 45679999 1 1 COP3530",
        "insret \"Brian\" 45679999 1 1 COP3530",
        "insert \"Brian\" 45679999 1 1 ABC1234",
    };

    for (int i = 0; i < (int)badCommands.size(); i++) {
        string output = runCommands(compass, {badCommands[i]});
        REQUIRE(output == "unsuccessful\n");
    }
}

TEST_CASE("Edge cases", "[edge]") {
    CampusCompass compass;
    REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));

    SECTION("removing a student that does not exist") {
        REQUIRE(runCommands(compass, {"remove 12345678"}) == "unsuccessful\n");
    }

    SECTION("inserting a duplicate student ID") {
        string output = runCommands(compass, {
            "insert \"Brian\" 12345678 1 1 COP3530",
            "insert \"Briana\" 12345678 2 1 MAC2311",
        });
        REQUIRE(output == "successful\nunsuccessful\n");
    }

    SECTION("dropping a class the student does not have") {
        string output = runCommands(compass, {
            "insert \"Brian\" 12345678 1 1 COP3530",
            "dropClass 12345678 MAC2311",
        });
        REQUIRE(output == "successful\nunsuccessful\n");
    }

    SECTION("removing a class no student has") {
        REQUIRE(runCommands(compass, {"removeClass COP3530"}) == "unsuccessful\n");
    }

    SECTION("checking an edge that does not exist") {
        REQUIRE(runCommands(compass, {"checkEdgeStatus 1 999"}) == "DNE\n");
    }
}

TEST_CASE("dropClass, removeClass, remove, and replaceClass", "[classes]") {
    CampusCompass compass;
    REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));

    runCommands(compass, {
        "insert \"Brandon\" 45679999 20 2 COP3530 MAC2311",
        "insert \"Brian\" 35459999 21 3 COP3530 CDA3101 MAC2311",
        "insert \"Briana\" 87879999 22 1 COP3530",
    });

    SECTION("dropClass") {
        string output = runCommands(compass, {
            "dropClass 45679999 COP3530",
            "dropClass 45679999 COP3530",
            "dropClass 87879999 COP3530",
            "remove 87879999",
        });
        REQUIRE(output == "successful\nunsuccessful\nsuccessful\nunsuccessful\n");
    }

    SECTION("removeClass") {
        string output = runCommands(compass, {
            "removeClass COP3530",
            "removeClass COP3530",
            "remove 87879999",
        });
        REQUIRE(output == "3\nunsuccessful\nunsuccessful\n");
    }

    SECTION("remove") {
        string output = runCommands(compass, {
            "remove 35459999",
            "remove 35459999",
            "dropClass 35459999 MAC2311",
        });
        REQUIRE(output == "successful\nunsuccessful\nunsuccessful\n");
    }

    SECTION("replaceClass") {
        string output = runCommands(compass, {
            "replaceClass 45679999 COP3530 COP3502",
            "replaceClass 45679999 COP3530 COP3503",
            "replaceClass 45679999 COP3502 MAC2311",
            "replaceClass 45679999 MAC2311 ZZZ9999",
            "replaceClass 11111111 MAC2311 MAC2312",
            "dropClass 45679999 COP3530",
            "dropClass 45679999 COP3502",
        });
        REQUIRE(output == "successful\nunsuccessful\nunsuccessful\nunsuccessful\nunsuccessful\nunsuccessful\nsuccessful\n");
    }
}

TEST_CASE("printShortestEdges after closing edges", "[shortest]") {
    CampusCompass compass;
    REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));

    // location 1 (Hume Hall) only connects to 2, 4, and 50
    string before = runCommands(compass, {
        "insert \"Hume\" 11111111 1 2 MAC2311 COP3530",
        "printShortestEdges 11111111",
    });
    REQUIRE(before == "successful\nTime For Shortest Edges: Hume\nCOP3530: 23\nMAC2311: 18\n");

    string after = runCommands(compass, {
        "toggleEdgesClosure 3 1 2 1 4 1 50",
        "isConnected 1 50",
        "printShortestEdges 11111111",
    });
    REQUIRE(after == "successful\nunsuccessful\nTime For Shortest Edges: Hume\nCOP3530: -1\nMAC2311: -1\n");

    string reopened = runCommands(compass, {
        "toggleEdgesClosure 3 1 2 1 4 1 50",
        "printShortestEdges 11111111",
    });
    REQUIRE(reopened == "successful\nTime For Shortest Edges: Hume\nCOP3530: 23\nMAC2311: 18\n");
}

TEST_CASE("Example CampusCompass Output Test", "[flag]") {
    string input = R"(6
insert "Student A" 10000001 1 1 COP3502
insert "Student B" 10000002 1 1 COP3502
insert "Student C" 10000003 1 2 COP3502 MAC2311
dropClass 10000001 COP3502
remove 10000001
removeClass COP3502
)";

    string expectedOutput = R"(successful
successful
successful
successful
unsuccessful
2
)";

    CampusCompass compass;
    REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));

    stringstream inputStream(input);
    string line;
    getline(inputStream, line);
    int commandCount = stoi(line);
    vector<string> commands;
    for (int i = 0; i < commandCount; i++) {
        getline(inputStream, line);
        commands.push_back(line);
    }

    string actualOutput = runCommands(compass, commands);
    REQUIRE(actualOutput == expectedOutput);
}
