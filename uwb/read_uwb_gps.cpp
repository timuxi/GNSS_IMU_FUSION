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

const int uwb_data_size = 16;
const int gps_data_size = 88;
const int BORD_UWB = 115200;
const int BORD_GPS = 115200;
const string gpsPort = "/dev/ttyUSBgps";
const string uwbPort = "/dev/ttyUSBuwb";

static int ret;
static int fd_uwb;
static int fd_gps;
static char uwb_r_buf[uwb_data_size];
static char gps_r_buf[gps_data_size];

int main(void)
{

  bzero(uwb_r_buf, uwb_data_size);
  bzero(gps_r_buf, gps_data_size);

  // usb初始化
  fd_uwb = UartUtil::usbInit(uwbPort, BORD_UWB);
  fd_gps = UartUtil::usbInit(gpsPort, BORD_GPS);
  ofstream OutFile(GpsUtil::getTime());

  while (1)
  {
    bzero(uwb_r_buf, uwb_data_size);
    bzero(gps_r_buf, gps_data_size);
    ret = UartUtil::readData(fd_gps, gps_r_buf, gps_data_size);
    ret = UartUtil::readData(fd_uwb, uwb_r_buf, uwb_data_size);
    GpsUtil::printGpsDataAll(gps_r_buf);
    cout << uwb_r_buf << endl;
    OutFile << gps_r_buf << " " << uwb_r_buf << endl;
    usleep(1000000);
  }

  OutFile.close();
  UartUtil::uart_close(fd_gps);
  UartUtil::uart_close(fd_uwb);
}
