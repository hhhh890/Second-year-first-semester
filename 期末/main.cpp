#include<vector>
#include<map>
#include<iostream>
#include<queue>
#include<algorithm>
#include<climits>
#include<tuple>
using namespace std;

struct Node
{
    int id;
    int can_use_gpu;
    int used_gpu;
    Node(int id=0,int can=0,int used=0):id(id),can_use_gpu(can),used_gpu(used){}
};

struct Task
{
    int id;
    int original_node_id;
    int final_node_id;
    int gpu;            
    double migration_cost;
    bool is_migrated;
    vector<int> path_nodes;
    Task(int id = 0, int ori_node = 0, int g = 0) 
        : id(id), original_node_id(ori_node), final_node_id(-1), gpu(g),migration_cost(0.0), is_migrated(false) {}
};

class Net
{
private:
    int nodeNum,linkNum,taskNum,total_times;
    double cost_all;
    vector<Node> nodes;
    vector<Task> tasks;
    map<pair<int,int>,pair<double,double>> connect; // 存储节点ID对的链路信息
    vector<vector<double>> f; // 最短路径成本（索引）
    vector<vector<int>> prev; // 前驱节点（索引）
    vector<vector<tuple<int,int,int,int>>> time_plans; // 时间计划
    map<pair<int, int>, vector<int>> linkBandwidthUsage; // 链路带宽使用情况

    int findNodeIndexById(int node_id) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].id == node_id) return i;
        }
        return -1;
    }

    void floyd(){
        int n = nodes.size();
        const double INF = 1e18;
        f.assign(n,vector<double>(n,INF));
        prev.assign(n,vector<int>(n,-1));
        
        for (int i = 0; i < n; i++) {
            f[i][i] = 0;
            prev[i][i] = i;
        }

        for (auto& [link,costs] : connect) {
            int u_id = link.first;
            int v_id = link.second;
            int u_idx = findNodeIndexById(u_id);
            int v_idx = findNodeIndexById(v_id);
            if (u_idx == -1 || v_idx == -1) {
                cerr << "警告：链路节点不存在 " << u_id << "-" << v_id << endl;
                continue;
            }
            double cost = costs.first;
            if (cost < f[u_idx][v_idx]) {
                f[u_idx][v_idx] = cost;
                f[v_idx][u_idx] = cost;
                prev[u_idx][v_idx] = u_idx;
                prev[v_idx][u_idx] = v_idx;
            }
        }
        
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                if (f[i][k] == INF) continue;
                for (int j = 0; j < n; j++) {
                    if (f[i][j] > f[i][k] + f[k][j]) {
                        f[i][j] = f[i][k] + f[k][j];
                        prev[i][j] = prev[k][j];
                    }
                }
            }
        }
    }

    void getShortestPath(int u_idx, int v_idx, vector<int>& path) {
        path.clear();
        if (prev[u_idx][v_idx] == -1) return;
        int curr = v_idx;
        while (curr != u_idx) {
            path.push_back(nodes[curr].id); // 存储节点ID而非索引
            curr = prev[u_idx][curr];
        }
        path.push_back(nodes[u_idx].id);
        reverse(path.begin(), path.end());
    }
    pair<vector<int>, map<int, int>> BFS(int start_node_id) {
        vector<int> connected;
        map<int, int> predecessor; // 记录前驱节点（key:当前节点ID, value:前驱节点ID）
        if (findNodeIndexById(start_node_id) == -1) {
            return {connected, predecessor};
        }

        map<int, bool> visited;
        for (const auto& node : nodes) {
            visited[node.id] = false;
            predecessor[node.id] = -1; // 初始化前驱为-1
        }

        queue<int> q;
        q.push(start_node_id);
        visited[start_node_id] = true;
        predecessor[start_node_id] = start_node_id; // 起始节点前驱为自身

        while (!q.empty()) {
            int node_id = q.front();
            q.pop();
            connected.push_back(node_id);

            // 遍历所有节点，寻找相邻节点
            for (const auto& node : nodes) {
                int next_id = node.id;
                if (node_id == next_id || visited[next_id]) continue;
                // 若两节点直接相连，则视为相邻节点
                if (isConnect(node_id, next_id)) {
                    visited[next_id] = true;
                    predecessor[next_id] = node_id; // 记录前驱
                    q.push(next_id);
                }
            }
        }
        return {connected, predecessor};
    }
public:
    Net():nodeNum(0),linkNum(0),taskNum(0),cost_all(0.0){}
    Net(int nn,int ln,int tn):nodeNum(nn),linkNum(ln),taskNum(tn),cost_all(0.0){}
    ~Net(){}

    void buildNode(int node_id ,int capacity){
        nodes.emplace_back(node_id, capacity, 0);
    }

    void buildConnect(int u,int v,double path_cost,double bandwidth){
        if (u == v) {
            cerr << "错误：链路两端不能为同一节点" << endl;
            return;
        }
        if (u > v) swap(u, v);
        connect[{u, v}] = {path_cost, bandwidth};
    }

    bool isConnect(int u,int v){
        if (u == v) return true;
        if (u > v) swap(u, v);
        return connect.count({u, v});
    }

    pair<double,double> linkInfo(int u,int v){
        if (u == v) return {0.0, 0.0};
        if (u > v) swap(u, v);
        auto it = connect.find({u, v});
        return it != connect.end() ? it->second : make_pair(-1.0, -1.0);
    }

    void addTask(int task_id, int ori_node_id, int gpu_need) {
        if (gpu_need <= 0) {
            cerr << "任务" << task_id << "GPU需求无效" << endl;
            return;
        }
        if (findNodeIndexById(ori_node_id) == -1) {
            cerr << "任务" << task_id << "原始节点不存在" << endl;
            return;
        }
        tasks.emplace_back(task_id, ori_node_id, gpu_need);
    }


    // 基础分配：用BFS，优先分配到原始节点，否则选第一个可用节点
    // 基础分配：用BFS，优先分配到原始节点，计算迁移成本
    void assignBasic() {
        for (auto& task : tasks) {
            bool assigned = false;
            // 执行BFS，获取连通节点和前驱映射
            auto [connected_nodes, predecessor] = BFS(task.original_node_id);

            for (auto& node_id : connected_nodes) {
                int node_idx = findNodeIndexById(node_id);
                Node& node = nodes[node_idx];
                int remaining = node.can_use_gpu - node.used_gpu;

                if (remaining >= task.gpu) {
                    // 分配节点
                    task.final_node_id = node.id;
                    node.used_gpu += task.gpu;
                    assigned = true;

                    // 计算迁移成本：若目标节点是原始节点，成本为0
                    if (node_id == task.original_node_id) {
                        task.migration_cost = 0.0;
                        task.is_migrated = false;
                    } else {
                        // 基于前驱重建路径，累加链路成本
                        vector<int> path;
                        int curr = node_id;
                        // 从目标节点回溯到原始节点
                        while (curr != task.original_node_id) {
                            path.push_back(curr);
                            curr = predecessor[curr];
                            if (curr == -1) break; // 路径异常（理论上不会发生，因BFS确保连通）
                        }
                        path.push_back(task.original_node_id);
                        reverse(path.begin(), path.end()); // 得到从原始节点到目标节点的路径

                        // 累加路径上所有链路的成本
                        double cost = 0.0;
                        for (size_t i = 0; i < path.size() - 1; ++i) {
                            int u = path[i];
                            int v = path[i + 1];
                            auto [link_cost, _] = linkInfo(u, v); // 获取链路成本
                            cost += link_cost;
                        }
                        task.migration_cost = cost*task.gpu;
                        task.is_migrated = true;
                        task.path_nodes = path; // 记录路径
                    }
                    break;
                }
            }

            if (!assigned) {
                cerr << "失败：任务" << task.id << "无足够GPU资源的节点！" << endl;
            }
        }
    }

    // 高级分配：选择迁移成本最低的可用节点
    void assignAdvanced() {
        floyd();
        for (auto& task : tasks) {
            int ori_id = task.original_node_id;
            int ori_idx = findNodeIndexById(ori_id);
            if (ori_idx == -1) continue;

            // 检查原始节点
            if (nodes[ori_idx].can_use_gpu - nodes[ori_idx].used_gpu >= task.gpu) {
                task.final_node_id = ori_id;
                task.is_migrated = false;
                nodes[ori_idx].used_gpu += task.gpu;
                continue;
            }

            // 寻找成本最低的可用节点
            double min_cost = INT_MAX/2;
            int target_idx = -1;
            for (size_t i = 0; i < nodes.size(); ++i) {
                if (i == (size_t)ori_idx) continue;
                if (nodes[i].can_use_gpu - nodes[i].used_gpu >= task.gpu) {
                    if (f[ori_idx][i] < min_cost) {
                        min_cost = f[ori_idx][i];
                        target_idx = i;
                    }
                }
            }

            if (target_idx == -1) {
                cerr << "任务" << task.id << "无可用节点" << endl;
                continue;
            }

            task.final_node_id = nodes[target_idx].id;
            task.is_migrated = true;
            task.migration_cost = min_cost*task.gpu;
            nodes[target_idx].used_gpu += task.gpu;
            getShortestPath(ori_idx, target_idx, task.path_nodes);
        }
    }

    // 附加分配：考虑带宽限制，规划迁移时间
    void assignAdditional() {
        floyd();
        linkBandwidthUsage.clear();
        time_plans.clear();

        for (auto& task : tasks) {
            int ori_id = task.original_node_id;
            int ori_idx = findNodeIndexById(ori_id);
            if (ori_idx == -1) continue;

            // 检查原始节点
            if (nodes[ori_idx].can_use_gpu - nodes[ori_idx].used_gpu >= task.gpu) {
                task.final_node_id = ori_id;
                task.is_migrated = false;
                nodes[ori_idx].used_gpu += task.gpu;
                continue;
            }

            // 选择成本最低的目标节点
            double min_cost = 1e18;
            int target_idx = -1;
            for (size_t i = 0; i < nodes.size(); ++i) {
                if (i == (size_t)ori_idx) continue;
                if (nodes[i].can_use_gpu - nodes[i].used_gpu >= task.gpu && f[ori_idx][i] < min_cost) {
                    min_cost = f[ori_idx][i];
                    target_idx = i;
                }
            }

            if (target_idx == -1) {
                cerr << "任务" << task.id << "无可用节点" << endl;
                continue;
            }

            // 获取迁移路径
            vector<int> path;
            getShortestPath(ori_idx, target_idx, path);
            if (path.size() < 2) {
                cerr << "任务" << task.id << "无有效路径" << endl;
                continue;
            }

            // 更新任务信息
            task.path_nodes = path;
            task.final_node_id = nodes[target_idx].id;
            task.is_migrated = true;
            task.migration_cost = min_cost*task.gpu;
            nodes[target_idx].used_gpu += task.gpu;

            // 收集路径中的链路（节点ID对）
            vector<pair<int, int>> path_links;
            for (size_t i = 0; i < path.size() - 1; ++i) {
                int u = path[i], v = path[i+1];
                if (u > v) swap(u, v);
                path_links.emplace_back(u, v);
            }

            // 寻找最早可用时间步
            int t = 0;
            while (true) {
                bool ok = true;
                for (auto& link : path_links) {
                    auto [cost, bw] = linkInfo(link.first, link.second);
                    if (bw <= 0) { ok = false; break; }

                    // 检查链路在t时刻的带宽使用
                    if (linkBandwidthUsage[link].size() <= (size_t)t) continue;
                    if (linkBandwidthUsage[link][t] + 1 > bw) {
                        ok = false;
                        break;
                    }
                }
                if (ok) break;
                t++;
            }

            // 更新链路带宽使用
            for (auto& link : path_links) {
                if (linkBandwidthUsage[link].size() <= (size_t)t) {
                    linkBandwidthUsage[link].resize(t + 1, 0.0);
                }
                linkBandwidthUsage[link][t] += 1;
            }

            // 记录时间计划
            if (t >= (int)time_plans.size()) {
                time_plans.resize(t + 1);
            }
            for (size_t i = 0; i < path.size() - 1; ++i) {
                time_plans[t].emplace_back(t, task.id, path[i], path[i+1]);
            }
        }
    }

    void printAssign(){
        //cout<<"任务分配结果:"<<endl;
        //cout<<"任务id \t 起始节点 \t 目标节点 \t 迁移成本"<<endl;
        cost_all = 0;
        for (const auto& task : tasks) {
            cout << task.id << " " << task.original_node_id << " "
                 << task.final_node_id << " " << task.migration_cost << endl;
            cost_all += task.migration_cost;
        }

        //cout<<"节点GPU使用情况:"<<endl;
        //cout<<"节点ID \t 已用GPU"<<endl;
        for (const auto& nd:nodes) {
            cout<< nd.id<<" "<<nd.used_gpu<<endl;
        }

        //cout<<"总迁移成本"<<endl;
        cout<<cost_all<<endl;

        //cout<<"迁移时间计划（时间步 任务ID 源节点 目标节点）:"<<endl;
        for(auto& times : time_plans) {
            for (auto& plan : times) {
                cout<<get<0>(plan)<<" "<<get<1>(plan)<<" "<<get<2>(plan)<<" "<<get<3>(plan)<<endl;
            }
        }
    }

    const vector<Task>& getTasks() const { return tasks; }
    const vector<Node>& getNodes() const { return nodes; }
};

int main(){
    int n,m,t;
    cin>>n>>m>>t;

    Net mynet(n,m,t);
    for (int i = 0; i < n; i++) {
        int node_id,capacity;
        cin>>node_id>>capacity;
        mynet.buildNode(node_id,capacity);
    }

    for (int i = 0; i < m; i++) {
        int u,v;
        double pathcost,bandwidth;
        cin>>u>>v>>pathcost>>bandwidth;
        mynet.buildConnect(u,v,pathcost,bandwidth);
    }

    for (int i = 0; i < t; i++) {
        int task_id,start_node,demand;
        cin>>task_id>>start_node>>demand;
        mynet.addTask(task_id,start_node,demand);
    }

    //cout << "请选择分配方式（1-基础分配 2-高级分配 3-附加分配）：";
    int choice = 3;
    //cin >> choice;
    switch(choice) {
        case 1: mynet.assignBasic(); break;
        case 2: mynet.assignAdvanced(); break;
        case 3: mynet.assignAdditional(); break;
        default: cerr << "默认使用基础分配" << endl; mynet.assignBasic();
    }

    mynet.printAssign();
    return 0;
}