//
// Created by kuoppves on 20.8.2016.
//

#include "LineDetector.h"
#define DEBUG_MODE 0

LineDetector::LineDetector(string n, double relative_b_x, double relative_b_y,
                           double relative_e_x, double relative_e_y,
                           int frame_width, int frame_height,
                           Mat k, double s_f, int blob_threshold) {

    int begin_x = relative_b_x * frame_width*s_f;
    int begin_y = relative_b_y * frame_height*s_f;
    int end_x = relative_e_x * frame_width*s_f;
    int end_y = relative_e_y * frame_height*s_f;

    begin = new Point(begin_x, begin_y);
    end = new Point(end_x, end_y);

    kernel = k;
    name = n;

    scale_factor = s_f;
    blob_area_threshold = blob_threshold;

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

    imshow("ori", slit_mat.t());

    Mat gray_slit_mat;
    cvtColor(slit_mat, gray_slit_mat, COLOR_BGR2GRAY);

    GaussianBlur(gray_slit_mat, gray_slit_mat, Size(3,3), 0, 0, BORDER_DEFAULT);

    if (DEBUG_MODE)
        imshow("gaussian", gray_slit_mat.t());

    Mat grad_y;
    Mat abs_grad_y;

    int scale = 1;
    int delta = 0;
    int ddpeth = CV_16S;

    Sobel(gray_slit_mat, grad_y, ddpeth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    if (DEBUG_MODE)
        imshow("gray_slit_mat", grad_y.t());

    convertScaleAbs(grad_y, abs_grad_y);
    if (DEBUG_MODE)
        imshow("abs_grad", abs_grad_y.t());

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
    //dilate(dilation_dst, dilation_dst, kernel);
    erode(dilation_dst, erosion_dst, kernel);

    if (DEBUG_MODE)
        imshow("dilation_dst", dilation_dst.t());

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(erosion_dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    if (DEBUG_MODE)
        imshow("contour", erosion_dst.t());

    Mat dst = (Mat)(Mat::zeros(erosion_dst.rows, erosion_dst.cols, CV_8U));

    int idx = 0;
    Scalar color(128);

    for ( ; idx >= 0; idx = hierarchy[idx][0]) {
        //drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
        drawContours(dst, contours, -1, color, CV_FILLED);
    }

    /*
    stringstream ss;
    ss << name;
    ss << ".jpg";
    cout  << ss.str() << endl;
    imwrite(ss.str(), dst);
    */

    idx = 0;
    if (DEBUG_MODE) {
        cout << "Number of contours: " << contours.size() << endl;
        cout << "scale factor: " << scale_factor << endl;
        cout << "BLOB_AREA_THRESHOLD: " << blob_area_threshold << endl;
    }

    int counter = 0;

    for (int j = 0; j < contours.size(); j++) {
        int area = contourArea(contours[j]);

        // get bounding box around the contour, utilising shape to help judge does it "look like" pedestrian
        Rect rect = boundingRect(contours[j]);

        if (DEBUG_MODE) {
            cout << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << endl;
            cout << "Contour area: " << area << endl;
            Point pt1(rect.x, rect.y);
            Point pt2(rect.x + rect.width, rect.y + rect.height);
            Scalar color(255);
            cv::rectangle(dst, pt1, pt2, color, 1);
            imshow("contour", dst.t());
            waitKey(0);
        }

        // the reason for rect.width > rect.height is that the shape of the pedestrian
        // is usually in south-to-north orientation
        if (area > scale_factor*blob_area_threshold && rect.width > rect.height) {
            counter++;
            if (rect.height > rect.width * 0.6) {
                // for the case when contours of two pedestrians are mixed together
                counter++;
            }
        }
    }

    if (DEBUG_MODE) {
        imshow("contour", dst.t());
        waitKey(0);
    }
    cout << "Number of detected pedestrians: " << counter << endl;
    detected_pedestrians = counter;
}

