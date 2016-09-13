import os
import platform
import logging
import time
import argparse
import smtplib
from email.mime.text import MIMEText


class WatchDog:

    def __init__(self, args):
        self.router_one_counter = 0
        self.router_two_counter = 0
        self.router1 = args.router1
        self.router2 = args.router2
        self.poll_period = args.poll_period
        self.alert_time = args.alert_time

        logging.basicConfig(filename=args.logfile, level=logging.DEBUG, format='%(asctime)s %(message)s')


    def run(self):
        try:
            while True:
                time.sleep(1)
                router_1_up = self.isUp(self.router1)
                logging.info("router: {} is up: {}".format(self.router1, str(router_1_up)))
                if not router_1_up:
                    self.router_one_counter += 1
                    if self.router_one_counter == self.alert_time:
                        self.send_email(self.router1)
                        self.router_one_counter = 0
                else:
                    self.router_one_counter = 0

                time.sleep(self.poll_period)
                router_2_up = self.isUp(self.router2)
                logging.info("router: {} is up: {}".format(self.router2, str(router_2_up)))
                if not router_2_up:
                    self.router_two_counter += 1
                    if self.router_two_counter == self.alert_time:
                        self.send_email(self.router2)
                        self.router_two_counter = 0

                else:
                    self.router_two_counter = 0

                time.sleep(self.poll_period)

        except KeyboardInterrupt:
            print "shutting down on request..."
            logging.info("Shutting down on request...")
            exit(1)

    def isUp(self, hostname):

        giveFeedback = False

        if platform.system() == 'Windows':
            response = os.system("ping " + hostname + " -n 1")
        else:
            response = os.system("ping -c 1 " + hostname)

        isUpBool = False

        if response == 0:
            if giveFeedback:
                print(hostname, 'is up')
            isUpBool = True
        else:
            if giveFeedback:
                print(hostname, 'is down')

        return isUpBool

    def send_email(self, router):

        text = router + "is down" + "\n"
        msg = MIMEText(text, 'plain')


        # me == the sender's email address
        # you == the recipient's email address
        msg['Subject'] = 'The router %s is down' % router
        msg['From'] = "vesa.kuoppala@affecto.com"
        msg['To'] = "vkuoppala@hotmail.com"

        # Send the message via our own SMTP server, but don't include the
        # envelope header.
        s = smtplib.SMTP('localhost')
        logging.info("sending email")
        s.sendmail("vesa.kuoppala@affecto.com", ["vkuoppala@hotmail.com"], msg.as_string())
        s.quit()


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Check router connections with ping")
    parser.add_argument('--logfile', type=str, help='logfile name', default='./ping_routers.log')
    parser.add_argument('--router1', type=str, help='router 1 ip address', default='10.85.0.38')

    parser.add_argument('--router2', type=str, help='router 2 ip address', default='10.85.0.39')
    parser.add_argument('--poll_period', type=int, help='period between pings', default=30)
    parser.add_argument('--alert_time', type=int, help='time of consecutive fails on connect', default=1)

    args = parser.parse_args()

    watchdog = WatchDog(args)
    watchdog.run()


