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
using namespace std;

class GpsUtil
{
public:
    static void getX(char *chr);
    static void getY(char *chr);
    static void getZ(char *chr);
    static string getTime();
    static void printGpsDataAll(char *chr);

private:
};