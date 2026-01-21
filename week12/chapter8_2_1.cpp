#include<iostream>
#include<vector>
using namespace std;

vector<vector<int>> f;
vector<vector<int>> path;
void floyd(const vector<vector<pair<int, int>>>& adj, int n){
    const int INF = INT_MAX/2;
    f.resize(n, vector<int>(n, INF));
    path.resize(n, vector<int>(n, -1));
    for(int i=0;i<n;i++){
        f[i][i]=0;
    }
    for(int u=0;u<n;u++){
        for(const auto& edge: adj[u]){
            int v = edge.first;
            int w = edge.second;
            f[u][v] = w;
            path[u][v] = u;
        }
    }
    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (f[i][j] > f[i][k]+f[k][j])
                {
                    f[i][j] = f[i][k]+f[k][j];
                    path[i][j] = path[k][j];
                }
            }
            /* code */
        }
        
        /* code */
    }
}

int main(){
    int n,m;
    cout<<"请输入小区数和道路数："<<endl;
    cin >> n>>m;//n个小区
    /*示例：
    3 3
    0 1 4
    1 2 5
    0 2 7
    */
    vector<vector<pair<int, int>>> adj(n);//邻接表存储图
    cout<<"请输入每条道路的起点 终点 和长度：(起点和终点范围在0到"<<n-1<<")"<<endl;
    for(int i=0;i<m;i++){
        int u,v,w;
        cin>>u>>v>>w;
        adj[u].emplace_back(v,w);
        adj[v].emplace_back(u,w);//无向图
    }
    f.clear();
    path.clear();
    floyd(adj,n);
    // 寻找最中心的小区
    int center = -1;
    int minDist = INT_MAX;
    for(int i=0;i<n;i++){
        int maxDist = 0;
        for(int j=0;j<n;j++){
            if(f[i][j]<INT_MAX/2){
                maxDist = max(maxDist, f[i][j]);
            }
        }
        if(maxDist < minDist){
            minDist = maxDist;
            center = i;
        }
    }
    cout<<"最中心的小区是："<<center<<"，到其他小区的最大距离是："<<minDist<<endl;
    return 0;

}