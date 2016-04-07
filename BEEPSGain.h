#pragma once

//这是算法乘式的中间部分 是要减去的

class BEEPSGain
{
private:
	//float* data;   //数据内容
	double* data;
	int startIndex;  //起始索引
	int length;    //长度
private:
	static double mu;   //乘积常数
public:
	BEEPSGain(double* _data,int _startIndex,int _length)
	{
		data=_data;
		startIndex=_startIndex;
		length=_length;
	}

	static void Init(double spatialContraDecay)	 
	{
		mu=(1.0-spatialContraDecay)/(1.0+spatialContraDecay);  //空域的值  计算  
	}
   
	void Run()
	{
		for(int i=startIndex;i<startIndex+length;i++)
			data[i]*=mu;  //乘积 
	}
};
//静态变量初始化
double BEEPSGain::mu=0;