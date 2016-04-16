#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>

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
#include "lp_lib.h"
#pragma comment(lib, "liblpsolve55.lib")
using namespace std;

#include "lp_lib.h"


#define N 601
#define E_LEN 20000
int x_[E_LEN] = { 0 };
int y_[E_LEN] = { 0 };
int g_index[N][N] = { 0 };
int g_cost[N][N] = { 0 }; //存储i到j的路径长度
int route_id[N][N] = { 0 }; //存储i到j的路径的SourceID
int out_node[N][N] = { 0 }; //存储I节点的出节点。
int link[N] = { 0 };
bool gone[N] = { 0 };
int x, y, l, s; //临时变量
int tmp_len;        //临时变量
int con_num = 0;
int v_gone = 0;
char str_test[10];        //临时字符数组
bool ok = false;    //临时布尔变量
int source, target; //开始节点和结束节点
int v_len = 0;    //必经节点的个数
int v[100];            //必经节点数组
bool is_v[N] = { 0 };

int MAX_NODE = 5000;

//时间变量
int now_time_s = 0;
int now_time_ms = 0;

int min_cost_link[N] = { 0 };//最短路径节点数组
int min_cost = 15000;    //最小cost
int delete_num = 0;			//要删除的节点数目
int delete_times = 0;		//删除的次数

//下面两个结构体用于宽搜链表队列
bool hurry = false;		//判断时间是否足够

//优先队列节点
struct queNode
{
	short id; //当前节点route_id
	short cost; //目前的耗费
	short goes_node;//经过的必经节点数目
	vector<queNode*> path;    //默认会包含起点
	queNode(short i) : id(i), cost(0), goes_node(0) {};
	queNode(short i, short c, vector<queNode*> p, short a) : id(i), cost(c), path(p), goes_node(a) {};
	bool inPath(short target) {
		for (queNode* i : path)
		if (target == i->id)
			return true;
		return false;
	}
};
//pq中优先级的比较
struct Compare {
	bool operator () (queNode* node1, queNode* node2) {
		if (!node1->goes_node || !node2->goes_node) {
			if (!node1->goes_node)
				return true;
			return false;
		}
		return node1->cost / node1->goes_node > node2->cost / node2->goes_node;
	}
};
//pq中优先级的比较
struct Compare_2 {
	bool operator () (queNode* node1, queNode* node2) {
		if (!node1->goes_node || !node2->goes_node) {
			if (!node1->goes_node)
				return true;
			return false;
		}
		return node1->cost - node1->goes_node * 25 > node2->cost - node2->goes_node * 25;
	}
};
priority_queue<queNode*, vector<queNode*>, Compare_2> pq_14;
priority_queue<queNode*, vector<queNode*>, Compare_2> tmp_pq_14;

priority_queue<queNode*, vector<queNode*>, Compare_2> pq2_14;
priority_queue<queNode*, vector<queNode*>, Compare_2> tmp_pq2_14;

priority_queue<queNode*, vector<queNode*>, Compare> pq;
priority_queue<queNode*, vector<queNode*>, Compare> tmp_pq;
priority_queue<queNode*, vector<queNode*>, Compare> pq2;
priority_queue<queNode*, vector<queNode*>, Compare> tmp_pq_2;

void gogogo(char *topo[5000], int edge_num, char *demand);
/*
根据minCostink输出最优解，正向输出
*/
void print_res(){
	int i;
	for (i = 1; i < min_cost_link[0]; i++){
		record_result(route_id[min_cost_link[i]][min_cost_link[i + 1]]);
	}
}
/*
已经有解则存储该解
*/
bool saveResult(vector<queNode*> path){
	min_cost_link[0] = 0;
	for (queNode* n : path){
		cout << n->id << " ";
		min_cost_link[0]++;
		min_cost_link[min_cost_link[0]] = n->id;
	}
	min_cost_link[0]++;
	min_cost_link[min_cost_link[0]] = target;
	return true;
}

//获取当前时间
void get_time()
{
	struct timeb rawtime;
	ftime(&rawtime);
	static int ms = rawtime.millitm;
	static unsigned long s = rawtime.time;
	int out_ms = rawtime.millitm - ms;
	unsigned long out_s = rawtime.time - s;
	ms = rawtime.millitm;
	s = rawtime.time;
	if (out_ms < 0)
	{
		out_ms += 1000;
		out_s -= 1;
	}
	now_time_ms += out_ms;
	now_time_s += out_s;
	if (now_time_ms > 1000){
		now_time_ms -= 1000;
		now_time_s++;
	}
}

//优先队列节点出站
queNode* priorityPop() {
	queNode* result = pq.top();
	pq.pop();
	return result;
}
//优先队列节点出站
queNode* priorityPop2() {
	queNode* result = pq2.top();
	pq2.pop();
	return result;
}
//优先队列节点出站
queNode* priorityPop_14() {
	queNode* result = pq_14.top();
	pq_14.pop();
	return result;
}

//优先队列节点出站
queNode* priorityPop2_14() {
	queNode* result = pq2_14.top();
	pq2_14.pop();
	return result;
}

/*
中间节点正向性价比宽度优先搜索第二阶段
*/
bool forward_midnode_bfs_14_2(int mid_node_id, queNode* endNode){
	int i;
	int tmp_cost, now_id, next_id;
	int tmpTarget = mid_node_id;
	queNode* now = new queNode(source);
	now->path.push_back(now); //初始节点
	pq2_14.push(now);              //初始节点进入队列
	while (!pq2_14.empty()){
		get_time();
		if (now_time_s >= 16){
			hurry = true;
			return false;
		}
		now = priorityPop2_14();
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++){
			next_id = out_node[now_id][i];
			if ((next_id == target || endNode->inPath(next_id)) && next_id != tmpTarget) continue;
			tmp_cost = now->cost + g_cost[now_id][next_id];
			if (next_id == tmpTarget){
				min_cost = tmp_cost;
				for (queNode* n : endNode->path){
					now->path.push_back(n);
				}
				saveResult(now->path);
				return true;
			}
			else if (!now->inPath(next_id)){
				queNode * next = new queNode(next_id, tmp_cost, now->path, is_v[next_id] ? now->goes_node + 1 : now->goes_node);
				next->path.push_back(next);
				pq2_14.push(next);
				if (pq2_14.size() > MAX_NODE){
					if (MAX_NODE >= delete_times)
						delete_num = MAX_NODE - delete_times;
					else
						delete_num = 1;
					delete_times += 5;
					if (delete_num < pq2_14.size()){
						queNode * node;
						while (delete_num>0){
							node = priorityPop2_14();
							tmp_pq2_14.push(node);
							delete_num--;
						}
						while (pq2_14.size()>0){
							node = priorityPop2_14();
							delete node;
						}
						while (tmp_pq2_14.size() > 0){
							node = tmp_pq2_14.top();
							tmp_pq2_14.pop();
							pq2_14.push(node);
						}
					}
				}
			}
		}
	}
	return false;
}

/*
中间节点正向性价比宽度优先搜索第一阶段
*/
bool forward_midnode_bfs_14_1(int mid_node_id){
	int i;
	int tmp_cost, now_id, next_id;
	queNode* now = new queNode(mid_node_id);
	now->goes_node = 1;
	now->path.push_back(now); //初始节点
	pq_14.push(now);              //初始节点进入队列
	while (!pq_14.empty()){
		get_time();
		if (now_time_s >= 16){
			hurry = true;
			return false;
		}
		now = priorityPop_14();
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++){
			next_id = out_node[now_id][i];
			tmp_cost = now->cost + g_cost[now_id][next_id];
			if (tmp_cost >= min_cost) {
				continue;
			}
			if (next_id == target){
				if (now->goes_node == v_len){
					min_cost = tmp_cost;
					if (forward_midnode_bfs_14_2(mid_node_id, now)) return true;
				}
				else continue;
			}
			else if (!now->inPath(next_id)){
				queNode * next = new queNode(next_id, tmp_cost, now->path, is_v[next_id] ? now->goes_node + 1 : now->goes_node);
				next->path.push_back(next);
				pq_14.push(next);
				if (pq_14.size() > MAX_NODE){
					if (MAX_NODE >= delete_times)
						delete_num = MAX_NODE - delete_times;
					else
						delete_num = 1;
					delete_times += 5;
					if (delete_num < pq_14.size()){
						queNode * node;
						while (delete_num>0){
							node = priorityPop_14();
							tmp_pq_14.push(node);
							delete_num--;
						}
						while (pq_14.size()>0){
							node = priorityPop_14();
							delete node;
						}
						while (tmp_pq_14.size() > 0){
							node = tmp_pq_14.top();
							tmp_pq_14.pop();
							pq_14.push(node);
						}
					}
				}
			}
		}
	}
	return false;
}

/*
中间节点正向性价比宽度优先搜索第二阶段
*/
bool forward_midnode_bfs_15_2(int mid_node_id, queNode* endNode){
	int i;
	int tmp_cost, now_id, next_id;
	int tmpTarget = mid_node_id;
	queNode* now = new queNode(source);
	now->path.push_back(now); //初始节点
	pq2.push(now);              //初始节点进入队列
	while (!pq2.empty()){
		get_time();
		if (now_time_s >= 16){
			hurry = true;
			return false;
		}
		now = priorityPop2();
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++){
			next_id = out_node[now_id][i];
			if ((next_id == target || endNode->inPath(next_id)) && next_id != tmpTarget) continue;
			tmp_cost = now->cost + g_cost[now_id][next_id];
			if (next_id == tmpTarget){
				min_cost = tmp_cost;
				for (queNode* n : endNode->path){
					now->path.push_back(n);
				}
				saveResult(now->path);
				return true;
			}
			else if (!now->inPath(next_id)){
				queNode * next = new queNode(next_id, tmp_cost, now->path, is_v[next_id] ? now->goes_node + 1 : now->goes_node);
				next->path.push_back(next);
				pq2.push(next);
				if (pq2.size() > MAX_NODE){
					if (MAX_NODE >= delete_times)
						delete_num = MAX_NODE - delete_times;
					else
						delete_num = 1;
					delete_times += 5;
					if (delete_num < pq2.size()){
						queNode * node;
						while (delete_num>0){
							node = priorityPop2();
							tmp_pq_2.push(node);
							delete_num--;
						}
						while (pq2.size()>0){
							node = priorityPop2();
							delete node;
						}
						while (tmp_pq_2.size() > 0){
							node = tmp_pq_2.top();
							tmp_pq_2.pop();
							pq2.push(node);
						}
					}
				}
			}
		}
	}
	return false;
}
/*
中间节点正向性价比宽度优先搜索第一阶段
*/
bool forward_midnode_bfs_15_1(int mid_node_id){
	int i;
	int tmp_cost, now_id, next_id;
	queNode* now = new queNode(mid_node_id);
	now->goes_node = 1;
	now->path.push_back(now); //初始节点
	pq.push(now);              //初始节点进入队列
	while (!pq.empty()){
		get_time();
		if (now_time_s >= 16){
			hurry = true;
			return false;
		}
		now = priorityPop();
		now_id = now->id;
		for (i = 1; i <= out_node[now_id][0]; i++){
			next_id = out_node[now_id][i];
			tmp_cost = now->cost + g_cost[now_id][next_id];
			if (tmp_cost >= min_cost) {
				continue;
			}
			if (next_id == target){
				if (now->goes_node == v_len){
					min_cost = tmp_cost;
					if (forward_midnode_bfs_15_2(mid_node_id, now)) return true;
				}
				else continue;
			}
			else if (!now->inPath(next_id)){
				queNode * next = new queNode(next_id, tmp_cost, now->path, is_v[next_id] ? now->goes_node + 1 : now->goes_node);
				next->path.push_back(next);
				pq.push(next);
				if (pq.size() > MAX_NODE){
					if (MAX_NODE >= delete_times)
						delete_num = MAX_NODE - delete_times;
					else
						delete_num = 1;
					delete_times += 5;
					if (delete_num < pq.size()){
						queNode * node;
						while (delete_num>0){
							node = priorityPop();
							tmp_pq.push(node);
							delete_num--;
						}
						while (pq.size()>0){
							node = priorityPop();
							delete node;
						}
						while (tmp_pq.size() > 0){
							node = tmp_pq.top();
							tmp_pq.pop();
							pq.push(node);
						}
					}
				}
			}
		}
	}
	return false;
}
/*
中间节点性价比宽度优先搜索入口函数
*/
bool forward_midnode_bfs_start_15(){
	int mid_node_id, i, j;
	for (i = v_len - 1; i >= 0; i--){
		mid_node_id = v[i];
		if (forward_midnode_bfs_15_1(mid_node_id)){//找到了一个路径经过必经节点。存储在minCostlink中
			if (hurry) return false;
			return true;
		}
	}
}

/*
中间节点性价比宽度优先搜索入口函数
*/
bool forward_midnode_bfs_start_14(){
	int mid_node_id, i, j;
	for (i = v_len - 1; i >= 0; i--){
		mid_node_id = v[i];
		if (forward_midnode_bfs_14_1(mid_node_id)){//找到了一个路径经过必经节点。存储在minCostlink中
			if (hurry) return false;
			return true;
		}
	}
}


/*
正向格式化存储 从编号大的节点开始遍历
*/
void init_forward_large(char *topo[5000], int edge_num, char *demand){
	//根据读入进行初始化
	int i, j;
	for (i = 0; i < N; i++){
		gone[i] = false;
	}
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			g_cost[i][j] = 100;//表示无路径
		}
	}
	for (i = 0; i < edge_num; i++){
		sscanf(topo[i], "%d,%d,%d,%d", &s, &x, &y, &l);
		if (l < g_cost[x][y]) {
			g_cost[x][y] = l;//筛选出较短的边
			route_id[x][y] = s;//记录他们的route_id
		}
	}
	for (i = N - 1; i >= 0; i--){
		for (j = N - 1; j >= 0; j--){
			if (g_cost[i][j] != 100){
				out_node[i][0]++;
				out_node[i][out_node[i][0]] = j;
			}
		}
	}
	tmp_len = strlen(demand);
	v_len = 0;
	j = 0;
	for (i = 0; i <= tmp_len; i++){
		if (demand[i] == '|' || i == tmp_len) {
			str_test[j] = '\0';
			v[v_len] = atoi(str_test);
			v_len++;
			j = 0;
		}
		else if (demand[i] == ','){
			str_test[j] = '\0';
			if (!ok){
				source = atoi(str_test);
				ok = true;
			}
			else{
				target = atoi(str_test);
			}
			j = 0;
		}
		else {
			str_test[j] = demand[i];
			j++;
		}
	}
	for (i = 0; i < N; i++){
		is_v[i] = false;
	}
	for (i = 0; i < v_len; i++){
		is_v[v[i]] = true;
	}
	//v_len存储v中节点个数，v数组存储节点信息
	//Len存储i节点到j节点的路径长度，如果为100,则表示没有路径
}


/*
正向格式化存储
*/
void init_forward(char *topo[5000], int edge_num, char *demand){
	//根据读入进行初始化
	int i, j;
	for (i = 0; i < N; i++){
		gone[i] = false;
	}
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			g_cost[i][j] = 100;//表示无路径
		}
	}
	for (i = 0; i < edge_num; i++){
		sscanf(topo[i], "%d,%d,%d,%d", &s, &x, &y, &l);
		if (l < g_cost[x][y]) {
			g_cost[x][y] = l;//筛选出较短的边
			route_id[x][y] = s;//记录他们的route_id
		}
	}
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			if (g_cost[i][j] != 100){
				out_node[i][0]++;
				out_node[i][out_node[i][0]] = j;
			}
		}
	}
	tmp_len = strlen(demand);
	v_len = 0;
	j = 0;
	for (i = 0; i <= tmp_len; i++){
		if (demand[i] == '|' || i == tmp_len) {
			str_test[j] = '\0';
			v[v_len] = atoi(str_test);
			v_len++;
			j = 0;
		}
		else if (demand[i] == ','){
			str_test[j] = '\0';
			if (!ok){
				source = atoi(str_test);
				ok = true;
			}
			else{
				target = atoi(str_test);
			}
			j = 0;
		}
		else {
			str_test[j] = demand[i];
			j++;
		}
	}
	for (i = 0; i < N; i++){
		is_v[i] = false;
	}
	for (i = 0; i < v_len; i++){
		is_v[v[i]] = true;
	}
	//v_len存储v中节点个数，v数组存储节点信息
	//Len存储i节点到j节点的路径长度，如果为100,则表示没有路径
}

/*
程序开始
*/
void search_route(char *topo[5000], int edge_num, char *demand)
{
	if (edge_num >= 2200 && edge_num < 2375){
		MAX_NODE = 3000;
		init_forward_large(topo, edge_num, demand);
		forward_midnode_bfs_start_15();
		print_res();
	}
	else if(edge_num >= 2375){
		MAX_NODE = 5000;
		init_forward(topo, edge_num, demand);
		forward_midnode_bfs_start_14();
		print_res();
	}
	else{
		gogogo(topo,edge_num,demand);
	}

}

/*
格式化存储
*/
void init(char *topo[5000], int edge_num, char *demand){
	//根据读入进行初始化
	int i, j;
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			g_cost[i][j] = 100;//表示无路径
		}
	}

	j = 1;
	for (i = 0; i < edge_num; i++){
		sscanf(topo[i], "%d,%d,%d,%d", &s, &x, &y, &l);
		if (l < g_cost[x][y]) {
			g_cost[x][y] = l;//筛选出较短的边
			route_id[x][y] = s;//记录他们的sourceID
			if (g_index[x][y] == 0){

				out_node[x][0]++;
				out_node[x][out_node[x][0]] = y;

				x_[j] = x;
				y_[j] = y;
				g_index[x][y] = j;
				j++;

			}
		}
	}
	con_num = j - 1;
	cout<<"con_num"<<con_num<<endl;

	tmp_len = strlen(demand);
	v_len = 0;
	j = 0;
	for (i = 0; i <= tmp_len; i++){
		if (demand[i] == '|' || i == tmp_len) {
			str_test[j] = '\0';
			v[v_len] = atoi(str_test);
			v_len++;
			j = 0;
		}
		else if (demand[i] == ','){
			str_test[j] = '\0';
			if (!ok){
				source = atoi(str_test);
				ok = true;
			}
			else{
				target = atoi(str_test);
			}
			j = 0;
		}
		else {
			str_test[j] = demand[i];
			j++;
		}
	}

	for (i = 0; i < N; i++){
		is_v[i] = false;
	}
	for (i = 0; i < v_len; i++){
		is_v[v[i]] = true;
	}
}
//你要完成的功能总入口
void gogogo(char *topo[5000], int edge_num, char *demand)
{
	lprec *lp;
	int Ncol, *colno = NULL, i, k, j, ret = 0;
	REAL *row = NULL;
	REAL *res = NULL;

	init(topo, edge_num, demand);
	Ncol = con_num;
	lp = make_lp(0, Ncol); //建立n条边的参数
	if (lp == NULL)
		ret = 1;
	for (i = 1; i <= Ncol; i++){ //设置所有参数为整型
		set_int(lp, i, TRUE);
	}
	if (ret == 0) {
		colno = (int *)malloc(Ncol * sizeof(*colno));
		row = (REAL *)malloc(Ncol * sizeof(*row));
		res = (REAL *)malloc(Ncol * sizeof(*res));
		if ((colno == NULL) || (row == NULL))
			ret = 2;
	}
	if (ret == 0)
	{
		set_add_rowmode(lp, TRUE);  //开始添加约束条件
		/* 对于每个v，出度为1 */
		for (i = 0; i < v_len; i++){
			k = 0;
			for (j = 0; j < N; j++){
				if (v[i] != j && g_cost[v[i]][j] != 100){
					colno[k] = g_index[v[i]][j];
					row[k++] = 1;
				}
			}
			if (!add_constraintex(lp, k, row, colno, EQ, 1))
				ret = 3;
		}

	}

	if (ret == 0) {
		/* 对于每个v，入度为1 */
		for (i = 0; i < v_len; i++){
			k = 0;
			for (j = 0; j < N; j++){
				if (v[i] != j && g_cost[j][v[i]] != 100){
					colno[k] = g_index[j][v[i]];
					row[k++] = 1;
				}
			}
			if (!add_constraintex(lp, k, row, colno, EQ, 1))
				ret = 3;
		}
	}

	if (ret == 0) {
		/* source节点入度为0 */
		k = 0;
		for (j = 0; j < N; j++){
			if (source != j && g_cost[j][source] != 100){
				colno[k] = g_index[j][source];
				row[k++] = 1;
			}
		}
		if (!add_constraintex(lp, k, row, colno, EQ, 0))
			ret = 3;
	}


	if (ret == 0) {
		/* source节点出度为1 */
		k = 0;
		for (j = 0; j < N; j++){
			if (source != j && g_cost[source][j] != 100){
				colno[k] = g_index[source][j];
				row[k++] = 1;
			}
		}
		if (!add_constraintex(lp, k, row, colno, EQ, 1))
			ret = 3;
	}

	if (ret == 0) {
		/* target节点出度为0 */
		k = 0;
		for (j = 0; j < N; j++){
			if (target != j && g_cost[target][j] != 100){
				colno[k] = g_index[target][j];
				row[k++] = 1;
			}
		}
		if (!add_constraintex(lp, k, row, colno, EQ, 0))
			ret = 3;
	}

	if (ret == 0) {
		/* target节点入度为1 */
		k = 0;
		for (j = 0; j < N; j++){
			if (target != j && g_cost[j][target] != 100){
				colno[k] = g_index[j][target];
				row[k++] = 1;
			}
		}
		if (!add_constraintex(lp, k, row, colno, EQ, 1))
			ret = 3;
	}

	if (ret == 0) {
		/* 其余节点入度等于出度 */
		for (i = 0; i < N; i++){
			k = 0;
			if (i == target) continue;
			if (i == source) continue;
			if (is_v[i]) continue;
			for (j = 0; j < N; j++){
				if (i != j && g_cost[j][i] != 100){
					colno[k] = g_index[j][i];
					row[k++] = 1;
				}
			}
			for (j = 0; j < N; j++){
				if (i != j && g_cost[i][j] != 100){
					colno[k] = g_index[i][j];
					row[k++] = -1;
				}
			}
			if (!add_constraintex(lp, k, row, colno, EQ, 0))
				ret = 3;
		}
	}


	if (ret == 0) {
		/* 所有节点入度出度小于等于1 */
		for (i = 0; i < N; i++){
			k = 0;
			for (j = 0; j < N; j++){
				if (i != j && g_cost[j][i] != 100){
					colno[k] = g_index[j][i];
					row[k++] = 1;
				}
			}
			if (!add_constraintex(lp, k, row, colno, LE, 1))
				ret = 3;

			k = 0;
			for (j = 0; j < N; j++){
				if (i != j && g_cost[i][j] != 100){
					colno[k] = g_index[i][j];
					row[k++] = 1;
				}
			}
			if (!add_constraintex(lp, k, row, colno, LE, 1))
				ret = 3;
		}
	}
	/*
	if (ret == 0) {
		for (i = 0; i < N; i++){
			k = 0;
			for (j = 0; j < N; j++){
				if (i != j && g_cost[j][i] != 100 && g_cost[i][j] != 100){
					colno[0] = g_index[j][i];
					row[0] = 1;
					colno[1] = g_index[i][j];
					row[1] = 1;
					if (!add_constraintex(lp, 2, row, colno, LE, 1))
						ret = 3;
					}
			}
		}
	}*/
	if (ret == 0) {
		set_add_rowmode(lp, FALSE); //添加参数完毕
		/* 求解目标方程 */
		j = 0;
		for (i = 1; i <= con_num; i++){
			colno[j] = i;
			row[j++] = g_cost[x_[i]][y_[i]];
		}
		if (!set_obj_fnex(lp, j, row, colno))
			ret = 4;
	}

	if (ret == 0) {
		//设置求最小值
		set_minim(lp);
		//只打印重要信息
		set_verbose(lp, IMPORTANT);
	}
	if (ret == 0) {
		while (1){
			//开始解题
			ret = solve(lp);
			if (ret == OPTIMAL)
				ret = 0;
			else
				ret = 5;
			//解题结果存在res中
			get_variables(lp, res);

			//source 开始出发
			for (i = 0; i<N; i++){
				gone[i] = false;
			}
			int x = source;
			int y = -1;
			link[0] = 0;
			v_gone = 0;
			while (x != target){
				for (i = 1; i <= out_node[x][0]; i++){
					y = out_node[x][i];
					if (fabs(res[g_index[x][y] - 1] - 1) < 0.0001){
						if (is_v[y]) v_gone++;
						gone[y] = true;
						link[0]++;
						link[link[0]] = g_index[x][y];
						x = y;
						break;
					}
				}
			}
			if (x == target && v_gone == v_len){//已经求出最优解
				for (i = 1; i <= link[0]; i++){
					x = x_[link[i]];
					y = y_[link[i]];
					record_result(route_id[x][y]);
				}
				break;
			}
			else{//中间有环
				for (i = 0; i<v_len; i++){
					link[0] = 0;
					x = v[i];
					if (gone[x]) continue;

					//if (is_v[x]) printf("(v)");
					//printf("%d -> ", x);

					do{
						for (j = 0; j <= out_node[x][0]; j++){
							y = out_node[x][j];
							if (fabs(res[g_index[x][y] - 1] - 1) < 0.00001){
								gone[y] = true;
								link[0]++;
								link[link[0]] = g_index[x][y];
								x = y;
								break;
							}
						}
						//if (is_v[x]) printf("(v)");
						//printf("%d -> ", x);
					} while (x != v[i]);
					//printf("\n");
					set_add_rowmode(lp, TRUE);
					/* 添加去除环的约束 */
					for (j = 1; j <= link[0]; j++){ //去除环
						colno[j - 1] = link[j];
						row[j - 1] = 1;
					}
					if (!add_constraintex(lp, link[0], row, colno, LE, link[0] - 1))
						ret = 3;
					set_add_rowmode(lp, FALSE);
				}
			}
		}
	}

	//打印最小值
	printf("Objective value: %f\n", get_objective(lp));

	if (row != NULL)
		free(row);
	if (colno != NULL)
		free(colno);
	if (lp != NULL) {
		delete_lp(lp);
	}
	return;
}
