///C标准类库
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include<iostream>
#include <fstream>
using namespace std;

///自定义头文件
#include "route.h"
#include "lib_record.h"


///定义限制
#define MAX_NODE        600
#define MAX_DEMAND_NODE 50
#define MAX_OUT_DEGREE  8
#define MAXSIZE         500

//广度优先搜索相关定义
typedef	struct MARK
{
	int	price;				//
	int n;					//路径数目，含头尾
	int path[MAX_NODE + 1];	//存储路径
	int states;				//0:正常；-1，此路不通；1此路已经结束
}MARK,*pMARK;

///图信息存储结构体
typedef struct DLink
{
    int id;
    int cost;
}DLink;


DLink GraphMatric[MAX_NODE][MAX_NODE];

int demand_node[MAX_DEMAND_NODE];
int demand_node_num = 0;


int ex_node_num = 0;
int ex_src_node = 0;
int ex_dst_node = 0;


void initMark(MARK *strMark, int n, int ex_src_node)
{
	for(int i = 0; i < n; i ++)
	{
		strMark[i].price = 0;
		strMark[i].n = 1;
		strMark[i].states = 0;
		strMark[i].path[0] = ex_src_node;
	}
}
pMARK mark_alloc(void)
{
    pMARK p = (pMARK)malloc(sizeof(MARK));
    if(!p)
    {
        printf("alloc failed!");
        getchar();
    }
    p->price = 0;
    p->n = 1;
    p->path[0] = ex_src_node;
    p->states = 0;

    return p;
}

int factor(int n)		//n的阶乘
{
	int sum = 1;
	if(n <= 0)
		return sum;
	for(int i = 2; i <= n; i ++)
		sum = sum * i;
	return sum;
}

#ifdef false            //true 打印 ;false 不打印
void BFSprint(MARK *pMyMark,int route)//打印
{
	int i = 0;
	//打印出 pMyMark
	int minPrice = MAXSIZE;		//最小代价
	int iSignalIndex = -1;		//最小代价标记

	for(i = 0; i < route; i++)
	{
		if(pMyMark[i].states == 1)			//完成了遍历,且满足条件
		{
			if(pMyMark[i].price < minPrice)		//代价小
			{
				iSignalIndex = i;
				minPrice = pMyMark[i].price;
			}
		}


		fout<<"    "<<i<<"    ";
		fout<<"    "<<pMyMark[i].states<<"    ";
		fout<<"     "<<pMyMark[i].price<<"    ";
		fout<<"      "<<pMyMark[i].n<<"       ";
		int k = 0;
		for( k = 0; k < pMyMark[i].n; k ++)
		{
			fout<<pMyMark[i].path[k]<<"  ";
		}


		fout<<"代价:   0"<<"  ";
		for( k = 1; k < pMyMark[i].n; k ++)
		{
			int iPointStart = pMyMark[i].path[k-1];
			int iPointEnd = pMyMark[i].path[k];
			fout<<GraphMatric[iPointStart][iPointEnd].cost<<"  ";
		}
		fout<<endl<<endl;
		}

	//将结果写入文件
	fout<<"最短路径搜索结果如下："<<endl;
	fout<<"最短路径编号:   "<<iSignalIndex<<endl;
	fout<<"最短路径代价：  "<<minPrice<<endl;
	fout<<"最短路径为:     ";
	for(i = 0; i < pMyMark[iSignalIndex].n; i ++)
	{
		fout<<pMyMark[iSignalIndex].path[i]<<"  ";
	}
	fout<<endl;
	fout<<"最短路径各代价:   0"<<"  ";
	for( i = 1; i < pMyMark[iSignalIndex].n; i ++)
	{
		int iPointStart = pMyMark[iSignalIndex].path[i-1];
		int iPointEnd = pMyMark[iSignalIndex].path[i];
		fout<<GraphMatric[iPointStart][iPointEnd].cost<<"  ";
	}
	fout<<endl;
	fout.close();



	//屏幕显示结果
	cout<<"最短路径搜索结果如下："<<endl;
	cout<<"最短路径编号:   "<<iSignalIndex<<endl;
	cout<<"最短路径代价：  "<<minPrice<<endl;
	cout<<"最短路径为:     ";
	for(i = 0; i < pMyMark[iSignalIndex].n; i ++)
	{
		cout<<pMyMark[iSignalIndex].path[i]<<"  ";
	}
	cout<<endl;
	cout<<"最短路径各代价:   0"<<"  ";
	for( i = 1; i < pMyMark[iSignalIndex].n; i ++)
	{
		int iPointStart = pMyMark[iSignalIndex].path[i-1];
		int iPointEnd = pMyMark[iSignalIndex].path[i];
		cout<<GraphMatric[iPointStart][iPointEnd].cost<<"  ";
	}
	cout<<endl;

}
#else
void BFSprint(MARK *pMyMark,int route)//不打印
{
	int i = 0;
	//打印出 pMyMark
	int minPrice = MAXSIZE;		//最小代价
	int iSignalIndex = -1;		//最小代价标记

	for(i = 0; i < route; i++)
	{
		if(pMyMark[i].states == 1)			//完成了遍历,且满足条件
		{
			if(pMyMark[i].price < minPrice)		//记录代价最小的路径
			{
				iSignalIndex = i;
				minPrice = pMyMark[i].price;
			}
		}
	}

	//屏幕显示结果
	cout<<"最短路径搜索结果如下："<<endl;
	cout<<"最短路径代价：  "<<pMyMark[iSignalIndex].price<<endl;
	cout<<"最短路径为:     ";
	for(i = 0; i < pMyMark[iSignalIndex].n; i ++)
	{
		cout<<pMyMark[iSignalIndex].path[i]<<"  ";

	    if(i != pMyMark[iSignalIndex].n - 1)
	    {
	        record_result(GraphMatric[pMyMark[iSignalIndex].path[i]][pMyMark[iSignalIndex].path[i+1]].id);
	    }
	}
	cout<<endl;

}
#endif

//Num在数组arr中是否存在
bool isExistNum(int arr[], int n, int Num)
{
    bool bExist = false;
    for(int i = 0; i < n; i++)
    {
        if(arr[i] == Num)
        {
            bExist = true;
            break;
        }
    }
    return bExist;
}


	//函数功能：返回比原来arr数组中最后一个数  大于N的不重复数字
	//返回值：-1即没有这个数；否则返回应该填入数据
	//arr 数组， n  数组中个数, N 及节点数目, M为加多少
int getArryBigM(int arr[], int n, int N, int M)     //check ok
{
	int result = -1;		//返回结果
	int i = 0;				//记录
	int arrtemp[n + M + 1];			//存放临时数据
	int iRegest;		//记录while循环次数，防止死循环
	int initNum = 0;

	for(i = 0; i < n; i++)
	{
		arrtemp[i] = arr[i];
	}

	if(n <= 0 || N <= 1 || M <= 0 || arr == NULL)
		return result;


	for( i = 0; i < M; i ++)		//向后移动次数
	{
		iRegest = 1;
		initNum = arrtemp[n - 1 + i];
		result = (initNum + iRegest) % N;

		while(isExistNum(arrtemp, n+i, result) && (iRegest < (n+M+5)))
		{
			iRegest++;
			result = (initNum + iRegest) % N;
		}

		if(iRegest >= n + M + 5)
		{
			result = -1;
			break;
		}
		else
		{
			arrtemp[n+i] = result;
		}
	}
	return result;
}

bool isPass(int arr[], int n)
{
    bool result = false;
    int i,j;
    int count = 0;
    for(i = 0;i < demand_node_num;i++)
    {
        for(j = 0;j < n;j++)
        {
            if(arr[j] == demand_node[i])
                count++;
        }
    }
    if(count == demand_node_num)
    result = true;
    return result;
}

void BFS()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int m = 0;
	int nextPathPoint = 0;
	int iStates = 0;
	int iPathStart = 0;
	int iPrice = 0;
#warning:need too much space ,fix it
	int MarkNum = factor(ex_node_num - 1) ;     //FIXME:路径需要的空间太大了,内存不够

	MARK pMyMark[MarkNum + 1];			        //定义 存储路径 结构体

	initMark(pMyMark, MarkNum + 1, ex_src_node);    //初始化路径
    printf("\nMarkNum:%d\n",MarkNum);
	for(i = 1; i < ex_node_num; i ++)				//更新路径的第2到第NPoint个点
	{
		m = factor(ex_node_num-i-1);				//一次更新m个
		for(j = 0; j < MarkNum/m; j++)		        //第j组m个
		{
			if(pMyMark[m*j].states == 0)		    //正常状态，可以更新
			{
				nextPathPoint = -1;
				nextPathPoint = getArryBigM(pMyMark[m*j].path, pMyMark[m*j].n, ex_node_num, j%(ex_node_num-i)+1);
				if(nextPathPoint == -1)
				{
					cout<<"在遍历时候，系统异常"<<endl;
					return;
				}

				iStates = 0;
				iPathStart = pMyMark[m*j].path[pMyMark[m*j].n-1];
				iPrice = GraphMatric[iPathStart][nextPathPoint].cost;			//计算路径
				if(nextPathPoint == ex_dst_node)
				{
                    if(isPass(pMyMark[m*j].path,pMyMark[m*j].n))
					    iStates = 1;
					else
					    iStates = -1;
			    }
				else if(iPrice >= MAXSIZE || nextPathPoint == ex_src_node)			//不可到达
					iStates = -1;
				for(k = 0; k < m; k ++)
				{
					pMyMark[m*j + k].n++;
					pMyMark[m*j + k].states = iStates;
					pMyMark[m*j + k].price += iPrice;
					if(pMyMark[m*j + k].price >= MAXSIZE)
					    pMyMark[m*j + k].states = -1;
					pMyMark[m*j + k].path[pMyMark[m*j + k].n-1] = nextPathPoint;
				}
			}
		}
	}

	BFSprint(pMyMark,MarkNum);			//保存结果
}


void search_route(char *topo[5000], int edge_num, char *demand)
{
    ///存储图的信息
    int node_num = 0;

    ///存储demand

    int row = 0, column = 0,id = 0,cost = 0;;
    int rc = 0;
    int i = 0,j =0;


    ///初始化邻接矩阵
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            if(i == j)
            {
                GraphMatric[i][j].cost = 0;
            }
            else
            {
                GraphMatric[i][j].cost = 600;
            }
        }
    }

    ///存储图信息
    for (i = 0; i < edge_num; i++)
    {

        rc = sscanf(topo[i],"%d%*c%d%*c%d%*c%d",&id,&row,&column,&cost);
        if(rc != 4)
        {
            printf("sscanf error and return %d",rc);
            getchar();
            return;
        }
        else
        {
            GraphMatric[row][column].cost = cost;
            GraphMatric[row][column].id = id;
            node_num = (row > node_num)? row: node_num;
            //printf("%d\t%d\t%d\t%d\n",id,row,column,cost);//打印top
        }
    }
    ex_node_num = ++node_num;

    ///存储命令信息
    {
        const char *d = ",";
        char *p;
        p = strtok(demand, d);
        for (j = 0; p != NULL; j++)
        {
            switch(j){
                case 0: ex_src_node = atoi(p);  break;
                case 1: ex_dst_node = atoi(p); break;
                case 2:
                        {
                            const char *dd = "|";
                            char *pp;
                            pp = strtok(p, dd);
                            while(pp != NULL)
                            {
                                demand_node[demand_node_num] = atoi(pp);
                                demand_node_num++;
                                pp = strtok(NULL, dd);
                            }
                            break;
                        }
                default: break;
            }
            p = strtok(NULL, d);
        }
    }

    ///打印拓扑结构信息和命令
    puts("\n**************************拓扑结构****************************");
    printf("node num: %d\n", ex_node_num);
    printf("edge_num: %d\n", edge_num);
    puts("\n");

    for(i = 0;i < ex_node_num ;i++)
    {
        for(j = 0;j < ex_node_num;j++)
        {

                printf("%d\t",GraphMatric[i][j].cost);
        }
        printf("\n");
    }

    ///打印命令
    puts("\n**************************必经路径****************************");
    printf("source       id: %d\n", ex_src_node);
    printf("destination  id: %d\n", ex_dst_node);
    printf("demand node num: %d\n", demand_node_num);
    printf("pass      route: ");
    for (int i = 0; i < demand_node_num; i++)
    {
        printf("%d ", demand_node[i]);
    }
    putchar('\n');


    ///×××××××××××××××××××××××××××××××××××××算法×××××××××××××××××××××××××××××××××××××///


    BFS();


    ///×××××××××××××××××××××××××××××××××××××算法×××××××××××××××××××××××××××××××××××××///

    ///分割
    putchar('\n');
    putchar('\n');


    ///输出方法
//    unsigned short result[] = {2, 6, 3};//示例中的一个解
//    for (int i = 0; i < 3; i++)
//        record_result(result[i]);
}


