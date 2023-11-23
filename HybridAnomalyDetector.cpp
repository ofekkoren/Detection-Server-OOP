/*
 * HybridAnomalyDetector.cpp
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {
    // TODO Auto-generated constructor stub

}

HybridAnomalyDetector::~HybridAnomalyDetector() {
    // TODO Auto-generated destructor stub
}
//virtual void learnCheckCorrelation (const TimeSeries &ts, int featIndex, int correlatedIndex, float pearsonValue);

void HybridAnomalyDetector::learnCheckCorrelation(const TimeSeries &ts, int featIndex, int correlatedIndex,
                                                  float pearsonValue) {
    //Check if correlation > 0.9
    if (pearsonValue > this->getThreshold()) {
        learnNormalByLine(ts, featIndex, correlatedIndex, pearsonValue);
    } else {
        //If correlation is bigger than 0.5 and smaller than 0.9
        if (pearsonValue > WEAKER_THRESHOLD) {
            learnNormalByCircle(ts, featIndex, correlatedIndex, pearsonValue);
        }
    }
}

void HybridAnomalyDetector::learnNormalByCircle(const TimeSeries &ts, int featIndex,
                                                int correlatedIndex, float pearsonValue) {
    Point **points = createPointsOfPairs(ts.getFeatureValuesVector(featIndex),
                                         ts.getFeatureValuesVector(correlatedIndex));
    Circle circle = findMinCircle(points, ts.getNumOfValuesInFeature());

    float thresholdBYRadius = circle.radius;
    Point center = circle.center;
    correlatedFeatures pair = correlatedFeatures(ts.getFeatureName(featIndex), ts.getFeatureName(correlatedIndex),
                                                 center, pearsonValue, thresholdBYRadius);
    deletePoints(points, ts.getNumOfValuesInFeature());
    delete[] points;
    this->cf.push_back(pair);
}

void HybridAnomalyDetector::detectByCircle(const TimeSeries &ts, correlatedFeatures &features,
                                           vector<AnomalyReport> &reports, Point **points) {
    float maxDistance = 0;
    long maxIndex = -1;
    int numberOfVlaues = ts.getNumOfValuesInFeature();
    //Finding the pair point with the maximal deviation from the regression line of the features.
    for (int j = 0; j < numberOfVlaues; ++j) {
        float dist = distance(*(points[j]), features.enclosingCircleCenter);
        if ((features.threshold) * THRESHOLD_SCALE < dist) {
            addAnomalyReport(reports, features, j);
        }
    }
}

void HybridAnomalyDetector::detectCheckCorrelation(const TimeSeries &ts, correlatedFeatures &features,
                                                   vector<AnomalyReport> &reports, Point **points) {
    if (features.corrlation > getThreshold()) {
        detectByLine(ts, features, reports, points);
    } else {
        detectByCircle(ts, features, reports, points);
    }
}