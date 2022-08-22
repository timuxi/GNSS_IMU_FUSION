#include <iostream>
#include "uart.h"
class VTImu
{
public:
  double a[3];
  double w[3];
  double Angle[3];
  void getImuData(int fd, char *recv_buffer, int recv_length);

private:
  void imuParse(char chr);
};