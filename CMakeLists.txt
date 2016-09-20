cmake_minimum_required(VERSION 2.8)
project(jc_pilot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Parameters.cpp Parameters.h Detector.cpp Detector.h PreProcessor.cpp PreProcessor.h ROI.cpp ROI.h PeopleCountDetector.cpp PeopleCountDetector.h LineDetector.cpp LineDetector.h FaceDetector.cpp FaceDetector.h DetectedPerson.cpp DetectedPerson.h MovingAvgVal.cpp MovingAvgVal.h sendJsonData.cpp)
add_executable(jc_pilot ${SOURCE_FILES})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

target_link_libraries(jc_pilot ${Boost_LIBRARIES} ${OpenCV_LIBS} restclient-cpp crowdsight)
