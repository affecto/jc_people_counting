//
// Created by kuoppves on 20.8.2016.
//

#ifndef JC_PILOT_LINEDETECTOR_H
#define JC_PILOT_LINEDETECTOR_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

class LineDetector {

public:
    Point* begin;
    Point* end;
    Mat kernel;
    vector< vector <Vec3b> > lines;
    int detected_pedestrians;
    string name;
    double scale_factor;
    int blob_area_threshold;

    LineDetector(string name, double relative_b_x, double relative_b_y,
    double relative_e_x, double relative_e_y,
    int frame_width, int frame_height,
    Mat kernel, double scale_factor, int blob_area_threshold);

    void AddLine(Mat frame);

    void fillEdgeImage(Mat edgesIn, Mat& filledEdgesOut);
    Mat writeSlitFrame();
    void DoDetection();


};


#endif //JC_PILOT_LINEDETECTOR_H
