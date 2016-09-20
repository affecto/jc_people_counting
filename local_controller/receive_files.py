#!/usr/bin/env python
# -*- coding: utf-8 -*-

from time import sleep
import os, shutil
import csv
import argparse
import logging
from subprocess import Popen

import datetime

from arrow import Arrow
from dateutil import tz


def is_mp4(filename):
    try:
        f = open(filename, 'rb')
        d = f.read(8)
        f.close()
        return d[4] == 'f' and d[5] == 't' and d[6] == 'y' and d[7] == 'p'
    except:
        return False


EPOCH = datetime.datetime(1970, 1, 1, tzinfo=tz.gettz('UTC'))
HELSINKI = tz.gettz('Europe/Helsinki')

parser = argparse.ArgumentParser(description='Receive MP4 files from remote controllers')

parser.add_argument('--transferdir', type=str, help='the directory on the remote SSH server to send the files',
                    default='/home/ubuntu/video/cam1/')
parser.add_argument('--transfercsv', type=str, help='the transferred files registry CSV file name',
                    default='sent.csv')

parser.add_argument('--backupdir', type=str,
                    help='if you want to move the processed MP4 files instead of deleting them')

#parser.add_argument("--detector", required=True)
#parser.add_argument("--license", required=True)
#parser.add_argument("--url", required=True)
parser.add_argument("--id", required=True)

parser.add_argument("--scale", type=float, help='Set scaling factor to resize video frames')
parser.add_argument("--blob", type=int, help='threshold for people counting')
parser.add_argument("-R", type=float, required=True, nargs='+',
                    help='specify the roi regions for placing virtual lines, e.g. if you \
                        want roi to be the area defined by the upper left point (0.1, 0.1) \
                        and bottom right point (0.9, 1.0), you should type something like "-R 0.1 0.1 0.9 1.0" ')

args = parser.parse_args()
if len(args.R) != 4:
    print 'error: argument -r requires exactly 4 inputs, e.g. 0.1 0.65 0.9 1.0'
    print 'you have entered', len(args.R), 'inputs as', args.R
    exit()

logging.basicConfig(filename='receive_files.log', level=logging.DEBUG, format='%(asctime)s %(message)s')

def read_sent_csv(name):
    ret = {}
    try:
        with open(name, 'rb') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                ret[row[0]] = row[1]
    except IOError:
        return ret
    return ret


p = None

try:
    while True:
        logging.info('Checking for files...')
        print 'Checking for files...'

        try:
            sent_files = read_sent_csv(args.transferdir + args.transfercsv)
        except IOError:
            logging.error('Could not read csv file')
            sleep(1)
            continue

        for k1, v1 in sent_files.iteritems():
            fullpath = args.transferdir + k1
            if os.access(fullpath, os.R_OK):
                logging.info('Processing ' + k1)
                print 'Processing', k1

                if not is_mp4(fullpath):
                    logging.info('Decrypting' + k1)
                    print 'Decrypting', k1
                    ep = Popen(['gpg', '--batch', '--quiet',
                                '-d', '-r', '31758A2B', '-o', fullpath + '.decrypted', fullpath])
                    ep.wait()
                    shutil.move(fullpath + '.decrypted', fullpath)

                stamp = Arrow.strptime(k1.split('-')[1][:-4], '%Y%m%d%H%M%S', HELSINKI)

                t1 = datetime.datetime.now()
                logging.info('Starting detector...')
                p = Popen([args.detector, '--license', args.license, '--url', args.url, '-I', args.id, '--dev', '1',
                           '--filename', args.transferdir + k1, '--startsecond', str(stamp.timestamp)])
                p.wait()
                t2 = datetime.datetime.now()

                if args.backupdir:
                    shutil.move(fullpath, args.backupdir)
                else:
                    os.remove(fullpath)
                logging.info('Done with' + k1 + ' in ')
                print 'Done with', k1, "in", (t2 - t1).seconds, "seconds"

        sleep(1)

except KeyboardInterrupt:
    print "shutting down on request..."
    if p:
        p.kill()
    exit(1)
