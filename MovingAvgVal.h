//
// Created by kuoppves on 21.8.2016.
//

#ifndef JC_PILOT_MOVINGAVGVAL_H
#define JC_PILOT_MOVINGAVGVAL_H


class MovingAvgVal {

    static const int AVERAGE_LENGTH = 20;
    float value;
    float valueArr[AVERAGE_LENGTH];
    int index = 0;
    bool filled = false;

public:
    void addValue(float value);
    void printVals();
    void calcValue();
    float getValue();

};


#endif //JC_PILOT_MOVINGAVGVAL_H
