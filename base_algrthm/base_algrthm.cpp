#include <iostream>
#include <vector>
#include<queue>
using namespace std;
#define MaxVertices 100 // 最多有 MaxVertices 个点
#define FullEle 360 // 充满电后能跑 FullEle km
#define ComforEle 60 // 电量在任何时刻都要能支撑跑完 ComforEle km
#define NumOfNode 17 // 有17个节点
#define NumOfEdge 18 // 有18条边
#define StartNode 0 // 起点
#define EndNode 16  // 终点


// 节点存储 (剩余可走，走了多远，充电次数，父节点)
struct NodeSave {
    int battery_level, distance_travelled, charge_num, pre_node;
    NodeSave* next = NULL;
    // 重载完全优势符号
    bool operator>(const NodeSave& other) const {
        // 如果我的剩余电量大过你，走的距离又短，充电次数又少，我对你就是绝对优势。我这里把相等的也干掉了，如果有对称路线，那我就考虑不到了。
        //return battery_level >= other.battery_level && distance_travelled <= other.distance_travelled && charge_num <= other.charge_num;
        if (battery_level >= other.battery_level && distance_travelled <= other.distance_travelled && charge_num <= other.charge_num) return true;
        else return false;
    }
};


// 全局变量声明，主要是充电次数和电站列表
int min_charge_num = INT_MAX;
struct ChargeStation {
    int index;  // 是哪个电站
    int charging_num;   // 是第几次充电，定义了太多名字了，我都定晕掉了
    NodeSave nsave; // 用于识别是哪个nodesave发出的充电请求
    // 小根堆，重载大于号
    bool operator>(const ChargeStation& other) const {
        return charging_num > other.charging_num;
    }
};
priority_queue<ChargeStation, vector<ChargeStation>, greater<ChargeStation>> stations_queue;

// 定义结点
struct VertexNode {
    int data;    //结点编号
    int weight = 0;    //指向下一个结点的边的权值
    VertexNode* next = NULL;
    NodeSave* head = NULL;  //会存在于邻接表的头结点，给AdjList每个头结点的head都整一个头结点
};

// 定义邻接表
struct GraphAdjList {
    VertexNode* AdjList[MaxVertices];    //存储所有结点
    int numV, numE;
};

// 实例图：起点 终点 距离。
void GraphCase(vector<vector<int>>& edges) {
    // 给出的边数要严格按照  NumOfEdge 个数
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

// 创建图 (目前的图是无向有权图，用邻接表实现)
void CreatGraph(GraphAdjList& G) {
    G.numV = NumOfNode;
    // 初始化邻接表，打头的节点都是自己，头节点方便头插法(虽然用的是尾插)
    for (int i = 0; i < G.numV; i++) {
        VertexNode* new_node = new VertexNode();
        new_node->data = i;
        new_node->next = NULL;
        new_node->head = new NodeSave();   // 初始化头结点方便后面添加
        G.AdjList[i] = new_node;
    }
    G.numE = NumOfEdge;
    // 边数组
    vector<vector<int>> edges(G.numE, vector<int>(3));
    GraphCase(edges);
    // 把边添加到邻接表
    for (int i = 0; i < G.numE; i++) {
        VertexNode* temp = G.AdjList[edges[i][0]];
        while (temp->next != NULL) temp = temp->next;   //尾插法
        VertexNode* newEdge = new VertexNode();
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
}

// 实例电站
void StationsCase(vector<bool>& stations) {
    vector<int> stationscase = { 2,3,5,10,12,13 };
    for (int i = 0; i < stationscase.size(); i++) {
        stations[stationscase[i]] = true;
    }
}

// 剪枝预处理，把死胡同剪掉
void PreCutGraph(GraphAdjList& G, int start, int end, vector<bool> stations) {
    bool flag = true;
    while(flag) {
        flag = false;
        for (int i = 0; i < G.numV; i++) {
            // 如果该节点度数为1，且不是起点终点或电站，则抹掉这个点
            if (G.AdjList[i]->next != NULL && G.AdjList[i]->next->next == NULL && stations[i] != true && i != end && i != start) {
                flag = true;
                // 删除自身
                int node = G.AdjList[i]->next->data;
                VertexNode* p = G.AdjList[i]->next;
                G.AdjList[i]->next = NULL;
                delete p;
                // 在邻点删除自己
                p = G.AdjList[node];
                while (p->next->data != i) p = p->next; // 按道理肯定会找到的
                VertexNode* q = p->next;
                p->next = p->next->next;
                delete q;
            }
        }
    }
}

// Dijkstra的比较结构
struct D_node {
    int node_seq;   // 该点编号
    int lenth;  // 距电站的距离
    int pre;    // 父节点

    // 构造函数
    D_node(const int node_seq, const int lenth, const int pre) :node_seq(node_seq), lenth(lenth), pre(pre) {};

    // 最小堆，重载大于号
    bool operator>(const D_node& other) const {
        return lenth > other.lenth;
    }
};

// 开始扩网
void NetworkExpansion(GraphAdjList& GAL, vector<bool>stations, int v_start, int v_end, int ele, int comfor_ele, NodeSave station_nodesave) {
    // 首先要看这个nodesave还在不在，因为充电是预充电，假如先来一个nodesave，然后报到电站队列，万一后来这个nodesave给淘汰了，就没必要再走这个扩网了


    // 初始化途中可充电电站队列。把电站的满电NodeSave揪出来，在AdjList的第一个节点的head里。Dijkstra优先队列，电站邻点入队。这里要把电站节点分开来，是因为以下遍历的时候每出一个点都要生成NodeSave，但电站节点是不需要的。
    // 这里的ele和comfor_ele都不可以更改。nodesave是预充的，直接拿来用。
    priority_queue<D_node,vector<D_node>, greater<D_node>> min_D;

    /*NodeSave station_nodesave;
    for (NodeSave* nodesave_p = GAL.AdjList[v_start]->head->next; nodesave_p != NULL; nodesave_p = nodesave_p->next)
        if (nodesave_p->battery_level == FullEle) station_nodesave = *nodesave_p;*/

    for (VertexNode* ver_p = GAL.AdjList[v_start]->next; ver_p != NULL; ver_p = ver_p->next) {  // AdjList有头结点
        if (ver_p->weight <= ele - comfor_ele) {
            min_D.push(D_node{ ver_p->data, ver_p->weight, v_start });
        }
    }
    
    // 开始Dijkstra遍历，每出队一个节点，计算生成对应的NodeSave，若不是完全劣势，就存入该点的NodeSave。可以入NodeSave的话，那邻点也入min_D，不怕以前遍历过，遍历过的话之后生成的NodeSave入不了。
    while (!min_D.empty()) {
        D_node cur_D_node = min_D.top();
        min_D.pop();
        NodeSave* cur_nodesave = new NodeSave{ ele - cur_D_node.lenth, cur_D_node.lenth + station_nodesave.distance_travelled, station_nodesave.charge_num, cur_D_node.pre, NULL };
        NodeSave* exist_nodesave_p = GAL.AdjList[cur_D_node.node_seq]->head->next;
        bool flag = true;
        while (exist_nodesave_p != NULL) {
            if (*exist_nodesave_p > *cur_nodesave) { // 若存在的对该nodesave有绝对优势，就淘汰这个nodesave
                flag = false;
                break;
            }
            exist_nodesave_p = exist_nodesave_p->next;
        }
        // 确定了要添加这个nodesave，先把原先的nodesave淘汰一轮再放进去。如果是终点，直接退出，否则添加该点的邻点入min_D。如果是电站的话，还要放进容器里，为了防止深度搜索，电站必须按照充电次数由低往高进行路网扩张，一旦找到终点后，放开一个充电次数去找，到这个上限后就终止。
        if (flag) {
            exist_nodesave_p = GAL.AdjList[cur_D_node.node_seq]->head->next;
            NodeSave* q = GAL.AdjList[cur_D_node.node_seq]->head;
            // 淘汰原先的nodesave
            while (exist_nodesave_p != NULL) {
                if (*cur_nodesave > *exist_nodesave_p) {
                    q->next = q->next->next;
                    delete exist_nodesave_p;
                    exist_nodesave_p = q->next;
                }
                else {
                    q = q->next;
                    exist_nodesave_p = exist_nodesave_p->next;
                }
            }
            // 头插法放进去
            cur_nodesave->next = GAL.AdjList[cur_D_node.node_seq]->head->next;
            GAL.AdjList[cur_D_node.node_seq]->head->next = cur_nodesave;
            // 如果是终点，直接return，这个电站扩网结束。把目前最小充电次数更改
            if (cur_D_node.node_seq == v_end) {
                if (cur_nodesave->charge_num < min_charge_num) min_charge_num = cur_nodesave->charge_num;
                return;
            }
            // 放邻点
            for (VertexNode* ver_p = GAL.AdjList[cur_D_node.node_seq]->next; ver_p != NULL; ver_p = ver_p->next) {  // AdjList有头结点
                if (ver_p->weight + cur_D_node.lenth <= ele - comfor_ele) {
                    min_D.push(D_node{ ver_p->data, ver_p->weight + cur_D_node.lenth, cur_D_node.node_seq });
                }
            }
            // 如果是电站就放入电站队列，把nodesave放进电站，这样好识别一点
            if (stations[cur_D_node.node_seq]) {
                // 头插法放进充电后的nodesave。这个是预充电，不一定会进行扩网
                NodeSave precharge = NodeSave{ ele, cur_nodesave->distance_travelled, cur_nodesave->charge_num + 1,cur_nodesave->pre_node, NULL };
                stations_queue.push(ChargeStation{ cur_D_node.node_seq, precharge.charge_num, precharge });
                NodeSave* charge_p = new NodeSave{ ele, cur_nodesave->distance_travelled, cur_nodesave->charge_num + 1,cur_nodesave->pre_node, NULL };
                charge_p->next = GAL.AdjList[cur_D_node.node_seq]->head->next;
                GAL.AdjList[cur_D_node.node_seq]->head->next = charge_p;
            }
        }
    }

}

int main() {

    GraphAdjList GAL;
    CreatGraph(GAL);    // 默认是连通图
    // 初始化电站，之所以没放一起，是因为之前的样例电站不在电上
    vector<bool> stations(GAL.numV, false);
    StationsCase(stations);
    // 起点 终点
    int v_start = StartNode;
    int v_end = EndNode;
    // 剪枝预处理
    PreCutGraph(GAL, v_start, v_end, stations);
    // 初始化
    GAL.AdjList[v_start]->head->next = new NodeSave{ FullEle, 0, 0, v_start, NULL };
    // 把起始点当成电站开始扩网
    int v_station = v_start;
    int cn = 0; // 用来表示目前的充电次数，太多名字了
    ChargeStation cs;
    NodeSave station_nodesave = NodeSave{ FullEle, 0, 0, v_start, NULL };
    while (cn - 1 <= min_charge_num) {  // 最多充多两次电到
        NetworkExpansion(GAL, stations, v_station, v_end, FullEle, ComforEle, station_nodesave);
        if (stations_queue.empty()) {
            if (GAL.AdjList[v_end]->head->next == NULL) {
                cout << "到不了" << endl;
                return 0;
            }
            break;
        }
        else {  // queue里存的都是预充电nodesave，打头的都是最少充电数.
            cs =  stations_queue.top();
            stations_queue.pop();
            cn = cs.charging_num;
            station_nodesave = cs.nsave;
            v_station = cs.index;
        }
    }

    // 输出路径，end有几个nodesave就有几条路径，每一个nodesave都要和父节点的nodesave一一对应，遇到电站的时候要在同站换nodesave
    for (NodeSave* p = GAL.AdjList[v_end]->head->next; p != NULL; p = p->next) {
        int cur = v_end;
        NodeSave* cur_p = p;
        NodeSave cur_ns = *cur_p;
        // 输出本nodesave的路径
        while (cur != v_start) {
            cout << cur << '-';
            cur = cur_p->pre_node;  // cur变父值
            if (cur == v_start) break;
            // 现在开始找上家，这个pre_ns是指针，标错了
            for (NodeSave* pre_ns = GAL.AdjList[cur]->head->next; pre_ns != NULL; pre_ns = pre_ns->next) {
                if (pre_ns->battery_level + pre_ns->distance_travelled == cur_ns.battery_level + cur_ns.distance_travelled && pre_ns->charge_num == cur_ns.charge_num) {
                    if (pre_ns->battery_level == FullEle) { // 这种情况要换nodesave
                        for (NodeSave* change_ns = GAL.AdjList[cur]->head->next; change_ns != NULL; change_ns = change_ns->next) {
                            if (change_ns->distance_travelled == pre_ns->distance_travelled && change_ns->charge_num == pre_ns->charge_num - 1 && change_ns->pre_node == pre_ns->pre_node && change_ns->battery_level != FullEle) {
                                pre_ns = change_ns;
                                break;
                            }
                        }
                    }
                    cur_ns = *pre_ns;
                    cur_p = pre_ns;
                    break;
                }
            }
        }
        cout << v_start << endl;
    }

        
    return 0;
}



