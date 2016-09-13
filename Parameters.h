//
// Created by kuoppves on 20.8.2016.
//
#include <string>
#include <boost/program_options.hpp>


#ifndef JC_PILOT_PARAMETERS_H
#define JC_PILOT_PARAMETERS_H

using namespace std;
namespace po = boost::program_options;

class Parameters {

public:

    int unitGUID;
    int inputCamera;
    int frame_width;
    int frame_height;
    double frame_rate;
    double desired_frame_rate;

    string inputFile;
    vector<string> inputStream;

    string url;

    int display;

    double scale_factor;
    int use_grayscale;
    int use_roi;

    double roi_center_x;
    double roi_center_y;
    double roi_width;
    double roi_height;

    int dev;
    string license;
    string datadir;

    int startSecond;

    int ad_point_x;
    int ad_point_y;

    int detectionCount;

};


#endif //JC_PILOT_PARAMETERS_H
