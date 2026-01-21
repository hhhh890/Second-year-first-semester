#include<vector>
#include<map>
#include<iostream>
#include<queue>
#include<algorithm>
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
    double migration_cost; // 该任务的迁移成本（最短路径成本×GPU数）   
    Task(int id = 0, int ori_node = 0, int g = 0) 
        : id(id), original_node_id(ori_node), final_node_id(-1), gpu(g),migration_cost(0.0) {}

    /* data */
};


/*接下来输入M行，每行描述一条网络连接（无向链路）：
 u v path_cost bandwidth*/
class Net
{
private:
    int nodeNum,linkNum,taskNum;
    double cost_all;
    vector<Node> nodes;
    vector<Task> tasks;
    map<pair<int,int>,pair<double,double>> connect;
    int findNodeIndexById(int node_id) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].id == node_id) return i;
        }
        return -1;  // 未找到返回-1
    }
    //高级任务：用最短路径算法dijkstra来求优化
    map<int, double> dijkstra(int start_node_id){
        map<int, double> dist;
        for (auto& node:nodes)
        {
            dist[node.id] = INT_MAX;
        }
        dist[start_node_id] = 0.0;
        priority_queue<pair<double,int>,vector<pair<double,int>>,greater<> > pq;
        pq.push({0.0,start_node_id});
        while (!pq.empty())
        {
            pair<double,int> Pair = pq.top();
            double curr_dist = Pair.first;
            int curr_id = Pair.second;
            pq.pop();
            if (curr_dist>dist[curr_id])
            {
                continue;
                /* code */
            }
            for (auto& node : nodes)
            {
                int next_id = node.id;
                if (curr_id == next_id || !isConnect(curr_id,next_id))
                {
                    continue;
                }
                int u = curr_id , v = next_id;
                if (u>v)
                {
                    int temp = u;
                    u = v;
                    v = temp;
                }
                double cost = connect[{u,v}].first;
                if (dist[next_id] > curr_dist+cost)
                {
                    dist[next_id] = curr_dist + cost;
                    pq.push({dist[next_id],next_id});
                    /* code */
                }
            }
        }
        return dist;
        
    }
public:
    Net():nodeNum(0),linkNum(0),taskNum(0),cost_all(0.0){}
    Net(int nn,int ln,int tn):nodeNum(nn),linkNum(ln),taskNum(tn),cost_all(0.0){
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
    // 核心：任务分配
    bool assign() {
    // 步骤1：按GPU需求降序排序任务（先分配大任务）
        sort(tasks.begin(), tasks.end(), [](Task& a, Task& b) {
            return a.gpu > b.gpu;
        });

        for (auto& task : tasks) {
            int ori = task.original_node_id;
            vector<int> candidates = BFS(ori);
            if (candidates.empty()) return false;

            map<int, double> shortest = dijkstra(ori);
            vector<pair<double, int>> valid; // (成本, 节点ID)

            // 筛选可用节点
            for (int node_id : candidates) {
                int idx = findNodeIndexById(node_id);
                int remain = nodes[idx].can_use_gpu - nodes[idx].used_gpu;
                if (remain >= task.gpu) {
                    double cost = shortest[node_id] * task.gpu;
                    valid.emplace_back(cost, node_id);
                }
            }

            if (valid.empty()) return false;
            sort(valid.begin(), valid.end(), [](pair<double, int>& a, pair<double, int>& b) {
                if (a.first != b.first) return a.first < b.first;
                return a.second < b.second;
            });

            //选成本最低的节点
            int best_node = valid[0].second;
            int idx = findNodeIndexById(best_node);
            task.final_node_id = best_node;
            task.migration_cost = shortest[best_node] * task.gpu;
            nodes[idx].used_gpu += task.gpu;
        }
        return true;
    }

    // 打印任务分配结果和节点GPU使用情况
    void printAssign(){
        /*任务最终分配结果（含迁移成本）输出 T 行，第 i 行对应任务 i 的最终信息，格式为：
        task_id start_node end_node migration_cost
        各节点最终负载,同基础要求，输出 N 行：
        node_id final_load
        总迁移成本,最后输出 1 行，为所有任务的总迁移成本：
         total_migration_cost
        */
        cout<<"任务最终分配结果:"<<endl;
        cout<<"任务id \t 初始节点 \t 最终节点 \t 迁移成本"<<endl;
        for (const auto& task : tasks) {
            cout << task.id << " \t " << task.original_node_id << " \t "
                 << task.final_node_id<< " \t " << task.migration_cost << endl;

            cost_all+=task.migration_cost;
        }
        cout<<"节点GPU使用情况:"<<endl;
        cout<<"节点ID \t 已用GPU数"<<endl;
        for (const auto& nd:nodes)
        {
            cout<< nd.id<<" \t "<<nd.used_gpu<<endl;
            /* code */
        }
        cout<<"总迁移成本"<<endl;
        cout<<cost_all<<endl;
        
    }
    // 获取任务/节点列表（外部验证用）
    const vector<Task>& getTasks() const { return tasks; }
    const vector<Node>& getNodes() const { return nodes; }


};





