#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

#include "DaumErgoPremium8i.h"
#include "DaumErgo8008TRS.h"
#include "AntService.h"

#include "AntBikeSpeedCadence.h"
#include "AntBikePower.h"
#include "AntFECProfile.h"
#include "TestErgo.h"

using namespace std;
#define USAGE_COL_WIDTH 25

// command line arguments
#define PROG_NAME "daum-ergoant"
#define VERBOSE "-v"
#define HELP "-h"
#define WORKOUT "-w"

bool verbose = false;
DaumErgo *ergo = nullptr;
string param;
int antDeviceNr;
vector<tuple<int, int>> workout;


static void showUsage() {
    cout << "Usage: " << endl;
    cout << "\t" PROG_NAME " <option(s)> BIKETYPE [PARAMETERS] [ANT device number]" << endl;
    cout << "Options:" << endl;
    cout <<  "\t" << left << setw(USAGE_COL_WIDTH) << HELP;
    cout << "Show this help message" << endl;
    cout << "\t" << left << setw(USAGE_COL_WIDTH) << VERBOSE;
    cout << "Verbose output" << endl;
    cout << "\t" << left << setw(USAGE_COL_WIDTH) << WORKOUT " [filename]";
    cout << "Workout to run. File with two numbers per line: TIME WATT" << endl;
    cout << "Biketypes:" << endl;
    cout << "\t" << left << setw(USAGE_COL_WIDTH) << ARG_P8I " [IP ADDRESS]";
    cout << "Premium 8i" << endl;
    cout << "\t" << left << setw(USAGE_COL_WIDTH) << ARG_8K8TRS " [SERIAL_PORT]";
    cout <<  "8008trs:" << endl;
    cout << left << setw(USAGE_COL_WIDTH) << "ANT device number:" << endl;
    cout << "\t0 for first USB stick plugged etc." << endl;

}

bool parseWorkout(char *filename) {
    ifstream infile(filename);
    if (infile.fail()) {
        cerr << "Workout file does not exist" << endl;
        return false;
    }
    int time, watt;
    while (infile >> time >> watt) {
        workout.emplace_back(time, watt);
    }
    return true;
}


bool parseArguments(int argc, char**argv) {
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];
        if (arg == HELP) {
            showUsage();
            return false;
        } else if (arg == VERBOSE) {
            verbose = true;
        } else if (arg == WORKOUT) {
            if (!parseWorkout(argv[i+1]))
                return false;
        } else if (arg == ARG_P8I) {
            param = argv[i+1];
            antDeviceNr = stoi(argv[i+2], nullptr, 10);
            ergo = new DaumErgoPremium8i(verbose);
            return true;
        } else if (arg == ARG_8K8TRS) {
            param = argv[i+1];
            antDeviceNr = stoi(argv[i + 2], nullptr, 10);
            ergo = new DaumErgo8008TRS(verbose);
            return true;
        } else if (arg == ARG_TEST) {
            antDeviceNr = stoi(argv[i + 1], nullptr, 10);
            ergo = new TestErgo();
            ((TestErgo*) ergo)->SetPower((unsigned short) 25);
            ((TestErgo*) ergo)->SetSpeed((unsigned short) 30);
            ((TestErgo*) ergo)->SetCadence((unsigned short) 80);
            return true;
        } else if (i == argc - 1) {
            cerr << "Not valid arguments" << endl;
            showUsage();
            return false;
        }
    }
    return false;
}

int run() {
    if (!ergo->Init(param.c_str())) {
        std::cerr << "Failed to initialise Ergo" << std::endl;
        return 1;
    }
    if (!ergo->RunDataUpdater()) {
        std::cerr << "Failed to run Ergo data updater " << std::endl;
        return 1;
    }

    auto *antService = new AntService(verbose);
    auto *antFec = new AntFECProfile(ergo);
    if (!antService->AddAntProfile(antFec)) {
        ergo->Close();
    }

    if (!antService->Init(antDeviceNr)) {
        ergo->Close();
        return 1;
    }

    if (!workout.empty()) {
        if (ergo->RunWorkout(workout))
            cout << "Starting workout in 10s..." << endl;
        else
            return 1;
    }

    cout << "Started to transmit data" << endl;
    string input;
    while (input != "q") {
        cout << "Enter q to quit: ";
        cin >> input;
    }

    antService->Close();
    ergo->Close();
    return 0;
}


int main(int argc, char**argv) {
    if (argc < 4) {
        showUsage();
        return 1;
    }

    if (!parseArguments(argc, argv))
        return 1;

    return run();
}


