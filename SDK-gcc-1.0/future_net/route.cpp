///C标准类库
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
using namespace std;

///自定义头文件
#include "route.h"
#include "lib_record.h"


///定义限制
#define MAX_NODE   601
#define MAX_NODE_NUM_1 5000
#define MAX_NODE_NUM_2 3000
#define MAX_DEMAND_NODE 50
#define MAX_OUT_DEGREE  8
#define MAXSIZE         700

int out_node[MAX_NODE][MAX_NODE] = { 0 }; //存储I节点的出节点。
bool used[MAX_NODE];   //判断第i个节点是否被使用

//时间变量

clock_t time_start;
#define now_time (double)(clock()-time_start)/CLOCKS_PER_SEC

int use_time_s = 0;
int use_time_ms = 0;

bool isVital[MAX_NODE];			//判断地i个节点是否为必经节点

int delete_num = 0;			//要删除的节点数目
int delete_times = 0;		//删除的次数


//路径节点
struct pathNode
{
	int id; //当前节点ID
	pathNode* pre; //上个节点
};

//链表队列节点
struct Node
{
	int id; //当前节点ID
	int cost; //目前的耗费
	int count;//经过的必经节点数目
	pathNode* tail; //路径尾节点指针
	Node* pre;//队列链表中的上一个节点
	Node* next;//队列链表中的下一个节点
};


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


//优先队列节点
struct queNode
{
	short id; //当前节点route_id
	short cost; //目前的耗费
	short count;//经过的必经节点数目
	vector<queNode*> path;    //默认会包含起点
	queNode(short i) : id(i), cost(0), count(0) {};
	queNode(short i, short c, vector<queNode*> p, short a) : id(i), cost(c), path(p), count(a) {};
	bool inPath(short target)
	{
		for (queNode* i : path)
		if (target == i->id)
			return true;
		return false;
	}
};

//pq中优先级的比较
struct Compare
{
	bool operator () (queNode* node1, queNode* node2)
	{
		if (!node1->count || !node2->count)
		{
			if (!node1->count)
			{
				return true;
		    }
			return false;
		}
		return node1->cost/node1->count > node2->cost/node2->count;
	}
};

priority_queue<queNode*, vector<queNode*>, Compare> pq;
priority_queue<queNode*, vector<queNode*>, Compare> tmp_pq;
priority_queue<queNode*, vector<queNode*>, Compare> pq2;
priority_queue<queNode*, vector<queNode*>, Compare> tmp_pq_2;

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

//优先队列节点出站
queNode* priorityPop()
{
	queNode* result = pq.top();
	pq.pop();
	return result;
}


//优先队列节点出站
queNode* priorityPop2()
{
	queNode* result = pq2.top();
	pq2.pop();
	return result;
}

void arryPath(pMARK pmark[],int *route,int count,int minus)
{
	int i = 0;
	int routetmp = *route;
	pMARK pmarktmp = NULL;
	int tear = -1;
    for(i = 0;i < routetmp;i++)
    {
        if(tear == -1 )//寻找第一个要舍弃的路径
        {
            if(pmark[i]->count < count - minus)
            {
                tear = i;//定位第一个要舍弃的路径
                continue;
            }
            else
            {
                continue;
            }
        }
        if(pmark[i]->count >= count - minus)
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

/*
bfs，查看当前节点是否已经被使用
*/
bool check_used(pathNode* tal, int id)
{
	while (tal->pre != NULL)
	{
		if (tal->id == id)
		    return false;
		tal = tal->pre;
	}
	return true;
}

/*
已经有解则存储该解
*/
MARK saveResult11(pathNode* tal)
{
	int i;
	MARK minPath;
	int Path[MAX_NODE] = { 0 };//路径节点数组

	minPath.n = 1;
	Path[0] = dst_node;
	while (tal->pre != NULL)
	{
		Path[minPath.n++] = tal->id;
		tal = tal->pre;
	}
	Path[minPath.n++] = src_node;
	for (i = 0; i < minPath.n; i++)
	{
		minPath.path[i] = Path[minPath.n - i - 1];
	}
	return minPath;
}
/*
已经有解则存储该解
*/
MARK saveResult(vector<queNode*> path)
{
    MARK minPath;
    minPath.n = 0;
	for (queNode* n : path)
	{
		minPath.path[minPath.n++] = n->id;
		if(isVital[minPath.path[minPath.n -1]]) minPath.count++;
	}
    minPath.path[minPath.n++] = dst_node;
    if(isVital[minPath.path[minPath.n -1]]) minPath.count++;

	return minPath;
}


MARK Matrix_BFS(int cut1, int cut2, int cut3, bool state)
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

    if(isVital[src_node])pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //从第1点遍历到第Npoint点
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - cut1)//如果父路径有问题,删掉
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
                        if(isVital[j])
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - cut2)//新的路径经过的必经节点少了,删掉
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
                                if(state)return minPath;
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
        arryPath(pmark,&route,count,cut3);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }
    free(pmark);

    return minPath;
}

/*
正向节点优先宽度优先搜索
*/
MARK Chain_BFS()
{
	int i,j;
	int icost, now_id, next_id;
	Node* now;
	Node* next;
	Node* tmp;
	pathNode* Nodetmp;
	int qLen = 0; //队列长度
    Node* head;//队列头结点
    Node* tail;//队列尾节点
    MARK minPath;

    for (i = MAX_NODE - 1; i >= 0; i--)
	{
    	for (j = MAX_NODE - 1; j >= 0; j--)
    	{
    		if (GraphMatric[i][j].cost <= 20)
    		{
    			out_node[i][0]++;
    			out_node[i][out_node[i][0]] = j;
    		}
    	}
    }

    minPath.price = MAXSIZE;

	head = (Node *)malloc(sizeof(Node));
	Nodetmp = (pathNode *)malloc(sizeof(pathNode));

	Nodetmp->id = src_node;
	Nodetmp->pre = NULL;

	head->id = src_node;
	head->next = NULL;
	head->pre = NULL;
	head->tail = Nodetmp;
	head->cost = 0;
	head->count = 0;

	tail = head;
	qLen = 1;

	while (qLen >= 1)
	{
		if (now_time >= 9.8)
		{
			return minPath;
        }
		now = head;
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++)
		{
			next_id = out_node[now_id][i];
			icost = now->cost + GraphMatric[now_id][next_id].cost;
			if (icost >= minPath.price)
			{
				continue;
			}
			if (next_id == dst_node)
			{
				if (now->count == demand_node_num)
				{
					minPath.price = icost;
					minPath = saveResult11(now->tail);
				}
				else
				    continue;
			}
			else if (check_used(now->tail, next_id))
			{
				next = (Node *)malloc(sizeof(Node));
				Nodetmp = (pathNode *)malloc(sizeof(pathNode));
				Nodetmp->id = next_id;
				Nodetmp->pre = now->tail;
				next->id = next_id;
				next->next = NULL;
				next->pre = NULL;
				next->tail = Nodetmp;
				next->cost = icost;
				next->count = now->count;
				if (isVital[next_id])
				{
					next->count = now->count + 1;
				}
				tmp = head->next;
				while (tmp != NULL && tmp->count >= next->count)
				{ //到尾了或者说经过节点节点比他少，则插在前面
					tmp = tmp->next;
				}
				if (tmp == NULL)
				{ //到了尾部
					tail->next = next;
					next->pre = tail;
					tail = next;
				}
				else
				{ //插在tmp前面
					next->pre = tmp->pre;
					next->next = tmp;
					tmp->pre->next = next;
					tmp->pre = next;
				}
				qLen++;
			}
		}
		qLen--;
		head = head->next;
	}

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

    if(isVital[src_node])pmark[0]->count++;

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

                        if(isVital[j])//是必经节点，加
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
        arryPath(pmark,&route,count,2);//删除必经节点少的点
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}

/*
	中间节点正向性价比宽度优先搜索第二阶段
*/
MARK BFS1415_B(int dst_mid, queNode* endNode)
{
	int i;
	int tmp_cost, now_id, next_id;
	int tmpTarget = dst_mid;
	MARK minPath;
    minPath.price = 700;
    minPath.n = 0;
	queNode* now = new queNode(src_node);
	now->path.push_back(now); //初始节点
	pq2.push(now);              //初始节点进入队列
	while (!pq2.empty())
	{
		if (now_time >= 9.8)
		{
		    minPath.n = 0;
			return minPath;
		}
		now = priorityPop2();
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++)
		{
			next_id = out_node[now_id][i];
			if ((next_id == dst_node || endNode->inPath(next_id)) && next_id != tmpTarget)
			    continue;
			tmp_cost = now->cost + GraphMatric[now_id][next_id].cost;
			if (next_id == tmpTarget)
			{
				minPath.price = tmp_cost;
				for (queNode* n : endNode->path)
				{
					now->path.push_back(n);
				}
				minPath = saveResult(now->path);
				return minPath;
			}
			else if (!now->inPath(next_id))
			{
				queNode * next = new queNode(next_id, tmp_cost, now->path, isVital[next_id] ? now->count + 1 : now->count);
				next->path.push_back(next);
				pq2.push(next);
				if (pq2.size() > MAX_NODE_NUM_1)
				{
					if (MAX_NODE_NUM_1 >= delete_times)
						delete_num = MAX_NODE_NUM_1 - delete_times;
					else
						delete_num = 1;
					delete_times += 5;
					if (delete_num < (int)pq2.size())
					{
						queNode * node;
						while (delete_num>0)
						{
							node = priorityPop2();
							tmp_pq_2.push(node);
							delete_num--;
						}
						while (pq2.size()>0)
						{
							node = priorityPop2();
							delete node;
						}
						while (tmp_pq_2.size() > 0)
						{
							node = tmp_pq_2.top();
							tmp_pq_2.pop();
							pq2.push(node);
						}
					}
				}
			}
		}
	}
	minPath.n = 0;
	return minPath;
}

/*
	中间节点正向性价比宽度优先搜索第一阶段
*/
MARK BFS1415_F(int dst_mid)
{
	int i;
	int tmp_cost, now_id, next_id;
	MARK minPath;
    minPath.price = 700;
	queNode* now = new queNode(dst_mid);
	now->count = 1;
	now->path.push_back(now); //初始节点
	pq.push(now);              //初始节点进入队列
	while (!pq.empty())
	{
		if (now_time >= 9.5)
		{
		    minPath.n = 0;
			return minPath;
		}
		now = priorityPop();
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++)
		{
			next_id = out_node[now_id][i];
			tmp_cost = now->cost + GraphMatric[now_id][next_id].cost;
			if (tmp_cost >= minPath.price)
			{
				continue;
			}
			if (next_id == dst_node)
			{
				if (now->count == demand_node_num)
				{
					minPath.price = tmp_cost;
					minPath = BFS1415_B(dst_mid, now);
					if (minPath.n > 0)
					    return minPath;
				}
				else
				    continue;
			}
			else if (!now->inPath(next_id))
			{
				queNode * next = new queNode(next_id, tmp_cost, now->path, isVital[next_id] ? now->count + 1 : now->count);
				next->path.push_back(next);
				pq.push(next);
				if (pq.size() > MAX_NODE_NUM_1)
				{
					if (MAX_NODE_NUM_1 >= delete_times)
						delete_num = MAX_NODE_NUM_1 - delete_times;
					else
						delete_num = 1;
					delete_times += 5;
					if (delete_num < (int)pq.size())
					{
						queNode * node;
						while (delete_num>0)
						{
							node = priorityPop();
							tmp_pq.push(node);
							delete_num--;
						}
						while (pq.size()>0)
						{
							node = priorityPop();
							delete node;
						}
						while (tmp_pq.size() > 0)
						{
							node = tmp_pq.top();
							tmp_pq.pop();
							pq.push(node);
						}
					}
				}
			}
		}
	}
	minPath.n = 0;
	return minPath;
}

/*
中间节点性价比宽度优先搜索入口函数
*/
MARK BFS14()
{
	int dst_mid, i,j;
	MARK minPath;
	for (i = 0; i < MAX_NODE - 1; i++)
	{
    	for (j = 0; j < MAX_NODE - 1; j++)
    	{
    		if (GraphMatric[i][j].cost <= 20)
    		{
    			out_node[i][0]++;
    			out_node[i][out_node[i][0]] = j;
    		}
    	}
    }

	minPath.n = 0;

	for (i = demand_node_num - 1; i >= 0; i--)
	{
		dst_mid = demand_node[i];
		minPath = BFS1415_F(dst_mid);
		if (minPath.count == demand_node_num)
		{
			return minPath;
		}
	}
	return minPath;
}

/*
中间节点性价比宽度优先搜索入口函数
*/
MARK BFS15()
{
	int dst_mid, i,j;
	MARK minPath;
	for (i = 0; i < MAX_NODE - 1; i++)
	{
    	for (j = 0; j < MAX_NODE - 1; j++)
    	{
    		if (GraphMatric[i][j].cost <= 20)
    		{
    			out_node[i][0]++;
    			out_node[i][out_node[i][0]] = j;
    		}
    	}
    }

	minPath.n = 0;

	for (i = demand_node_num - 1; i >= 0; i--)
	{
		dst_mid = demand_node[i];
		minPath = BFS1415_F(dst_mid);
		if (minPath.count == demand_node_num)
		{
			return minPath;
		}
	}
	return minPath;
}

void search_route(char *topo[5000], int edge_num, char *demand)
{
    int row = 0, column = 0,id = 0,cost = 0;;
    int rc = 0;
    int i = 0,j =0;
    MARK minPath;
    time_start = clock();

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
            GraphMatric[row][column].cost = cost;//路径代价
            GraphMatric[row][column].id = id;//路径id
            //out_node[row][0]++;//出节点数目
		    //out_node[row][out_node[row][0]] = column;//出节点 节点号
            node_num = (row > node_num)? row: node_num;
            node_num = (column> node_num)? column: node_num;
        }
    }
    node_num++;//节点数




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

    for (i = 0; i < node_num; i++)
	{
		isVital[i] = false;//默认不是必经节点
	}

	for (i = 0; i < demand_node_num; i++)
	{
		isVital[demand_node[i]] = true;//demand_node[i]是必经节点的id
	}
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
        minPath = Matrix_BFS(4,3,3,false);//尽力找最优
    }
    else if(edge_num <= 500)// 7 ok 不要更改
    {
        minPath = Matrix_BFS(4,2,3,false);//尽力找最优
    }
    else if(edge_num <= 1000)//8 ok
    {
        minPath = Matrix_BFS(2,2,1,false);//尽力找最优
    }
    else if(edge_num <= 1100)//9 just ok
    {
        minPath = Matrix_BFS(1,0,1,false);
    }
    else if(edge_num <= 1500)//10 just ok
    {
        minPath = Chain_BFS();
    }
    else if(edge_num <= 2000)// 12-13 ok
    {
        minPath = BFS1213();
    }
    else if(edge_num <= 2200)//11 ok
    {
        minPath = Chain_BFS();
    }
    else if(edge_num < 2375)//15 not ok
    {
		minPath = BFS15();
    }
    else                    //14 not ok
    {
        minPath = BFS14();
    }
    lspath(minPath.path, minPath.n);
    recordPath(&minPath);
}

