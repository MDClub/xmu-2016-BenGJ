#pragma  once

#include <stdlib.h>
#include "BEEPSGain.h"
#include "BEEPSProgressive.h"
#include "BEEPSRegressive.h"

class BEEPSVerticalHorizontal
{
private:
	double* data;  
	int width;    
	int height;    
	double photometricDec;   
	double spatialDecay;     
public:
	BEEPSVerticalHorizontal(double* _data,int _width,int _height,double _photometricDec,double _spatialDecay)
	{
		data=_data;
		width=_width;
		height=_height;
		photometricDec=_photometricDec;
		spatialDecay=_spatialDecay;
	}

	void Run()
	{
		BEEPSGain::Init(spatialDecay);   
		BEEPSProgressive::Init(photometricDec,spatialDecay);
		BEEPSRegressive::Init(photometricDec,spatialDecay);
	    int k1,k2;
		//��ʱ���ݻ�����
		double* g=new double[width*height];
		double* p=new double[width*height];
		double* r=new double[width*height];
		//��ɴ�ֱ����
		int m=0,n=0;
		for(k1=0;k1<height;k1++)
		{
			n=k1;
			for(k2=0;k2<width;k2++)
			{
				p[n]=data[m++];
				n+=height;
			}
		}
		//����
		memcpy(g,p,width*height*sizeof(double));
		memcpy(r,p,width*height*sizeof(double));

		for(k1=0;k1<width;k1++)
		{
			BEEPSProgressive bepp(g,k1*height,height);
			BEEPSGain bepg(p,k1*height,height);
			BEEPSRegressive bepr(r,height*k1,height);
			bepp.Run();
			bepg.Run();
			bepr.Run();
		}
		//��һ�ּ���  �����Ĺ�ʽ
		for(k1=0;k1<width*height;k1++)
		{
			r[k1]+=g[k1]-p[k1];
		}
		//���ˮƽ���� 
		m=0;
		for(k1=0;k1<width;k1++)
		{
			n=k1;
			for(k2=0;k2<height;k2++)
			{
				p[n]=r[m++];
				n+=width;
			}
		}
		//��������
		memcpy(g,p,width*height*sizeof(double));
		memcpy(r,p,width*height*sizeof(double));
		//ˮƽ�����ϵļ���
		for(k1=0;k1<height;k1++)
		{
			BEEPSProgressive bepp(g,k1*width,width);
			BEEPSGain bepg(p,k1*width,width);
			BEEPSRegressive bepr(r,width*k1,width);
			bepp.Run();
			bepg.Run();
			bepr.Run();
		}
		for(k1=0;k1<width*height;k1++)
		{
			data[k1]=r[k1]+g[k1]-p[k1];
		}
		delete[] g;
		delete[] p;
		delete[] r;
	}
};