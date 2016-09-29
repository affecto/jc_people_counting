#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, shutil
import datetime
import argparse
import logging
from time import sleep
import subprocess
from subprocess import Popen
from glob import glob

from arrow import Arrow
from dateutil import tz

EPOCH = datetime.datetime(1970, 1, 1, tzinfo=tz.gettz('UTC'))
HELSINKI = tz.gettz('Europe/Helsinki')

parser = argparse.ArgumentParser(description='Receive MP4 files from remote controllers')

parser.add_argument('--transferdir', type=str, help='the directory oon the remote SSH server to send the files',
                    default='/home/ubuntu/video/cam1/')

parser.add_argument('--backupdir', type=str,
                    help='if you want to move the processed MP4 files instead of deleting them')

parser.add_argument('--detector', required=True)
parser.add_argument('--license', required=True)
parser.add_argument('--url', required=True)
parser.add_argument('--id', required=True)
parser.add_argument('--scale', type=float, help='Setscaling factor to resize video frames')
parser.add_argument('--blob', type=int, help='threshold for people counting')
parser.add_argument('-R', required=True, nargs='+', help='specify the roi regions for placing virtual lines, '
                                                                     'e.g. if you want roi to be the area defined by the '
                                                                     'upper left point (0.1, 0.1) and bottom right point (0.9 1.0), '
                                                                     'you should type something like "-R 0.1 0.1 0.9 1.0"')
parser.add_argument('--roi', required=True, nargs='+',
                    help='specify the roi regions for face detector, e.g. if you want roi to be the area defined by the'
                         ' upper left point (0.1, 0.1) and bottom right point (0.9, 1.0), you should type something like'
                         ' --roi 0.1 0.1 0.9 1.0"')
parser.add_argument('--password', type=str, default='/home/kuoppves/Projects/jc_decoding_test/password')


args = parser.parse_args()


logging.basicConfig(filename='receive_encoded_files.log', level=logging.DEBUG, format='%(asctime)s %(message)s')


def is_reserved(filepath):
    try:
        result = subprocess.check_output(['fuser', fn])
    #process = Popen(['fuser', filepath], shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    #lines = process.stdout.readlines()
    #process.wait()
    except subprocess.CalledProcessError:
        result = ''

    if len(result) == 0:
        return False
    else:
        return True

def is_locked(filepath):
    """Checks if a file is locked by opening it in append mode.
    If no exception thrown, then the file is not locked.
    """
    locked = None
    file_object = None
    if os.path.exists(filepath):
        try:
            print "Trying to open %s." % filepath
            buffer_size = 8
            # Opening file in append mode and read the first 8 characters.
            file_object = open(filepath, 'a', buffer_size)
            if file_object:
                print "%s is not locked." % filepath
                locked = False
        except IOError, message:
            print "File is locked (unable to open in append mode). %s." % \
                  message
            locked = True
        finally:
            if file_object:
                file_object.close()
                print "%s closed." % filepath
    else:
        print "%s not found." % filepath
    return locked

p = None

try:
    while True:
        print 'Checking for files....'

        try:
            for fn in glob(args.transferdir + 'encrypted_*.mp4'):
                try:
                    if is_reserved(fn):
                        continue
                    else:
                        if os.access(fn, os.R_OK):
                            print 'Processing', fn

                            try:
                                print 'Decrypting', fn
                                out = fn + ".decrypted"
                                password = 'file:' + args.password
                                ep = Popen(['openssl', 'aes-128-cbc', '-d', '-in', fn, '-out', out, '-salt', '-pass', password])
                                ep.wait()
                                # Strip filename
                                basename = os.path.basename(fn)
                                base = basename.split('_')[1]
                                outfile = args.transferdir + base
                                shutil.move(out, outfile)

                                stamp = Arrow.strptime(outfile.split('-')[1][:-4], '%Y%m%d%H%M%S', HELSINKI)
                                t1 = datetime.datetime.now()
                                p = Popen([args.detector, '--license', args.license, '--url', args.url, '-I', args.id, '--dev', '1',
                                           '--filename', outfile, '--startsecond', str(stamp.timestamp),
                                           '--roi', args.roi[0], args.roi[1], args.roi[2], args.roi[3],
                                           '-R', args.R[0], args.R[1], args.R[2], args.R[3]])
                                p.wait()
                                t2 = datetime.datetime.now()

                                if args.backupdir:
                                    shutil.move(outfile, args.backupdir)
                                else:
                                    os.remove(outfile)
                                    os.remove(fn)
                                print 'Done with', outfile, "in", (t2 - t1).seconds, "seconds"
                            except IOError:
                                pass
                except IOError:
                    pass

        except IOError:
            pass

        sleep(1)

except KeyboardInterrupt:
    print "shuttng down on request..."
    if p:
        p.kill()
    exit(1)
