#pragma once

//公式里面第一个  

class BEEPSRegressive
{
private:
	double* data;
	int startIndex;
	int length;
private:
	static double c;   
	static double rho;      
	static double spatialContradecay; 
public:
	BEEPSRegressive(double* _data,int _startIndex,int _length)
	{
		data=_data;
		startIndex=_startIndex;
		length=_length;
	}

	static void Init(double photoStandDev,double _spatialContradecay)
	{
		spatialContradecay=_spatialContradecay;
		rho=1.0+_spatialContradecay;
		c=-0.5/(photoStandDev*photoStandDev);  
	}

	void Run()
	{   //和前面相反  
		double mu=0.0;
		data[startIndex+length-1]/=rho;  
		for(int k=startIndex+length-2;k>=startIndex;k--)
		{
			mu=data[k]-rho*data[k+1];   
			mu=spatialContradecay*exp(c*mu*mu);  
			data[k]=data[k+1]*mu+data[k]*(1-mu)/rho;  
		}
	}
};

double BEEPSRegressive::c=0;
double BEEPSRegressive::rho=0;
double BEEPSRegressive::spatialContradecay=0;