#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np
import pandas as pd
import os
import matplotlib.pyplot as plt

from scipy.fft import fft
import scipy.integrate
from scipy.signal import hilbert

class KalmanFilter:
    """
    初始值：
    t 采样频率
    P 误差协方差矩阵
    F 先验估计的系统参数（固定，取决于公式推导）
    B 系统过程参数（固定，取决于公式推导）
    Q 系统过程噪声协方差矩阵
    H 系统的测量系数（固定，取决于公式推导）
    R 测量噪声
    K 卡尔曼增益
    N 先验估计
    NN 后验估计
    """
    t = 0.1  # 采样频率
    P = np.mat([[1, 0],
                [0, 1]])
    F = np.mat([[1, t],
                [0, 1]])
    B = np.mat([[t * t / 2],
                [t]])
    Q = np.mat([[0.001, 0],
                [0, 0.01]])
    H = np.mat([[1, 0]])
    # R = 0.003112 * t
    R = 20

    def __init__(self):
        self.N = np.zeros((2, 1))
        self.NN = np.zeros((2, 1))
        self.K = np.zeros((2, 1))
        self.a = 0
        self.z = 0
        return

    def basicKf(self):
        # Predict
        self.N = self.F * self.N + self.B * self.a
        self.P = self.F * self.P * np.transpose(self.F) + self.Q
        # Update
        self.K = self.P * np.transpose(self.H) / (self.H * self.P * np.transpose(self.H) + self.R)
        self.NN = self.N + self.K * (self.z - self.H * self.N)
        self.P = (np.identity(2) - self.K * self.H) * self.P
        self.N = self.NN

