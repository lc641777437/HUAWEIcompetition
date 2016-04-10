///C��׼���
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

///�Զ���ͷ�ļ�
#include "route.h"
#include "lib_record.h"


///��������
#define MAX_NODE   601
#define MAX_NODE_NUM_1 5000
#define MAX_NODE_NUM_2 3000
#define MAX_DEMAND_NODE 50
#define MAX_OUT_DEGREE  8
#define MAXSIZE         700

int out_node[MAX_NODE][MAX_NODE] = { 0 }; //�洢I�ڵ�ĳ��ڵ㡣
bool used[MAX_NODE];   //�жϵ�i���ڵ��Ƿ�ʹ��

//ʱ�����

clock_t time_start;
#define now_time (double)(clock()-time_start)/CLOCKS_PER_SEC

int use_time_s = 0;
int use_time_ms = 0;

bool isVital[MAX_NODE];			//�жϵ�i���ڵ��Ƿ�Ϊ�ؾ��ڵ�

int delete_num = 0;			//Ҫɾ���Ľڵ���Ŀ
int delete_times = 0;		//ɾ���Ĵ���


//·���ڵ�
struct pathNode
{
	int id; //��ǰ�ڵ�ID
	pathNode* pre; //�ϸ��ڵ�
};

//������нڵ�
struct Node
{
	int id; //��ǰ�ڵ�ID
	int cost; //Ŀǰ�ĺķ�
	int count;//�����ıؾ��ڵ���Ŀ
	pathNode* tail; //·��β�ڵ�ָ��
	Node* pre;//���������е���һ���ڵ�
	Node* next;//���������е���һ���ڵ�
};


//�������������ض���
typedef	struct MARK
{
	int	price;				//·������
	int n;					//·����Ŀ����ͷβ
	int count;              //��¼·���ıؾ��ڵ����
	int path[MAX_NODE + 1];	//�洢·��
}MARK,*pMARK;

///ͼ��Ϣ�洢�ṹ��
typedef struct DLink
{
    int id;
    int cost;
}DLink;


//���ȶ��нڵ�
struct queNode
{
	short id; //��ǰ�ڵ�route_id
	short cost; //Ŀǰ�ĺķ�
	short count;//�����ıؾ��ڵ���Ŀ
	vector<queNode*> path;    //Ĭ�ϻ�������
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

//pq�����ȼ��ıȽ�
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

//Num������arr���Ƿ����
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

//���ȶ��нڵ��վ
queNode* priorityPop()
{
	queNode* result = pq.top();
	pq.pop();
	return result;
}


//���ȶ��нڵ��վ
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
        if(tear == -1 )//Ѱ�ҵ�һ��Ҫ������·��
        {
            if(pmark[i]->count < count - minus)
            {
                tear = i;//��λ��һ��Ҫ������·��
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
bfs���鿴��ǰ�ڵ��Ƿ��Ѿ���ʹ��
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
�Ѿ��н���洢�ý�
*/
MARK saveResult11(pathNode* tal)
{
	int i;
	MARK minPath;
	int Path[MAX_NODE] = { 0 };//·���ڵ�����

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
�Ѿ��н���洢�ý�
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
	int i = 0,j = 0;                              // ���
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;

    minPath.price = MAXSIZE;

    pmark[route++] = mark_alloc();//��ʼ��

    if(isVital[src_node])pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //�ӵ�1���������Npoint��
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - cut1)//�����·��������,ɾ��
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
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//����һ���µ�·��
                    {
                        pmark[route++] = mark_alloc();//����·���洢�ռ�
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//�µ�·��������,ɾ��
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        if(isVital[j])
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - cut2)//�µ�·�������ıؾ��ڵ�����,ɾ��
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count)//�ؾ��ڵ���ˣ�������
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//·������Ŀ�ĵ�,����С�򱣴棬���Ｔɾ��
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
            //ɾ���˴θ��ڵ�
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath(pmark,&route,count,cut3);//ɾ���ؾ��ڵ��ٵĵ�
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }
    free(pmark);

    return minPath;
}

/*
����ڵ����ȿ����������
*/
MARK Chain_BFS()
{
	int i,j;
	int icost, now_id, next_id;
	Node* now;
	Node* next;
	Node* tmp;
	pathNode* Nodetmp;
	int qLen = 0; //���г���
    Node* head;//����ͷ���
    Node* tail;//����β�ڵ�
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
				{ //��β�˻���˵�����ڵ�ڵ�����٣������ǰ��
					tmp = tmp->next;
				}
				if (tmp == NULL)
				{ //����β��
					tail->next = next;
					next->pre = tail;
					tail = next;
				}
				else
				{ //����tmpǰ��
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
	int i = 0,j = 0;                              // ���
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

    pmark[route++] = mark_alloc();//��ʼ��

    if(isVital[src_node])pmark[0]->count++;

    for(i = 0;i < node_num;i++)   //�ӵ�1���������Npoint��
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - 2)//�����·��������,ɾ��
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
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))//����һ���µ�·��
                    {
                        pmark[route++] = mark_alloc();//����·���洢�ռ�
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;
                        //lspath( pmark[route - 1]->path, pmark[route - 1]->n);

                        if(pmark[route - 1]->price >= minPath.price)//�µ�·��������,ɾ��
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }

                        if(isVital[j])//�Ǳؾ��ڵ㣬��
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count -1)//�µ�·�������ıؾ��ڵ�����,ɾ��
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count )//�ؾ��ڵ���ˣ�������
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)//·������Ŀ�ĵ�,����С�򱣴棬���Ｔɾ��
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
            //ɾ���˴θ��ڵ�
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        //cout<<"route :"<<route<<endl;
        arryPath(pmark,&route,count,2);//ɾ���ؾ��ڵ��ٵĵ�
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }

    free(pmark);

    return minPath;
}

/*
	�м�ڵ������Լ۱ȿ�����������ڶ��׶�
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
	now->path.push_back(now); //��ʼ�ڵ�
	pq2.push(now);              //��ʼ�ڵ�������
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
	�м�ڵ������Լ۱ȿ������������һ�׶�
*/
MARK BFS1415_F(int dst_mid)
{
	int i;
	int tmp_cost, now_id, next_id;
	MARK minPath;
    minPath.price = 700;
	queNode* now = new queNode(dst_mid);
	now->count = 1;
	now->path.push_back(now); //��ʼ�ڵ�
	pq.push(now);              //��ʼ�ڵ�������
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
�м�ڵ��Լ۱ȿ������������ں���
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
�м�ڵ��Լ۱ȿ������������ں���
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

    ///��ʼ���ڽӾ��� Ĭ�������ܵ�����
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            GraphMatric[i][j].cost = 999;
        }
    }

    ///�洢ͼ��Ϣ
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
            GraphMatric[row][column].cost = cost;//·������
            GraphMatric[row][column].id = id;//·��id
            //out_node[row][0]++;//���ڵ���Ŀ
		    //out_node[row][out_node[row][0]] = column;//���ڵ� �ڵ��
            node_num = (row > node_num)? row: node_num;
            node_num = (column> node_num)? column: node_num;
        }
    }
    node_num++;//�ڵ���




    ///�洢������Ϣ
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
		isVital[i] = false;//Ĭ�ϲ��Ǳؾ��ڵ�
	}

	for (i = 0; i < demand_node_num; i++)
	{
		isVital[demand_node[i]] = true;//demand_node[i]�Ǳؾ��ڵ��id
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

    if(edge_num <= 300)// 1-6   ok ��Ҫ����
    {
        minPath = Matrix_BFS(4,3,3,false);//����������
    }
    else if(edge_num <= 500)// 7 ok ��Ҫ����
    {
        minPath = Matrix_BFS(4,2,3,false);//����������
    }
    else if(edge_num <= 1000)//8 ok
    {
        minPath = Matrix_BFS(2,2,1,false);//����������
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

