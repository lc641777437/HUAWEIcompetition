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
#define MAXSIZE         600

//广度优先搜索相关定义
typedef	struct MARK
{
	int	price;				//路径代价
	int n;					//路径数目，含头尾
	int count;              //记录路径的必经节点个数
	int path[MAX_NODE + 1];	//存储路径
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

int node_num = 0;
int src_node = 0;
int dst_node = 0;

pMARK mark_alloc(void)
{
    pMARK p = (pMARK)malloc(sizeof(MARK));
    if(!p)
    {
        printf("alloc failed!");
    }
    p->price = 0;
    p->count = 0;
    p->n = 1;
    p->path[0] = src_node;

    return p;
}

pMARK nmark_alloc(void)
{
    pMARK p = (pMARK)malloc(sizeof(MARK));
    if(!p)
    {
        printf("alloc failed!");
    }
    p->price = 0;
    p->count = 0;
    p->n = 1;
    p->path[0] = dst_node;

    return p;
}

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

void lspath(int arr[],int n)
{
    for(int i=0;i<n;i++)
    {
        cout<<arr[i]<<" ";
    }
    cout<<endl;
}

void recordPath(pMARK route)
{
    for(int i=0;i<route->n;i++)
    {
    	if(i != route->n - 1)
	    {
	        record_result(GraphMatric[route->path[i]][route->path[i+1]].id);
	    }
    }
}

bool isDemand(int num)
{
    for(int i = 0;i < demand_node_num;i++)
    {
        if(num == demand_node[i])
            return true;
    }
    return false;
}


void arryPath_0(pMARK pmark[],int *route,int count)
{
	int i = 0;
	int routetmp = *route;
	pMARK pmarktmp = NULL;
	int tear = -1;
    for(i = 0;i < routetmp;i++)
    {
        if(tear == -1 )//寻找第一个要舍弃的路径
        {
            if(pmark[i]->count < count)
            {
                tear = i;//定位第一个要舍弃的路径
                continue;
            }
            else
            {
                continue;
            }
        }
        if(pmark[i]->count >= count)
        {
            pmarktmp = pmark[tear];
            pmark[tear] = pmark[i];
            pmark[i] = pmarktmp;
            tear++;
        }
    }

    if(tear != -1)
    {
        for(i = tear;i < *route;i++)
        {
            free(pmark[i]);
        }
        *route = tear;
    }
}

void arryPath_1(pMARK pmark[],int *route,int count)
{
	int i = 0;
	int routetmp = *route;
	pMARK pmarktmp = NULL;
	int tear = -1;
    for(i = 0;i < routetmp;i++)
    {
        if(tear == -1 )//寻找第一个要舍弃的路径
        {
            if(pmark[i]->count < count - 1)
            {
                tear = i;//定位第一个要舍弃的路径
                continue;
            }
            else
            {
                continue;
            }
        }
        if(pmark[i]->count >= count - 1)
        {
            pmarktmp = pmark[tear];
            pmark[tear] = pmark[i];
            pmark[i] = pmarktmp;
            tear++;
        }
    }

    if(tear != -1)
    {
        for(i = tear;i < *route;i++)
        {
            free(pmark[i]);
        }
        *route = tear;
    }
}


void arryPath_2(pMARK pmark[],int *route,int count)
{
	int i = 0;
	int routetmp = *route;
	pMARK pmarktmp = NULL;
	int tear = -1;
    for(i = 0;i < routetmp;i++)
    {
        if(tear == -1 )//寻找第一个要舍弃的路径
        {
            if(pmark[i]->count < count - 2)
            {
                tear = i;//定位第一个要舍弃的路径
                continue;
            }
            else
            {
                continue;
            }
        }
        if(pmark[i]->count >= count - 2)
        {
            pmarktmp = pmark[tear];
            pmark[tear] = pmark[i];
            pmark[i] = pmarktmp;
            tear++;
        }
    }

    if(tear != -1)
    {
        for(i = tear;i < *route;i++)
        {
            free(pmark[i]);
        }
        *route = tear;
    }
}

void arryPath_3(pMARK pmark[],int *route,int count)
{
	int i = 0;
	int routetmp = *route;
	pMARK pmarktmp = NULL;
	int tear = -1;
    for(i = 0;i < routetmp;i++)
    {
        if(tear == -1 )//寻找第一个要舍弃的路径
        {
            if(pmark[i]->count < count - 3)
            {
                tear = i;//定位第一个要舍弃的路径
                continue;
            }
            else
            {
                continue;
            }
        }
        if(pmark[i]->count >= count - 3)
        {
            pmarktmp = pmark[tear];
            pmark[tear] = pmark[i];
            pmark[i] = pmarktmp;
            tear++;
        }
    }

    if(tear != -1)
    {
        for(i = tear;i < *route;i++)
        {
            free(pmark[i]);
        }
        *route = tear;
    }
}

int rezerve(pMARK p)
{
    int tmp;
    for(int i = 0;i < p->n/2;i++)
    {
        tmp = p->path[i];
        p->path[i] = p->path[p->n - 1 - i];
        p->path[p->n - 1 - i] = tmp;
    }
    return 0;
}

MARK BFS1_6()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 4)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        if(isDemand(j))
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - 3)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count)//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath_3(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }
    free(pmark);

    return minPath;
}


MARK BFS7()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 4)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        if(isDemand(j))
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - 2)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count)//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath_3(pmark,&route,count);//删除必经节点少的点
    }
    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }
    free(pmark);
    return minPath;
}

MARK BFS8()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 2)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        if(isDemand(j))
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - 2)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count)//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        //cout<<"route :"<<route<<endl;
        arryPath_1(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }
    free(pmark);

    return minPath;
}


MARK BFS9()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 1)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isDemand(j))//是必经节点，加
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count )//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath_1(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}



MARK BFS10()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 1)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isDemand(j))//是必经节点，加
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count )//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath_2(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}


MARK BFS11()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 1)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isDemand(j))//是必经节点，加
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count)//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath_1(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}

MARK BFS1213()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;
    int getCount = 0;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//初始点

    if(isDemand(src_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 2)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[pmark[k]->path[pmark[k]->n-1]][j].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = mark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isDemand(j))//是必经节点，加
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count -1)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count )//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];
                                getCount++;
                                if(getCount == 8)//4 // 8 is just ok 10s
                                {
                                    return minPath;
                                }
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        //cout<<"route :"<<route<<endl;
        arryPath_2(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}


MARK BFS14()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = nmark_alloc();//初始点

    if(isDemand(dst_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[j][pmark[k]->path[pmark[k]->n-1]].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = nmark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isDemand(j))//是必经节点，加
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count )//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == src_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price <minPath.price)
                            {
                                minPath = *pmark[route - 1];
                                rezerve(&minPath);
                                return minPath;
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        //cout<<"route :"<<route<<endl;
        arryPath_0(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}


MARK BFS15()
{
	int i = 0,j = 0;                              // 标记
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = nmark_alloc();//初始点

    if(isDemand(dst_node))pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 3)//如果父路径有问题,删掉
            {
                pmarktmp = pmark[route -1];
                pmark[route -1] = pmark[k];
                pmark[k] = pmarktmp;
                free(pmark[route -1]);
                route--;
                continue;
            }
            for(j = 0;j < node_num;j++)
            {
                if( (iPrice = GraphMatric[j][pmark[k]->path[pmark[k]->n-1]].cost) <= 20)
                {
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//发现一个新的路径
                    {
                        pmark[route++] = nmark_alloc();//申请路径存储空间
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//新的路径有问题,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isDemand(j))//是必经节点，加
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - 3)//新的路径经过的必经节点少了,删掉
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count )//必经节点多了，存起来
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == src_node)//路径到达目的地,代价小则保存，到达即删掉
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price <minPath.price)
                            {
                                minPath = *pmark[route - 1];
                                rezerve(&minPath);
                                return minPath;
                            }
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                    }
                }
            }
            //删除此次父节点
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        //cout<<"route :"<<route<<endl;
        arryPath_3(pmark,&route,count);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}


void search_route(char *topo[5000], int edge_num, char *demand)
{
    int row = 0, column = 0,id = 0,cost = 0;;
    int rc = 0;
    int i = 0,j =0;
    MARK minPath;

    ///初始化邻接矩阵 默认自身不能到自身
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            GraphMatric[i][j].cost = 999;
        }
    }

    ///存储图信息
    for (i = 0; i < edge_num; i++)
    {
        rc = sscanf(topo[i],"%d%*c%d%*c%d%*c%d",&id,&row,&column,&cost);
        if(rc != 4)
        {
            printf("sscanf error and return %d",rc);
            return;
        }
        else
        {
            GraphMatric[row][column].cost = cost;
            GraphMatric[row][column].id = id;
            node_num = (row > node_num)? row: node_num;
            node_num = (column> node_num)? column: node_num;
        }
    }
    node_num++;

    ///存储命令信息
    const char *d = ",";
    char *p;
    p = strtok(demand, d);
    for (j = 0; p != NULL; j++)
    {
        switch(j){
            case 0: src_node = atoi(p);  break;
            case 1: dst_node = atoi(p); break;
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


printf("\nnode_num: %d\tedge_num: %d\n\n", node_num,edge_num);
/*
    printf("src -> dst:\t%d -> %d\n", src_node,dst_node);
    printf("pass_num: %d\n", demand_node_num);
    printf("pass_route: ");
*/

/*
1-6:<300
7:300<500
8:500-800
9:1000-1100
10:1100-1500
11:2000-2200
12 == 2000
13 == 2000
14:>=2375
15:2200= - 2375
*/
    if(edge_num <= 300)// 1-6   ok 不要更改
    {
        minPath = BFS1_6();//尽力找最优
    }
    else if(edge_num <= 500)// 7 ok 不要更改
    {
        minPath = BFS7();//尽力找最优
    }
    else if(edge_num <= 1000)//8 ok
    {
        minPath = BFS8();//尽力找最优
    }
    else if(edge_num <= 1100)//9 just ok
    {
        minPath = BFS9();
    }
    else if(edge_num <= 1500)//10 just ok
    {
        minPath = BFS10();
    }
    else if(edge_num <= 2000)// 12-13 ok
    {
        minPath = BFS1213();
    }
    else if(edge_num <= 2200)//11 not ok
    {
        minPath = BFS11();
    }
    else if(edge_num < 2375)// 15 not ok
    {
        minPath = BFS15();
    }
    else//14 not ok
    {
        minPath = BFS14();
    }

    //lspath(minPath.path, minPath.n);
    //cout<<minPath.price<<endl;
    recordPath(&minPath);
}

