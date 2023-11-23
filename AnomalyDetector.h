#ifndef ANOMALYDETECTOR_H_
#define ANOMALYDETECTOR_H_

#include <string>
#include <vector>
#include "timeseries.h"
#include "math.h"

using namespace std;

/*
 * The class represents an anomaly in the test inputs. It consists of a string with the names of the features the
 * anomaly occurs in and the time step it occurs in.
 */
class AnomalyReport {
public:
    const string description;
    const long timeStep;


    //Constructor method of AnomalyReport class.
    AnomalyReport(string description, long timeStep) : description(description), timeStep(timeStep) {}
};

/*
 * An interface of a general anomaly detector.
 */
class TimeSeriesAnomalyDetector {
public:

    //The method receives a database of normal input and finds it's correlated features.
    virtual void learnNormal(const TimeSeries &ts) = 0;

    /*
     * The method receives a database and detects anomalies between the correlated features. A vector of reports about
     * the anomalies is returned.
     */
    virtual vector<AnomalyReport> detect(const TimeSeries &ts) = 0;

    //Destructor method.
    virtual ~TimeSeriesAnomalyDetector() {}
};

#endif /* ANOMALYDETECTOR_H_ */
