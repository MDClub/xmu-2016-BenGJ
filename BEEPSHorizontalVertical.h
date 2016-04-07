#pragma  once

#include <stdlib.h>
#include "BEEPSGain.h"
#include "BEEPSProgressive.h"
#include "BEEPSRegressive.h"

class BEEPSHorizontalVertical
{
private:
	double* data;  //数据
	int width;    //宽度
	int height;    //高度
	double photometricDec;   //值域系数
	double spatialDecay;     //空域系数
public:
	BEEPSHorizontalVertical(double* _data,int _width,int _height,double _photometricDec,double _spatialDecay)
	{
		data=_data;
		width=_width;
		height=_height;
		photometricDec=_photometricDec;
		spatialDecay=_spatialDecay;
	}

	void Run()
	{
		BEEPSGain::Init(spatialDecay);                       //各自参数的初始化
		BEEPSProgressive::Init(photometricDec,spatialDecay);
		BEEPSRegressive::Init(photometricDec,spatialDecay);

		//临时数据缓存区
		double* g=new double[width*height];
		double* p=new double[width*height];
		double* r=new double[width*height];
		//复制
		memcpy(g,data,width*height*sizeof(double));
		memcpy(p,data,width*height*sizeof(double));
		memcpy(r,data,width*height*sizeof(double));

		//水平方向上的计算
		int k1,k2;
		for(k1=0;k1<height;k1++)
		{
			BEEPSProgressive bepp(g,k1*width,width);
			BEEPSGain bepg(p,k1*width,width);
			BEEPSRegressive bepr(r,width*k1,width);
			bepp.Run();
			bepg.Run();
			bepr.Run();
		}
		//第一轮计算  见论文公式
		for(k1=0;k1<width*height;k1++)
		{
			r[k1]+=g[k1]-p[k1];
		}
		//变成垂直数据
		int m=0,n=0;
		for(k1=0;k1<height;k1++)
		{
			n=k1;
			for(k2=0;k2<width;k2++)
			{
				p[n]=r[m++];
				n+=height;
			}
		}
		//复制数据
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
		for(k1=0;k1<width*height;k1++)
		{
			r[k1]+=g[k1]-p[k1];
		}
		//变成水平数据 
		m=0;
		for(k1=0;k1<width;k1++)
		{
			n=k1;
			for(k2=0;k2<height;k2++)
			{
				data[n]=r[m++];
				n+=width;
			}
		}
		//释放资源
		delete[] g;
		delete[] p;
		delete[] r;
	}
};