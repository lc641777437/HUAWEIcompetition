#ifndef __ROUTE_H__
#define __ROUTE_H__

///图信息存储结构体
typedef struct DLink{
    int id;
    int cost;
}DLink, *pDLink;

class CfileRead
{

public:
	bool	StartReadFile();
	bool    readSD_VG();				//读取文件到weightArry,并记录数据N*N
	void	clearChArry(char *chArry, int Num);		//清空字符数组
private:
	const	char	*m_pFilePath;
};

void search_route(char *graph[5000], int edge_num, char *condition);

#endif
