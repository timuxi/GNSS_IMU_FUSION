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
#include "gps_util.h"
using namespace std;

const int gps_data_size = 88;

//得到gps经度
void GpsUtil::getX(char *chr)
{
  int st = -1;
  int ed = -1;
  for (int i = 0; i < gps_data_size; i++)
  {
    if ((chr[i] == 'N' || chr[i] == 'S') && st == -1)
    {
      st = i + 2;
    }
    else if ((chr[i] == 'E' || chr[i] == 'W') && ed == -1)
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
}

//得到gps纬度
void GpsUtil::getY(char *chr)
{
  int st = -1;
  int ed = -1;
  int cnt = 0;
  for (int i = 0; i < gps_data_size; i++)
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

    if (chr[i] == 'N' && ed == -1)
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
}

//得到gps高程
void GpsUtil::getZ(char *chr)
{
  int st = -1;
  int ed = -1;
  int cnt = 0;
  for (int i = 0; i < gps_data_size; i++)
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
}

string GpsUtil::getTime()
{
  time_t now1 = time(NULL);
  tm *tm_t = localtime(&now1);
  stringstream ss;
  ss << tm_t->tm_year + 1900 << ((tm_t->tm_mon + 1) > 9 ? "-" : "-0") << tm_t->tm_mon + 1 << ((tm_t->tm_mday) > 9 ? "-" : "-0") << tm_t->tm_mday
     << ((tm_t->tm_hour) > 9 ? " " : " 0") << tm_t->tm_hour << ((tm_t->tm_min) > 9 ? ":" : ":0") << tm_t->tm_min << ((tm_t->tm_sec) > 9 ? ":" : ":0") << tm_t->tm_sec;
  return ss.str();
}

void GpsUtil::printGpsDataAll(char *chr)
{
  for (int i = 0; i < gps_data_size; i++)
  {
    printf("%c", chr[i]);
  }
}