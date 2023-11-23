/*
 * SimpleAnomalyDetector.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

//The minimal Pearson correlation coefficient value for features to be considered correlated.
#define WEAKER_THRESHOLD 0.5
//A stronger Pearson correlation coefficient value for correlated features.
#define STRONGER_THRESHOLD 0.9
//Value to scale and prevent false anomaly alarms.
#define THRESHOLD_SCALE 1.1

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

/*
 * A struct that represents a pair of correlated features from a database.
 */
struct correlatedFeatures {
    // names of the correlated features
    string feature1, feature2;
    //Their Pearson correlation coefficient value.
    float corrlation;
    //The regression line created between the values of the features. Used if corrlation >= STRONGER_THRESHOLD
    Line lin_reg;
    //The maximal distance between a (feature1,feature2) pair in some time step and the regression line.
    float threshold;
    /*
     * The center of the circle that encloses all the pair points created between the features . Used if
     * STRONGER_THRESHOLD > correlation >= WEAKER_THRESHOLD
     */
    Point enclosingCircleCenter;


    //Constructor method of correlatedFeatures struct with line_reg field.
    correlatedFeatures(string feature1, string feature2, float corrlation, float threshold)
            : enclosingCircleCenter(Point(0, 0)) {
        this->feature1 = feature1;
        this->feature2 = feature2;
        this->lin_reg = {};
        this->corrlation = corrlation;
        this->threshold;
    }

    //Constructor method of correlatedFeatures struct with line_reg field.
    correlatedFeatures(string feature1, string feature2, Line lineReg, float corrlation, float threshold)
            : enclosingCircleCenter(Point(0, 0)) {
        this->feature1 = feature1;
        this->feature2 = feature2;
        this->lin_reg = lineReg;
        this->corrlation = corrlation;
        this->threshold = threshold;
    }

    //Constructor method of correlatedFeatures struct with enclosingCircleCenter.
    correlatedFeatures(string feature1, string feature2, Point circleCenter, float corrlation,
                       float threshold) : enclosingCircleCenter(circleCenter) {
        this->feature1 = feature1;
        this->feature2 = feature2;
        this->lin_reg = {};
        this->corrlation = corrlation;
        this->threshold = threshold;
    }
};

/*
 * A simple anomaly detector that implements the TimeSeriesAnomalyDetector interface.
 */
class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector {
protected:
    //Vector of the correlated features in the normal database input the detector operates on.
    vector<correlatedFeatures> cf;
    //The minimal Pearson correlation coefficient value for features to be considered correlated.
    float threshold;


    /*
     * Returns a vector with each index stores it's most correlated feature index. For example if A[i]=j so j is the
     * most correlated feature to the i'th feature in ts. The index 0 is not used.
     */
    vector<int> findCorrelatedFeatures(const TimeSeries &ts);

    /*
     * The method returns a pointer to an array of point pointers. Each pointer in the array points to a pair of
     * values from the two input vectors in the same time step.
     */
    Point **createPointsOfPairs(vector<float> feature1, vector<float> feature2);

    /*
     * Returning the maximal deviation between a (feature1,feature2) pair in some time step and the regression line.
     * Point **points - an array of point pointers of (feature1,feature2) pairs in the same time step.
     * Line regLine - the regression line created by the values of the two features.
     * int size - the length of points.
     */
    float findThreshold(Point **points, Line regLine, int size);


    //The method frees allocated memory for Point **points.
    void deletePoints(Point **points, int size);

    /*
     * The method checks the correlation level of two features and sends them to a helper method according to
     * their correlation level or doesn't send them at all if they are not correlated enough. This way,
     * their correlation learning method is adjusted to their correlation level.
     * const TimeSeries &ts - database of a table with feature names and their values at certain time steps.
     * int featIndex - index of a feature in ts
     * int correlatedIndex - the most correlated feature to the feature of featIndex.
     * float pearsonValue - the pearson value that was calculated for both features mentioned above.
     */
    virtual void learnCheckCorrelation(const TimeSeries &ts, int featIndex, int correlatedIndex, float pearsonValue);

    /*
    * The method checks the correlation level of two features and sends them to a helper method according to
    * their correlation level. This way, the anomaly detection method is adjusted to their correlation level.
    * const TimeSeries &ts - database of a table with feature names and their values at certain time steps.
    * correlatedFeatures &features - A pair of features which were found correlative.
    * vector<AnomalyReport> &reports - A vector of reports about anomalies between correlated features. If an
    *                                  anomaly will be detected it will be added to this vector.
    * Point **points - array of points that represent the values of the feature vectors at the same time step.
    */
    virtual void detectCheckCorrelation(const TimeSeries &ts, correlatedFeatures &features,
                                        vector<AnomalyReport> &reports, Point **points);

    /*
    * Adding a pair of correlated features to cf based on their regression line (meant for correlation above
    * STRONGER_THRESHOLD.)
    */
    void learnNormalByLine(const TimeSeries &ts, int featIndex, int correlatedIndex, float pearsonValue);

    /*
    * The method detects an anomaly between to correlated features based on the distance of the points from the
    * regression line created by the features.
    * const TimeSeries &ts - database of a table with feature names and their values at certain time steps.
    * correlatedFeatures &features - A pair of features which were found correlative.
    * vector<AnomalyReport> &reports - A vector of reports about anomalies between correlated features. If an
    *                                  anomaly will be detected it will be added to this vector.
    * Point **points - array of points that represent the values of the feature vectors at the same time step.
    */
    void detectByLine(const TimeSeries &ts, correlatedFeatures &features, vector<AnomalyReport> &reports,
                      Point **points);

    /*
     * The method adds an anomaly report the the reports vector.
     * vector<AnomalyReport> &reports - A vector of reports about anomalies between correlated features.
     * correlatedFeatures &features - A pair of features which were found correlative and an anomaly
     *                                was found between them.
     * long timeStep - the time step in which the anomaly was found.
     */
    void addAnomalyReport(vector<AnomalyReport> &reports, correlatedFeatures &features, long timeStep);

public:
    //Constructor method of SimpleAnomalyDetector class.
    SimpleAnomalyDetector();

    //Destructor method of SimpleAnomalyDetector class.
    virtual ~SimpleAnomalyDetector();

    //The method receives a database of normal input and finds it's correlated features.
    virtual void learnNormal(const TimeSeries &ts);

    /*
     * The method receives a database and detects anomalies between the correlated features. A vector of reports about
     * the anomalies is returned.
     */
    virtual vector<AnomalyReport> detect(const TimeSeries &ts);

    //Returns the Vector of the correlated features (cf field).
    vector<correlatedFeatures> getNormalModel() {
        return cf;
    }

    //Returns the threshold field of the class.
    float getThreshold() {
        return this->threshold;
    }
};

#endif /* SIMPLEANOMALYDETECTOR_H_ */
