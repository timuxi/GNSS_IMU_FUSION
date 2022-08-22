import math

from geopy import distance
from math import sin, cos, atan2, pi
import turtle

# 经纬度ddmm形式转dd形式
from matplotlib import pyplot as plt

from main import read_data, read_gps_data, read_data_old

R = 637100
L = 2 * math.pi * R
uncompressed = [[], []]
uncompressed2 = [[], []]
def ddmm2dd(x):
    degrees = int(x) // 100
    minutes = x - 100 * degrees
    return degrees + minutes / 60

def latTrans(Latitude):
    """
    参考地址：https://blog.csdn.net/Dust_Evc/article/details/1
    02847870?utm_medium=distribute.pc_relevant.none-task-blog-2%7
    Edefault%7EBlogCommendFromBaidu%7Edefault-7.control&depth_
    1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault
    %7EBlogCommendFromBaidu%7Edefault-7.control
    度数与弧度转化公式：1°=π/180°，1rad=180°/π。
    地球半径：6371000M
    地球周长：2 * 6371000M  * π = 40030173
    纬度38°地球周长：40030173 * cos38 = 31544206M
    任意地球经度周长：40030173M
    经度（东西方向）1M实际度：360°/31544206M=1.141255544679108e-5=0.00001141
    纬度（南北方向）1M实际度：360°/40030173M=8.993216192195822e-6=0.00000899
    """
    Latitude = ddmm2dd(Latitude)
    Lat_l = L * cos(Latitude * math.pi / 180)  # 当前纬度地球周长，弧度转化为度数
    Lat_C = Lat_l / 360
    Latitude_m = Latitude * Lat_C
    return Latitude_m


def lonTrans(Longitude):
    Longitude = ddmm2dd(Longitude)
    Lng_l = 2*R*math.pi  # 当前经度地球周长
    Lng_C = Lng_l / 360
    Longitude_m = Longitude * Lng_C
    return Longitude_m

def test(data):
    return data*1000

def showGpsByMy(_filename):
    lon = read_data(_filename, 1)
    lat = read_data(_filename, 2)
    print(ddmm2dd(lon[0]))
    for i in range(1000, lon.size-1000):
        if(lon[i] == 0) : continue
        uncompressed[0].append(ddmm2dd(lon[i]-lon[1000])*1000000*0.111)
        uncompressed[1].append(ddmm2dd(lat[i]-lat[1000])*1000000*0.111)
    plt.plot(uncompressed[0], uncompressed[1])
    plt.xlabel("longitude(m)")
    plt.ylabel("latitude(m)")
    plt.show()

def showGp(_filename):
    lon = read_gps_data(_filename, 12)
    lat = read_gps_data(_filename, 13)
    print(ddmm2dd(lon[0]))
    for i in range(0, lon.size):
        uncompressed[0].append(lon[i]-lon[0]*1000000*0.111)
        uncompressed[1].append(lat[i]-lat[0]*1000000*0.111)
    plt.plot(uncompressed[0], uncompressed[1])
    plt.xlabel("longitude(m)")
    plt.ylabel("latitude(m)")
    plt.show()

if __name__ == '__main__':
    lon = read_data("7-26-rtk-first.txt", 1)
    lat = read_data("7-26-rtk-first.txt", 2)
    for i in range(1000, lon.size-1000):
        if(lon[i] == 0) : continue
        uncompressed[0].append(lon[i])
        uncompressed[1].append(lat[i])
    plt.plot(uncompressed[0], uncompressed[1])

    lon = read_data_old("2022-06-28 13_32_06", 1)
    lat = read_data_old("2022-06-28 13_32_06", 0)
    for i in range(0, lon.size):
        if(lon[i] == 0) : continue
        uncompressed2[0].append(ddmm2dd(lon[i]))
        uncompressed2[1].append(ddmm2dd(lat[i]))
    plt.plot(uncompressed2[0], uncompressed2[1])
    plt.xlabel("longitude(m)")
    plt.ylabel("latitude(m)")
    plt.show()