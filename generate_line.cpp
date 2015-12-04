#include "generate_line.h"

#include <QDebug>

namespace xd
{

float degrees(float fudu) //幅度转化为角度的函数
{
    return(fudu/pi*180);
}
float radians(float jiaodu) //角度转换为幅度的函数
{
    return(jiaodu/180*pi);
}


float xdpoint::modulus() const
{
    return std::sqrt(x*x+y*y);
}
xdpoint & xdpoint::modulus(float new_modulus)
{
    float const old_modulus(modulus());
    float const scale(new_modulus/old_modulus);
    x*=scale;
    y*=scale;
    return *this;
}
float xdpoint::argument() const
{
    return degrees(atan2(y,x));
}
xdpoint & xdpoint::argument(float new_argument)
{
    float const mod(modulus());
    x=mod*std::cos(radians(new_argument));
    y=mod*std::sin(radians(new_argument));
    return *this;
}

typedef std::vector<xdpoint> outline;
typedef std::vector<outline> outlines;

bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b)  //判读极值点列表里是否包含某一个点的函数。
{
    //注意，list可能为为空！
    for (int i=0;i!=a.size();i++)
    {
        if ((a[i].first==b.x)&&(a[i].second==b.y))
        {
            return true;
        }
    }
    return false;
}

int IsLeft(xdpoint p1,xdpoint p2, xdpoint p) //判断p点是否在有向线段p1,p2左侧的函数。
{
    float x1=p1.x;
    float y1=p1.y;
    float x2=p2.x;
    float y2=p2.y;
    float x=p.x;
    float y=p.y;
    float det=x*y1-x*y2+y*x2-y*x1+x1*y2-x2*y1;
    if(det>0)
    {
        return(1); //p在左侧。
    }
    else if(det<0)
    {
        return(-1); //p在右侧。
    }
    else
    {
        return(0); //p与p1,p2共线。
    }
}
bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV)  //判断扫描线中的元素是否全部取完的函数。
{
    firstV=0;
    for (int j=0;j<tem.size();j++)
    {
        if (!tem[j].second.empty())
        {
            firstV=j;
            return 0;
        }
    }
    return 1;
}

void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem)   //将存放奇数点数据的扫描线删除的函数，正常情况下用不着此函数。
{                                                           //因为正常情况下就不可能存在奇数点的数据。
    int Num=0;
    for (int i=0;i!=tem.size();i++)
    {
        if (tem[i].second.size()%2==1)
        {
            Num++;
        }
    }
    for (int i=0;i!=Num;i++)
    {
        for (int j=0;j!=tem.size();j++)
        {
            if (tem[j].second.size()%2==1)
            {
                std::vector<std::pair<float,std::list<float> > >::iterator aaa=tem.begin()+j;
                tem.erase(aaa);
                break;
            }
        }
    }
}

bool IsDealFinish(std::vector<xdpoint> l,int & i)  //判断平行于x轴的直线上是否有多于两个点的函数。
{
    int n=(int)l.size();          //这里的n的取值千万不能减去1，因为起点和尾点已经未必相等了！
    for (i=0;i!=l.size();i++)     //注意，这里必须不经过最后一个点，因为最后一个点和第一个点必须相等，要减去2？
    {                               //想想为什么？呵呵！出现了奇怪的现像！！在此处循环要全部遍历！
        int benshen=i%n;
        int nextP=(i+1)%n;
        int nextnP=(i+2)%n;
        if ((l[nextP].y-l[benshen].y)*(l[nextnP].x-l[nextP].x)==(l[nextnP].y-l[nextP].y)*(l[nextP].x-l[benshen].x))
        {
            i=nextP; //要删除的元素的角标。
            return 0;
        }
    }
    return 1;
}

bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY)  //用来判断前后两条直线是否越过了局部极值点的函数
{
    if (bijiaoY1>bijiaoY2)
    {
        return false;
    }
    for (std::list<float>::iterator i=maxY.begin();i!=maxY.end();++i)
    {
        if ((*i-bijiaoY1)*(*i-bijiaoY2)<0)
        {
            return true;
        }
    }
    for (std::list<float>::iterator i=minY.begin();i!=minY.end();++i)
    {
        if ((*i-bijiaoY1)*(*i-bijiaoY2)<0)
        {
            return true;
        }
    }
    return false;
}



void DealCompensate(outline DealData,outline & ResultData,float R) //处理轮廓点数据，使其变为有补偿半径的轮廓点数据。 融入到自己后期程序中，2015/3/10 可能有错
{
    float layerZ=DealData[0].z;
    int subscript1=DealData.size()-2;  //轮廓数据最后一个点与第一个点重复！！时刻记着！！！
    int subscript2=0;
    int subscript3=1;
    CTwoDimensionVector L1(DealData[subscript2].x-DealData[subscript1].x,DealData[subscript2].y-DealData[subscript1].y);
    CTwoDimensionVector L2(DealData[subscript3].x-DealData[subscript2].x,DealData[subscript3].y-DealData[subscript2].y);

  /*  if ((L1.CrossProduct(L2)<1)&&(L1.DotProduct(L2)>=-1))
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));

    }
    else if((L1.CrossProduct(L2)>-5)) //这里主要是可能出现非常平行的情况，影响判断！！！
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    else if (L1.CrossProduct(L2)==0)
    {

    }
    else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
    {
        ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
       ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //出现两个点的时候累积一定不要弄反了！
    } */

    ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    for (int i=1;i<DealData.size()-1;i++)
    {
        subscript1=i-1;
        subscript2=i;
        subscript3=i+1;
        CTwoDimensionVector L1(DealData[subscript2].x-DealData[subscript1].x,DealData[subscript2].y-DealData[subscript1].y);
        CTwoDimensionVector L2(DealData[subscript3].x-DealData[subscript2].x,DealData[subscript3].y-DealData[subscript2].y);
        /*
        if(i==8)
        {
            double tem1=L1.CrossProduct(L2);
            double tem2=L1.DotProduct(L2);
            std::cout<<std::endl;
        }
        */
      /*  if ((L1.CrossProduct(L2)<1)&&(L1.DotProduct(L2)>=-1))
        {
            ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));

        }
        else if((L1.CrossProduct(L2)>-5))   //这里主要是可能出现非常平行的情况，影响判断！！！
        {
            ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
        }
        else if (L1.CrossProduct(L2)==0)
        {
             //这种情况不可能发生，因为前后共线的可能性已经被删除了！
        }
        else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
        {
           ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
           ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //出现两个点的时候累积一定不要弄反了！

        } */

         ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    ResultData.push_back(xdpoint(ResultData[0].x,ResultData[0].y,layerZ));  //保证最后一个点和第一个点相同。
}

void InfillLine(outlines TheOutline,outlines & TheResult,float width,float degree,int lunkuo ) //自己编写的填充线生成函数。
{
    if(!TheOutline.empty())  //首先必须保证有数据
    {
        float layerZValue=TheOutline[0][0].z;
        //将轮廓数据旋转一个角度
        for (int i=0;i!=TheOutline.size();++i)
        {
            for (int j=0;j!=TheOutline[i].size();++j)
            {
                TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
            }
        }

        //需要一次轮廓偏置来使得外轮廓比较平滑
        std::vector<outlines> dataOffsets;  //存储所有偏置轮廓的数据
        if(lunkuo!=0)
        {
            ClipperLib::ClipperOffset temO;
            for (int i=0;i!=TheOutline.size();++i)
            {
                ClipperLib::Path temP;
                for (int j=0;j!=TheOutline[i].size();++j)
                {
                    temP<<ClipperLib::IntPoint(TheOutline[i][j].x*1000000,TheOutline[i][j].y*1000000);
                }
                temO.AddPath(temP,ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
            }

            for(int temi=0;temi!=lunkuo;++temi)
            {
                ClipperLib::Paths solution;
                temO.Execute(solution, -width*1000000*(temi+1));

                xd::outlines dataOffset;
                for (int i=0;i!=solution.size();++i)
                {
                    xd::outline temData;
                    for (int j=0;j!=solution[i].size();++j)
                    {
                        temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000,(float)solution[i][j].Y/1000000));
                    }
                    temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000,(float)solution[i][0].Y/1000000));//加上最后一个点，保证封闭
                    dataOffset.push_back(temData);
                }
                dataOffsets.push_back(dataOffset);
            }
            TheOutline.clear();
            TheOutline=dataOffsets[dataOffsets.size()-1];
        }
        if(TheOutline.size()!=0)
        {
            //第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
            std::list<float> maxY;
            std::list<float> minY;
            std::vector<std::pair<float,float> > maxPoint;
            std::vector<std::pair<float,float> > minPoint;
            for (int i=0;i!=TheOutline.size();i++)
            {
                for (int j=1;j!=TheOutline[i].size();j++)
                {
                    int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                    int beforeP=(j-1+n)%n;
                    int beforebP=(j-2+n)%n;
                    int nextP=(j+1+n)%n;
                    int nextnP=(j+2+n)%n;
                    if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
                    {
                        maxY.push_back(TheOutline[i][j].y);
                        std::pair<float,float> tem;
                        tem.first=TheOutline[i][j].x;
                        tem.second=TheOutline[i][j].y;
                        maxPoint.push_back(tem);
                    }
                    else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
                    {
                        minY.push_back(TheOutline[i][j].y);
                        std::pair<float,float> tem;
                        tem.first=TheOutline[i][j].x;
                        tem.second=TheOutline[i][j].y;
                        minPoint.push_back(tem);
                    }
                    else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
                    {
                        if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                    else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
                    {
                        if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                }
            }
            maxY.sort();
            minY.sort();
            maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
            minY.unique();	  //同上。
            float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
            float MinY=*min_element(minY.begin(),minY.end());
            //第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
            std::vector<std::pair<float,std::list<float> > > Linedate;
            for (int i=1;i<(MaxY-MinY)/width-1;i++)
            {
                std::pair<float,std::list<float> > tem;
                tem.first=MinY+width*i;
                Linedate.push_back(tem);
            }

            if(!Linedate.empty())     //记住，Linedate可能是空的！ 2015_6_17
            {
                if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
                {

                    std::pair<float,std::list<float> > tem;
                    tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
                    Linedate.push_back(tem);
                }
                else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
                {
                    for (int i=0;i!=Linedate.size();++i)
                    {
                        Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
                    }
                }

                for (int i=0;i!=TheOutline.size();i++)
                {

                    for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
                    {
                        int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                        int beforeP=(j-1+n)%n;
                        int beforebP=(j-2+n)%n;
                        int nextP=(j+1+n)%n;
                        int nextnP=(j+2+n)%n;
                        if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
                        {
                            for (int k=0;k!=Linedate.size();k++)
                            {
                                if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
                                {
                                    float x1=TheOutline[i][j].x;
                                    float x2=TheOutline[i][nextP].x;
                                    float y1=TheOutline[i][j].y;
                                    float y2=TheOutline[i][nextP].y;
                                    Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                                }
                                else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
                                {
                                    if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                                    {
                                        //这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                    }
                                }
                                else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
                                {
                                    if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                                    {
                                        //这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                }
                            }
                        }
                        else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
                        {
                            for (int k=0;k!=Linedate.size();k++)
                            {
                                if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
                                {
                                    if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
                                    {   //顺时针向右转是内点。
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                    else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                                    {
                                        //两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
                                    }

                                    else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                                    {
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                    }
                                    else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))
                                    {
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                }
                            }
                        }
                    }

                }

                for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
                {
                    Linedate[i].second.sort();    //排序。
                    Linedate[i].second.unique();  //将多余的点取出来。
                }
                DeleteOddDate(Linedate);    //暂时加上，看看情况！

                //第三步，将数据分区域存放。
                int FirstNonZero;
                while(!IsEmpty(Linedate,FirstNonZero))
                {
                    outline tem;
                    int j=0;  //用来判断是奇数行还是偶数行的参数。
                    std::list<float>::iterator firstIn=Linedate[FirstNonZero].second.begin();
                    float bijiao1=*firstIn;   //第一条线的第一个点。
                    tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
                    tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
                    int i=(FirstNonZero+1)%Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
                    float bijiao2=*firstIn;  //第一条线的第二个点。
                    float bijiaoY1=Linedate[FirstNonZero].first;  //第一条线得Y坐标值。
                    std::list<float>::iterator tem1s=Linedate[FirstNonZero].second.begin();
                    std::list<float>::iterator tem1e=tem1s;
                    tem1e++;
                    tem1e++;
                    Linedate[FirstNonZero].second.erase(tem1s,tem1e);
                    while ((!Linedate[i].second.empty()))
                    {
                        std::list<float>::iterator Line2first=Linedate[i].second.begin();  //第二条线的第一个点。

                        float bijiaoY2=Linedate[i].first; //第二个条直线的Y坐标值。
                        if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
                        {
                            //前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
                            break;
                        }
                        bijiaoY1=bijiaoY2;

                        if(*Line2first>bijiao2)
                        {
                            //第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
                            break;
                        }
                        if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
                        {
                            //第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
                            break;
                        }
                        if (j%2==0)
                        {
                            std::list<float>::iterator tem2s=Linedate[i].second.begin();
                            std::list<float>::iterator tem2e=tem2s;
                            tem2e++;
                            tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
                            tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
                        }
                        else
                        {
                            std::list<float>::iterator tem2s=Linedate[i].second.begin();
                            std::list<float>::iterator tem2e=tem2s;
                            tem2e++;
                            tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
                            tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
                        }
                        std::list<float>::iterator tem3s=Linedate[i].second.begin();
                        std::list<float>::iterator tem3e=tem3s;
                        bijiao1=*tem3e;
                        tem3e++;
                        bijiao2=*tem3e;
                        tem3e++;
                        Linedate[i].second.erase(tem3s,tem3e);
                        ++j;
                        ++i;
                        if (i==Linedate.size())
                        {
                            break;
                        }
                    }
                    TheResult.push_back(tem);
                }

                //增加一步，为了适应FDM的格式，每个线段要缩短一个半径补偿
                for (int i=0;i!=TheResult.size();++i)
                {
                    for (int j=0;j!=TheResult[i].size();++j)
                    {

                        if (j%4==3||j%4==0)
                        {
                            TheResult[i][j].x+=0.28;    //此处是默认值。

                        }
                        else
                        {
                            TheResult[i][j].x-=0.28;
                        }
                    }
                }
            }
        }

        //记得最后加上偏置轮廓。
        if(lunkuo!=0)
        {
            for(int i=0;i!=dataOffsets.size();++i)
            {
                for(int j=0;j!=dataOffsets[i].size();++j)
                {
                    TheResult.insert(TheResult.begin(),dataOffsets[i][j]);
                }
            }
        }
        //填充线的结果转回选择角度
        for (int i=0;i!=TheResult.size();++i)
        {
            for (int j=0;j!=TheResult[i].size();++j)
            {
                TheResult[i][j].argument(TheResult[i][j].argument()+degree);
                TheResult[i][j].z=layerZValue;
            }
        }
    }
}

void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ) //自己编写的填充线生成函数。
{
    //将轮廓数据旋转一个角度
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
        }
    }

    //第一步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float> > maxPoint;
    std::vector<std::pair<float,float> > minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
    minY.unique();	  //同上。
    float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
    float MinY=*min_element(minY.begin(),minY.end());
    //第二步，生成每一条线填充线与轮廓线的交点，分别存储在双向链表中。
    std::vector<std::pair<float,std::list<float> > > Linedate;
    for (int i=1;i<(MaxY-MinY)/width-1;i++)
    {
        std::pair<float,std::list<float> > tem;
        tem.first=MinY+width*i;
        Linedate.push_back(tem);
    }
    if(!Linedate.empty())       //2015_6_17   记住，Linedate可能是空的！！
    {
        if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//为了使得最后一根填充线不至于与轮廓离得太远，要加一个是否需要增加一条线段判断
        {

            std::pair<float,std::list<float> > tem;
            tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
            Linedate.push_back(tem);
        }
        else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //让每一条线的间距都增加一点的自适应线宽补偿
        {
            for (int i=0;i!=Linedate.size();++i)
            {
                Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
            }
        }

        for(int i=0;i!=TheOutline.size();i++)
        {

            for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
            {
                int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
                int beforeP=(j-1+n)%n;
                int beforebP=(j-2+n)%n;
                int nextP=(j+1+n)%n;
                int nextnP=(j+2+n)%n;
                if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
                {
                    for (int k=0;k!=Linedate.size();k++)
                    {
                        if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
                        {
                            float x1=TheOutline[i][j].x;
                            float x2=TheOutline[i][nextP].x;
                            float y1=TheOutline[i][j].y;
                            float y2=TheOutline[i][nextP].y;
                            Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                        }
                        else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
                        {
                            if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                            {
                                //这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                Linedate[k].second.push_back(TheOutline[i][j].x);
                            }
                        }
                        else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
                        {
                            if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                            {
                                //这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                                Linedate[k].second.push_back(TheOutline[i][nextP].x);
                            }
                        }
                    }
                }
                else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
                {
                    for (int k=0;k!=Linedate.size();k++)
                    {
                        if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
                        {
                            if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
                            {   //顺时针向右转是内点。
                                Linedate[k].second.push_back(TheOutline[i][j].x);
                                Linedate[k].second.push_back(TheOutline[i][nextP].x);
                            }
                            else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                            {
                                //两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
                            }

                            else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                            {
                                Linedate[k].second.push_back(TheOutline[i][j].x);
                            }
                            else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))
                            {
                                Linedate[k].second.push_back(TheOutline[i][nextP].x);
                            }
                        }
                    }
                }
            }

        }

        for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
        {
            Linedate[i].second.sort();    //排序。
            Linedate[i].second.unique();  //将多余的点取出来。
        }
        //	DeleteOddDate(Linedate);    //暂时加上，看看情况！

        //第三步，将数据分区域存放。
        int FirstNonZero;
        while(!IsEmpty(Linedate,FirstNonZero))
        {
            outline tem;
            int j=0;  //用来判断是奇数行还是偶数行的参数。
            std::list<float>::iterator firstIn=Linedate[FirstNonZero].second.begin();
            float bijiao1=*firstIn;   //第一条线的第一个点。
            tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
            tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
            int i=(FirstNonZero+1)%Linedate.size();  //必须保证当第一个非零数据的线刚好是最后一条线时也不会加1越界！所以要模一下！
            float bijiao2=*firstIn;  //第一条线的第二个点。
            float bijiaoY1=Linedate[FirstNonZero].first;  //第一条线得Y坐标值。
            std::list<float>::iterator tem1s=Linedate[FirstNonZero].second.begin();
            std::list<float>::iterator tem1e=tem1s;
            tem1e++;
            tem1e++;
            Linedate[FirstNonZero].second.erase(tem1s,tem1e);
            while ((!Linedate[i].second.empty()))
            {
                std::list<float>::iterator Line2first=Linedate[i].second.begin();  //第二条线的第一个点。

                float bijiaoY2=Linedate[i].first; //第二个条直线的Y坐标值。
                if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
                {
                    //前后两条直线如果跨越了局部极值点，也需要退出，即需要分区。
                    break;
                }
                bijiaoY1=bijiaoY2;

                if(*Line2first>bijiao2)
                {
                    //第二条直线的第一个点大于第一条直线的第二个点则退出，即需要分区
                    break;
                }
                if (*(++Line2first)<bijiao1)   //注意：这里++必须在前，因为就算有括号，++在后的话也会比较完再++！！！
                {
                    //第二条直线的第二个点小于第一条直线的第一个点则退出，即需要分区
                    break;
                }
                if (j%2==0)
                {
                    std::list<float>::iterator tem2s=Linedate[i].second.begin();
                    std::list<float>::iterator tem2e=tem2s;
                    tem2e++;
                    tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
                    tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
                }
                else
                {
                    std::list<float>::iterator tem2s=Linedate[i].second.begin();
                    std::list<float>::iterator tem2e=tem2s;
                    tem2e++;
                    tem.push_back(xdpoint(*tem2s,Linedate[i].first,TheOutline[0][0].z));
                    tem.push_back(xdpoint(*tem2e,Linedate[i].first,TheOutline[0][0].z));
                }
                std::list<float>::iterator tem3s=Linedate[i].second.begin();
                std::list<float>::iterator tem3e=tem3s;
                bijiao1=*tem3e;
                tem3e++;
                bijiao2=*tem3e;
                tem3e++;
                Linedate[i].second.erase(tem3s,tem3e);
                ++j;
                ++i;
                if (i==Linedate.size())
                {
                    break;
                }
            }
            TheResult.push_back(tem);
        }
    }


    //增加一步，为了适应FDM的格式，每个线段要缩短一个半径补偿,但是这个函数不加补偿半径！
    //for (int i=0;i!=TheResult.size();++i)
    //{
    //	for (int j=0;j!=TheResult[i].size();++j)
    //	{

    //		if (j%4==3||j%4==0)
    //		{
    //			TheResult[i][j].x+=0.28;    //此处是默认值。

    //		}
    //		else
    //		{
    //			TheResult[i][j].x-=0.28;
    //		}
    //	}
    //}

    //填充线的结果转回选择角度
    for (int i=0;i!=TheResult.size();++i)
    {
        for (int j=0;j!=TheResult[i].size();++j)
        {
            TheResult[i][j].argument(TheResult[i][j].argument()+degree);
        }
    }
}
void InfillBMP(outlines TheOutline,std::vector<std::vector<int> > & TheResult,int piex,int size ) //自己编写的bmp生成函数。
{
    float amplificationFactor=(float)piex/size;  //定义放大倍数，140是加工极限尺寸。注意，一定要显示变为float，否则得出的是一个整数。
    //第一步，放大数据。
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x*=amplificationFactor;
            TheOutline[i][j].y*=amplificationFactor;
        }
    }
    //第二步，取得轮廓数据型心。
    xd::xdpoint limitPositionA(TheOutline[0][0].x,TheOutline[0][0].y),limitPositionB(TheOutline[0][0].x,TheOutline[0][0].y);
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            if (limitPositionA.x<TheOutline[i][j].x)
            {
                limitPositionA.x=TheOutline[i][j].x;
            }
            if (limitPositionA.y<TheOutline[i][j].y)
            {
                limitPositionA.y=TheOutline[i][j].y;
            }
            if (limitPositionB.x>TheOutline[i][j].x)
            {
                limitPositionB.x=TheOutline[i][j].x;
            }
            if (limitPositionB.y>TheOutline[i][j].y)
            {
                limitPositionB.y=TheOutline[i][j].y;
            }
        }
    }
    float centroidX=(limitPositionA.x+limitPositionB.x)/2;
    float centroidY=(limitPositionA.y+limitPositionB.y)/2;
    float offsetX=piex/2-centroidX;
    float offsetY=piex/2-centroidY;
    //第三步，平移数据。
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x+=offsetX;
            TheOutline[i][j].y+=offsetY;
        }
    }
    //第四步，轮廓点的局部极大极小值点，放在一个双向链表中。注意：选择双向链表的原因是其任意位置删除和添加元素非常快捷，降低时间复杂度。
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float> > maxPoint;
    std::vector<std::pair<float,float> > minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //这个点的y值大于前后的点的y值。
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点的y值小于前后的点的y值。
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //这个点和前一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //这个点和前一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //这个点和后一个点的y值大。
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //这个点和后一个点的y值小。
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //将链表中重复的点删除的函数，属于STL的变易算法。
    minY.unique();	  //同上。
    float MaxY=*max_element(maxY.begin(),maxY.end());   //调用求链表中求最大元素的函数。
    float MinY=*min_element(minY.begin(),minY.end());
    //第五步，生成每一条像素填充线与轮廓线的交点，分别存储在双向链表中。
    std::vector<std::pair<float,std::list<float> > > Linedate;
    for (int i=1;i<=piex;i++)
    {
        std::pair<float,std::list<float> > tem;
        tem.first=(i+i-1)/2;    //这里近似用像素的中点作为扫描线得y值
        Linedate.push_back(tem);
    }

    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)  //j=1，说明要从第二个点开始循环，直到第一个点。
        {
            int n=(int)TheOutline[i].size()-1;  //轮廓数据的最后一个点是第一个点，因此要减去1。
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //轮廓线段不平行于x轴的情况。
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //线与轮廓线段相交的情况。
                    {
                        float x1=TheOutline[i][j].x;
                        float x2=TheOutline[i][nextP].x;
                        float y1=TheOutline[i][j].y;
                        float y2=TheOutline[i][nextP].y;
                        Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                    }
                    else if ((Linedate[k].first==TheOutline[i][j].y))    //线与轮廓线段前一个顶点相交的情况。
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                        {
                            //这个顶点同时又不是极值点时。同时这个点与前一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                        }
                    }
                    else if ((Linedate[k].first==TheOutline[i][nextP].y))   //线与轮廓线段后一个顶点相交的情况。
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                        {
                            //这个顶点同时又不是极值点时。同时后一个点与后后一个点的y值都不想等才行，因为相等时的情况已经已经被后面的逻辑考虑过了！！！
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                    }
                }
            }
            else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //轮廓线段平行于x轴的情况。
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if (Linedate[k].first==TheOutline[i][j].y)  //有填充线恰好与平行于x轴的轮廓线相交。
                    {
                        if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //这个轮廓线的两点同时是内点时。
                        {   //顺时针向右转是内点。
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                        {
                            //两个点都不是内点，两点都舍去，换句话说就是什么也不做，其实不用写这段代码，为了以后好理解所以写上了。
                        }

                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                        {
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                        }
                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))
                        {
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                    }
                }
            }
        }

    }

    for(int i=0;i!=Linedate.size();i++)  //将存储好的数据重复的元素删除，并且排序。
    {
        Linedate[i].second.sort();    //排序。
        Linedate[i].second.unique();  //将多余的点取出来。
    }
    //	DeleteOddDate(Linedate);    //暂时加上，看看情况！
    //第六步，将对于需要填充的序列号提取到结果中去。
    for (int i=0;i!=piex;++i)   //先将像素初始化为0
    {
        std::vector<int> temData;
        for (int j=0;j!=piex;++j)
        {
            temData.push_back(0);
        }
        TheResult.push_back(temData);
    }
    for (int i=0;i!=piex;++i)
    {
        if (Linedate[i].second.size()==0)
        {
            continue;
        }
        else if(Linedate[i].second.size()==1)
        {
            std::list<float>::iterator a=Linedate[i].second.begin();
            TheResult[i][*a]=1;
        }
        else
        {
            while(!Linedate[i].second.empty())
            {
                std::list<float>::iterator a=Linedate[i].second.begin();
                std::list<float>::iterator b=a;
                b++;
                //std::cout<<*b<<"    OK";
                for(int j=*a;j<=*b;++j)
                {
                    //std::cout<<j<<"   OK";
                    TheResult[i][j]=1;
                }
                b++;
                Linedate[i].second.erase(a,b);
            }
        }
    }

}

void PickUpLayer(std::vector<xd::outlines> theOutline,std::vector<int> & theResult)
{
    //首先要遍历每两层的轮廓进行比较
    for (int i=0;i!=theOutline.size()-1;++i)
    {
        ClipperLib::Paths subject;
        ClipperLib::Paths clip;
        for (int j=0;j!=theOutline[i].size();++j)
        {
            ClipperLib::Path temP;
            for (int k=0;k!=theOutline[i][j].size();++k)
            {
                temP<<ClipperLib::IntPoint(theOutline[i][j][k].x*1000000,theOutline[i][j][k].y*1000000);
            }
            subject.push_back(temP);
        }
        for (int j=0;j!=theOutline[i+1].size();++j)
        {
            ClipperLib::Path temP;
            for (int k=0;k!=theOutline[i+1][j].size();++k)
            {
                temP<<ClipperLib::IntPoint(theOutline[i+1][j][k].x*1000000,theOutline[i+1][j][k].y*1000000);
            }
            clip.push_back(temP);
        }
        ClipperLib::Clipper clipper(ClipperLib::ioStrictlySimple | ClipperLib::ioPreserveCollinear);
        clipper.AddPaths(subject,ClipperLib::ptSubject,true);
        clipper.AddPaths(clip,ClipperLib::ptClip,true);
        ClipperLib::Paths solution; //存放路径结果
        clipper.Execute(ClipperLib::ctXor,solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
        double areaSum=0;
        for (int i=0;i!=solution.size();++i)
        {
            areaSum+=ClipperLib::Area(solution[i]);
        }
        if (areaSum>100*1000000000000)   //100平方毫米
        {
            theResult.push_back(i+1);
        }
    }
}

bool isSingleInfill(int layerNumber,std::vector<int> needInfillLayer)  //判断该填充层是否是单层填充的函数
{
    if (needInfillLayer.empty())
    {
        return false;
    }
    else
    {
        for (int i=0;i!=needInfillLayer.size();++i)
        {
            if (layerNumber==needInfillLayer[i])
            {
                return true;
            }
        }
        return false;
    }
}

void InfillOffset(outlines theOutline,outlines & theResult,float width)  //进行偏置填充的函数。  未验证
{
    if (!theOutline.empty())
    {
        ClipperLib::ClipperOffset temO;
        for (int i=0;i!=theOutline.size();++i)
        {
            ClipperLib::Path temP;
            for (int j=0;j!=theOutline[i].size();++j)
            {
                temP<<ClipperLib::IntPoint(theOutline[i][j].x*1000000,theOutline[i][j].y*1000000);
            }
            temO.AddPath(temP,ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
        }
        int temi=1; //每次偏置多一次的函数
        bool flag=true; //偏置是否结束的标志
        while(flag)
        {
            ClipperLib::Paths solution;
            temO.Execute(solution, -width*1000000*temi);
            temi+=1;
            xd::outlines dataOffset;  //用来存储廓偏置后的轮廓数据
            for (int i=0;i!=solution.size();++i)
            {
                xd::outline temData;
                for (int j=0;j!=solution[i].size();++j)
                {
                    temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000,(float)solution[i][j].Y/1000000));
                }
                temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000,(float)solution[i][0].Y/1000000));//加上最后一个点，保证封闭
                dataOffset.push_back(temData);
                theResult.push_back(temData);
            }
            if (dataOffset.empty())
            {
                flag=false;  //如果偏置数据没有了，则说明不需要再次偏置了。
            }
        }
    }
}

}
