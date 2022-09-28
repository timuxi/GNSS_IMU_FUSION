#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "./util/uart_util.h"
#include "./util/gps_util.h"
using namespace std;

const int gps_data_size = 88;
const int BORD_GPS = 115200;
const string gpsPort = "/dev/ttyUSBgps";

static int ret;
static int fd_gps;
static char gps_r_buf[gps_data_size];

int main(void)
{

  // usb初始化
  fd_gps = UartUtil::usbInit(gpsPort, BORD_GPS);
  ofstream OutFile(GpsUtil::getTime());

  while (1)
  {
    bzero(gps_r_buf, gps_data_size);
    ret = UartUtil::readData(fd_gps, gps_r_buf, gps_data_size);
    cout << gps_r_buf;
    OutFile << gps_r_buf;
    usleep(1000000);
  }

  OutFile.close();
  UartUtil::uart_close(fd_gps);
}
