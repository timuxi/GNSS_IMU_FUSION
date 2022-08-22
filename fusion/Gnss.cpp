#include <vector>
#include <string.h>
#include <iostream>
#include "uart.h"
#include "Gnss.h"
using namespace std;

#define GPS_DATA_SIZE 88
vector<string> gpsData(3);


void getGnssData(int fd, char *recv_buffer, int recv_length)
{
  int ret = Uart::readData(fd, recv_buffer, recv_length);
  getX(recv_buffer);
  getY(recv_buffer);
  getZ(recv_buffer);
}

//得到gps经度
void getX(char *chr)
{
  int st = -1;
  int ed = -1;
  for (int i = 0; i < GPS_DATA_SIZE; i++)
  {
    if ((chr[i] == 'N' || chr[i] == 'S') && st == -1)
    {
      st = i + 2;
    }
    else if (chr[i] == 'E' || chr[i] == 'W')
    {
      ed = i - 2;
    }
  }

  if (st == -1 || ed == -1)
  {
    printf("gpsX process error!!\n");
    return;
  }

  string gps1(chr);
  gps1 = gps1.substr(st, ed - st + 1);
  gpsData[0] = gps1;
}

//得到gps纬度
void getY(char *chr)
{
  int st = -1;
  int ed = -1;
  int cnt = 0;
  for (int i = 0; i < GPS_DATA_SIZE; i++)
  {
    if (cnt < 2)
    {
      if (chr[i] == ',')
        cnt++;
      continue;
    }
    else if (cnt == 2 && st == -1)
    {
      st = i;
    }

    if (chr[i] == 'N')
    {
      ed = i - 2;
    }
  }

  if (st == -1 || ed == -1)
  {
    printf("gpsY process error!! %d %d \n", st, ed);
    return;
  }

  string gps2(chr);
  gps2 = gps2.substr(st, ed - st + 1);
  gpsData[1] = gps2;
}

//得到gps高程
void getZ(char *chr)
{
  int st = -1;
  int ed = -1;
  int cnt = 0;
  for (int i = 0; i < GPS_DATA_SIZE; i++)
  {
    if (cnt < 9)
    {
      if (chr[i] == ',')
      {
        cnt++;
      }
      continue;
    }
    else if (cnt == 9 && st == -1)
    {
      st = i;
    }

    if (chr[i] == 'M' && ed == -1)
    {
      ed = i - 2;
    }
  }
  if (st == -1 || ed == -1)
  {
    printf("gpsZ process error!!, st=%d , ed=%d\n", st, ed);
    return;
  }

  string gps3(chr);
  gps3 = gps3.substr(st, ed - st + 1);
  gpsData[2] = gps3;
}

void printGpsDataAll(char *chr)
{
  for (int i = 0; i < GPS_DATA_SIZE; i++)
  {
    printf("%c", chr[i]);
  }
}