# Release Notes, ver 1.0
## released on October 12th 

This repository contains C++ 11 implementation on algorithms such as people counting
and face detector with 3rd party library. 

# Script to run the program
* $ ./jc_pilot [configuration .json file name]
* e.g. 
'''
./jc_pilot conf_local.json
'''

# Change made to system configuration
* The way to configure the program has been changed in this release. Now it is done 
by parsing .json file that one can check out [the example for system configuration] (configurations/conf_local.json)

# Change made to verbosity configuration
* Now using [easylogging++ single header library] (https://github.com/easylogging/easyloggingpp) to control different levels of verbosity, 
e.g. "info", "debug", "fatal", etc. The program automatically ends if it shows message
in the "fatal" level.
* Check [the example for verbosity configuration] (configurations/logger.conf)

# New features added
* Support one performing face detection in pre-defined ROI region.
If one would like to define the ROI region where face detectors are performed as the
following example, one should add 
''' 
"roi" : [0.0, 0.0, 1.0, 1.0] 
''' 
into .json configuration file.

![ROI for face detectors](doc_images/fd_roi.png)


