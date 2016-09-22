#!/usr/bin/env bash

#TRANSFERDIR="/home/ubuntu/video/test_cam/"
TRANSFERDIR="/home/amin/ClionProjects/jc_people_counting/videos/"
TRANSFERCSV="sent.csv"
BACKUPDIR="/home/amin/ClionProjects/jc_people_counting/videos/backup/"
DETECTOR="/home/amin/ClionProjects/jc_people_counting/build/jc_pilot"
LICENSE="f187f8d8b27f424fbab9064c18a7e0e4"
#URL="0.0.0.0"
URL="DEBUG"
ID="1"
ROI_VLINES="0.1 0.25 0.9 1.0"

python receive_files.py --transferdir $TRANSFERDIR --transfercsv $TRANSFERCSV --backupdir $BACKUPDIR --detector $DETECTOR --license $LICENSE --id $ID --url URL -R $ROI_VLINES
