/*
 * CLI.cpp
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#include "CLI.h"

#define EXIT_OPTION 6

void CLI::start() {
    float chosenOption = 0;
    string optionString;
    DetectorInfo detectorInfo = DetectorInfo();
    //The menu runs until the user chooses the exit option.
    do {
        printMenu();
        //Choosing the command the user want to do.
        optionString = dio->read();
        chosenOption = stof(optionString);
        commandsList[chosenOption - 1]->execute(detectorInfo);
    } while (chosenOption != EXIT_OPTION);
}

void CLI::printMenu() {
    this->dio->write("Welcome to the Anomaly Detection Server.\nPlease choose an option:\n");
    int numberOfCommands = this->commandsList.size();
    //Printing the descriptions of each command.
    for (int i = 0; i < numberOfCommands; ++i) {
        string intToString = to_string((int) i + 1);
        this->dio->write(intToString);
        this->dio->write("." + this->commandsList[i]->getDescription() + "\n");
    }
}

CLI::~CLI() {
    for (Command *command: this->commandsList) {
        delete command;
    }
}

