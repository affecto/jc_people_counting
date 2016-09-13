#include <iostream>
#include <boost/program_options.hpp>
#include "Parameters.h"
#include "Detector.h"
#include "DetectedPerson.h"

using namespace std;

namespace po = boost::program_options;

void run(Parameters *params) {

    cerr << "Initializing sensor..." << endl;
    Detector detector(params);

    detector.init();

    if (detector.ok()) {
        detector.run();
    } else {
        cerr << "CameraSensor failed to init." << endl;
    }
}

int DetectedPerson :: idCounter = 0;

int main(int argc, const char *argv[]) {

    Parameters *parameters = new Parameters();
    po::options_description desc("Allowed options");

    //("interval,i", po::value<double>(&parameters->post_interval)->default_value(1), "output POST interval in seconds")
    //("license,L", po::value<string>(&parameters->license), "CrowdSight license code")
    //("datadir,D", po::value<string>(&parameters->datadir)->default_value("/usr/local/crowdsight/data/"), "CrowdSight data directory")
    //("debugImages,x", po::value<int>(&parameters->debugImages)->default_value(0), "Produce debug images")
    //("triggerFramesThreshold,t", po::value<int>(&parameters->triggerFramesThreshold)->default_value(4), "Number of frames needed for trigger")
    //("dev,M", po::value<int>(&parameters->DEVELOPMENT_MODE)->default_value(0), "Specify to run in develoment mode")
    //("trigger_dep_time,r", po::value<int>(&parameters->triggerObservationDeprecationTime)->default_value(2), "Time a single observation is remembered")

    /*
    desc.add_options()
            ("help,h", "produce help message")
            ("camera, C", po::value<int>(&parameters->inputCamera)->default_value(0), "OpenCV camera number")
            ("width,W", po::value<int>(&parameters->frame_width)->default_value(1024), "OpenCV frame width")
            ("height,H", po::value<int>(&parameters->frame_height)->default_value(768), "OpenCV frame height")
            ("framerate,F", po::value<double>(&parameters->frame_rate)->default_value(30.000), "OpenCV camera frame rate")
            ("unitGUID,I", po::value<int>(&parameters->unitGUID)->default_value(-1), "Unique identifier for this installation")
            ("display,d", po::value<int>(&parameters->display)->default_value(0), "Display fraems and info on window")
            ("roi", po::value<int>(&parameters->use_roi)->default_value(0), "Use ROI on frame")
            ("roi_center_x", po::value<double>(&parameters->roi_center_x)->default_value(0.5), "ROI center x")
            ("roi_center_y", po::value<double>(&parameters->roi_center_y)->default_value(0.5), "ROI center y")
            ("roi_width", po::value<double>(&parameters->roi_width)->default_value(0.5), "ROI width")
            ("roi_height", po::value<double>(&parameters->roi_height)->default_value(0.5), "ROI height")
            ("scale", po::value<double>(&parameters->scale_factor)->default_value(1.0), "Set scaling factor to resize video frames")
            ("grayscale", po::value<int>(&parameters->use_grayscale)->default_value(0), "Use grayscale frames for detection")
            ("desired_frame_rate", po::value<double>(&parameters->desired_frame_rate)->default_value(25.0), "Desired frame rate")
            ;
*/
    desc.add_options()
            ("help,h", "produce help message")
            ("filename,f", po::value<string>(&parameters->inputFile)->default_value(""), "File to process")
            ("camera,C", po::value<int>(&parameters->inputCamera)->default_value(0), "OpenCV camera id")
            ("width,W", po::value<int>(&parameters->frame_width)->default_value(1024), "OpenCV frame width")
            ("height,H", po::value<int>(&parameters->frame_height)->default_value(768), "OpenCV frame height")
            ("unitGUID,I", po::value<int>(&parameters->unitGUID)->default_value(-1), "Unique identifierfor this installation")
            ("display,d", po::value<int>(&parameters->display)->default_value(0), "Display frames and info on window")
            ("roi", po::value<int>(&parameters->use_roi)->default_value(0), "Use ROI of frame")
            ("scale", po::value<double>(&parameters->scale_factor)->default_value(0.5), "Set scaling factor to resize video frames")
            ("framerate", po::value<double>(&parameters->frame_rate)->default_value(25.0), "Frame rate f source")
            ("desired_frame_rate", po::value<double>(&parameters->desired_frame_rate)->default_value(25.0), "Desired frame rate")
            ("url", po::value<string>(&parameters->url)->default_value("DEBUG"), "URL to send json")
            ("license", po::value<string>(&parameters->license), "Crowdsight license key")
            ("dev", po::value<int>(&parameters->dev)->default_value(0), "Development mode on")
            ("datadir", po::value<string>(&parameters->datadir)->default_value("/usr/local/crowdsight/data"), "CrowdSight data directory")
            ("startsecond", po::value<int>(&parameters->startSecond)->default_value(0), "Clip's start second")
            ("adX", po::value<int>(&parameters->ad_point_x)->default_value(0), "Gaze middlepoint x val")
            ("adY", po::value<int>(&parameters->ad_point_y)->default_value(0), "Gaze middlepoint y val")
            ("detection_count", po::value<int>(&parameters->detectionCount)->default_value(1), "No of frames needed for person observation")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    run(parameters);

    return 0;
}
