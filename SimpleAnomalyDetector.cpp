/*
 * SimpleAnomalyDetector.cpp
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {
    this->cf = vector<correlatedFeatures>();
    this->threshold = STRONGER_THRESHOLD;
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
}

Point **SimpleAnomalyDetector::createPointsOfPairs(vector<float> feature1, vector<float> feature2) {
    //Getting the size of the feature vectors which assumed to have the same size.
    int size = feature1.size();
    Point **points = new Point *[size];
    //Create points of pairs of values from the two input vectors in the same time step.
    for (int i = 0; i < size; ++i) {
        Point *newPoint = new Point(feature1.at(i), feature2.at(i));
        points[i] = newPoint;
    }
    return points;
}

float SimpleAnomalyDetector::findThreshold(Point **points, Line regLine, int size) {
    float maxThreshold = 0;
    //Finding the maximal deviation of a point from the regression line.
    for (int i = 0; i < size; ++i) {
        float distance = dev((*(*(points + i))), regLine);
        if (distance > maxThreshold) {
            maxThreshold = distance;
        }
    }
    return maxThreshold;
}

void SimpleAnomalyDetector::deletePoints(Point **points, int size) {
    for (int d = 0; d < size; ++d) {
        delete points[d];
    }
}

vector<int> SimpleAnomalyDetector::findCorrelatedFeatures(const TimeSeries &ts) {
    //Getting the number of features in the database.
    int sizeOfTs = ts.getNumOfFeatures();
    vector<int> mostCorrelated = vector<int>(sizeOfTs + 1);
    //The loops find the correlated features in the database.
    for (int i = 1; i <= sizeOfTs; ++i) {
        float maxCorrelation = 0;
        int correlatedIndex = -1;

        /*
         * The loop finds for every feature it's the most correlated feature from it's Succeeding
         * features in the database.
         */
        for (int j = i + 1; j <= sizeOfTs; ++j) {
            //Calculating the Pearson correlation coefficient value of the features which indicates their correlation.
            float pearsonValue = pearson(&(ts.getFeatureValuesVector(i)[0]), &(ts.getFeatureValuesVector(j)[0]),
                                         ts.getNumOfValuesInFeature());
            if (std::abs(maxCorrelation) < std::abs(pearsonValue)) {
                maxCorrelation = pearsonValue;
                correlatedIndex = j;
            }
            //mostCorrelated[i] = correlatedIndex;
        }
        mostCorrelated[i] = correlatedIndex;
    }
    return mostCorrelated;
}

void SimpleAnomalyDetector::learnCheckCorrelation(const TimeSeries &ts, int featIndex, int correlatedIndex,
                                                  float pearsonValue) {
    /*
    * if the Pearson value of the features is greater than the minimal threshold value we consider them as
    * correlated features. Their indexes and pearson value are sent to a helper method which adds the
    * features to the correlated features vector of the detector.
    */
    if (pearsonValue > getThreshold()) {
        learnNormalByLine(ts, featIndex, correlatedIndex, pearsonValue);
    }

}

void SimpleAnomalyDetector::learnNormalByLine(const TimeSeries &ts, int featIndex, int correlatedIndex,
                                              float pearsonValue) {

    /*
     * We create points to represent the values of the feature vectors at the same time step , calculating the
     * regressions line created by the corresponding values of the features and finding the maximal deviation between
     * a point and the regression line. Then we add the features to the correlated features vector of the detector.
     */
    Point **points = createPointsOfPairs(ts.getFeatureValuesVector(featIndex),
                                         ts.getFeatureValuesVector(correlatedIndex));
    Line regLine = linear_reg(points, ts.getNumOfValuesInFeature());
    float newThreshold = findThreshold(points, regLine, ts.getNumOfValuesInFeature());
    correlatedFeatures pair = correlatedFeatures(ts.getFeatureName(featIndex), ts.getFeatureName(correlatedIndex),
                                                 regLine, pearsonValue, newThreshold);
    //Deallocating the memory allocated for points.
    deletePoints(points, ts.getNumOfValuesInFeature());
    delete[] points;
    this->cf.push_back(pair);
}

void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    vector<int> mostCorrelated = findCorrelatedFeatures(ts);
    int sizeOfMostCorrelated = mostCorrelated.size();
    /*
     * If we found a pair of correlated features and if their Pearson value is greater than the minimal threshold
     * value we consider them as correlated features.  add the
     * features to the correlated features vector of the detector.
     */
    //START FROM 1 AND END IN N (NOT N-1)
    for (int i = 1; i < sizeOfMostCorrelated; ++i) {
        int correlatedIndex = mostCorrelated[i];
        //If the index of the most Correlated is -1 it indicates that feature i doesn't have Correlated feature.
        if (mostCorrelated[i] == -1)
            continue;
        //Calculating the Pearson correlation coefficient value of the features, which indicates their correlation.
        float pearsonValue = pearson(&(ts.getFeatureValuesVector(i)[0]),
                                     &(ts.getFeatureValuesVector(correlatedIndex)[0]), ts.getNumOfValuesInFeature());
        //The helper method checks if the features are Correlated and if so, adds them as Correlated features.
        learnCheckCorrelation(ts, i, correlatedIndex, pearsonValue);
    }
}

void SimpleAnomalyDetector::detectCheckCorrelation(const TimeSeries &ts, correlatedFeatures &features,
                                                   vector<AnomalyReport> &reports, Point **points) {
    detectByLine(ts, features, reports, points);
}

void SimpleAnomalyDetector::detectByLine(const TimeSeries &ts, correlatedFeatures &features,
                                         vector<AnomalyReport> &reports, Point **points) {
    float maxDistance = 0;
    long maxIndex = -1;
    //Finding the pair point with the maximal deviation from the regression line of the features.
    int numOfPoints = ts.getNumOfValuesInFeature();
    for (int j = 0; j < numOfPoints; ++j) {
        float distance = dev(*(points[j]), features.lin_reg);

        /*
         * If we find a point with a deviation bigger than the threshold we consider it as an anomaly and adding it
         * to the anomaly vector. The threshold is multiplied by a scaling value to prevent false anomaly alarms.
         */
        if (features.threshold * THRESHOLD_SCALE < distance) {
            addAnomalyReport(reports, features, j);
        }
    }
}

void
SimpleAnomalyDetector::addAnomalyReport(vector<AnomalyReport> &reports, correlatedFeatures &features, long timeStep) {
    string report = features.feature1 + "-" + features.feature2;
    AnomalyReport anomalyReport = AnomalyReport(report, timeStep + 1);
    reports.push_back(anomalyReport);
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts) {
    //The vector will hold the reports about the anomalies that will be found.
    vector<AnomalyReport> reports = vector<AnomalyReport>();
    //The loop detects the anomalies between the features that should correlate with each other.
    for (int i = 0; i < this->cf.size(); ++i) {
        Point **points;
        //Getting the values vector of the correlated features.
        vector<float> feature1 = ts.getFeatureVectorByName(this->cf.at(i).feature1);
        vector<float> feature2 = ts.getFeatureVectorByName(this->cf.at(i).feature2);
        //Create points to represent the values of the feature vectors at the same time step.
        points = createPointsOfPairs(feature1, feature2);
        detectCheckCorrelation(ts, this->cf[i], reports, points);
        //Deallocating the memory allocated for points.
        deletePoints(points, ts.getNumOfValuesInFeature());
        delete[] points;
    }
    return reports;
}



