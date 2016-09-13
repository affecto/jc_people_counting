#!/usr/bin/env bash

TRANSFERDIR="/home/ubuntu/video/test_cam/"
TRANSFERCSV="sent.csv"
BACKUPDIR="/home/ubuntu/backup/"
DETECTOR="/home/ubuntu/jc_pilot/build/jc_pilot"
LICENSE=""
URL="0.0.0.0"
ID="test_cam"

python receive_files.py --transferdir $TRANSFERDIR --transfercsv $TRANSFERCSV --backupdir $BACKUPDIR --detector $DETECTOR --license $LICENSE --id $ID
