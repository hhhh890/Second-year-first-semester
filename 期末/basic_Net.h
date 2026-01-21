#include<vector>
#include<map>
#include<iostream>
#include<queue>
using namespace std;

/* 节点信息数据结构
记录服务器节点集合：包括节点编号、节点可用GPU数量和当前已使用的GPU
数量；
记录节点之间的网络连接：包括是否连通，以及在两节点之间迁移“单个GPU占用”所需要的成本
*/
struct Node
{
    int id;
    int can_use_gpu;
    int used_gpu;
    Node(int id=0,int can=0,int used=0):id(id),can_use_gpu(can),used_gpu(used){}
    /* data */
};
struct Task
{
    int id;
    int original_node_id;  // 初始所在节点
    int final_node_id;     // 最终分配节点
    int gpu;               
    Task(int id = 0, int ori_node = 0, int g = 0) 
        : id(id), original_node_id(ori_node), final_node_id(-1), gpu(g) {}

    /* data */
};


/*接下来输入M行，每行描述一条网络连接（无向链路）：
 u v path_cost bandwidth*/
class Net
{
private:
    int nodeNum,linkNum,taskNum;
    vector<Node> nodes;
    vector<Task> tasks;
    map<pair<int,int>,pair<double,double>> connect;
    int findNodeIndexById(int node_id) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].id == node_id) return i;
        }
        return -1;  // 未找到返回-1
    }
public:
    Net():nodeNum(0),linkNum(0),taskNum(0){}
    Net(int nn,int ln,int tn):nodeNum(nn),linkNum(ln),taskNum(tn){
    }
    ~Net(){}
    void buildNode(int node_id ,int capacity){
        /* node_id：节点编号（1 ? N）
        capacity：该节点的最大可用 GPU 总数（正整数)*/
        Node curr(node_id,capacity,0);
        nodes.push_back(curr);
    }
    void buildConnect(int u,int v,double path_cost,double bandwidth){
        /*接下来输入M行，每行描述一条网络连接（无向链路）：
         u v path_cost bandwidth
        ? u, v：相连的两个节点编号（1?N）
        ? path_cost：在这条链路上迁移 “单个GPU占用”所需的成本（用于高级要求中的迁移成本统计与优化）
        ? bandwidth：该链路的带宽容量（正整数），表示在同一时间单位内最多可通过的任务总量
        在基础要求中：只需要利用u和v建立图的连通关系；不要求使用path_cost 和bandwidth，读入之后不使用*/
        if (u == v) {
            cerr << "错误：链路两端不能为同一节点！" << endl;
            return;
        }
        if (u>v)
        {
            int temp = u;
            u=v;
            v=temp;
        }
        auto Pair = std::make_pair(u,v);
        auto Pair2 = std::make_pair(path_cost,bandwidth);
        connect[Pair] = Pair2;
    }
    bool isConnect(int u,int v){
        if (u == v) return true; 
        if (u>v)
        {
            int temp = u;
            u=v;
            v=temp;
        }
        auto Pair = std::make_pair(u,v);
        return connect.find(Pair)!=connect.end();
    }
    pair<double,double> linkInfo(int u,int v){
        if (u == v) return {0.0, 0.0}; 
        if (u>v)
        {
            int temp = u;
            u=v;
            v=temp;
        }
        auto it = connect.find(make_pair(u, v));
        return it != connect.end() ? it->second : make_pair(-1.0, -1.0);
    }
    // 添加任务
    void addTask(int task_id, int ori_node_id, int gpu_need) {
        if (gpu_need <= 0) {
            cerr << "错误：任务" << task_id << "GPU需求必须为正整数！" << endl;
            return;
        }
        if (findNodeIndexById(ori_node_id) == -1) {
            cerr << "错误：任务" << task_id << "初始节点" << ori_node_id << "不存在！" << endl;
            return;
        }
        tasks.emplace_back(task_id, ori_node_id, gpu_need);
    }
    // 更新节点初始已使用GPU数
    bool updateGPU(int id,int used){
        int i = findNodeIndexById(id);
        if (i==-1)
        {
            cerr<<"错误：id不存在"<<endl;
            return false;
            /* code */
        }
        if (used>nodes[i].can_use_gpu)
        {
            cout<<"used>nodes[i].can_use_gpu"<<endl;
            return false;
            /* code */
        }
        
        nodes[i].used_gpu = used;
        return true;
    }
    // 核心：任务分配
    vector<int> BFS(int start_node_id){
        vector<int> connected;  
        if (findNodeIndexById(start_node_id) == -1) return connected;
        map<int, bool> visited;
        for (const auto& node : nodes) {
            visited[node.id] = false;
        }

        queue<int> q;
        q.push(start_node_id);
        visited[start_node_id] = true;
        while (!q.empty())
        {
            int node_id = q.front();
            q.pop();
            connected.push_back(node_id);
            for (const auto& node : nodes)
            {
                int next_id = node.id;
                if (node_id == next_id || visited[next_id]) continue;
                if (isConnect(node_id, next_id)) {
                    visited[next_id] = true;
                    q.push(next_id);
                }
            }
            /* code */
        }
        return connected;
    }
    bool assign(){
        for (auto& task : tasks) {
            bool assigned = false;
            vector<int> con = BFS(task.original_node_id);
            for (auto& node_id : con) {
                Node& node = nodes[findNodeIndexById(node_id)];
                int remaining = node.can_use_gpu - node.used_gpu;
                if (remaining >= task.gpu) {
                    task.final_node_id = node.id;
                    node.used_gpu += task.gpu;
                    assigned = true;
                    break;
                }
            }
            if (!assigned) {
                cerr << "失败：任务" << task.id << "无足够GPU资源的节点！" << endl;
                return false;
            }
        }
        return true;
    }
    // 打印任务分配结果和节点GPU使用情况
    void printAssign(){
        cout<<"任务迁移情况:"<<endl;
        cout<<"任务id \t 初始节点 \t 最终节点"<<endl;
        for (const auto& task : tasks) {
            cout << task.id << " \t " << task.original_node_id << " \t "
                 << task.final_node_id << endl;
        }
        cout<<"节点GPU使用情况:"<<endl;
        cout<<"节点ID \t 已用GPU数"<<endl;
        for (const auto& nd:nodes)
        {
            cout<< nd.id<<" \t "<<nd.used_gpu<<endl;
            /* code */
        }
    }
    // 获取任务/节点列表（外部验证用）
    const vector<Task>& getTasks() const { return tasks; }
    const vector<Node>& getNodes() const { return nodes; }


};





