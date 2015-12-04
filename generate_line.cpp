#include "generate_line.h"

#include <QDebug>

namespace xd
{

float degrees(float fudu) //����ת��Ϊ�Ƕȵĺ���
{
    return(fudu/pi*180);
}
float radians(float jiaodu) //�Ƕ�ת��Ϊ���ȵĺ���
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

bool IsContainPoint(std::vector<std::pair<float,float> > &a, xdpoint b)  //�ж���ֵ���б����Ƿ����ĳһ����ĺ�����
{
    //ע�⣬list����ΪΪ�գ�
    for (int i=0;i!=a.size();i++)
    {
        if ((a[i].first==b.x)&&(a[i].second==b.y))
        {
            return true;
        }
    }
    return false;
}

int IsLeft(xdpoint p1,xdpoint p2, xdpoint p) //�ж�p���Ƿ��������߶�p1,p2���ĺ�����
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
        return(1); //p����ࡣ
    }
    else if(det<0)
    {
        return(-1); //p���Ҳࡣ
    }
    else
    {
        return(0); //p��p1,p2���ߡ�
    }
}
bool IsEmpty(std::vector<std::pair<float,std::list<float> > > tem,int & firstV)  //�ж�ɨ�����е�Ԫ���Ƿ�ȫ��ȡ��ĺ�����
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

void DeleteOddDate(std::vector<std::pair<float,std::list<float> > > & tem)   //��������������ݵ�ɨ����ɾ���ĺ���������������ò��Ŵ˺�����
{                                                           //��Ϊ��������¾Ͳ����ܴ�������������ݡ�
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

bool IsDealFinish(std::vector<xdpoint> l,int & i)  //�ж�ƽ����x���ֱ�����Ƿ��ж���������ĺ�����
{
    int n=(int)l.size();          //�����n��ȡֵǧ���ܼ�ȥ1����Ϊ����β���Ѿ�δ������ˣ�
    for (i=0;i!=l.size();i++)     //ע�⣬������벻�������һ���㣬��Ϊ���һ����͵�һ���������ȣ�Ҫ��ȥ2��
    {                               //����Ϊʲô���Ǻǣ���������ֵ����񣡣��ڴ˴�ѭ��Ҫȫ��������
        int benshen=i%n;
        int nextP=(i+1)%n;
        int nextnP=(i+2)%n;
        if ((l[nextP].y-l[benshen].y)*(l[nextnP].x-l[nextP].x)==(l[nextnP].y-l[nextP].y)*(l[nextP].x-l[benshen].x))
        {
            i=nextP; //Ҫɾ����Ԫ�صĽǱꡣ
            return 0;
        }
    }
    return 1;
}

bool exceedExtremum(float bijiaoY1,float bijiaoY2,std::list<float> maxY,std::list<float> minY)  //�����ж�ǰ������ֱ���Ƿ�Խ���˾ֲ���ֵ��ĺ���
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



void DealCompensate(outline DealData,outline & ResultData,float R) //�������������ݣ�ʹ���Ϊ�в����뾶�����������ݡ� ���뵽�Լ����ڳ����У�2015/3/10 �����д�
{
    float layerZ=DealData[0].z;
    int subscript1=DealData.size()-2;  //�����������һ�������һ�����ظ�����ʱ�̼��ţ�����
    int subscript2=0;
    int subscript3=1;
    CTwoDimensionVector L1(DealData[subscript2].x-DealData[subscript1].x,DealData[subscript2].y-DealData[subscript1].y);
    CTwoDimensionVector L2(DealData[subscript3].x-DealData[subscript2].x,DealData[subscript3].y-DealData[subscript2].y);

  /*  if ((L1.CrossProduct(L2)<1)&&(L1.DotProduct(L2)>=-1))
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));

    }
    else if((L1.CrossProduct(L2)>-5)) //������Ҫ�ǿ��ܳ��ַǳ�ƽ�е������Ӱ���жϣ�����
    {
        ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    else if (L1.CrossProduct(L2)==0)
    {

    }
    else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
    {
        ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
       ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //�����������ʱ���ۻ�һ����ҪŪ���ˣ�
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
        else if((L1.CrossProduct(L2)>-5))   //������Ҫ�ǿ��ܳ��ַǳ�ƽ�е������Ӱ���жϣ�����
        {
            ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
        }
        else if (L1.CrossProduct(L2)==0)
        {
             //������������ܷ�������Ϊǰ���ߵĿ������Ѿ���ɾ���ˣ�
        }
        else //if((L1.CrossProduct(L2)<0)&&(L1.DotProduct(L2)<0))
        {
           ResultData.push_back(xdpoint((L1.x-L1.y)*R+DealData[subscript2].x,(L1.x+L1.y)*R+DealData[subscript2].y,layerZ));
           ResultData.push_back(xdpoint(-(L2.x+L2.y)*R+DealData[subscript2].x,(L2.x-L2.y)*R+DealData[subscript2].y,layerZ));  //�����������ʱ���ۻ�һ����ҪŪ���ˣ�

        } */

         ResultData.push_back(xdpoint((L2.x-L1.x)*R/L1.CrossProduct(L2)+DealData[subscript2].x,(L2.y-L1.y)*R/L1.CrossProduct(L2)+DealData[subscript2].y,layerZ));
    }
    ResultData.push_back(xdpoint(ResultData[0].x,ResultData[0].y,layerZ));  //��֤���һ����͵�һ������ͬ��
}

void InfillLine(outlines TheOutline,outlines & TheResult,float width,float degree,int lunkuo ) //�Լ���д����������ɺ�����
{
    if(!TheOutline.empty())  //���ȱ��뱣֤������
    {
        float layerZValue=TheOutline[0][0].z;
        //������������תһ���Ƕ�
        for (int i=0;i!=TheOutline.size();++i)
        {
            for (int j=0;j!=TheOutline[i].size();++j)
            {
                TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
            }
        }

        //��Ҫһ������ƫ����ʹ���������Ƚ�ƽ��
        std::vector<outlines> dataOffsets;  //�洢����ƫ������������
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
                    temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000,(float)solution[i][0].Y/1000000));//�������һ���㣬��֤���
                    dataOffset.push_back(temData);
                }
                dataOffsets.push_back(dataOffset);
            }
            TheOutline.clear();
            TheOutline=dataOffsets[dataOffsets.size()-1];
        }
        if(TheOutline.size()!=0)
        {
            //��һ����������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
            std::list<float> maxY;
            std::list<float> minY;
            std::vector<std::pair<float,float> > maxPoint;
            std::vector<std::pair<float,float> > minPoint;
            for (int i=0;i!=TheOutline.size();i++)
            {
                for (int j=1;j!=TheOutline[i].size();j++)
                {
                    int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
                    int beforeP=(j-1+n)%n;
                    int beforebP=(j-2+n)%n;
                    int nextP=(j+1+n)%n;
                    int nextnP=(j+2+n)%n;
                    if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
                    {
                        maxY.push_back(TheOutline[i][j].y);
                        std::pair<float,float> tem;
                        tem.first=TheOutline[i][j].x;
                        tem.second=TheOutline[i][j].y;
                        maxPoint.push_back(tem);
                    }
                    else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
                    {
                        minY.push_back(TheOutline[i][j].y);
                        std::pair<float,float> tem;
                        tem.first=TheOutline[i][j].x;
                        tem.second=TheOutline[i][j].y;
                        minPoint.push_back(tem);
                    }
                    else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
                    {
                        if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                    else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
                    {
                        if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
                        {
                            maxY.push_back(TheOutline[i][j].y);
                        }
                        if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
                        {
                            minY.push_back(TheOutline[i][j].y);
                        }
                    }
                }
            }
            maxY.sort();
            minY.sort();
            maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
            minY.unique();	  //ͬ�ϡ�
            float MaxY=*max_element(maxY.begin(),maxY.end());   //�����������������Ԫ�صĺ�����
            float MinY=*min_element(minY.begin(),minY.end());
            //�ڶ���������ÿһ����������������ߵĽ��㣬�ֱ�洢��˫�������С�
            std::vector<std::pair<float,std::list<float> > > Linedate;
            for (int i=1;i<(MaxY-MinY)/width-1;i++)
            {
                std::pair<float,std::list<float> > tem;
                tem.first=MinY+width*i;
                Linedate.push_back(tem);
            }

            if(!Linedate.empty())     //��ס��Linedate�����ǿյģ� 2015_6_17
            {
                if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//Ϊ��ʹ�����һ������߲��������������̫Զ��Ҫ��һ���Ƿ���Ҫ����һ���߶��ж�
                {

                    std::pair<float,std::list<float> > tem;
                    tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
                    Linedate.push_back(tem);
                }
                else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //��ÿһ���ߵļ�඼����һ�������Ӧ�߿���
                {
                    for (int i=0;i!=Linedate.size();++i)
                    {
                        Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
                    }
                }

                for (int i=0;i!=TheOutline.size();i++)
                {

                    for (int j=1;j!=TheOutline[i].size();j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
                    {
                        int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
                        int beforeP=(j-1+n)%n;
                        int beforebP=(j-2+n)%n;
                        int nextP=(j+1+n)%n;
                        int nextnP=(j+2+n)%n;
                        if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
                        {
                            for (int k=0;k!=Linedate.size();k++)
                            {
                                if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
                                {
                                    float x1=TheOutline[i][j].x;
                                    float x2=TheOutline[i][nextP].x;
                                    float y1=TheOutline[i][j].y;
                                    float y2=TheOutline[i][nextP].y;
                                    Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                                }
                                else if ((Linedate[k].first==TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
                                {
                                    if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                                    {
                                        //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                    }
                                }
                                else if ((Linedate[k].first==TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
                                {
                                    if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                                    {
                                        //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                }
                            }
                        }
                        else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //�����߶�ƽ����x��������
                        {
                            for (int k=0;k!=Linedate.size();k++)
                            {
                                if (Linedate[k].first==TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
                                {
                                    if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
                                    {   //˳ʱ������ת���ڵ㡣
                                        Linedate[k].second.push_back(TheOutline[i][j].x);
                                        Linedate[k].second.push_back(TheOutline[i][nextP].x);
                                    }
                                    else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                                    {
                                        //�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
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

                for(int i=0;i!=Linedate.size();i++)  //���洢�õ������ظ���Ԫ��ɾ������������
                {
                    Linedate[i].second.sort();    //����
                    Linedate[i].second.unique();  //������ĵ�ȡ������
                }
                DeleteOddDate(Linedate);    //��ʱ���ϣ����������

                //�������������ݷ������š�
                int FirstNonZero;
                while(!IsEmpty(Linedate,FirstNonZero))
                {
                    outline tem;
                    int j=0;  //�����ж��������л���ż���еĲ�����
                    std::list<float>::iterator firstIn=Linedate[FirstNonZero].second.begin();
                    float bijiao1=*firstIn;   //��һ���ߵĵ�һ���㡣
                    tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
                    tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
                    int i=(FirstNonZero+1)%Linedate.size();  //���뱣֤����һ���������ݵ��߸պ������һ����ʱҲ�����1Խ�磡����Ҫģһ�£�
                    float bijiao2=*firstIn;  //��һ���ߵĵڶ����㡣
                    float bijiaoY1=Linedate[FirstNonZero].first;  //��һ���ߵ�Y����ֵ��
                    std::list<float>::iterator tem1s=Linedate[FirstNonZero].second.begin();
                    std::list<float>::iterator tem1e=tem1s;
                    tem1e++;
                    tem1e++;
                    Linedate[FirstNonZero].second.erase(tem1s,tem1e);
                    while ((!Linedate[i].second.empty()))
                    {
                        std::list<float>::iterator Line2first=Linedate[i].second.begin();  //�ڶ����ߵĵ�һ���㡣

                        float bijiaoY2=Linedate[i].first; //�ڶ�����ֱ�ߵ�Y����ֵ��
                        if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
                        {
                            //ǰ������ֱ�������Խ�˾ֲ���ֵ�㣬Ҳ��Ҫ�˳�������Ҫ������
                            break;
                        }
                        bijiaoY1=bijiaoY2;

                        if(*Line2first>bijiao2)
                        {
                            //�ڶ���ֱ�ߵĵ�һ������ڵ�һ��ֱ�ߵĵڶ��������˳�������Ҫ����
                            break;
                        }
                        if (*(++Line2first)<bijiao1)   //ע�⣺����++������ǰ����Ϊ���������ţ�++�ں�Ļ�Ҳ��Ƚ�����++������
                        {
                            //�ڶ���ֱ�ߵĵڶ�����С�ڵ�һ��ֱ�ߵĵ�һ�������˳�������Ҫ����
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

                //����һ����Ϊ����ӦFDM�ĸ�ʽ��ÿ���߶�Ҫ����һ���뾶����
                for (int i=0;i!=TheResult.size();++i)
                {
                    for (int j=0;j!=TheResult[i].size();++j)
                    {

                        if (j%4==3||j%4==0)
                        {
                            TheResult[i][j].x+=0.28;    //�˴���Ĭ��ֵ��

                        }
                        else
                        {
                            TheResult[i][j].x-=0.28;
                        }
                    }
                }
            }
        }

        //�ǵ�������ƫ��������
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
        //����ߵĽ��ת��ѡ��Ƕ�
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

void InfillLineIn(outlines TheOutline,outlines & TheResult,float width,float degree ) //�Լ���д����������ɺ�����
{
    //������������תһ���Ƕ�
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].argument(TheOutline[i][j].argument()-degree);
        }
    }

    //��һ����������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float> > maxPoint;
    std::vector<std::pair<float,float> > minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
    minY.unique();	  //ͬ�ϡ�
    float MaxY=*max_element(maxY.begin(),maxY.end());   //�����������������Ԫ�صĺ�����
    float MinY=*min_element(minY.begin(),minY.end());
    //�ڶ���������ÿһ����������������ߵĽ��㣬�ֱ�洢��˫�������С�
    std::vector<std::pair<float,std::list<float> > > Linedate;
    for (int i=1;i<(MaxY-MinY)/width-1;i++)
    {
        std::pair<float,std::list<float> > tem;
        tem.first=MinY+width*i;
        Linedate.push_back(tem);
    }
    if(!Linedate.empty())       //2015_6_17   ��ס��Linedate�����ǿյģ���
    {
        if ((MaxY-Linedate[Linedate.size()-1].first)>width*3/2)//Ϊ��ʹ�����һ������߲��������������̫Զ��Ҫ��һ���Ƿ���Ҫ����һ���߶��ж�
        {

            std::pair<float,std::list<float> > tem;
            tem.first=(MaxY+Linedate[Linedate.size()-1].first)/2;
            Linedate.push_back(tem);
        }
        else if ((MaxY-Linedate[Linedate.size()-1].first)<=width*3/2&&(MaxY-Linedate[Linedate.size()-1].first)>width)  //��ÿһ���ߵļ�඼����һ�������Ӧ�߿���
        {
            for (int i=0;i!=Linedate.size();++i)
            {
                Linedate[i].first+=(MaxY-Linedate[Linedate.size()-1].first-width)/Linedate.size();
            }
        }

        for(int i=0;i!=TheOutline.size();i++)
        {

            for (int j=1;j!=TheOutline[i].size();j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
            {
                int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
                int beforeP=(j-1+n)%n;
                int beforebP=(j-2+n)%n;
                int nextP=(j+1+n)%n;
                int nextnP=(j+2+n)%n;
                if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
                {
                    for (int k=0;k!=Linedate.size();k++)
                    {
                        if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
                        {
                            float x1=TheOutline[i][j].x;
                            float x2=TheOutline[i][nextP].x;
                            float y1=TheOutline[i][j].y;
                            float y2=TheOutline[i][nextP].y;
                            Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                        }
                        else if ((Linedate[k].first==TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
                        {
                            if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                            {
                                //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                                Linedate[k].second.push_back(TheOutline[i][j].x);
                            }
                        }
                        else if ((Linedate[k].first==TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
                        {
                            if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                            {
                                //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                                Linedate[k].second.push_back(TheOutline[i][nextP].x);
                            }
                        }
                    }
                }
                else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //�����߶�ƽ����x��������
                {
                    for (int k=0;k!=Linedate.size();k++)
                    {
                        if (Linedate[k].first==TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
                        {
                            if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
                            {   //˳ʱ������ת���ڵ㡣
                                Linedate[k].second.push_back(TheOutline[i][j].x);
                                Linedate[k].second.push_back(TheOutline[i][nextP].x);
                            }
                            else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                            {
                                //�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
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

        for(int i=0;i!=Linedate.size();i++)  //���洢�õ������ظ���Ԫ��ɾ������������
        {
            Linedate[i].second.sort();    //����
            Linedate[i].second.unique();  //������ĵ�ȡ������
        }
        //	DeleteOddDate(Linedate);    //��ʱ���ϣ����������

        //�������������ݷ������š�
        int FirstNonZero;
        while(!IsEmpty(Linedate,FirstNonZero))
        {
            outline tem;
            int j=0;  //�����ж��������л���ż���еĲ�����
            std::list<float>::iterator firstIn=Linedate[FirstNonZero].second.begin();
            float bijiao1=*firstIn;   //��һ���ߵĵ�һ���㡣
            tem.push_back(xdpoint(*firstIn,Linedate[FirstNonZero].first,TheOutline[0][0].z));
            tem.push_back(xdpoint(*(++firstIn),Linedate[FirstNonZero].first,TheOutline[0][0].z));
            int i=(FirstNonZero+1)%Linedate.size();  //���뱣֤����һ���������ݵ��߸պ������һ����ʱҲ�����1Խ�磡����Ҫģһ�£�
            float bijiao2=*firstIn;  //��һ���ߵĵڶ����㡣
            float bijiaoY1=Linedate[FirstNonZero].first;  //��һ���ߵ�Y����ֵ��
            std::list<float>::iterator tem1s=Linedate[FirstNonZero].second.begin();
            std::list<float>::iterator tem1e=tem1s;
            tem1e++;
            tem1e++;
            Linedate[FirstNonZero].second.erase(tem1s,tem1e);
            while ((!Linedate[i].second.empty()))
            {
                std::list<float>::iterator Line2first=Linedate[i].second.begin();  //�ڶ����ߵĵ�һ���㡣

                float bijiaoY2=Linedate[i].first; //�ڶ�����ֱ�ߵ�Y����ֵ��
                if (exceedExtremum(bijiaoY1,bijiaoY2,maxY,minY))
                {
                    //ǰ������ֱ�������Խ�˾ֲ���ֵ�㣬Ҳ��Ҫ�˳�������Ҫ������
                    break;
                }
                bijiaoY1=bijiaoY2;

                if(*Line2first>bijiao2)
                {
                    //�ڶ���ֱ�ߵĵ�һ������ڵ�һ��ֱ�ߵĵڶ��������˳�������Ҫ����
                    break;
                }
                if (*(++Line2first)<bijiao1)   //ע�⣺����++������ǰ����Ϊ���������ţ�++�ں�Ļ�Ҳ��Ƚ�����++������
                {
                    //�ڶ���ֱ�ߵĵڶ�����С�ڵ�һ��ֱ�ߵĵ�һ�������˳�������Ҫ����
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


    //����һ����Ϊ����ӦFDM�ĸ�ʽ��ÿ���߶�Ҫ����һ���뾶����,��������������Ӳ����뾶��
    //for (int i=0;i!=TheResult.size();++i)
    //{
    //	for (int j=0;j!=TheResult[i].size();++j)
    //	{

    //		if (j%4==3||j%4==0)
    //		{
    //			TheResult[i][j].x+=0.28;    //�˴���Ĭ��ֵ��

    //		}
    //		else
    //		{
    //			TheResult[i][j].x-=0.28;
    //		}
    //	}
    //}

    //����ߵĽ��ת��ѡ��Ƕ�
    for (int i=0;i!=TheResult.size();++i)
    {
        for (int j=0;j!=TheResult[i].size();++j)
        {
            TheResult[i][j].argument(TheResult[i][j].argument()+degree);
        }
    }
}
void InfillBMP(outlines TheOutline,std::vector<std::vector<int> > & TheResult,int piex,int size ) //�Լ���д��bmp���ɺ�����
{
    float amplificationFactor=(float)piex/size;  //����Ŵ�����140�Ǽӹ����޳ߴ硣ע�⣬һ��Ҫ��ʾ��Ϊfloat������ó�����һ��������
    //��һ�����Ŵ����ݡ�
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x*=amplificationFactor;
            TheOutline[i][j].y*=amplificationFactor;
        }
    }
    //�ڶ�����ȡ�������������ġ�
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
    //��������ƽ�����ݡ�
    for (int i=0;i!=TheOutline.size();++i)
    {
        for (int j=0;j!=TheOutline[i].size();++j)
        {
            TheOutline[i][j].x+=offsetX;
            TheOutline[i][j].y+=offsetY;
        }
    }
    //���Ĳ���������ľֲ�����Сֵ�㣬����һ��˫�������С�ע�⣺ѡ��˫�������ԭ����������λ��ɾ�������Ԫ�طǳ���ݣ�����ʱ�临�Ӷȡ�
    std::list<float> maxY;
    std::list<float> minY;
    std::vector<std::pair<float,float> > maxPoint;
    std::vector<std::pair<float,float> > minPoint;
    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)
        {
            int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if ((TheOutline[i][j].y>TheOutline[i][beforeP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y))  //������yֵ����ǰ��ĵ��yֵ��
            {
                maxY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                maxPoint.push_back(tem);
            }
            else if ((TheOutline[i][j].y<TheOutline[i][beforeP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������yֵС��ǰ��ĵ��yֵ��
            {
                minY.push_back(TheOutline[i][j].y);
                std::pair<float,float> tem;
                tem.first=TheOutline[i][j].x;
                tem.second=TheOutline[i][j].y;
                minPoint.push_back(tem);
            }
            else if (TheOutline[i][j].y==TheOutline[i][beforeP].y)
            {
                if ((TheOutline[i][beforeP].y>TheOutline[i][beforebP].y)&&(TheOutline[i][j].y>TheOutline[i][nextP].y)) //������ǰһ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][beforeP].y<TheOutline[i][beforebP].y)&&(TheOutline[i][j].y<TheOutline[i][nextP].y)) //������ǰһ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
            else if (TheOutline[i][j].y==TheOutline[i][nextP].y)
            {
                if ((TheOutline[i][nextP].y>TheOutline[i][nextnP].y)&&(TheOutline[i][j].y>TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵ��
                {
                    maxY.push_back(TheOutline[i][j].y);
                }
                if ((TheOutline[i][nextP].y<TheOutline[i][nextnP].y)&&(TheOutline[i][j].y<TheOutline[i][beforeP].y))  //�����ͺ�һ�����yֵС��
                {
                    minY.push_back(TheOutline[i][j].y);
                }
            }
        }
    }
    maxY.sort();
    minY.sort();
    maxY.unique();    //���������ظ��ĵ�ɾ���ĺ���������STL�ı����㷨��
    minY.unique();	  //ͬ�ϡ�
    float MaxY=*max_element(maxY.begin(),maxY.end());   //�����������������Ԫ�صĺ�����
    float MinY=*min_element(minY.begin(),minY.end());
    //���岽������ÿһ������������������ߵĽ��㣬�ֱ�洢��˫�������С�
    std::vector<std::pair<float,std::list<float> > > Linedate;
    for (int i=1;i<=piex;i++)
    {
        std::pair<float,std::list<float> > tem;
        tem.first=(i+i-1)/2;    //������������ص��е���Ϊɨ���ߵ�yֵ
        Linedate.push_back(tem);
    }

    for (int i=0;i!=TheOutline.size();i++)
    {
        for (int j=1;j!=TheOutline[i].size();j++)  //j=1��˵��Ҫ�ӵڶ����㿪ʼѭ����ֱ����һ���㡣
        {
            int n=(int)TheOutline[i].size()-1;  //�������ݵ����һ�����ǵ�һ���㣬���Ҫ��ȥ1��
            int beforeP=(j-1+n)%n;
            int beforebP=(j-2+n)%n;
            int nextP=(j+1+n)%n;
            int nextnP=(j+2+n)%n;
            if (TheOutline[i][j].y!=TheOutline[i][nextP].y) //�����߶β�ƽ����x��������
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if ((Linedate[k].first-TheOutline[i][j].y)*(Linedate[k].first-TheOutline[i][nextP].y)<0) //���������߶��ཻ�������
                    {
                        float x1=TheOutline[i][j].x;
                        float x2=TheOutline[i][nextP].x;
                        float y1=TheOutline[i][j].y;
                        float y2=TheOutline[i][nextP].y;
                        Linedate[k].second.push_back((x2-x1)/(y2-y1)*(Linedate[k].first-y1)+x1);
                    }
                    else if ((Linedate[k].first==TheOutline[i][j].y))    //���������߶�ǰһ�������ཻ�������
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][j]))&&(!IsContainPoint(minPoint,TheOutline[i][j]))&&(TheOutline[i][j].y!=TheOutline[i][beforeP].y))
                        {
                            //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ�������ǰһ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                        }
                    }
                    else if ((Linedate[k].first==TheOutline[i][nextP].y))   //���������߶κ�һ�������ཻ�������
                    {
                        if ((!IsContainPoint(maxPoint,TheOutline[i][nextP]))&&(!IsContainPoint(minPoint,TheOutline[i][nextP]))&&(TheOutline[i][nextP].y!=TheOutline[i][nextnP].y))
                        {
                            //�������ͬʱ�ֲ��Ǽ�ֵ��ʱ��ͬʱ��һ��������һ�����yֵ������Ȳ��У���Ϊ���ʱ������Ѿ��Ѿ���������߼����ǹ��ˣ�����
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                    }
                }
            }
            else if(TheOutline[i][j].y==TheOutline[i][nextP].y) //�����߶�ƽ����x��������
            {
                for (int k=0;k!=Linedate.size();k++)
                {
                    if (Linedate[k].first==TheOutline[i][j].y)  //�������ǡ����ƽ����x����������ཻ��
                    {
                        if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==-1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==-1))  //��������ߵ�����ͬʱ���ڵ�ʱ��
                        {   //˳ʱ������ת���ڵ㡣
                            Linedate[k].second.push_back(TheOutline[i][j].x);
                            Linedate[k].second.push_back(TheOutline[i][nextP].x);
                        }
                        else if((IsLeft(TheOutline[i][beforeP],TheOutline[i][j],TheOutline[i][nextP])==1)&&(IsLeft(TheOutline[i][j],TheOutline[i][nextP],TheOutline[i][nextnP])==1))
                        {
                            //�����㶼�����ڵ㣬���㶼��ȥ�����仰˵����ʲôҲ��������ʵ����д��δ��룬Ϊ���Ժ���������д���ˡ�
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

    for(int i=0;i!=Linedate.size();i++)  //���洢�õ������ظ���Ԫ��ɾ������������
    {
        Linedate[i].second.sort();    //����
        Linedate[i].second.unique();  //������ĵ�ȡ������
    }
    //	DeleteOddDate(Linedate);    //��ʱ���ϣ����������
    //����������������Ҫ�������к���ȡ�������ȥ��
    for (int i=0;i!=piex;++i)   //�Ƚ����س�ʼ��Ϊ0
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
    //����Ҫ����ÿ������������бȽ�
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
        ClipperLib::Paths solution; //���·�����
        clipper.Execute(ClipperLib::ctXor,solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);
        double areaSum=0;
        for (int i=0;i!=solution.size();++i)
        {
            areaSum+=ClipperLib::Area(solution[i]);
        }
        if (areaSum>100*1000000000000)   //100ƽ������
        {
            theResult.push_back(i+1);
        }
    }
}

bool isSingleInfill(int layerNumber,std::vector<int> needInfillLayer)  //�жϸ������Ƿ��ǵ������ĺ���
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

void InfillOffset(outlines theOutline,outlines & theResult,float width)  //����ƫ�����ĺ�����  δ��֤
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
        int temi=1; //ÿ��ƫ�ö�һ�εĺ���
        bool flag=true; //ƫ���Ƿ�����ı�־
        while(flag)
        {
            ClipperLib::Paths solution;
            temO.Execute(solution, -width*1000000*temi);
            temi+=1;
            xd::outlines dataOffset;  //�����洢��ƫ�ú����������
            for (int i=0;i!=solution.size();++i)
            {
                xd::outline temData;
                for (int j=0;j!=solution[i].size();++j)
                {
                    temData.push_back(xd::xdpoint((float)solution[i][j].X/1000000,(float)solution[i][j].Y/1000000));
                }
                temData.push_back(xd::xdpoint((float)solution[i][0].X/1000000,(float)solution[i][0].Y/1000000));//�������һ���㣬��֤���
                dataOffset.push_back(temData);
                theResult.push_back(temData);
            }
            if (dataOffset.empty())
            {
                flag=false;  //���ƫ������û���ˣ���˵������Ҫ�ٴ�ƫ���ˡ�
            }
        }
    }
}

}
