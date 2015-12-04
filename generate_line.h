//������ 2014/1/20 ��������ֱ��ɨ���ߵ�ͷ�ļ���Ϊ��ֹ������ͻ�����������ռ�xd���档
//������ 2015/4/7  ������ֱ��ɨ����ǰ������ֱ�߿�Խ��ֵ��ʱ�ķ����㷨������FDM�ӹ����ͼ����ܻ�������⡣
#ifndef GENERATE_LINE
#define GENERATE_LINE

#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include "clipper.hpp"

namespace xd
{
	float const pi = 3.141592653589793;
    float degrees(float fudu); //����ת��Ϊ�Ƕȵĺ���
    float radians(float jiaodu); //�Ƕ�ת��Ϊ���ȵĺ���
	class xdpoint   //�������࣬����ƽ���ڼ�����͵ѿ�������ϵ��ת��
	{
	public:
		float x;
		float y;
		float z;
		xdpoint():x(0),y(0),z(0){}
		xdpoint(float a,float b):x(a),y(b),z(0){}  //��������غ���zĬ��Ϊ0���Ժ���ԸĽ�
		xdpoint(float a, float b,float c):x(a),y(b),z(c){}
		float modulus() const;
		float argument() const;
		xdpoint & modulus(float new_modulus);
		xdpoint & argument(float new_argument);
	};

	
	typedef std::vector<xdpoint> outline;
	typedef std::vector<outline> outlines;

    bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b);  //�ж���ֵ���б����Ƿ����ĳһ����ĺ�����


    int IsLeft(xdpoint p1,xdpoint p2, xdpoint p); //�ж�p���Ƿ��������߶�p1,p2���ĺ�����

    bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV);  //�ж�ɨ�����е�Ԫ���Ƿ�ȫ��ȡ��ĺ�����


    void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem);   //��������������ݵ�ɨ����ɾ���ĺ���������������ò��Ŵ˺�����


    bool IsDealFinish(std::vector<xdpoint> l,int & i);  //�ж�ƽ����x���ֱ�����Ƿ��ж���������ĺ�����


    bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY);  //�����ж�ǰ������ֱ���Ƿ�Խ���˾ֲ���ֵ��ĺ���


    class CTwoDimensionVector //���쵥λʸ���࣬����ʸ���Ĳ�˺͵�ˡ�
	{
	public: 
		float x;
		float y;
		CTwoDimensionVector(float a, float b)
		{
			float m=sqrt(pow(a,2)+pow(b,2));
			x=a/m;
			y=b/m;
		}
		float CrossProduct(const CTwoDimensionVector & v)
		{
			return (x*v.y-y*v.x);
		}
		float DotProduct(const CTwoDimensionVector & v)
		{
			return (x*v.x-y*v.y);
		}
	};

    void DealCompensate(outline DealData,outline & ResultData,float R); //�������������ݣ�ʹ���Ϊ�в����뾶�����������ݡ� ���뵽�Լ����ڳ����У�2015/3/10 �����д�

    void InfillLine(outlines TheOutline,outlines & TheResult,float width,float degree,int lunkuo ); //�Լ���д����������ɺ�����
	
    void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ); //�Լ���д����������ɺ�����

    void InfillBMP(outlines TheOutline,std::vector<std::vector<int> > & ResultData,int piex ,int size);

    void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult);    //��ѡ��ͻ�����ĺ���

    bool isSingleInfill(int layerNumber,std::vector<int> needInfillLayer);

    void InfillOffset(outlines theOutline,outlines & theResult,float width);
}


#endif
