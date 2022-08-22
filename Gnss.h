#include <vector>
#include <string.h>
#include <iostream>
#include "uart.h"
using namespace std;

class Gnss
{
public:
    vector<string> gpsData;
    void getGnssData(int fd, char *recv_buffer, int recv_length);

private:
    void getX(char *chr);
    void getY(char *chr);
    void getZ(char *chr);
    void printGpsDataAll(char *chr);
};