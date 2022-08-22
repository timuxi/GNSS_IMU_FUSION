import os
import numpy as np
import pandas as pd
from scipy.spatial.distance import cdist
import matplotlib.pyplot as plt

def readDataAll(_filename):
    gps1 = read_data(_filename, 0)
    gps2 = read_data(_filename, 1)
    gps3 = read_data(_filename, 2)
    a1 = read_data(_filename, 3)
    a2 = read_data(_filename, 4)
    a3 = read_data(_filename, 5)
    pos_z = read_data(_filename, 6)
    axs[0].plot(gps3)
    axs[0].set_title('gps3')
    axs[1].plot(a3)
    axs[1].set_title('a3')
    axs[2].plot(pos_z)
    axs[2].set_title('pos_z')

def read_data_old(_filename, col):
    """
    _filename:   文件名
    col:         读取数据的第 col 列
    """
    path = os.path.abspath('.\\data') + "\\" + _filename
    print('读取文件的路径: ' + path)
    pd_read = pd.read_table(path, sep=' ', header=None)
    pd_read = pd_read.values[:, col]
    print('读取数据量: ', pd_read.size, ' 末端数据的值: ', pd_read[pd_read.size - 1])
    return pd_read

def read_data(_filename, col):
    """
    _filename:   文件名
    col:         读取数据的第 col 列
    """
    path = os.path.abspath('.\\data') + "\\" + _filename
    print('读取文件的路径: ' + path)
    pd_read = pd.read_table(path, sep=' ', header=None,error_bad_lines=False)
    pd_read = pd_read.values[:, col]
    print('读取数据量: ', pd_read.size, ' 末端数据的值: ', pd_read[pd_read.size - 1])
    return pd_read

def read_gps_data(_filename, col):
    """
    _filename:   文件名
    col:         读取数据的第 col 列
    """
    path = os.path.abspath('.\\data') + "\\" + _filename
    print('读取文件的路径: ' + path)
    pd_read = pd.read_table(path, sep=' ', header=None,error_bad_lines=False)
    pd_read = pd_read.values[:, col]
    print('读取数据量: ', pd_read.size, ' 末端数据的值: ', pd_read[pd_read.size - 1])
    return pd_read

def data_remove(_data, base, bias):
    temp = np.zeros(_data.size)
    index = 0
    for i in range(_data.size):
        if base-bias < _data[i] < base+bias:
            temp[index] = _data[i]
            index += 1

    print("处理后的数据量：" + str(len(temp)))
    return temp



if __name__ == '__main__':
    fig, axs = plt.subplots(0, 1)

    # data = data_remove(data,-10,2)
    plt.show()


