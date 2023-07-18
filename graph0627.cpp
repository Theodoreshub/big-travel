#include <iostream>
#include <vector>
#include<queue>
using namespace std;
#define MaxVertices 100
#define FullEle 400
#define ComforEle 100

//定义结点
struct VertexNode {
    int data;    //结点编号
    int weight = 0;    //指向下一个结点的边的权值
    VertexNode* next = NULL;
};

//定义邻接表
struct GraphAdjList {
    VertexNode* AdjList[MaxVertices];    //存储所有结点
    int numV, numE;
};

//network expansion 中队列的元素，感觉用不上队列。确实没用上。
class Node {    
public:
    int data;   //结点编号
    int lenth;  //到源点的距离
    Node(int data, int lenth):data(data), lenth(lenth){}

    bool operator<(const Node& a)const {
        return lenth < a.lenth;
    }
    bool operator>(const Node& a)const {
        return lenth > a.lenth;
    }
};

//实例图
void GraphCase(vector<vector<int>> &edges) {
    edges[0][0] = 0; edges[0][1] = 2; edges[0][2] = 230;
    edges[1][0] = 1; edges[1][1] = 2; edges[1][2] = 60;
    edges[2][0] = 1; edges[2][1] = 3; edges[2][2] = 49;
    edges[3][0] = 1; edges[3][1] = 11; edges[3][2] = 90;
    edges[4][0] = 3; edges[4][1] = 4; edges[4][2] = 90;
    edges[5][0] = 3; edges[5][1] = 8; edges[5][2] = 30;
    edges[6][0] = 4; edges[6][1] = 5; edges[6][2] = 190;
    edges[7][0] = 4; edges[7][1] = 9; edges[7][2] = 80;
    edges[8][0] = 5; edges[8][1] = 6; edges[8][2] = 140;
    edges[9][0] = 5; edges[9][1] = 7; edges[9][2] = 100;
    edges[10][0] = 6; edges[10][1] = 16; edges[10][2] = 50;
    edges[11][0] = 9; edges[11][1] = 10; edges[11][2] = 130;
    edges[12][0] = 9; edges[12][1] = 11; edges[12][2] = 90;
    edges[13][0] = 10; edges[13][1] = 16; edges[13][2] = 120;
    edges[14][0] = 11; edges[14][1] = 12; edges[14][2] = 30;
    edges[15][0] = 11; edges[15][1] = 13; edges[15][2] = 44;
    edges[16][0] = 13; edges[16][1] = 14; edges[16][2] = 53;
    edges[17][0] = 13; edges[17][1] = 15; edges[17][2] = 29;
}

//创建图 (目前的图是无向有权图，用邻接表实现)
void CreatGraph(GraphAdjList& G) {
    // 先实现例图

    int vi, vj, w;
    //cout << "请输入顶点数：" << endl;
    //cin >> G.numV;
    G.numV = 17;
    //cout << "请输入顶点信息：" << endl;
    // 换成固定从0开始的顺序编号
    for (int i = 0; i < G.numV; i++) {
        //cin >> vi;
        VertexNode* new_node = new VertexNode;
        //new_node->data = vi;
        new_node->data = i;
        new_node->next = NULL;
        G.AdjList[i] = new_node;
    }
    //cout << "请输入边的数量：" << endl;
    //cin >> G.numE;
    G.numE = 18;
    //cout << "请输入边的信息：" << endl;
    // 边数组
    vector<vector<int>> edges(18, vector<int>(3));
    GraphCase(edges);
    for (int i = 0; i < G.numE; i++) {
        VertexNode* temp = G.AdjList[edges[i][0]];
        while (temp->next != NULL) temp = temp->next;   //尾插法
        VertexNode* newEdge = new VertexNode;
        newEdge->data = edges[i][1];
        newEdge->weight = edges[i][2];
        newEdge->next = NULL;
        temp->next = newEdge;
        // 无向图，边需要添加两次
        temp = G.AdjList[edges[i][1]];
        while (temp->next != NULL) temp = temp->next;
        newEdge = new VertexNode;
        newEdge->data = edges[i][0];
        newEdge->weight = edges[i][2];
        newEdge->next = NULL;
        temp->next = newEdge;
    }
    //for (int i = 0; i < G.numE; i++) {
    //    cin >> vi >> vj >> w;
    //    //找到邻接表中对应结点的位置，往其中链表插入对应边
    //    for (int j = 0; j < G.numV; j++) {
    //        if (vi == G.AdjList[j]->data) {
    //            VertexNode* temp = G.AdjList[j];
    //            //这里用的是尾插法
    //            while (temp->next != NULL) {
    //                temp = temp->next;
    //            }
    //            VertexNode* newEdge = new VertexNode;
    //            newEdge->data = vj;
    //            newEdge->weight = w;
    //            temp->next = newEdge;
    //            break;
    //        }
    //    }
    //}
    
    // 完成
}

//Dijkstra
void Dijkstra(GraphAdjList& GAL, int v0, vector<int> &D, vector<int> &P) {
    //初始化，加入初始点和记录邻接点距离
    vector<bool> Final(GAL.numV, false);
    D[v0] = 0; Final[v0] = 1; P[v0] = v0;
    VertexNode* temp = GAL.AdjList[v0];
    while (temp->next != NULL) {
        temp = temp->next;
        D[temp->data] = temp->weight;
        P[temp->data] = v0;
    }
    //开始算法
    int k = v0; // 当前最近的结点
    for (int i = 0; i < GAL.numV; i++) {
        int MIN = INT_MAX;
        //int last = k;
        for (int j = 0; j < GAL.numV; j++) {
            if (Final[j] == 0 && D[j] < MIN) {
                MIN = D[j];
                k = j;
            }
        }
        Final[k] = 1;
        temp = GAL.AdjList[k];
        while (temp->next != NULL) {
            temp = temp->next;
            if (Final[temp->data] == 0 && temp->weight + MIN < D[temp->data]) {
                D[temp->data] = temp->weight + MIN;
                P[temp->data] = k;
            }
        }
    }
    for (int i = 0; i < GAL.numV; i++) cout << Final[i] << "   ";
    cout << endl;


}

//network expansion
void NetworkExpansion(GraphAdjList& GAL, int v_start, int v_end) {  //我认为和Dijkstra唯一的区别在于不用全部遍历完
    vector<int> D(GAL.numV, INT_MAX);   //和源点的距离
    vector<int> P(GAL.numV, -1);    //上一个结点
    vector<bool> Final(GAL.numV, false);
    //初始化
    D[v_start] = 0; P[v_start] = v_start;   Final[v_start] = true;
    VertexNode* temp = GAL.AdjList[v_start];
    while (temp->next != NULL) {
        temp = temp->next;
        P[temp->data] = v_start;
        D[temp->data] = temp->weight;
    }
    //算法开始
    int k = v_start;
    while (Final[v_end] == false) {     //这应该是唯一不同的点
        int MIN = INT_MAX;
        for (int i = 0; i < GAL.numV; i++) {
            if (Final[i] == false && D[i] < MIN) {
                MIN = D[i];
                k = i;
            }
        }
        Final[k] = true;
        temp = GAL.AdjList[k];
        while (temp->next != NULL) {
            temp = temp->next;
            if (Final[temp->data] == false && temp->weight + D[k] < D[temp->data]) {
                D[temp->data] = temp->weight + D[k];
                P[temp->data] = k;
            }
        }
    }

    for (int i = 0; i < GAL.numV; i++) {
        cout << P[i] << ' ';
    }
}

//实例电站,电站的数量应该是远远低于点的数量的
void StationCase(vector<vector<int>>& stations) {
    // (p1, p2, l1 到l1的距离, l2)
    stations[0][0] = 0; stations[0][1] = 2; stations[0][2] = 150; stations[0][3] = 80;
    stations[1][0] = 1; stations[1][1] = 3; stations[1][2] = 30; stations[1][3] = 19;
    stations[2][0] = 4; stations[2][1] = 5; stations[2][2] = 110; stations[2][3] = 80;
    stations[3][0] = 9; stations[3][1] = 10; stations[3][2] = 110; stations[3][3] = 20;
    stations[4][0] = 9; stations[4][1] = 11; stations[4][2] = 30; stations[4][3] = 60;
    stations[5][0] = 10; stations[5][1] = 16; stations[5][2] = 40; stations[5][3] = 80;
    stations[6][0] = 11; stations[6][1] = 12; stations[6][2] = 23; stations[6][3] = 7;
    stations[7][0] = 13; stations[7][1] = 15; stations[7][2] = 6; stations[7][3] = 23;
}

//定义路径
struct Arrive {
    bool isArrive;  //是否可达
    vector<int> road;   //路径
    int lenth;  //路径长度
};

//递归调用电站寻路
Arrive EleRecursion(GraphAdjList& GAL, vector<vector<int>> stations, int s_station, int v_end, int ele, int comfor_ele, vector<bool> Final) {   //从电桩开始出发，代码同EleNetworkExpansion类似。这里的ele是满电。
    Arrive arr; //初始化返回结果，vector应该默认为空。
    arr.isArrive = false;
    arr.lenth = INT_MAX;
    //初始化辅助数组
    vector<int> D(GAL.numV, INT_MAX);   //到电桩的距离
    vector<int> P(GAL.numV, -1);    //父节点
    vector<bool> Margin(GAL.numV, false);   //是否为边缘点
    VertexNode* temp;
    if (Final[stations[s_station][0]] == false) {   //两个端点到电桩的距离，若已走过则不计算。一点为false和两点为false都有可能。
        Final[stations[s_station][0]] = true;
        D[stations[s_station][0]] = stations[s_station][2];
        temp = GAL.AdjList[stations[s_station][0]];
        while (temp->next != NULL) {
            temp = temp->next;
            if (temp->data != stations[s_station][1]) { //另一个邻接点一定是相邻边，不参与此点扩展
                P[temp->data] = stations[s_station][0];
                D[temp->data] = temp->weight + stations[s_station][2];
            }
        }
    }
    if (Final[stations[s_station][1]] == false) {
        Final[stations[s_station][1]] = true;
        D[stations[s_station][1]] = stations[s_station][3];
        temp = GAL.AdjList[stations[s_station][1]];
        while (temp->next != NULL) {
            temp = temp->next;
            if (temp->data != stations[s_station][0]) {
                P[temp->data] = stations[s_station][1];
                D[temp->data] = stations[s_station][3] + temp->weight < D[temp->data] ? stations[s_station][3] + temp->weight : D[temp->data];    //可能会跟上面的if有相同的相邻点
            }          
        }
    }
    //算法开始
    int k = stations[s_station][0]; //随便一个在Final中的点都行
    while (Final[v_end] == false) { //未找到终点则在下方break跳出循环
        int MIN = INT_MAX;
        int last = k;
        for (int i = 0; i < GAL.numV; i++) {
            if (Final[i] == false && D[i] < MIN && Margin[i] == false) {
                MIN = D[i];
                k = i;
            }
        }
        if (k == last) break;   //没有改变则跳出循环
        
        if (ele - MIN > comfor_ele) {   //安心范围内寻路
            Final[k] = true;
            temp = GAL.AdjList[k];
            while (temp->next != NULL) {    //松弛操作
                temp = temp->next;
                if (Final[temp->data] == false && temp->weight + D[k] < D[temp->data]) {
                    D[temp->data] = temp->weight + D[k];
                    P[temp->data] = k;
                }
            }
        }
        else if (ele - MIN <= comfor_ele && ele - MIN > 0) {    //开始找电桩
            Final[k] = true;
            Margin[k] = true;
        }
        else if (ele - MIN <= 0 ) {    //这里把条件写出来是为了逻辑更清晰
            Margin[P[k]] = true;
        }

    }
    if (Final[v_end] == true) { //找到终点的情况，直接返回
        arr.isArrive = true;
        arr.lenth = D[v_end];
        k = v_end;  //k本身就是v_end
        while (k != -1) {
            arr.road.push_back(k);
            k = P[k];
        }
        arr.road.push_back(-s_station);
        return arr;
    }
    //如果没有抵达终点，则找充电桩
    vector<bool> Available(stations.size(), false);
    //计算可用电桩
    for (int i = 0; i < GAL.numV; i++) {
        if (Margin[i] == true) {
            for (int j = 0; j < stations.size(); j++) { //在边缘点邻接段找电桩
                if (stations[j][0] == i && (stations[j][2] <= ele - D[i] || stations[j][1] == P[i]) || stations[j][1] == i && (stations[j][3] <= ele - D[i] || stations[j][0] == P[i])) {  //这里如果刚好到达电桩那也算吧。P[i]是父节点，如果是打这来的，可以先加油。
                    Available[j] = true;
                }
            }
        }
    }
    //充电后寻路
    vector<Arrive> Arr;
    arr.isArrive == false;
    Arr.push_back(arr); //这里要保证函数一定有返回值
    for (int i = 0; i < stations.size(); i++) {
        if (Available[i] == true) {
            Margin[stations[i][0]] == true ? Final[stations[i][0]] = false : Final[stations[i][1]] = false;
            arr = EleRecursion(GAL, stations, i, v_end, FullEle, ComforEle, Final); //充满电后寻路
            Margin[stations[i][0]] == true ? Final[stations[i][0]] = true : Final[stations[i][1]] = true;   //恢复Fianl
            //若寻路后找到终点
            if (arr.isArrive == true) {
                //加上该电桩到起始电桩的距离
                int len1 = D[stations[i][0]] == INT_MAX ? D[stations[i][0]] : D[stations[i][0]] + stations[i][2];
                int len2 = D[stations[i][1]] == INT_MAX ? D[stations[i][1]] : D[stations[i][1]] + stations[i][3];
                arr.lenth += len1 < len2 ? len1 : len2;
                //把路径加进去。
                int point = len1 < len2 ? stations[i][0] : stations[i][1];
                while (point != -1) {
                    arr.road.push_back(point);
                    point = P[point];
                }
                arr.road.push_back(-s_station); //电桩在路径中用负数表示
                Arr.push_back(arr);
            }
        }
    }

    int MIN = INT_MAX;
    k = 0;
    for (int i = 0; i < Arr.size(); i++) {  //拿出最短距离的路径
        if (Arr[i].isArrive == true) {
            if (Arr[i].lenth < MIN) {
                k = i;
                MIN = Arr[i].lenth;
            }
        }
    }
    return Arr[k];
}

//带电桩寻路
void EleNetworkExpansion(GraphAdjList& GAL, vector<vector<int>> stations, int v_start, int v_end, int ele, int comfor_ele) {
    vector<int> D(GAL.numV, INT_MAX);   //这个空间或许是不可接受的，因为充一次电，就可能出现好几个分支，都需要该空间
    vector<int> P(GAL.numV, -1);
    vector<bool> Final(GAL.numV, false);
    vector<bool> Margin(GAL.numV, false);   //电量边缘点，即需要找电桩的点。电桩只会在相邻段找，不会跨段找电桩。边缘点是不安心范围内的点和不可达点的上一个点，属于可达点。
    //初始化
    D[v_start] = 0; Final[v_start] = true;    //要保证递归后不能走以前的路
    VertexNode* temp = GAL.AdjList[v_start];
    while (temp->next != NULL) {
        temp = temp->next;
        P[temp->data] = v_start;
        D[temp->data] = temp->weight;
     }
    //算法开始
    int k = v_start;
    while (Final[v_end] == false) {  //没电了就结束，在下方的break处出去
        int MIN = INT_MAX;
        int last = k;
        for (int i = 0; i < GAL.numV; i++) {    //选出最近的点
            if (Final[i] == false && D[i] < MIN && Margin[i] == false) {    //D[i] < MIN 表示该点目前联通，不安心点不会再扩展。Margin是防止每次循环都在同一个边缘点。
                MIN = D[i];
                k = i;
            }
        }
        if (k == last) break;   //如果该电量范围已经找完，则退出，在每个边缘点上找电桩。
        /*
        //计划放到后边去
        if (comfor_ele >= ele - MIN) {    //如果当前电量低于安心充电范围，则在k附近找充电桩，然后重开一个路径扩张
            for (int i = 0; i < stations.size(); i++) {
                if (stations[i][0] == k && stations[i][1] == last || stations[i][0] == last && stations[i][1] == k) {   //来路上有充电桩，先充电

                }
                else if (stations[i][0] == k && stations[i][2] <= ele - MIN || stations[i][1] == k && stations[i][3] <= ele - MIN) {    //前往充电桩

                }
            }
        }
        */
        if (ele - MIN > comfor_ele) {   //仍在安心电量范围内寻路。
            Final[k] = true;
            temp = GAL.AdjList[k];
            while (temp->next != NULL) {    //松弛操作
                temp = temp->next;
                if (Final[temp->data] == false && temp->weight + D[k] < D[temp->data]) {
                    D[temp->data] = temp->weight + D[k];
                    P[temp->data] = k;
                }
            }
        }
        else if (ele - MIN <= comfor_ele && ele - MIN > 0) {
            Final[k] = true;
            Margin[k] = true; //如果该点是不安心的，则找电桩。
        }
        else if (ele - MIN <= 0) Margin[P[k]] = true;   //如果该点不可达，则在上一个点找电桩。
    }
    if (Final[v_end] == true) { //如果找到了可以直接退出
        int a = v_end;
        while (a != -1) {
            cout << a << "  ";
            a = P[a];
        }
        return;
    }
    /*for (int i = 0; i < GAL.numV; i++) {
        cout << P[i] << "  ";
    }
    cout << endl;
    for (int i = 0; i < GAL.numV; i++) {
        cout << Final[i] << "  ";
    }
    cout << endl;
    for (int i = 0; i < GAL.numV; i++) {
        cout << Margin[i] << "  ";
    }
    cout << endl;*/
    //如何才能考虑完全，不可能沿着走过的路径寻路。所以只开放边缘点的邻接点，好让车往回走。如果都开放了，车会来回开，递归就不会终止。
    //这里先考虑走过的点都不走，直接以电桩为起始点。
    vector<bool> Available(stations.size(), false);
    //计算可用电桩
    for (int i = 0; i < GAL.numV; i++) {    
        if (Margin[i] == true) {
            for (int j = 0; j < stations.size(); j++) { //在边缘点邻接段找电桩
                if (stations[j][0] == i && (stations[j][2] <= ele - D[i] || stations[j][1] == P[i]) || stations[j][1] == i && (stations[j][3] <= ele - D[i] || stations[j][0] == P[i])) {  //这里如果刚好到达电桩那也算吧。P[i]是父节点，如果是打这来的，可以先加油。
                    Available[j] = true;
                }
            }
        }
    }
    //充电后寻路
    vector<Arrive> Arr;
    Arrive arr;
    for (int i = 0; i < stations.size(); i++) {
        if (Available[i] == true) {
            //要区分这个电桩是否已经路过，走过就只设一个出口，反之就两个出口。Final方面代码一样。不同在于走的路径长不同。Final为false表示开放出口
            Margin[stations[i][0]] == true ? Final[stations[i][0]] = false : Final[stations[i][1]] = false;
            arr = EleRecursion(GAL, stations, i, v_end, FullEle, ComforEle, Final); //充满电后寻路
            Margin[stations[i][0]] == true ? Final[stations[i][0]] = true : Final[stations[i][1]] = true;   //恢复Fianl
            //若寻路后找到终点
            if (arr.isArrive == true) {
                //加上该电桩到起点的距离
                int len1 = D[stations[i][0]] == INT_MAX ? D[stations[i][0]] : D[stations[i][0]] + stations[i][2];
                int len2 = D[stations[i][1]] == INT_MAX ? D[stations[i][1]] : D[stations[i][1]] + stations[i][3];
                arr.lenth += len1 < len2 ? len1 : len2;
                //把路径加进去。
                int point = len1 < len2 ? stations[i][0] : stations[i][1];
                while (point != -1) {
                    arr.road.push_back(point);
                    point = P[point];
                }
                Arr.push_back(arr);
            }
            
        }
    }

    int MIN = INT_MAX;
    k = 0;
    for (int i = 0; i < Arr.size(); i++) {  //拿出最短距离的路径
        if (Arr[i].isArrive == true) {
            if (Arr[i].lenth < MIN) {
                k = i;
                MIN = Arr[i].lenth;
            }
        }
    }
    if (MIN == INT_MAX) cout << "can not arrive";
    else {
        for (int i = 0; i < Arr[k].road.size(); i++) {  //输出路径
            cout << Arr[k].road[i] << "  ";
        }
    }

}

int main()
{
    GraphAdjList GAL;
    CreatGraph(GAL);    //默认是连通图

    /*
    // Dijkstra
    // D：最短距离  P：路径中的上一个节点
    int v0 = 0; //起始点
    vector<int> D(GAL.numV, INT_MAX);
    vector<int> P(GAL.numV, -1);
    Dijkstra(GAL, v0, D, P);
    for (int i = 0; i < GAL.numV; i++) cout << D[i] << " ";
    cout << endl;
    for (int i = 0; i < GAL.numV; i++) cout << P[i] << "   ";
    cout << endl;
    int j = 16；
    cout << "V16";
    while (P[j] != j) {
        cout << "->V" << P[j];
        j = P[j];
    }

    */

    //network expansion
    //NetworkExpansion(GAL, 0, 16);

    //标注电站
    vector<vector<int>> stations(8, vector<int>(4));
    StationCase(stations);

    //network expansion with power stations

    EleNetworkExpansion(GAL, stations, 0, 16, 400, 100);


    return 0;
}

