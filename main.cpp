#include <iostream>
#include <boost/program_options.hpp>
#include <crowdsight.h>
#include "include/rapidjson/reader.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/rapidjson.h"
#include "Parameters.h"
#include "Detector.h"
#include "DetectedPerson.h"
#include "FaceDetector.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace rapidjson;
namespace po = boost::program_options;
el::Logger* mainLogger;

/*example of scripts if you are using parsing them with boost library
 * -f /home/amin/ClionProjects/jc_people_counting/videos/cam2-20160921080253.mp4 --display 0 --blob 1500 -M 1 -L f187f8d8b27f424fbab9064c18a7e0e4 -I 1 --roi 0.05 0.25 0.97 0.85 -R 0.1 0.2 0.7 1.0
 *
 * */
// /home/amin/ClionProjects/jc_people_counting/configurations/conf.json

void run(Parameters *params) {

    Detector detector(params);
    detector.init();

    if (detector.ok()) {
        detector.run();
    } else {
        mainLogger->error("CameraSensor failed to init or API used has failed to authenticate.");
    }

    detector.sendJsonData(detector.face_detector->detectedPersonMap,
                          detector.countedPedestrians, detector.frameNo, detector.parameters);
}

bool parameter_check(Parameters *params) {

    if (params->getroi().size() > 0 && params->getroi().size() != 4) {
        mainLogger->error("error: ""roi"" requires exactly four parameters for defining roi region, e.g. [0.1 0.1 0.9 0.9]");
        return false;
    }
    if (params->getroi()[2] <= params->getroi()[0] || params->getroi()[3] <= params->getroi()[1]) {
        mainLogger->error("error: roi[2] and roi[3] cannot be lower than roi[0] and roi[1], respectively.");
        return false;
    }

    if (params->getroi_vlines().size() > 0 && params->getroi_vlines().size() != 4) {
        mainLogger->error("error: ""roi_vlines"" requires exactly four parameters for defining roi region, e.g. [0.1 0.1 0.9 0.9]");
        return false;
    }
    if (params->getroi_vlines()[2] <= params->getroi_vlines()[0] || params->getroi_vlines()[3] <= params->getroi_vlines()[1]) {
        mainLogger->error("error: roi_vlines[2] and roi_vlines[3] cannot be lower than roi_vlines[0] and roi_vlines[1], respectively.");
        return false;
    }
    std::vector<std::vector<float>> dontcare_rois = params->getdontcare_rois();
    for (std::vector<float> roi : dontcare_rois) {
        if (roi.size() != 4) {
            mainLogger->error("error: ""dontcare_rois"" requires exactly four parameters for defining a roi region, "
                                      "e.g. [0.1 0.1 0.15 0.15]. Input of multiple vectors to define more than one"
                                      "roi regions is allowed, e.g. [[0.1, 0.1, 0.15, 0.15],[0.6, 0.6, 0.7, 0.7]]");
            return false;
        }
    }
    return true;
}

Parameters* init_parameters(const char* filename, string deviceID) {

    Parameters* parameters = new Parameters(filename, deviceID);
    parameters->is_params_checked = parameter_check(parameters);

    return parameters;
}

int DetectedPerson :: idCounter = 0;

int main(int argc, const char *argv[]) {

    START_EASYLOGGINGPP(argc, argv);

    mainLogger = el::Loggers::getLogger("main");
    //el::Helpers::setCrashHandler(myCrashHandler);     // here to define your own crash handler

    mainLogger->info("Start jc people counting and face detector");
    mainLogger->info("Read program arguments");

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << "configuration filename" << " deviceId" << std::endl;
        return 1;
    }

    mainLogger->info("Read parameters from json file and device id from arguments");
    mainLogger->info("device ID: %v", argv[2]);
    Parameters* parameters = init_parameters(argv[1], string(argv[2]));

    mainLogger->info("License mode: %v", parameters->getLicenseMode());
    mainLogger->info("License: %v", parameters->getlicense());
    mainLogger->info("DeviceId: %v", parameters->getdeviceId());

    if (parameters) {
        if (parameters->is_params_checked)
            run(parameters);
        delete parameters;
    }

    return 0;
}