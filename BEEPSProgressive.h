#pragma once

//公式里面第一个  

class BEEPSProgressive
{
private:
	double* data;
	int startIndex;
	int length;
private:
	static double c;   //值域的系数 
	static double rho;    //1+空域系数 做分母使用  
	static double spatialContradecay;  //空域系数
public:
	BEEPSProgressive(double* _data,int _startIndex,int _length)
	{
		data=_data;
		startIndex=_startIndex;
		length=_length;
	}

	static void Init(double photoStandDev,double _spatialContradecay)
	{
		spatialContradecay=_spatialContradecay;
		rho=1.0+_spatialContradecay;
		c=-0.5/(photoStandDev*photoStandDev);  //下面的高斯滤波前面的数
	}

	void Run()
	{
		double mu=0.0;
		data[startIndex]/=rho;  //首先除以 这个  保证后面每个数都除以
		for(int k=startIndex+1;k<startIndex+length;k++)
		{
			mu=data[k]-rho*data[k-1];   //上面平方值的计算
			mu=spatialContradecay*exp(c*mu*mu);  //高斯公式和空域系数乘积
			data[k]=data[k-1]*mu+data[k]*(1-mu)/rho;  //公式一 计算    /rho  
		}
	}
};

double BEEPSProgressive::c=0;
double BEEPSProgressive::rho=0;
double BEEPSProgressive::spatialContradecay=0;