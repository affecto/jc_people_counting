#include <iostream>
#include <boost/program_options.hpp>
#include <crowdsight.h>
#include "Parameters.h"
#include "Detector.h"
#include "DetectedPerson.h"
#include "FaceDetector.h"

using namespace std;
using namespace rapidjson;
namespace po = boost::program_options;

void run(Parameters *params) {

    cerr << "Initializing sensor..." << endl;
    Detector detector(params);
    detector.init();

    if (detector.ok()) {
        detector.run();
    } else {
        cerr << "CameraSensor failed to init." << endl;
        cerr << "or API used has failed to authenticate." << endl;
    }

    detector.sendJsonData(detector.face_detector->detectedPersonMap,
                          detector.countedPedestrians, detector.frameNo, detector.parameters);
}

bool parameter_check(Parameters *params) {

    cerr << " Unit GUID: " << params->unitGUID << endl;
    cerr << " Mode: " << params->dev << endl;
    cerr << " Postback Url: " << params->url << endl;
    cerr << " Post interval: " << params->post_interval << endl;
    cerr << " License: " << params->license << endl;
    cerr << " Camera #: " << params->inputCamera << endl;
    cerr << " Reaction sample threshold: " << params->reactFramesThreshold << endl;
    cerr << " Reaction sample deprecation: " << params->reactObservationDeprecationTime << endl;
    cerr << " Startsecond " << params->startSecond << endl;

    if (params->roi.size() > 0 && params->roi.size() != 4) {
        cerr << "error: --roi requires exactly four parameters for defining roi region, e.g. 0.1 0.1 0.9 0.9" << "" << endl;
        return 0;
    }
    if (params->roi[2] <= params->roi[0] || params->roi[3] <= params->roi[1]) {
        cerr << "error: roi[2] and roi[3] cannot be lower than roi[0] and roi[1], respectively.";
        return 0;
    }

    if (params->roi_vlines.size() > 0 && params->roi_vlines.size() != 4) {
        cerr << "error: -R requires exactly four parameters for defining roi region, e.g. 0.1 0.1 0.9 0.9" << "" << endl;
        return 0;
    }
    if (params->roi_vlines[2] <= params->roi_vlines[0] || params->roi_vlines[3] <= params->roi_vlines[1]) {
        cerr << "error: roi_vlines[2] and roi_vlines[3] cannot be lower than roi_vlines[0] and roi_vlines[1], respectively.";
        return 0;
    }
    return 1;
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

            // Crowdsight API configuration
            ("license,L", po::value<string>(&parameters->license), "CrowdSight license key")
            ("dev,M", po::value<int>(&parameters->dev)->default_value(0), "Specify to run in development mode. Only works on authorized computers.")
            ("datadir,D", po::value<string>(&parameters->datadir)->default_value("/usr/local/crowdsight/data/"), "CrowdSight data directory")

            ("url,U", po::value<string>(&parameters->url)->default_value("http://localhost:8080/_api/samples"), "output POST URL")
            ("interval,i", po::value<double>(&parameters->post_interval)->default_value(60), "output POST interval in seconds")

            // video input options
            ("camera,C", po::value<int>(&parameters->inputCamera)->default_value(0), "OpenCV camera id")
            ("stream,s", po::value<string>(&parameters->inputStream), "RTSP stream URL")
            ("filename,f", po::value<string>(&parameters->inputFile)->default_value(""), "File to process")

            // video frame configurations
            (",R", po::value<vector<float>>(&parameters->roi_vlines)->multitoken(), "define area of virtual lines for people counting (in relative coordinates)")
            //("roi", po::value<bool>(&parameters->use_roi)->default_value(false), "Use ROI of frame")
            ("roi", po::value<vector<float>>(&parameters->roi)->multitoken(), "Use ROI of frame")
            ("width,W", po::value<int>(&parameters->frame_width)->default_value(1024), "OpenCV frame width")
            ("height,H", po::value<int>(&parameters->frame_height)->default_value(768), "OpenCV frame height")
            ("scale", po::value<double>(&parameters->scale_factor)->default_value(0.5), "Set scaling factor to resize video frames")
            ("framerate", po::value<double>(&parameters->frame_rate)->default_value(25.0), "Frame rate f source")
            ("desired_frame_rate", po::value<double>(&parameters->desired_frame_rate)->default_value(25.0), "Desired frame rate")
            ("display", po::value<int>(&parameters->display)->default_value(0), "Display frames and info on window")

            ("X-Device-ID,X", po::value<string>(&parameters->deviceId), "X-Device-ID header for http" )
            ("unitGUID,I", po::value<int>(&parameters->unitGUID)->default_value(-1), "Unique identifierfor this installation")

            // for line detector configuration
            ("blob,b", po::value<int>(&parameters->blob_area_threshold)->default_value(1500), "threshold for people counting")

            ("observationCountInterval,c", po::value<int>(&parameters->observationCountInterval)->default_value(60), "Observation count threshold" )
            ("observationDeprecationTime,d", po::value<int>(&parameters->observationDeprecationTime)->default_value(60), "Observation deprecation threshold" )
            ("reactFramesThreshold,t", po::value<int>(&parameters->reactFramesThreshold)->default_value(1), "Number of frames needed for reaction")
            ("reactObservationDeprecationTime,r", po::value<int>(&parameters->reactObservationDeprecationTime)->default_value(5), "Time a single observation is remembered.")

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

    if(!parameter_check(parameters))
        return 0;

    run(parameters);

    return 0;
}
