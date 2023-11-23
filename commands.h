/*
 * commands.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"
#include "timeseries.h"

#define STOP_UPLOAD1 "done\n"
#define STOP_UPLOAD2 "done"
#define ANOMALY_TEST_PATH "anomalyTest.csv"
#define ANOMALY_TRAIN_PATH "anomalyTrain.csv"

using namespace std;

class DefaultIO {
public:
    virtual string read() = 0;

    virtual void write(string text) = 0;

    virtual void write(float f) = 0;

    virtual void read(float *f) = 0;

    virtual ~DefaultIO() {}

    virtual void uploadFile(string fileDestination) {
        ofstream csv;
        csv.open(fileDestination);
        string line = read();
        while (line != STOP_UPLOAD2 && line != STOP_UPLOAD1) {
            csv << line << endl;
            line = read();
        }
        csv.close();
    }

    // you may add additional methods here
};

class StandardIO : public DefaultIO {
public:
    virtual void write(float f) override {
        cout << f;
    }

    virtual string read() override {
        string str;
        cin >> str;
        return str;
    }

    virtual void read(float *f) override {
        cin >> *f;
    }

    virtual void write(string text) override {
        cout << text;
    }
};


// you may add here helper classes
class SequentialReports {
    long timeStepStart;
    long timeStepEnd;
public:
    SequentialReports(long timeStepStart, long timeStepEnd) {
        this->timeStepStart = timeStepStart;
        this->timeStepEnd = timeStepEnd;
    }

    long getTimeStepStart() {
        return this->timeStepStart;
    }

    long getTimeStepEnd() {
        return this->timeStepEnd;
    }
};

class DetectorInfo {
private:
    TimeSeriesAnomalyDetector *anomalyDetector;
    vector<AnomalyReport> reports;
    vector<SequentialReports> sequentReports;
    float threshold;
    int numOfRows;

public:
    DetectorInfo() {
        this->threshold = 0.9;
        this->anomalyDetector = NULL;
    }

    TimeSeriesAnomalyDetector *getAnomalyDetector() {
        return this->anomalyDetector;
    }

    vector<AnomalyReport> getReports() {
        return this->reports;
    }

    vector<SequentialReports> getSequentReports() {
        return this->sequentReports;
    }

    float getThreshold() {
        return this->threshold;
    }

    int getNumOfRows() {
        return this->numOfRows;
    }

    void setThreshold(float threshold) {
        this->threshold = threshold;
    }

    void setReports(vector<AnomalyReport> &newReports) {
        vector<AnomalyReport> copy = newReports;
        this->reports.swap(copy);
    }

    void setSequentialReports(vector<SequentialReports> &newSequentReports) {
        vector<SequentialReports> copy = newSequentReports;
        this->sequentReports.swap(newSequentReports);
    }

    void setAnomalyDetector(TimeSeriesAnomalyDetector *newAnomalyDetector) {
        this->anomalyDetector = newAnomalyDetector;
    }

    void setNumOfRows(int numOfRows) {
        this->numOfRows = numOfRows;
    }

    ~DetectorInfo() {
        delete this->anomalyDetector;
    }
};

// you may edit this class
class Command {
protected:
    DefaultIO *dio;
    string description;

public:
    Command(DefaultIO *dio, string description) : dio(dio) {
        this->description = description;
    }

    virtual void execute(DetectorInfo &detectorInfo) = 0;

    string getDescription() {
        return this->description;
    }

    virtual ~Command() {}
};


class Upload : public Command {
public:
    Upload(DefaultIO *dio) : Command(dio, "upload a time series csv file") {};

    void execute(DetectorInfo &detectorInfo) override {
        this->dio->write("Please upload your local train CSV file.\n");
        this->dio->uploadFile(ANOMALY_TRAIN_PATH);
        this->dio->write("Upload complete.\n");
        this->dio->write("Please upload your local test CSV file.\n");
        this->dio->uploadFile(ANOMALY_TEST_PATH);
        this->dio->write("Upload complete.\n");
        HybridAnomalyDetector *anomalyDetector = new HybridAnomalyDetector();
        detectorInfo.setAnomalyDetector(anomalyDetector);
        anomalyDetector->learnNormal(TimeSeries(ANOMALY_TRAIN_PATH));
    }
};


class Detect : public Command {
public:
    Detect(DefaultIO *dio) : Command(dio, "detect anomalies") {};

    void execute(DetectorInfo &detectorInfo) override {
        TimeSeries timeSeries = TimeSeries(ANOMALY_TEST_PATH);
        vector<AnomalyReport> anomalyReports = detectorInfo.getAnomalyDetector()->detect(timeSeries);
        detectorInfo.setNumOfRows(timeSeries.getNumOfValuesInFeature());
        detectorInfo.setReports(anomalyReports);
        vector<SequentialReports> sequentialReports = uniteToSequentialReports(anomalyReports);
        detectorInfo.setSequentialReports(sequentialReports);
        this->dio->write("anomaly detection complete.\n");
    }

private:
    vector<SequentialReports> uniteToSequentialReports(vector<AnomalyReport> &singleReports) {
        vector<SequentialReports> sequentialReports = vector<SequentialReports>();
        int singleReportsSize = singleReports.size();
        int i = 0;
        int j;
        while (i < singleReportsSize) {
            //Getting the description and the beginning time step of the current anomaly.
            long currentStart = singleReports[i].timeStep;
            long currentEnd = singleReports[i].timeStep;
            string currentDescription = singleReports[i].description;
            //The loop finds until which time step the current anomaly continues.
            for (j = i + 1; j < singleReportsSize; ++j) {

                /*
                 * Check if the description of the j anomaly is the same as the current anomaly and if the j timestep
                 * follows the latest timestep of the current anomaly. If so, then the j anomaly is part of the
                 * sequence of the current anomaly and the end time of the sequence will be updated to j.
                 */
                if (currentDescription.compare(singleReports[j].description) == 0 &&
                    currentEnd + 1 == singleReports[j].timeStep) {
                    currentEnd++;
                } else {
                    break;
                }
            }
            //Adding the current sequence to the anomaly sequence vector.
            sequentialReports.push_back(SequentialReports(currentStart, currentEnd));
            i = j;
        }
        return sequentialReports;
    }
};


class DisplayResults : public Command {
public:
    DisplayResults(DefaultIO *dio) : Command(dio, "display results") {};

    void execute(DetectorInfo &detectorInfo) override {
        vector<AnomalyReport> anomalyReports = detectorInfo.getReports();
        for (AnomalyReport report: anomalyReports) {
            this->dio->write(report.timeStep);
            this->dio->write("\t" + report.description + "\n");
        }
        this->dio->write("Done.\n");
    }
};


class ChangeThresholdSettings : public Command {
public:
    ChangeThresholdSettings(DefaultIO *dio) : Command(dio, "algorithm settings") {};

    void execute(DetectorInfo &detectorInfo) override {
        float chosenThreshold = 0;
        bool validThreshold = false;
        while (!validThreshold) {
            dio->write("The current correlation threshold is ");
            dio->write(detectorInfo.getThreshold());
            dio->write("\n");
            dio->write("Type a new threshold\n");
            chosenThreshold = std::stof(dio->read());
            if (!(chosenThreshold > 0 && chosenThreshold <= 1)) {
                dio->write("please choose a value between 0 and 1.\n");
            } else {
                validThreshold = true;
                detectorInfo.setThreshold(chosenThreshold);
            }
        }
    }
};


class AnalyzeResults : public Command {
public:
    AnalyzeResults(DefaultIO *dio) : Command(dio, "upload anomalies and analyze results") {};

    vector<SequentialReports> createUserReports() {
        dio->write("Please upload your local anomalies file.\n");
        vector<SequentialReports> usersReports;
        string report = "";
        report = dio->read();
        while (report != STOP_UPLOAD2 && report != STOP_UPLOAD1) {
            stringstream ssReports(report);
            string timeStepStart, timeStepEnd;
            getline(ssReports, timeStepStart, ',');
            getline(ssReports, timeStepEnd, ',');
            usersReports.push_back(SequentialReports(stol(timeStepStart), stol(timeStepEnd)));
            report = dio->read();
        }
        this->dio->write("Upload complete.\n");
        return usersReports;
    }

    int numOfUnreportedLines(DetectorInfo &detectorInfo, vector<SequentialReports> usersReports) {
        int numOfLines = detectorInfo.getNumOfRows();
        int numOfUserReports = usersReports.size();
        int reportedTimeSteps = 0;
        for (int i = 0; i < numOfUserReports; ++i) {
            reportedTimeSteps += (usersReports[i].getTimeStepEnd() - usersReports[i].getTimeStepStart() + 1);
        }
        return numOfLines - reportedTimeSteps;
    }

    void printResults(DetectorInfo &detectorInfo, vector<SequentialReports> usersReports, int tp, int fp) {
        float truePositiveRate = tp / (float) usersReports.size();
        float falsePositiveRate = fp / (float) numOfUnreportedLines(detectorInfo, usersReports);
        //rounding 3 decimal digits after point.
        truePositiveRate = ((int)(truePositiveRate * 1000.0)) / 1000.0f;
        falsePositiveRate = ((int)(falsePositiveRate * 1000.0)) / 1000.0f;
        dio->write("True Positive Rate: ");
        dio->write(truePositiveRate);
        dio->write("\nFalse Positive Rate: ");
        dio->write(falsePositiveRate);
        dio->write("\n");
    }


    void execute(DetectorInfo &detectorInfo) override {
        vector<SequentialReports> usersReports = createUserReports();
        vector<SequentialReports> reports = detectorInfo.getSequentReports();
        int numOfUserReports = usersReports.size();
        int numOfReports = detectorInfo.getSequentReports().size();
        int tp = 0, fp = reports.size();
        bool trueReport;
        for (int i = 0; i < numOfReports; ++i) {
            trueReport = false;
            for (int j = 0; j < numOfUserReports; ++j) {
                // If there is a match in a report, swap the report of the user to the end
                // and decrease the number of reports left in the users reports and end the inner loop.
                if(!((usersReports[j].getTimeStepStart() > reports[i].getTimeStepEnd())
                    || (reports[i].getTimeStepStart() > usersReports[j].getTimeStepEnd()))) {
                    swap(usersReports[j], usersReports[numOfUserReports - 1]);
                    numOfUserReports--;
                    tp++;
                    fp--;
                    trueReport = true;
                    j = numOfUserReports;
                }
            }
        }
        printResults(detectorInfo, usersReports, tp, fp);
    }
};


class Exit : public Command {
public:
    Exit(DefaultIO *dio) : Command(dio, "exit") {};

    void execute(DetectorInfo &detectorInfo) override {
    }
};
// implement here your command classes



#endif /* COMMANDS_H_ */
