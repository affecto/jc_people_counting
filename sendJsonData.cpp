//
// Created by amin on 19.9.2016.
//
#include <iostream>
#include <string>
#include "Parameters.h"
#include "DetectedPerson.h"
#include "include/rapidjson/writer.h"
#include "restclient-cpp/restclient.h"

void sendJsonData( std::map<long, DetectedPerson> detectedPeopleMap, int frameNo,  Parameters *parameters) {

    RestClient::headermap headers;
    //headers["X-Device-ID"] = parameters->deviceId;

    std::map<long, DetectedPerson>::iterator pIt;
    pIt = detectedPeopleMap.begin();
    int count = 0;

    std::stringstream jsonStr;
    while (pIt != detectedPeopleMap.end()) {
      if ( pIt->second.getDetected() ) {
            if (count == 0) {
                jsonStr << "[";
            } else if (count > 0) {
                jsonStr << " , ";
            }
            count++;
            jsonStr << pIt->second.asJSON(frameNo, parameters->unitGUID );
            pIt->second.setSent(true);
        }

        pIt++;
    }
    if (count > 0)
        jsonStr << "]";
    std::string str = jsonStr.str();
    if (str.length() > 0) {
        if (parameters->url.compare("DEBUG") == 0) {
            std::cout << "DEBUG: " << str << std::endl;
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