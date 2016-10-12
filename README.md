# Release Notes, ver 1.0
## released on October 12th 

This repository contains C++ 11 implementation on algorithms such as people counting
and face detector with 3rd party library. 

# Script to run the program
* $ ./jc_pilot [configuration .json file name], e.g.
``` 
./jc_pilot conf_local.json 
```
# Changes of Configurable Parameters or The Newly Added
To avoid expanding the exhausted list of the configurable parameters here,
one can refer to the following places in the source codes to check those 
configurable parameters that could be defined in [.json system configuration file] (configurations/conf_local.json)
* [Parameters::Parameters(const char* filename)] (Parameter.cpp)
* [update_parameters(rapidjson::Document &configuration)] (FaceDetector.h)

# Change made to system configuration
The way to configure the program has been changed in this release. Now it is done 
by parsing .json file that one can check out [the example for system configuration] (configurations/conf_local.json)

# Change made to verbosity configuration
* Now using [easylogging++ single header library] (https://github.com/easylogging/easyloggingpp) to control different levels of verbosity, 
e.g. "info", "debug", "fatal", etc. The program automatically ends if it shows message
in the "fatal" level.
* Check [the example for verbosity configuration] (configurations/logger.conf)

# New features added
1. Support one performing face detection in pre-defined ROI region.
If one would like to define the ROI region where face detectors are performed as the
following example, 
![ROI for face detectors](doc_images/fd_roi.png)
one should add the following line into [.json system configuration file] ((configurations/conf_local.json)).
``` 
"roi" : [0.1, 0.15, 0.9, 0.75] 
``` 
Please notice that these are the normalized coordinates (from 0 to 1) with respect 
to the image's width and height.
2. Support one excluding the areas **within the active area for face detector**. 
If one would like to define the deactivated region in which face detectors would not
take any effect as the following example does, 
![ROI for face detectors](doc_images/fd_dcroi.png)
one should add the following into [.json system configuration file] (configurations/conf_local.json) 
```
"dontcare_rois" : [[0.3, 0.01, 0.4, 0.4], [0.6, 0.6, 0.7, 0.9]]
```
Please notice that these are the normalized coordinates (from 0 to 1) with respect
to **active area's width and height** and the origin is the top left corner of 
the active area marked with blue dot and (0,0) in the figure above.
3. Suport one defining where to place the virtual lines for counting pedestrians
passing by. If one would like to place two virtual lines as it does as follows
![virtual lines for people counting](doc_images/roi_vlines.png)
one should add the following into [.json system configuration file] (configurations/conf_local.json)
```
"roi_vlines" : [0.2, 0.2, 0.75, 1.0]
```
