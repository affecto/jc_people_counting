//
// Created by kuoppves on 21.8.2016.
//

#include <iostream>
#include "MovingAvgVal.h"

void MovingAvgVal::addValue(float value) {

    if (index == AVERAGE_LENGTH) {
        index = 0;
        filled = true;
    }

    valueArr[index] = value;
    index++;
}

void MovingAvgVal::printVals() {

    std::cout << "values: ";

    for (int i = 0; i < AVERAGE_LENGTH; i++) {
        std::cout << valueArr[i] << ", ";
    }

    std::cout << std::endl;
}

void MovingAvgVal::calcValue() {

    float sum = 0;
    int endVal = AVERAGE_LENGTH;

    if (!filled)
        endVal = index;

    for (int i = 0; i < endVal; i++) {
        sum += valueArr[i];
    }

    value = sum / endVal;
}

float MovingAvgVal::getValue() {

    return value;

}
