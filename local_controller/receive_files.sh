#!/usr/bin/env bash

#TRANSFERDIR="/home/ubuntu/video/test_cam/"
TRANSFERDIR="/data/video/cam2/"
TRANSFERCSV="sent.csv"
BACKUPDIR="/home/ubuntu/jc_people_counting/videos/backup/"
DETECTOR="/home/ubuntu/jc_people_counting/build/jc_pilot"
LICENSE=""
#URL="0.0.0.0"
URL="DEBUG"
ID="1"
ROI_VLINES="0.1 0.2 0.7 1.0"
ROI_DETECTOR="0.05 0.25 0.97 0.85"
python receive_files.py --transferdir $TRANSFERDIR --transfercsv $TRANSFERCSV --backupdir $BACKUPDIR --detector $DETECTOR --license $LICENSE --id $ID --url URL -R $ROI_VLINES --roi $ROI_DETECTOR
