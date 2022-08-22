#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "VTImu.h"
#include "Gnss.h"
#include "uart.h"
using namespace std;

#define IMU_PORT_NAME "/dev/ttyUSBimu"
#define GPS_PORT_NAME "/dev/ttyUSBgps"
#define IMU_BORD 9600
#define GPS_BORD 115200
#define IMU_DATA_SIZE 1024
#define GPS_DATA_SIZE 88

string getTime()
{
  time_t now1 = time(NULL);
  tm *tm_t = localtime(&now1);
  stringstream ss;
  ss << tm_t->tm_year + 1900 << ((tm_t->tm_mon + 1) > 9 ? "-" : "-0") << tm_t->tm_mon + 1 << ((tm_t->tm_mday) > 9 ? "-" : "-0") << tm_t->tm_mday
     << ((tm_t->tm_hour) > 9 ? " " : " 0") << tm_t->tm_hour << ((tm_t->tm_min) > 9 ? ":" : ":0") << tm_t->tm_min << ((tm_t->tm_sec) > 9 ? ":" : ":0") << tm_t->tm_sec;
  return ss.str();
}

int main(void)
{

  char imu_rbuf[IMU_DATA_SIZE], gps_rbuf[GPS_DATA_SIZE];
  int fd_imu, fd_gps;
  ofstream OutFile(getTime());

  /*串口初始化*/
  if (!Uart::uartInit(fd_imu, IMU_PORT_NAME, IMU_BORD))
  {
    exit(EXIT_FAILURE);
  }
  if (!Uart::uartInit(fd_gps, GPS_PORT_NAME, GPS_BORD))
  {
    exit(EXIT_FAILURE);
  }

  VTImu vtimu;
  Gnss gnss;
  while (1)
  {
    bzero(imu_rbuf, IMU_DATA_SIZE);
    vtimu.getImuData(fd_imu, imu_rbuf, 29);
    bzero(gps_rbuf, GPS_DATA_SIZE);
    gnss.getGnssData(fd_gps, gps_rbuf, GPS_DATA_SIZE);
    cout << "gps1=" << gnss.gpsData[0]
         << " gps2=" << gnss.gpsData[1]
         << " gps3=" << gnss.gpsData[2]
         << " a0=" << vtimu.a[0]
         << " a1=" << vtimu.a[1]
         << " a2=" << vtimu.a[2]
         << " w0=" << vtimu.w[0]
         << " w1=" << vtimu.w[1]
         << " w2=" << vtimu.w[2]
         << " angle0=" << vtimu.Angle[0]
         << " angle1=" << vtimu.Angle[1]
         << " angle2=" << vtimu.Angle[2]
         << endl;
    OutFile << gnss.gpsData[0]
            << " " << gnss.gpsData[1]
            << " " << gnss.gpsData[2]
            << " " << vtimu.a[0]
            << " " << vtimu.a[1]
            << " " << vtimu.a[2]
            << " " << vtimu.w[0]
            << " " << vtimu.w[1]
            << " " << vtimu.w[2]
            << " " << vtimu.Angle[0]
            << " " << vtimu.Angle[1]
            << " " << vtimu.Angle[2]
            << endl;
    // hz=10
    usleep(100000);
  }

  OutFile.close();
  Uart::uart_close(fd_imu);
  Uart::uart_close(fd_gps);
  exit(EXIT_SUCCESS);
}
