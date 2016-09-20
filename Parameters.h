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
    string deviceId;
    int inputCamera;
    int frame_width;
    int frame_height;
    double frame_rate;
    double desired_frame_rate;

    string inputFile;
    //vector<string> inputStream;
    string inputStream;

    string url;

    int display;

    double scale_factor;
    int use_grayscale;

    bool use_roi;
    double roi_center_x;
    double roi_center_y;
    double roi_width;
    double roi_height;

    // relative coordinates of ROI for virtual lines, [0, 1]
    // roi should be like 0.1, 0.5, 0.9, 1.0
    // which means the two virtual lines will be located
    // at 1. [0.1, 0.5] to [0.1, 1.0] and at 2. [0.9, 0.5] to [0.9, 1.0]
    vector<float> roi_vlines;

    int dev;
    string license;
    string datadir;

    int startSecond;

    double framerate;
    double post_interval;
    int ad_point_x;
    int ad_point_y;

    int detectionCount; // how many frames needed to accecpt as a person

    int observationCountInterval;
    int observationDeprecationTime;

    int reactFramesThreshold;
    int reactObservationDeprecationTime;

    // for line detectors
    int blob_area_threshold;

};


#endif //JC_PILOT_PARAMETERS_H
