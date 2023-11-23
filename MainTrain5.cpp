/*
 * run2.cpp
 *
 *  Created on:
 *      Author: Eli
 */

#include <iostream>
#include <fstream>
#include "commands.h"
#include "CLI.h"

using namespace std;


class STDtest : public DefaultIO {
    ifstream in;
    ofstream out;
public:
    STDtest(string inputFile, string outputFile) {
        in.open(inputFile);
        out.open(outputFile);
    }

    virtual string read() {
        string s;
        in >> s;
        return s;
    }

    virtual void write(string text) {
        out << text;
    }

    virtual void write(float f) {
        out << f;
    }

    virtual void read(float *f) {
        in >> *f;
    }

    void close() {
        if (in.is_open())
            in.close();
        if (out.is_open())
            out.close();
    }

    ~STDtest() {
        close();
    }
};

void check(string outputFile, string expectedOutputFile) {
    size_t chk[] = {31, 62, 63, 74, 75, 86, 87, 98, 99, 110, 111};
    ifstream st(outputFile);
    ifstream ex(expectedOutputFile);
    size_t i = 1, j = 0;
    string lst, lex;
    while (!st.eof() && !ex.eof()) {
        getline(st, lst);
        getline(ex, lex);
        if (i < 13 && lst.compare(lex) != 0) { // 12
            cout << "line " << i << " expected: " << lex << " you got " << lst << endl;
            cout << "wrong output (-1)" << endl;
        } else if (j < 11 && i == chk[j]) {
            if (lst.compare(lex) != 0) { // 88
                cout << "line " << i << " expected: " << lex << " you got " << lst << endl;
                cout << "wrong output (-8)" << endl;
            }
            j++;
        }
        i++;
    }
    if (j < 11)
        cout << "wrong output size (-" << (11 - j) * 8 << ")" << endl;
    st.close();
    ex.close();
}

void checkSequential() {
    AnomalyReport a1 = AnomalyReport("A-D", 15);
    AnomalyReport a2 = AnomalyReport("A-D", 16);
    AnomalyReport a3 = AnomalyReport("A-D", 17);
    AnomalyReport a4 = AnomalyReport("A-D", 18);
    AnomalyReport a5 = AnomalyReport("A-V", 19);
    AnomalyReport a6 = AnomalyReport("B-C", 38);
    AnomalyReport a7 = AnomalyReport("B-C", 39);
    AnomalyReport a8 = AnomalyReport("B-C", 41);
    std::vector<AnomalyReport> v1 = {a1, a2, a3, a4, a5, a6, a7, a8};
    StandardIO io = StandardIO();
    Detect d = Detect(&io);
    vector <SequentialReports> sr;
    //TO USE: uncomment next line and make unite.. function public in commands.h
    //sr=d.uniteToSequentialReports(v1);
    int u=6;


}

//small test
int main() {
    STDtest std("input.txt", "output.txt");
    CLI cli(&std);

    StandardIO stio;
    /*CLI sec(&stio);
    sec.start();*/
    //CLI cli(&stio);


    cli.start();
    std.close();
    check("output.txt", "expectedOutput.txt");
    cout << "done" << endl;
    return 0;
}
