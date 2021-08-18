#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include "DaumErgoPremium8i.h"
#include "DaumErgo8008TRS.h"
#include "AntService.h"


#define USAGE_COL_WIDTH 25

// command line arguments
#define PROG_NAME "daum-ergoant"
#define VERBOSE "-v"
#define HELP "-h"


static void showUsage() {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" PROG_NAME " <option(s)> BIKETYPE [PARAMETERS] <ANT device number>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout <<  "\t" << std::left << std::setw(USAGE_COL_WIDTH) << HELP;
    std::cout << "Show this help message" << std::endl;
    std::cout << "\t" << std::left << std::setw(USAGE_COL_WIDTH) << VERBOSE;
    std::cout << "Verbose output" << std::endl;
    std::cout << "Biketypes:" << std::endl;
    std::cout << "\t" << std::left << std::setw(USAGE_COL_WIDTH) << ARG_P8I " [IP ADDRESS]";
    std::cout << "Premium 8i" << std::endl;
    std::cout << "\t" << std::left << std::setw(USAGE_COL_WIDTH) << ARG_8K8TRS " [SERIAL_PORT]";
    std::cout <<  "8008trs:" << std::endl;
    std::cout << std::left << std::setw(USAGE_COL_WIDTH) << "ANT device number:" << std::endl;
    std::cout << "\t0 for first USB stick plugged etc." << std::endl;

}

bool parseArguments(bool *verbose, int *antDeviceNr, DaumErgo **ergo, std::string *param, int argc, char**argv) {
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == HELP) {
            showUsage();
            return false;
        } else if (arg == VERBOSE) {
            *verbose = true;
        } else if (arg == ARG_P8I) {
            *param = argv[i+1];
            *antDeviceNr = std::stoi(argv[i+2], nullptr, 10);
            *ergo = new DaumErgoPremium8i(*verbose);
            return true;
        } else if (arg == ARG_8K8TRS) {
            *param = argv[i+1];
            *antDeviceNr = std::stoi(argv[i + 2], nullptr, 10);
            *ergo = new DaumErgo8008TRS(*verbose);
            return true;
        } else if (i == argc - 1) {
            std::cerr << "Not valid arguments" << std::endl;
            showUsage();
            return false;
        }
    }
    return false;
}

int run(DaumErgo *ergo, const std::string& param, char antDeviceNr, bool verbose) {
    if (!ergo->Init(param.c_str()))
        return 1;
    if (!ergo->RunDataUpdater())
        return 1;

    auto* antService = new AntService(ergo, verbose);
    if (!antService->Init(antDeviceNr)) {
        ergo->Close();
        return 1;
    }

    std::cout << "Started to transmit data" << std::endl;
    std::string input;
    while (input != "q") {
        std::cout << "Enter q to quit: ";
        std::cin >> input;
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

    bool verbose;
    DaumErgo *ergo = nullptr;
    std::string param;
    int antDeviceNr;

    if (!parseArguments(&verbose, &antDeviceNr, &ergo, &param, argc, argv))
        return 1;

    return run(ergo, param, (char)antDeviceNr, verbose);
}


