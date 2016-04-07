#pragma once

//��ʽ�����һ��  

class BEEPSProgressive
{
private:
	double* data;
	int startIndex;
	int length;
private:
	static double c;   //ֵ���ϵ�� 
	static double rho;    //1+����ϵ�� ����ĸʹ��  
	static double spatialContradecay;  //����ϵ��
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
		c=-0.5/(photoStandDev*photoStandDev);  //����ĸ�˹�˲�ǰ�����
	}

	void Run()
	{
		double mu=0.0;
		data[startIndex]/=rho;  //���ȳ��� ���  ��֤����ÿ����������
		for(int k=startIndex+1;k<startIndex+length;k++)
		{
			mu=data[k]-rho*data[k-1];   //����ƽ��ֵ�ļ���
			mu=spatialContradecay*exp(c*mu*mu);  //��˹��ʽ�Ϳ���ϵ���˻�
			data[k]=data[k-1]*mu+data[k]*(1-mu)/rho;  //��ʽһ ����    /rho  
		}
	}
};

double BEEPSProgressive::c=0;
double BEEPSProgressive::rho=0;
double BEEPSProgressive::spatialContradecay=0;