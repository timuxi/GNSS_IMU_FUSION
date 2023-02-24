import datetime
import time

import serial
import os

from FileUtil import FileUtil

port = '/dev/ttyUSBnet'
btl = 115200
current_path = os.path.dirname(os.path.abspath(__file__))
current_time = "B" + datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S') + ".rtcm"
fileutil = FileUtil(path=current_path, filename=current_time)
ser = serial.Serial(port, btl)
print(ser)

time_stamp = time.time()
cnt = 0
while True:
    if ser.in_waiting:
        data = ser.read()
        fileutil.writeByte(data)
        cnt = cnt + 1
        if cnt % 100 == 0:
            print(end=f'\r{"time :" +str(time.time()-time_stamp)}')
