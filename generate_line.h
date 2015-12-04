//王晓东 2014/1/20 编制生成直线扫描线的头文件，为防止命名冲突，放在命名空间xd里面。
//王晓东 2015/4/7  增加了直线扫描在前后两条直线跨越极值点时的分区算法，否则FDM加工成型件可能会出现问题。
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
    float degrees(float fudu); //幅度转化为角度的函数
    float radians(float jiaodu); //角度转换为幅度的函数
	class xdpoint   //基本点类，包含平面内极坐标和笛卡尔坐标系的转换
	{
	public:
		float x;
		float y;
		float z;
		xdpoint():x(0),y(0),z(0){}
		xdpoint(float a,float b):x(a),y(b),z(0){}  //这里的重载函数z默认为0，以后可以改进
		xdpoint(float a, float b,float c):x(a),y(b),z(c){}
		float modulus() const;
		float argument() const;
		xdpoint & modulus(float new_modulus);
		xdpoint & argument(float new_argument);
	};

	
	typedef std::vector<xdpoint> outline;
	typedef std::vector<outline> outlines;

    bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b);  //判读极值点列表里是否包含某一个点的函数。


    int IsLeft(xdpoint p1,xdpoint p2, xdpoint p); //判断p点是否在有向线段p1,p2左侧的函数。

    bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV);  //判断扫描线中的元素是否全部取完的函数。


    void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem);   //将存放奇数点数据的扫描线删除的函数，正常情况下用不着此函数。


    bool IsDealFinish(std::vector<xdpoint> l,int & i);  //判断平行于x轴的直线上是否有多于两个点的函数。


    bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY);  //用来判断前后两条直线是否越过了局部极值点的函数


    class CTwoDimensionVector //构造单位矢量类，包含矢量的叉乘和点乘。
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

    void DealCompensate(outline DealData,outline & ResultData,float R); //处理轮廓点数据，使其变为有补偿半径的轮廓点数据。 融入到自己后期程序中，2015/3/10 可能有错

    void InfillLine(outlines TheOutline,outlines & TheResult,float width,float degree,int lunkuo ); //自己编写的填充线生成函数。
	
    void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ); //自己编写的填充线生成函数。

    void InfillBMP(outlines TheOutline,std::vector<std::vector<int> > & ResultData,int piex ,int size);

    void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult);    //挑选出突变截面的函数

    bool isSingleInfill(int layerNumber,std::vector<int> needInfillLayer);

    void InfillOffset(outlines theOutline,outlines & theResult,float width);
}


#endif
