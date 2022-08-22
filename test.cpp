#include <iostream>
#include<vector>

using namespace std;
const double epsilon=1e-12;  //小于该数判断为0
//创建 h行l列的矩阵，并将初始各值设定为0
vector<vector<double>> creatmatrix(int h,int l)
{
	vector<vector<double>> v;
	for (int i = 0; i < h; i++)
	{
		vector<double>v1(l,0);
		v.push_back(v1);
	}
	return v;
}
//矩阵A*矩阵B=矩阵C，并返回
vector<vector<double>> multiply(const vector<vector<double>>&A,const vector<vector<double>>&B)
{
	int A_h=A.size();
	int A_l=A[0].size();
	int B_h=B.size();
	int B_l=B[0].size();
	if(A_l !=B_h)
	{
		cout<<"两矩阵维数无法相乘"<<endl;
		exit(0);
	}
	vector<vector<double>> C=creatmatrix(A_h,B_l);
	for (int i = 0; i < A_h; i++)
	{
		for (int j = 0; j < B_l; j++)
		{
			C[i][j]=0;
			for (int k = 0; k < A_l; k++)
			{
				C[i][j] +=A[i][k]*B[k][j];
			}
			if (abs(C[i][j])<epsilon)
			{
				C[i][j]=0.0;
			}
			//cout<<C[i][j]<<"\t";
		}
		//cout<<endl;
	}
	return C;
}

void show_matrix(const vector<vector<double>> &A)
{
	int h=A.size();
	int l=A[0].size();
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < l; j++)
		{
			cout<<A[i][j]<<"\t";
		}
		cout<<endl;
	}
}


int main(){
    vector<vector<double>> H = creatmatrix(1,2);
    H[0][0] = 1;
    H[0][1] = 2;
    vector<vector<double>> B = creatmatrix(2,1);
    B[0][0] = 1;
    B[1][0] = 2;
    vector<vector<double>> temp = multiply(H,B);
	double a = temp[0][0];
	printf("%f",a);
}