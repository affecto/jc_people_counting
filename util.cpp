//
// Created by amin on 10/11/16.
//

#ifndef JC_PILOT_UTIL_H_H
#define JC_PILOT_UTIL_H_H
#define EPSILON 1e-4

bool comparedoubles(double x, double y)
{
    double diff = x - y;
    return (diff < EPSILON) && (-diff < EPSILON);
}

bool comparefloats(float x, float y)
{
    float diff = x - y;
    return (diff < EPSILON) && (-diff < EPSILON);
}


#endif //JC_PILOT_UTIL_H_H
