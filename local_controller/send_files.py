#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, csv, datetime
from glob import glob
import socket
import logging

import paramiko
from paramiko import SSHClient, AutoAddPolicy
from scp import SCPClient, SCPException
from time import sleep
import argparse

parser = argparse.ArgumentParser(description='Transfer image files to the cloud')
parser.add_argument('--hostname', type=str, help='the cloud server hostname', default='52.58.86.18')
parser.add_argument('--username', type=str, help='the cloud server username', default='ubuntu')

parser.add_argument('--capturedir', type=str, help='the directory where captured MP4 files can be found',
                    default='/dev/shm/4/')
parser.add_argument('--capturecsv', type=str, help='the capture CSV file name',
                    default='cam1.csv')

parser.add_argument('--transferdir', type=str, help='the directory on the remote SSH server to send the files',
                    default='/home/ubuntu/video/cam1/')
parser.add_argument('--transfercsv', type=str, help='the transferred files registry CSV file name',
                    default='sent.csv')

args = parser.parse_args()

INPUT_CSV_PATH = args.capturedir + args.capturecsv
SENT_CSV_PATH = args.capturedir + args.transfercsv

logging.basicConfig(filename='send_files.log', level=logging.DEBUG, format='%(asctime)s %(message)s')


def read_input_csv(name):
    ret = {}
    with open(name, 'rb') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            ret[row[0]] = (float(row[1]), float(row[2]))
    return ret


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


def write_sent_csv(name, sent):
    with open(name, 'wb') as csvfile:
        writer = csv.writer(csvfile)
        for k, v in sent.iteritems():
            writer.writerow([k, v])


def reconnect(xssh):
    while True:
        try:
            xssh.connect(args.hostname, username=args.username)
            return SCPClient(xssh.get_transport())
        except (paramiko.ssh_exception.SSHException, EOFError, socket.error), e:
            print 'Failed to connect over SSH, sleeping and retrying...'
            sleep(10)


ssh = SSHClient()
ssh.set_missing_host_key_policy(AutoAddPolicy())
scp = reconnect(ssh)
logging.info("Scp acquired")


try:
    while True:

        try:
            input_files = read_input_csv(INPUT_CSV_PATH)
            sent_files = read_sent_csv(SENT_CSV_PATH)
        except (IOError, csv.Error), e:
            logging.error(e)
            sleep(1)
            continue

        for k1, v1 in input_files.iteritems():
            if k1 not in sent_files and os.access(args.capturedir + k1, os.R_OK):
                t1 = datetime.datetime.now()
                logging.info('sending' + k1 + " at " + str(t1))
                logging.info('Sending csv file')
                print 'sending ', k1, 'at', t1

                need_reconnect = False
                try:
                    scp.put(args.capturedir + k1, args.transferdir + k1)
                    scp.put(SENT_CSV_PATH, args.transferdir + args.transfercsv)
                except (SCPException, paramiko.ssh_exception.SSHException, EOFError, socket.error), e:
                    logging.error(e)
                    print e
                    need_reconnect = True

                if need_reconnect:
                    try:
                        logging.info('Trying to reconnect...')
                        scp.close()
                        ssh.close()
                    except (paramiko.ssh_exception.SSHException, EOFError, socket.error), e:
                        logging.error(e)
                        pass
                    scp = reconnect(ssh)
                    continue

                os.remove(args.capturedir + k1)
                t2 = datetime.datetime.now()
                print 'sent ', k1, 'in', (t2 - t1).seconds, 'seconds'
                sent_files[k1] = datetime.datetime.now()
                write_sent_csv(args.capturedir + args.transfercsv, sent_files)

        sleep(1)

except KeyboardInterrupt:
    print "shutting down on request..."
    scp.close()
    exit(1)

scp.close()
