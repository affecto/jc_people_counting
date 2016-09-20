//
// Created by kuoppves on 20.8.2016.
//

#include "Detector.h"
#include <sys/time.h>
#include "include/rapidjson/document.h"
#include <restclient-cpp/restclient.h>

void Detector::sendJsonData(std::map<long, DetectedPerson> detectedPeopleMap, int countedPedestrians, int frameNo, Parameters *parameters) {

    RestClient::headermap headers;

    std::map<long, DetectedPerson>::iterator pIt;
    pIt = detectedPeopleMap.begin();
    int count = 0;

    std::stringstream jsonStr;

    jsonStr << "[";
    jsonStr << "{";
    jsonStr << "'number_of_total_pedestrians'";
    jsonStr << ":";
    jsonStr << countedPedestrians;
    jsonStr << "}";
    jsonStr << " , ";

    while (pIt != detectedPeopleMap.end()) {
        if (pIt->second.getDetected()) {
            if (count > 0) {
                jsonStr << " , ";
            }
            count++;
            jsonStr << pIt->second.asJSON(frameNo, parameters->unitGUID);
            pIt->second.setSent(true);
        }

        pIt++;
    }

    jsonStr << "]";

    std::string str = jsonStr.str();

    if (str.length() > 0) {
        if (parameters->url.compare("DEBUG") == 0) {
            std::cout <<"DEBUG: " << str << std::endl;
        } else {
            RestClient::response r = RestClient::post(parameters->url, "application/json", str, headers);
            if (r.code != 200) {
                std::cerr << "POST failed, code " << r.code << std::endl;
                cout << "Failed to send" << str << std::endl;
            } else {
                cout << "Sent: " << str << std::endl;
            }
        }
    }
}

void Detector::InitOpenCV() {

    fileSource = false;
    if (!parameters->inputFile.empty()) {
        cout << "Clip from file: " << parameters->inputFile << endl;
        fileSource = true;
        videoCapture.open(parameters->inputFile);
        if (!videoCapture.isOpened()) {
            cout << "Couldn't open " << parameters->inputFile << endl;
        }

    } else if (!parameters->inputStream.empty()) {
        videoCapture.open(parameters->inputStream);
        if (!videoCapture.isOpened()) {
            cout << "Couldn't read streaming at " << parameters->inputStream << endl;
        }

    } else {
        videoCapture.open(parameters->inputCamera);
        if (!videoCapture.isOpened()) {
            cout << "Couldn't read the camera...!" << endl;
        }
        videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, parameters->frame_width);
        videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, parameters->frame_height);
        videoCapture.set(CV_CAP_PROP_FPS, parameters->frame_rate);
    }

    window_name = "JC Video";

    if (parameters->display != 0) {
        cvNamedWindow(window_name, cv::WINDOW_AUTOSIZE);
    }

    skip_frames = int(parameters->frame_rate / parameters->desired_frame_rate);

}

void Detector::InitPreProcessor() {

    preprocessor = new PreProcessor(*parameters);
}

void Detector::InitDetectors() {

    people_count_detector = new PeopleCountDetector(*parameters);
    face_detector = new FaceDetector(*parameters);

    if (parameters->dev == 1) {
        face_detector->ShowSettings();
    }

    if (!face_detector->crowdSight) {
        status = false;
    }

}

void Detector::init() {

    status = true;

    if (parameters->unitGUID == -1) {
        cerr << "Error:: unitGUID not set" << endl;
        status = false;
        return;
    }

    frameNo = 0;
    InitOpenCV();
    InitPreProcessor();
    InitDetectors();

    return;
}

bool Detector::ok() {
    return status;
}

void Detector::run() {

    time_t analysisStartTime = time(0);

    cv::Mat frame;

    timeval start;
    gettimeofday(&start, NULL);

    cerr << "Started detections on unit: " << parameters->unitGUID << endl;

    int count = 0;
    while(true) {
        videoCapture >> frame;

        frameNo++;

        if (frame.empty()) {
            videoCapture.release();
            break;
        }

        if ((frameNo % skip_frames) != 0) {
            continue;
        }

        preprocessed_frame = preprocessor->Process(frame);

        people_count_detector->line_det1->AddLine(preprocessed_frame);
        people_count_detector->line_det2->AddLine(preprocessed_frame);

        face_detector->Process(frame, *parameters, frameNo, fileSource);

        if (parameters->display) {
            imshow(window_name, frame);
            waitKey(1);
        }
        count++;
    }
    cout << count << endl;
    people_count_detector->line_det1->DoDetection();
    people_count_detector->line_det2->DoDetection();

    countedPedestrians = int((people_count_detector->line_det1->detected_pedestrians + people_count_detector->line_det2->detected_pedestrians + 0.5) / 2.0);

    cout << "Number of pedestrians: " << countedPedestrians << endl;

    //sendJsonData(face_detector->detectedPersonMap, countedPedestrians, frameNo, parameters);

    time_t endTime = time(0);

    std::cout << "Frame rate " << frameNo / (endTime - analysisStartTime) << " frame count " << frameNo << std::endl;

}

