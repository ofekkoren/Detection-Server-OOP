/*
 * CLI.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
    vector<Command*> commandsList;
    DefaultIO* dio;

public:
	CLI(DefaultIO* dio){
        this->dio=dio;
        commandsList.push_back(new Upload(this->dio));
        commandsList.push_back(new ChangeThresholdSettings(this->dio));
        commandsList.push_back(new Detect(this->dio));
        commandsList.push_back(new DisplayResults(this->dio));
        commandsList.push_back(new AnalyzeResults(this->dio));
        commandsList.push_back(new Exit(this->dio));
    }

	void start();
    void printMenu();
    virtual ~CLI();
};

#endif /* CLI_H_ */
