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

///自定义头文件
#include "route.h"
#include "lib_record.h"
#include "lp_lib.h"
using namespace std;


///定义限制
#define MAX_NODE   600
#define MAX_DEMAND_NODE 50
#define MAXSIZE  700
#define MAXEDGENUM 4800


unsigned int  maxsize = 735;
unsigned int maxnode = 5000;

int out_degree[MAX_NODE][MAX_NODE] = { 0 };

time_t start_time = 0;

int link[MAX_NODE] = { 0 };

bool timeout = false;

bool isVital[MAX_NODE];

unsigned int cut = 0;
unsigned int cut_num = 0;

struct pathNode
{
	int id;
	pathNode* pre;
};

struct Node
{
	int id;
	int cost;
	int count;
	pathNode* tail;
	Node* pre;
	Node* next;
};


typedef	struct MARK
{
	int	price;
	int n;
	int count;
	int path[MAX_NODE + 1];
}MARK,*pMARK;

MARK minpath;

typedef struct DLink
{
    int id;
    int cost;
}DLink;

typedef struct Sink
{
    int id;
    int src;
    int dst;
    int isUsed;
    int cost;
}Sink;


Sink Link[MAXEDGENUM];
struct queNode
{
	short id;
	short cost;
	short count;
	vector<queNode*> path;
	queNode(short i) : id(i), cost(0), count(0) {};
	queNode(short i, short c, vector<queNode*> p, short a) : id(i), cost(c), path(p), count(a) {};
	bool inPath(short dst_node)
	{
		for (queNode* i : path)
		if (dst_node == i->id)
			return true;
		return false;
	}
};


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

priority_queue<queNode*, vector<queNode*>, Compare> pqf;
priority_queue<queNode*, vector<queNode*>, Compare> pqb;

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
	        cout<<GraphMatric[route->path[i]][route->path[i+1]].id<<" ";
	        record_result(GraphMatric[route->path[i]][route->path[i+1]].id);
	    }
    }
    cout<<endl;
}

queNode* priorityPop_F()
{
	queNode* result = pqf.top();
	pqf.pop();
	return result;
}

queNode* priorityPop_B()
{
	queNode* result = pqb.top();
	pqb.pop();
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
        if(tear == -1 )
        {
            if(pmark[i]->count < count - minus)
            {
                tear = i;
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

bool isUsed(pathNode* tal, int id)
{
	while (tal->pre != NULL)
	{
		if (tal->id == id)
		    return false;
		tal = tal->pre;
	}
	return true;
}

MARK saveResult11(pathNode* tal)
{
	int i;
	MARK minPath;
	int Path[MAX_NODE] = {0};

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

bool save2minpath(vector<queNode*> path)
{
    minpath.n = 0;
	for (queNode* n : path)
	{
		minpath.path[minpath.n++] = n->id;
		if(isVital[minpath.path[minpath.n -1]]) minpath.count++;
	}
    minpath.path[minpath.n++] = dst_node;
    if(isVital[minpath.path[minpath.n -1]])
    {
        minpath.count++;
    }
	return true;
}

MARK save2minPath(vector<queNode*> path)
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
//你要完成的功能总入口
void lp_solve8(int edge_num)
{
    int i = 0, k = 0, j = 0;
	lprec *lp;
	int Ncol;
	int *colno = NULL;
	REAL *row = NULL;
	REAL *res = NULL;
    int flag = 0;
    int loop[MAXEDGENUM] = {0};
    int loopCount = 0;
    int demand_count = 0;
    int iNode = 0;
    int result_path[MAX_NODE] = {0};

	Ncol = edge_num;
	lp = make_lp(0, Ncol);

	for (i = 1; i <= Ncol; i++)
	{
		set_int(lp, i, TRUE);
	}


	colno = (int *)malloc(Ncol * sizeof(*colno));
	row = (REAL *)malloc(Ncol * sizeof(*row));
	res = (REAL *)malloc(Ncol * sizeof(*res));

	set_add_rowmode(lp, TRUE);
	for (i = 0; i < demand_node_num; i++)
	{
		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[demand_node[i]][j].cost <= 20)
			{
				colno[k] = GraphMatric[demand_node[i]][j].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, EQ, 1);
	}


	for (i = 0; i < demand_node_num; i++)
	{
		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[j][demand_node[i]].cost <= 20)
			{
				colno[k] = GraphMatric[j][demand_node[i]].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, EQ, 1);
	}


	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[j][src_node].cost <= 20)
		{
			colno[k] = GraphMatric[j][src_node].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 0);

	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[src_node][j].cost <= 20)
		{
			colno[k] = GraphMatric[src_node][j].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 1);


	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[dst_node][j].cost <= 20)
		{
			colno[k] = GraphMatric[dst_node][j].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 0);



	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[j][dst_node].cost <= 20)
		{
			colno[k] = GraphMatric[j][dst_node].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 1);

	for (i = 0; i < node_num; i++)
	{
		k = 0;
		if (i == dst_node) continue;
		if (i == src_node) continue;
		if (isVital[i]) continue;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[j][i].cost <= 20)
			{
				colno[k] = GraphMatric[j][i].id + 1;
				row[k++] = 1;
			}
		}
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[i][j].cost <= 20)
			{
				colno[k] = GraphMatric[i][j].id + 1;
				row[k++] = -1;
			}
		}
		add_constraintex(lp, k, row, colno, EQ, 0);
	}

	for (i = 0; i < node_num; i++)
	{
		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[j][i].cost <= 20)
			{
				colno[k] = GraphMatric[j][i].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, LE, 1);

		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[i][j].cost <= 20)
			{
				colno[k] = GraphMatric[i][j].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, LE, 1);
	}

	 for(i = 0;i < node_num;i++)
     {
         for(j = 0;j < node_num;j++)
         {
             row[i] = 0;
             if(GraphMatric[i][j].cost <= 20 && GraphMatric[j][i].cost <= 20)
             {
                 k = 0;
                 colno[k] = GraphMatric[i][j].id + 1;
                 row[k++] = 1;
                 colno[k] = GraphMatric[j][i].id + 1;
                 row[k++] = 1;
                 add_constraintex(lp, k, row, colno, LE, 1);
             }
         }
     }


	set_add_rowmode(lp, FALSE);
	j = 0;
	for (i = 1; i <= edge_num; i++)
	{
		colno[j] = i;
		row[j++] = Link[i - 1].cost;
	}
	set_obj_fnex(lp, j, row, colno);


    while(1)
    {
	    set_minim(lp);
      	solve(lp);
		get_variables(lp, res);

        for(j = 0;j < MAXEDGENUM;j++)
        {
            Link[i].isUsed = 0;
        }

        j = 0;
        demand_count = 0;
        iNode = src_node;

        for(i=0;i < MAX_NODE;i++)
        {
            result_path[i] = 0;
        }

        while(iNode != dst_node)
        {
            for(i = 0;i < edge_num;i++)
            {
                if((res[i] == 1) && (Link[i].src == iNode))
                {
                    if(isVital[Link[i].src])
                    {
                        demand_count++;
                    }
                    Link[i].isUsed = 1;
                    result_path[j++] = i;
                    iNode = Link[i].dst;
                    break;
                }
            }
        }

        if(demand_count == demand_node_num)
        {
            for(i = 0;i < j;i++)
            {
                record_result(result_path[i]);
            }
            return;
        }
        else//有环
        {
            flag = 0;

            loopCount = 0;
            for(i = 0;i < edge_num;i++)
            {
                if((res[i] == 1) && isVital[Link[i].src] && !Link[i].isUsed && flag == 0)
                {
                    loop[loopCount++] = i;
                    flag = 1;
                    Link[i].isUsed = 1;
                    iNode = Link[i].dst;
                    i = -1;
                    continue;
                }

                if((res[i] == 1) && (Link[i].src == iNode) && flag == 1)
                {
                    if(Link[i].isUsed == 1)
                    {
                        break;
                    }
                    loop[loopCount++] = i;
                    Link[i].isUsed = 1;
                    iNode = Link[i].dst;
                    i = -1;
                }
            }

            set_add_rowmode(lp, TRUE);
            k = 0;
            for(i = 0;i < loopCount;i++)
        	{
        	    colno[k] = loop[i] + 1;
        		row[k++] = 1;
        	}
        	add_constraintex(lp, k, row, colno, LE, loopCount - 1);
        	set_add_rowmode(lp, FALSE);

        }
    }
}

//你要完成的功能总入口
void lp_solve(int edge_num)
{
    int i = 0, k = 0, j = 0;
	lprec *lp;
	int Ncol;
	int *colno = NULL;
	REAL *row = NULL;
	REAL *res = NULL;
    int flag = 0;
    int loop[MAXEDGENUM] = {0};
    int loopCount = 0;
    int demand_count = 0;
    int iNode = 0;
    int result_path[MAX_NODE] = {0};

	Ncol = edge_num;
	lp = make_lp(0, Ncol);

	for (i = 1; i <= Ncol; i++)
	{
		set_int(lp, i, TRUE);
	}


	colno = (int *)malloc(Ncol * sizeof(*colno));
	row = (REAL *)malloc(Ncol * sizeof(*row));
	res = (REAL *)malloc(Ncol * sizeof(*res));

	set_add_rowmode(lp, TRUE);
	for (i = 0; i < demand_node_num; i++)
	{
		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[demand_node[i]][j].cost <= 20)
			{
				colno[k] = GraphMatric[demand_node[i]][j].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, EQ, 1);
	}


	for (i = 0; i < demand_node_num; i++)
	{
		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[j][demand_node[i]].cost <= 20)
			{
				colno[k] = GraphMatric[j][demand_node[i]].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, EQ, 1);
	}


	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[j][src_node].cost <= 20)
		{
			colno[k] = GraphMatric[j][src_node].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 0);

	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[src_node][j].cost <= 20)
		{
			colno[k] = GraphMatric[src_node][j].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 1);


	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[dst_node][j].cost <= 20)
		{
			colno[k] = GraphMatric[dst_node][j].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 0);



	k = 0;
	for (j = 0; j < node_num; j++)
	{
		if (GraphMatric[j][dst_node].cost <= 20)
		{
			colno[k] = GraphMatric[j][dst_node].id + 1;
			row[k++] = 1;
		}
	}
	add_constraintex(lp, k, row, colno, EQ, 1);

	for (i = 0; i < node_num; i++)
	{
		k = 0;
		if (i == dst_node) continue;
		if (i == src_node) continue;
		if (isVital[i]) continue;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[j][i].cost <= 20)
			{
				colno[k] = GraphMatric[j][i].id + 1;
				row[k++] = 1;
			}

			if (GraphMatric[i][j].cost <= 20)
			{
				colno[k] = GraphMatric[i][j].id + 1;
				row[k++] = -1;
			}
		}
		add_constraintex(lp, k, row, colno, EQ, 0);
	}

	for (i = 0; i < node_num; i++)
	{
		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[j][i].cost <= 20)
			{
				colno[k] = GraphMatric[j][i].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, LE, 1);

		k = 0;
		for (j = 0; j < node_num; j++)
		{
			if (GraphMatric[i][j].cost <= 20)
			{
				colno[k] = GraphMatric[i][j].id + 1;
				row[k++] = 1;
			}
		}
		add_constraintex(lp, k, row, colno, LE, 1);
	}

	 for(i = 0;i < node_num;i++)
     {
         for(j = 0;j < node_num;j++)
         {
             row[i] = 0;
             if(GraphMatric[i][j].cost <= 20 && GraphMatric[j][i].cost <= 20)
             {
                 k = 0;
                 colno[k] = GraphMatric[i][j].id + 1;
                 row[k++] = 1;
                 colno[k] = GraphMatric[j][i].id + 1;
                 row[k++] = 1;
                 add_constraintex(lp, k, row, colno, LE, 1);
             }
         }
     }


	set_add_rowmode(lp, FALSE);

	j = 0;
	for (i = 1; i <= edge_num; i++)
	{
		colno[j] = i;
		row[j++] = Link[i - 1].cost;
	}
	set_obj_fnex(lp, j, row, colno);

    set_minim(lp);
    set_verbose(lp, IMPORTANT);

    while(1)
    {
      	solve(lp);
		get_variables(lp, res);

        for(j = 0;j < MAXEDGENUM;j++)
        {
            Link[i].isUsed = 0;
        }

        j = 0;
        demand_count = 0;
        iNode = src_node;

        while(iNode != dst_node)
        {
            for(i = 0;i < edge_num;i++)
            {
                if((res[i] == 1) && (Link[i].src == iNode))
                {
                    if(isVital[Link[i].src])
                    {
                        demand_count++;
                    }
                    Link[i].isUsed = 1;
                    result_path[j++] = i;
                    iNode = Link[i].dst;
                    if(iNode == dst_node)
                    {
                        break;
                    }
                    i = -1;
                }
            }
        }

        if(demand_count == demand_node_num)
        {
            for(i = 0;i < j;i++)
            {
                record_result(result_path[i]);
            }
            return;
        }
        else//有环
        {
            flag = 0;
            loopCount = 0;
            for(i = 0;i < edge_num;i++)
            {
                if((res[i] == 1) && !(Link[i].isUsed) && flag == 0)
                {
                    loop[loopCount++] = i;
                    flag = 1;
                    Link[i].isUsed = 1;
                    iNode = Link[i].dst;
                    i = -1;
                    continue;
                }

                if((res[i] == 1) && (Link[i].src == iNode) && flag == 1)
                {
                    if(Link[i].isUsed == 1)
                    {
                        break;
                    }
                    loop[loopCount++] = i;
                    Link[i].isUsed = 1;
                    iNode = Link[i].dst;
                    i = -1;
                    continue;
                }
            }

            set_add_rowmode(lp, TRUE);
            k = 0;
            for(i = 0;i < loopCount;i++)
        	{
        	    colno[k] = loop[i] + 1;
        		row[k++] = 1;
        	}
        	add_constraintex(lp, k, row, colno, LE, loopCount - 1);
        	set_add_rowmode(lp, FALSE);
        }
    }
}



MARK Matrix_BFS(int cut1, int cut2, int cut3, bool state,int dev)
{
	int i = 0,j = 0;
	int k = 0;
	int iPrice = 0;
    pMARK *pmark= (pMARK*)malloc(sizeof(pMARK)*1000000);
    pMARK pmarktmp = NULL;
    int route = 0;
    int m = 0;
    int count = 0;
    MARK minPath;
    int getCount = 0;

    minPath.price = maxsize/dev;

    pmark[route++] = mark_alloc();

    if(isVital[src_node])pmark[0]->count++;

    for(i = 0;i < node_num;i++)
    {
        m = route;
        if(route <= 0)return minPath;
        for(k = 0;k < (m>route?route:m);k++)
        {
            if(pmark[k]->price >= minPath.price || pmark[k]->count < count - cut1)
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
                    if(!isExistNum(pmark[k]->path, pmark[k]->n, j))
                    {
                        pmark[route++] = mark_alloc();
                        *pmark[route - 1] = *pmark[k];
                        pmark[route - 1]->n += 1;
                        pmark[route - 1]->path[pmark[route - 1]->n -1] = j;
                        pmark[route - 1]->price += iPrice;

                        if(pmark[route - 1]->price >= minPath.price)
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        if(isVital[j])
                        {
                            pmark[route - 1]->count++;
                        }

                        if(pmark[route - 1]->count < count - cut2)
                        {
                            free(pmark[route - 1]);
                            route--;
                            continue;
                        }
                        else if(pmark[route - 1]->count > count)
                        {
                            count = pmark[route - 1]->count;
                        }

                        if(j == dst_node)
                        {
                            if(pmark[route - 1]->count == demand_node_num && pmark[route - 1]->price < minPath.price)
                            {
                                minPath = *pmark[route - 1];

                                if(state && ++getCount == 8)
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
            pmarktmp = pmark[route -1];
            pmark[route -1] = pmark[k];
            pmark[k] = pmarktmp;
            free(pmark[route -1]);
            route--;
        }
        arryPath(pmark,&route,count,cut3);
    }

    for(i=0;i<route;i++)
    {
        free(pmark[i]);
    }
    free(pmark);

    return minPath;
}

Node* getHead(pathNode* Nodetmp)
{
    Node* head;
    head = (Node *)malloc(sizeof(Node));
	head->id = src_node;
	head->next = NULL;
	head->pre = NULL;
	head->tail = Nodetmp;
	head->cost = 0;
	head->count = 0;

	return head;
}


MARK Chain_BFS()
{
	int i,j;
	int saveminpath, now_id, next_id;
	Node* iNode;
	Node* next;
	Node* tmp;
	pathNode* Nodetmp;
	int qLen = 0;
    Node* head;
    Node* tail;
    MARK minPath;

    for (i = MAX_NODE - 1; i >= 0; i--)
	{
    	for (j = MAX_NODE - 1; j >= 0; j--)
    	{
    		if (GraphMatric[i][j].cost <= 20)
    		{
    			out_degree[i][0]++;
    			out_degree[i][out_degree[i][0]] = j;
    		}
    	}
    }

    minPath.price = 700;

	Nodetmp = (pathNode *)malloc(sizeof(pathNode));
	Nodetmp->id = src_node;
	Nodetmp->pre = NULL;
	head = getHead(Nodetmp);

	tail = head;
	qLen = 1;

	while (qLen >= 1)
	{
		iNode = head;
		now_id = iNode->id;
		for (i = 1; i <= out_degree[now_id][0]; i++)
		{
			next_id = out_degree[now_id][i];
			saveminpath = iNode->cost + GraphMatric[now_id][next_id].cost;
			if (saveminpath >= minPath.price)
			{
				continue;
			}
			if (next_id == dst_node)
			{
				if (iNode->count == demand_node_num)
				{
					minPath.price = saveminpath;
					minPath = saveResult11(iNode->tail);
				}
				else
				    continue;
			}
			else if (isUsed(iNode->tail, next_id))
			{
				next = (Node *)malloc(sizeof(Node));
				Nodetmp = (pathNode *)malloc(sizeof(pathNode));
				Nodetmp->id = next_id;
				Nodetmp->pre = iNode->tail;
				next->id = next_id;
				next->next = NULL;
				next->pre = NULL;
				next->tail = Nodetmp;
				next->cost = saveminpath;
				next->count = iNode->count;
				if (isVital[next_id])
				{
					next->count = iNode->count + 1;
				}
				tmp = head->next;
				while (tmp != NULL && tmp->count >= next->count)
				{
					tmp = tmp->next;
				}
				if (tmp == NULL)
				{
					tail->next = next;
					next->pre = tail;
					tail = next;
				}
				else
				{
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



bool PQ_BFS_B15(int midnode, queNode* dstnode)
{
	int i;
	int icost, now_id, next_id;
    priority_queue<queNode*, vector<queNode*>, Compare> pq_tmp;

	queNode* iNode = new queNode(src_node);
	iNode->path.push_back(iNode);
	pqb.push(iNode);
	while (!pqb.empty())
	{
        if ((clock() - start_time)/CLOCKS_PER_SEC >= 9)
        {
            timeout = true;
            return false;
        }
		iNode = priorityPop_B();
		now_id = iNode->id;
		for (i = 1; i <= out_degree[now_id][0]; i++)
		{
			next_id = out_degree[now_id][i];
			if ((next_id == dst_node || dstnode->inPath(next_id)) && next_id != midnode)
			    continue;
			icost = iNode->cost + GraphMatric[now_id][next_id].cost;
			if (next_id == midnode)
			{
				for (queNode* n : dstnode->path)
				{
					iNode->path.push_back(n);
				}
				return save2minpath(iNode->path);
			}
			else if (!iNode->inPath(next_id))
			{
				queNode * next = new queNode(next_id, icost, iNode->path, isVital[next_id] ? iNode->count + 1 : iNode->count);
				next->path.push_back(next);
				pqb.push(next);
				if (pqb.size() > maxnode)
				{
					if (maxnode >= cut_num)
						cut = maxnode - cut_num;
					else
						cut = 1;
					cut_num += 5;
					if (cut < pqb.size())
					{
						queNode * node;
						while (cut>0)
						{
							node = priorityPop_B();
							pq_tmp.push(node);
							cut--;
						}
						while (pqb.size()>0)
						{
							node = priorityPop_B();
							delete node;
						}
						while (pq_tmp.size() > 0)
						{
							node = pq_tmp.top();
							pq_tmp.pop();
							pqb.push(node);
						}
					}
				}
			}
		}
	}
	return false;
}


bool PQ_BFS_F15(int midnode)
{
	int i;
	int icost, now_id, next_id;
	MARK minPath;
    priority_queue<queNode*, vector<queNode*>, Compare> pq_tmp;

    minPath.price = 700;
	queNode* iNode = new queNode(midnode);
	iNode->count = 1;
	iNode->path.push_back(iNode);
	pqf.push(iNode);
	while (!pqf.empty())
	{
		if ((clock() - start_time)/CLOCKS_PER_SEC >= 9)
		{
			timeout = true;
			return false;
		}
		iNode = priorityPop_F();
		now_id = iNode->id;
		for (i = 1; i <= out_degree[now_id][0]; i++)
		{
			next_id = out_degree[now_id][i];
			icost = iNode->cost + GraphMatric[now_id][next_id].cost;
			if (icost >= minPath.price)
			{
				continue;
			}
			if (next_id == dst_node)
			{
				if (iNode->count == demand_node_num)
				{
					minPath.price = icost;
					if(PQ_BFS_B15(midnode, iNode))
					{
					    return true;
					}
				}
				else
				    continue;
			}
			else if (!iNode->inPath(next_id))
			{
				queNode * next = new queNode(next_id, icost, iNode->path, isVital[next_id] ? iNode->count + 1 : iNode->count);
				next->path.push_back(next);
				pqf.push(next);
				if (pqf.size() > maxnode)
				{
					if (maxnode >= cut_num)
					{
						cut = maxnode - cut_num;
				    }
					else
					{
						cut = 1;
					}
					cut_num += 5;
					if (cut < pqf.size())
					{
						queNode * node;
						while (cut>0)
						{
							node = priorityPop_F();
							pq_tmp.push(node);
							cut--;
						}
						while (pqf.size()>0)
						{
							node = priorityPop_F();
							delete node;
						}
						while (pq_tmp.size() > 0)
						{
							node = pq_tmp.top();
							pq_tmp.pop();
							pqf.push(node);
						}
					}
				}
			}
		}
	}
	minPath.n = 0;
	return false;
}

MARK PQ_BFS_B(int midnode, queNode* dstnode)
{
	int i;
	int icost, now_id, next_id;
	MARK minPath;
    priority_queue<queNode*, vector<queNode*>, Compare> pq_tmp;

    minPath.price = 700;
    minPath.n = 0;
	queNode* iNode = new queNode(src_node);
	iNode->path.push_back(iNode);
	pqb.push(iNode);
	while (!pqb.empty())
	{
		iNode = priorityPop_B();
		now_id = iNode->id;
		for (i = 1; i <= out_degree[now_id][0]; i++)
		{
			next_id = out_degree[now_id][i];
			if ((next_id == dst_node || dstnode->inPath(next_id)) && next_id != midnode)
			    continue;
			icost = iNode->cost + GraphMatric[now_id][next_id].cost;
			if (next_id == midnode)
			{
				minPath.price = icost;
				for (queNode* n : dstnode->path)
				{
					iNode->path.push_back(n);
				}
				minPath = save2minPath(iNode->path);
				return minPath;
			}
			else if (!iNode->inPath(next_id))
			{
				queNode * next = new queNode(next_id, icost, iNode->path, isVital[next_id] ? iNode->count + 1 : iNode->count);
				next->path.push_back(next);
				pqb.push(next);
				if (pqb.size() > maxnode)
				{
					if (maxnode >= cut_num)
						cut = maxnode - cut_num;
					else
						cut = 1;
					cut_num += 5;
					if (cut < pqb.size())
					{
						queNode * node;
						while (cut>0)
						{
							node = priorityPop_B();
							pq_tmp.push(node);
							cut--;
						}
						while (pqb.size()>0)
						{
							node = priorityPop_B();
							delete node;
						}
						while (pq_tmp.size() > 0)
						{
							node = pq_tmp.top();
							pq_tmp.pop();
							pqb.push(node);
						}
					}
				}
			}
		}
	}
	minPath.n = 0;
	return minPath;
}


MARK PQ_BFS_F(int midnode)
{
	int i;
	int icost, now_id, next_id;
	MARK minPath;
    priority_queue<queNode*, vector<queNode*>, Compare> pq_tmp;

    minPath.price = 700;
	queNode* iNode = new queNode(midnode);
	iNode->count = 1;
	iNode->path.push_back(iNode);
	pqf.push(iNode);
	while (!pqf.empty())
	{
		iNode = priorityPop_F();
		now_id = iNode->id;
		for (i = 1; i <= out_degree[now_id][0]; i++)
		{
			next_id = out_degree[now_id][i];
			icost = iNode->cost + GraphMatric[now_id][next_id].cost;
			if (icost >= minPath.price)
			{
				continue;
			}
			if (next_id == dst_node)
			{
				if (iNode->count == demand_node_num)
				{
					minPath.price = icost;
					minPath = PQ_BFS_B(midnode, iNode);
					if (minPath.n > 0)
					{
					    return minPath;
					}
				}
				else
				    continue;
			}
			else if (!iNode->inPath(next_id))
			{
				queNode * next = new queNode(next_id, icost, iNode->path, isVital[next_id] ? iNode->count + 1 : iNode->count);
				next->path.push_back(next);
				pqf.push(next);
				if (pqf.size() > maxnode)
				{
					if (maxnode >= cut_num)
					{
						cut = maxnode - cut_num;
				    }
					else
					{
						cut = 1;
					}
					cut_num += 5;
					if (cut < pqf.size())
					{
						queNode * node;
						while (cut>0)
						{
							node = priorityPop_F();
							pq_tmp.push(node);
							cut--;
						}
						while (pqf.size()>0)
						{
							node = priorityPop_F();
							delete node;
						}
						while (pq_tmp.size() > 0)
						{
							node = pq_tmp.top();
							pq_tmp.pop();
							pqf.push(node);
						}
					}
				}
			}
		}
	}
	minPath.n = 0;
	return minPath;
}

MARK PQ_BFS(int edge_num)
{
	int midnode, i,j;
	MARK minPath;
    minPath.n = 0;

    for (i = 0; i < MAX_NODE - 1; i++)
	{
    	for (j = 0; j < MAX_NODE - 1; j++)
    	{
    		if (GraphMatric[i][j].cost <= 20)
    		{
    			out_degree[i][0]++;
    			out_degree[i][out_degree[i][0]] = j;
    		}
    	}
    }

    maxnode = 5000;

	for (i = demand_node_num - 1; i >= 0; i--)
	{
		midnode = demand_node[i];
		minPath = PQ_BFS_F(midnode);
		if (minPath.count == demand_node_num)
		{
			return minPath;
		}
	}
	return minPath;
}


MARK PQ_BFS15(int edge_num)
{
	int midnode, i,j;
	MARK minPath;
    minPath.n = 0;
    start_time = clock();
	for (i = MAX_NODE - 1; i >= 0; i--)
	{
		for (j = MAX_NODE - 1; j >= 0; j--)
		{
			if (GraphMatric[i][j].cost < 999)
			{
				out_degree[i][0]++;
				out_degree[i][out_degree[i][0]] = j;
			}
		}
	}
    maxnode = 3000;

	for (i = demand_node_num - 1; i >= 0; i--)
	{
		midnode = demand_node[i];
		if(PQ_BFS_F15(midnode))
		{
		    recordPath(&minpath);
		    minPath.n = 0;
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

    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            GraphMatric[i][j].cost = 999;
        }
    }

    for (i = 0; i < edge_num; i++)
    {
        rc = sscanf(topo[i],"%d%*c%d%*c%d%*c%d",&id,&row,&column,&cost);
        if(rc == 4 && cost < GraphMatric[row][column].cost)
        {
            GraphMatric[row][column].cost = cost;
            GraphMatric[row][column].id = id;
            Link[id].cost = cost;
            Link[id].id = id;
            Link[id].src = row;
            Link[id].dst = column;
            node_num = (row > node_num)? row: node_num;
            node_num = (column> node_num)? column: node_num;
        }
    }
    node_num++;

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
		isVital[i] = false;
	}

	for (i = 0; i < demand_node_num; i++)
	{
		isVital[demand_node[i]] = true;
	}

/*                      唉，人丑还是要多读书
//                .-~~~~~~~~~-._       _.-~~~~~~~~~-.
//            __.'              ~.   .~              `.__
//          .'//                  \./                  \\`.
//        .'//                     |                     \\`.
//      .'// .-~"""""""~~~~-._     |     _,-~~~~"""""""~-. \\`.
//    .'//.-"                 `-.  |  .-'                 "-.\\`.
//  .'//______.============-..   \ | /   ..-============.______\\`.
//.'______________________________\|/______________________________`.
//                        谢谢您观看我的代码
*/

    /*if(edge_num <= 300)//1-6
    {
        minPath = Matrix_BFS(4,3,3,false,5);
    }
    else if(edge_num <= 500)//78
    {
        minPath = Matrix_BFS(4,2,3,false,2);
    }*/
    if(edge_num > 500&&edge_num <= 1000)//8
    {
minPath = Matrix_BFS(2,2,1,false,2);
        if(minPath.n == 0)
        {
            return;
        }
        recordPath(&minPath);
    }
    /*else if(edge_num <= 1100)//9
    {
        minPath = Matrix_BFS(1,0,1,false,2);
    }*/
    else if(edge_num > 1100 && edge_num <= 1500)//10
    {
        minPath = Chain_BFS();
        if(minPath.n == 0)
        {
            return;
        }
        recordPath(&minPath);
    }
    else if(edge_num > 1500 && edge_num <= 2000)//12 13
    {
        minPath = Matrix_BFS(2,1,2,true,1);
        if(minPath.n == 0)
        {
            return;
        }
        recordPath(&minPath);
    }
    /*else if(edge_num <= 2200)//11
    {
        minPath = Chain_BFS();
    }*/
    else if(edge_num >= 2375)//14
    {
        minPath = PQ_BFS(edge_num);
        if(minPath.n == 0)
        {
            return;
        }
        recordPath(&minPath);
    }
    else  if(edge_num > 2200 &&edge_num < 2375)                  //15
    {
        minPath = PQ_BFS15(edge_num);
        if(minPath.n == 0)
        {
            return;
        }
        recordPath(&minPath);
    }
    else
        lp_solve(edge_num);


}

