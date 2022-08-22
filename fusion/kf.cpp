#include <iostream>
#include "martrix.h"
const double epsilon = 1e-12; //小于该数判断为0
/**
 * t  采样频率
 * K  卡尔曼增益
 * N  先验估计
 * NN 后验估计
 * Q  系统过程噪声协方差矩阵
 * R  测量噪声
 * ka imu中获取的加速度
 * z  gnss中获取的高程
 */
double ka = 0;
double z = 0;
double t = 1;
double R = 0.003112 * t;
vector<vector<double>> N = creatmatrix(2, 1);
vector<vector<double>> F = creatmatrix(2, 2);
vector<vector<double>> B = creatmatrix(2, 1);
vector<vector<double>> P = creatmatrix(2, 2);
vector<vector<double>> Q = creatmatrix(2, 2);
vector<vector<double>> H = creatmatrix(1, 2);
vector<vector<double>> G = creatmatrix(2, 2);
vector<vector<double>> K = creatmatrix(2, 1);
vector<vector<double>> NN = creatmatrix(2, 1);

void kalmanInit()
{
  F[0][0] = 1;
  F[0][1] = t;
  F[1][0] = 0;
  F[1][1] = 1;

  B[0][0] = 1 / 2 * t * t;
  B[1][0] = t;

  P[0][0] = 1;
  P[0][1] = 0;
  P[1][0] = 0;
  P[1][1] = 1;

  Q[0][0] = 0.001;
  Q[0][1] = 0;
  Q[1][0] = 0;
  Q[1][1] = 0.01;

  H[0][0] = 1;
  H[0][1] = 0;

  G[0][0] = 1;
  G[1][1] = 1;
}

void kalmanFilter(double _ka, string _z)
{
  ka = _ka;
  z = stringToNum<double>(_z);
  // Predict
  N = plusMartrix(multiply(F, N), multiplyWithNumber(B, ka));
  P = plusMartrix(multiply(multiply(F, P), trans(F)), Q);
  // Updata
  K = multiplyWithNumber(multiply(P, trans(H)), 1.0 / (getnum(multiply(multiply(H, P), trans(H))) + R));
  NN = plusMartrix(N, multiplyWithNumber(K, z - getnum(multiply(H, N))));
  P = multiply(minusMatrix(G, multiply(K, H)), P);
  N = NN;
}