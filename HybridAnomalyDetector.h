/*
 * HybridAnomalyDetector.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
protected:


    virtual void learnCheckCorrelation(const TimeSeries &ts, int featIndex, int correlatedIndex, float pearsonValue);

    /*
     * The method creates a couple of correlated features and updates the fields of two correlated
     * features where the correlation value is between 0.5 to 0.9.
     */
    void learnNormalByCircle(const TimeSeries &ts, int feature1, int feature2, float pearson);

    /*
     * The method creates a couple of correlated features and updates the fields of two correlated
     * features where the correlation value is between 0.5 to 0.9.
    */
    void detectByCircle(const TimeSeries &ts, correlatedFeatures &features,
                                               vector<AnomalyReport> &reports, Point** points);
    virtual void detectCheckCorrelation(const TimeSeries &ts, correlatedFeatures &features,
                                        vector<AnomalyReport> &reports, Point **points);
public:
	HybridAnomalyDetector();

    virtual ~HybridAnomalyDetector();
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
