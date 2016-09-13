//
// Created by kuoppves on 20.8.2016.
//

#include "LineDetector.h"


LineDetector::LineDetector(string n, double relative_b_x, double relative_b_y,
                           double relative_e_x, double relative_e_y,
                           int frame_width, int frame_height,
                           Mat k, double s_f) {

    int begin_x = relative_b_x * frame_width*s_f;
    int begin_y = relative_b_y * frame_height*s_f;
    int end_x = relative_e_x * frame_width*s_f;
    int end_y = relative_e_y * frame_height*s_f;

    begin = new Point(begin_x, begin_y);
    end = new Point(end_x, end_y);

    kernel = k;
    name = n;

    scale_factor = s_f;

}

void LineDetector::AddLine(Mat frame) {
    LineIterator it(frame, *begin, *end, 8);
    vector<Vec3b> buf(it.count);

    for (int i = 0; i < it.count; i++, ++it) {
        Vec3b val = frame.at<Vec3b>(it.pos());
        buf[i] = val;
        CV_Assert(buf[i] == val);
    }

    lines.push_back(buf);
}

void LineDetector::fillEdgeImage(Mat edgesIn, Mat &filledEdgesOut) {
    Mat edgesNeg = edgesIn.clone();

    floodFill(edgesNeg, Point(0,0), CV_RGB(255,255,255));
    bitwise_not(edgesNeg, edgesNeg);
    filledEdgesOut = (edgesNeg | edgesIn);

    return;
}

Mat LineDetector::writeSlitFrame() {
    Mat slit_mat(lines.size(), lines[0].size(), CV_8UC3);

    for (int i = 0; i < lines.size(); ++i)
        slit_mat.row(i) = Mat(lines[i]).t();

    return slit_mat;
}

void LineDetector::DoDetection() {

    Mat slit_mat(lines.size(), lines[0].size(), CV_8UC3);

    for (int i = 0; i < lines.size(); ++i)
        slit_mat.row(i) = Mat(lines[i]).t();

    Mat gray_slit_mat;
    cvtColor(slit_mat, gray_slit_mat, COLOR_BGR2GRAY);

    GaussianBlur(gray_slit_mat, gray_slit_mat, Size(3,3), 0, 0, BORDER_DEFAULT);

    Mat grad_y;
    Mat abs_grad_y;

    int scale = 1;
    int delta = 0;
    int ddpeth = CV_16S;

    Sobel(gray_slit_mat, grad_y, ddpeth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

    convertScaleAbs(grad_y, abs_grad_y);

    Mat thresh_y;
    int threshold_value = 75;
    int max_BINARY_Value = 255;
    int threshold_type = 0;
    threshold(abs_grad_y, thresh_y, threshold_value, max_BINARY_Value, threshold_type);

    Mat dilation_dst;

    dilate(thresh_y, dilation_dst, kernel);

    Mat erosion_dst;

    erode(dilation_dst, erosion_dst, kernel);

    dilate(erosion_dst, dilation_dst, kernel);

    erode(dilation_dst, erosion_dst, kernel);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(erosion_dst, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    Mat dst = (Mat)(Mat::zeros(erosion_dst.rows, erosion_dst.cols, CV_8U));

    int idx = 0;
    Scalar color(255);

    for ( ; idx >= 0; idx = hierarchy[idx][0]) {
        drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
    }

    stringstream ss;
    ss << name;
    ss << ".jpg";
    cout  << ss.str() << endl;
    imwrite(ss.str(), dst);

    idx = 0;

    cout << "Number of contours: " << contours.size() << endl;
    int counter = 0;

    for (int j = 0; j < contours.size(); j++) {
        int area = contourArea(contours[j]);
        cout << "Contour area: " << area << endl;

        if (area > scale_factor*BLOB_AREA_THRESHOLD) {
            counter++;
        }
    }

    cout << "Number of detected pedestrians: " << counter << endl;
    detected_pedestrians = counter;


}

