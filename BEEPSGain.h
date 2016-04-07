#pragma once

//�����㷨��ʽ���м䲿�� ��Ҫ��ȥ��

class BEEPSGain
{
private:
	//float* data;   //��������
	double* data;
	int startIndex;  //��ʼ����
	int length;    //����
private:
	static double mu;   //�˻�����
public:
	BEEPSGain(double* _data,int _startIndex,int _length)
	{
		data=_data;
		startIndex=_startIndex;
		length=_length;
	}

	static void Init(double spatialContraDecay)	 
	{
		mu=(1.0-spatialContraDecay)/(1.0+spatialContraDecay);  //�����ֵ  ����  
	}
   
	void Run()
	{
		for(int i=startIndex;i<startIndex+length;i++)
			data[i]*=mu;  //�˻� 
	}
};
//��̬������ʼ��
double BEEPSGain::mu=0;