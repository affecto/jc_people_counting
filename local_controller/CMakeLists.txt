cmake_minimum_required(VERSION 3.4)
project(local_controller)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    capture_video.py
    capture_video.sh
    ping_routers.py
    README.md
    receive_files.py
    receive_files.sh
    requirements.txt
    send_files.py
    send_files.sh)

add_executable(local_controller ${SOURCE_FILES})
target_link_libraries(local_controller ${PYTHON_LIBRARIES})