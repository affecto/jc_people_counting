//
// Created by kuoppves on 20.8.2016.
//
#include <string>
#include <boost/program_options.hpp>
#include "include/rapidjson/document.h"
#include "include/rapidjson/rapidjson.h"
#include "include/rapidjson/filereadstream.h"
#include "include/easylogging++.h"

#ifndef JC_PILOT_PARAMETERS_H
#define JC_PILOT_PARAMETERS_H

using namespace std;
namespace po = boost::program_options;

class Parameters {

public:
    bool is_params_checked = false;

private:
    // document object where to store the information from configuration files
    rapidjson::Document configuration;
    // Global identifier for the unit
    int unitGUID;
    // MediaPlayer device id
    string deviceId;
    // Configuration file name
    string conf_file;
    bool is_dev = true;
    string crowdsight_license;
    string crowdsight_datadir;

    int inputCamera;
    int frame_width = 1024;
    int frame_height = 768;
    double frame_rate = 25.0;
    double desired_frame_rate = 25.0;

    std::string inputFile;
    std::string inputStream;

    std::string url = std::string("DEBUG");

    bool is_display = false;

    double scale_factor = 0.5;
    bool use_grayscale = false;

    //bool use_roi;
    std::vector<float> roi{0.1f, 0.1f, 0.9f, 0.9f};

    // relative coordinates of ROI for virtual lines, [0, 1]
    // roi should be like 0.1, 0.5, 0.9, 1.0
    // which means the two virtual lines will be located
    // at 1. [0.1, 0.5] to [0.1, 1.0] and at 2. [0.9, 0.5] to [0.9, 1.0]
    std::vector<float> roi_vlines{0.1f, 0.5f, 0.9f, 1.0f};
    std::vector<std::vector<float>> dontcare_rois;

    int startSecond = 0;

    double post_interval = 60;
    int ad_point_x = 0;
    int ad_point_y = 0;

    int detectionCount = 1; // how many frames needed to accecpt as a person

    int observationCountInterval = 60;
    int observationDeprecationTime = 60;

    int reactFramesThreshold = 1;
    int reactObservationDeprecationTime = 5;

    // for line detectors
    int blob_area_threshold = 1500;

    int scheduled_sleep;
    std::string schedu1ed_sleep_start;
    std::string scheduled_sleep_stop;

    // crowdsight api configurations;
    int min_face_size = 30;

    // debugging functions
    bool is_debug_image = false;

public:
    //Parameters(rapidjson::Document& document, string deviceId);
    Parameters(const char* filename, string deviceId);
    ~Parameters();

    void setunitGUID(int unitID) {unitGUID = unitID; }
    void setMinFaceSize(int f_s) {min_face_size = f_s; }
    void setConfFile(string f) { conf_file = f; }
    void setScheduledSleep(int s_sleep) { scheduled_sleep = s_sleep; }
    void setScheduledSleepStart(string start_time) { schedu1ed_sleep_start = start_time; }
    void setScheduledSleepStop(string stop_time) {scheduled_sleep_stop = stop_time; }
    void setLicense(string l) { crowdsight_license = l; }
    void setDataDir(string d_dir) { crowdsight_datadir = d_dir; }
    void setDeviceId(string d_id) { deviceId = d_id; }
    void setInputCamera(int cam_id) { inputCamera = cam_id; }
    void setFrameWidth(int f_width) { frame_width = f_width; }
    void setFrameHeight(int f_height){ frame_height = f_height; }
    void setFrameRate(double f_rate) { frame_rate = f_rate; }
    void setDesiredFrameRate(double d_f_rate) { desired_frame_rate = d_f_rate; }
    void setInputFileName(string input_f_name) { inputFile = input_f_name; }
    void setPostInterval(double p_interval) { post_interval = p_interval; }
    void setInputStream(string input_stream) { inputStream = input_stream; }
    void seturl(string url_) { url = url_; }
    void setLicenseMode(bool is_dev_mode) { is_dev = is_dev_mode; }
    void setDisplay(bool is_disp) { is_display = is_disp; }
    void setroi(vector<float> coordinates) {
        for (float coord : coordinates)
            roi.push_back(coord);
    }
    void setroi_vlines(vector<float> coordinates) {
        for (float coord : coordinates)
            roi_vlines.push_back(coord);
    }
    void setdetectionCount(int count) { detectionCount = count; }
    void setScaleFactor(double scale_f) { scale_factor = scale_f; }
    void setUseGrayScale(bool grayscale_on) { use_grayscale = grayscale_on; }
    void setDebugImage(bool d_i) { is_debug_image = d_i; }
    void setblob_area_threshold(int blob_thrs) { blob_area_threshold = blob_thrs; }

    rapidjson::Document& getconfiguration() { return configuration; }
    int getunitGUID() { return unitGUID; }
    int getmin_face_size() { return min_face_size; }
    string getconf_file() { return conf_file; }
    int getscheduled_sleep() { return scheduled_sleep; }
    string getschedu1ed_sleep_start() { return schedu1ed_sleep_start; }
    string getscheduled_sleep_stop() { return scheduled_sleep_stop; }
    string getlicense() { return crowdsight_license; }
    string getdataDir() { return crowdsight_datadir; }
    string getdeviceId() { return deviceId; }
    int getinputCamera() { return inputCamera; }
    int getframe_width() { return frame_width; }
    int getframe_height() { return frame_height; }
    double getframe_rate() { return frame_rate; }
    double getdesired_frame_rate() { return  desired_frame_rate; }
    string getinputFile() { return inputFile; }
    double getpost_interval() { return post_interval; }
    string getinputStream() { return inputStream; }
    string geturl() { return url; }
    int getLicenseMode() { return is_dev; }
    bool getis_display() { return is_display; }
    int getis_debug_image() { return is_debug_image; }
    vector<float> getroi() { return roi; }
    vector<float> getroi_vlines() { return roi_vlines; }
    vector<vector<float>> getdontcare_rois() { return dontcare_rois; }
    double getscale_factor() { return scale_factor; }
    bool getuse_grayscale() { return use_grayscale; }
    int getblob_area_threshold() { return blob_area_threshold; }
    int getad_point_x() { return ad_point_x; }
    int getad_point_y() { return ad_point_y; }
    int getdetectionCount() { return detectionCount;}
    int getstartSecond() { return startSecond;}

};

#endif //JC_PILOT_PARAMETERS_H
