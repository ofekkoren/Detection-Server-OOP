/*
 * timeseries.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <map>
#include <vector>
#include <sstream>
#include <fstream>

//define constants to avoid magic numbers.
#define LINE_SEPARATOR '\n'
#define VALUES_SEPARATOR ','

using namespace std;

/*
 * The TimeSeries series class represents a database of a table with feature names and their values at certain
 * time steps. We assume that all the features has the same number of values.
 */
class TimeSeries {
    //Map that maps each feature name to it's values vectors.
    map<string, vector<float>> table;
    //Vector that holds the names of the feature by the order they appear in the TimeSeries database.
    vector<string> orderedFeatures;


public:
    /*
     * Constructor method for TimeSeries class.
     * const char *CSVfileName - path to a file from which we crate the TimeSeries database.
     */
    TimeSeries(const char *CSVfileName) {
        //Initializing the fields of the database.
        this->orderedFeatures = vector<string>();
        this->table = map<string, vector<float>>();
        //Opening the file of the database.
        std::ifstream csv(CSVfileName);
        if (!csv.is_open()) {
            throw std::runtime_error("could'nt open the file");
        }
        string line;
        string value;

        /*
         * Getting the first line of the file which includes the names of the features. Then we separate each feature
         * name with a loop and adding it to the features name vector so it will be placed based on it's order in the
         * database. We also add each feature to the map and initializing it's vector of values.
         */
        getline(csv, line, LINE_SEPARATOR);
        std::stringstream lineStream(line);
        while (getline(lineStream, value, VALUES_SEPARATOR)) {
            this->orderedFeatures.push_back(value);
            this->table.insert({value, vector<float>()});
        }

        /*
         * Going through each line of the file, separating the values and adding each value to the vector of it's
         * feature in the table map. Each line has the values of the features in a certain time step and the order
         * of the values is corresponding to the order of the features in the table.
         */
        while (getline(csv, line, LINE_SEPARATOR)) {
            std::stringstream lineStream(line);
            int i = 0;
            //The loop separates the values in each line.
            while (getline(lineStream, value, VALUES_SEPARATOR)) {
                //Converting the string value to float.
                float floatVal = std::stof(value);
                //Adding the value to the vector of it's feature in the map.
                this->table[this->orderedFeatures.at(i)].push_back(floatVal);
                i++;
            }
        }
        csv.close();
    }

    //Getting the name of the i feature in the database.
    string getFeatureName(int i) const {
        //We count the features from 1 instead of 0 so we subtract 1 from i.
        return this->orderedFeatures.at(i - 1);
    }

    /* Getting a value of a feature in a time step from the database table.
     * int feature - the order of the desired feature.
     * int time - the time step of the desired value.
     */
    float getValue(int feature, int time) const {
        return this->table.at(this->orderedFeatures.at(feature - 1)).at(time - 1);
    }

    /*
     * Getting the values vector of a feature based on the order of the feature.
     * int feature - the order of the feature.
     */
    vector<float> getFeatureValuesVector(int feature) const &{
        return this->table.at(this->orderedFeatures.at(feature - 1));
    }

    /*
     * Getting the values vector of a feature based on the name of the feature.
     * string feature - the name of the feature.
     */
    vector<float> getFeatureVectorByName(string feature) const &{
        return this->table.at(feature);
    }

    //Getting the number of the features in the database.
    int getNumOfFeatures() const {
        return this->orderedFeatures.size();
    }

    //Getting the number of the values in the database features assuming all features have equal number of values.
    int getNumOfValuesInFeature() const {
        return this->table.at(orderedFeatures.at(0)).size();
    }
};

#endif /* TIMESERIES_H_ */
