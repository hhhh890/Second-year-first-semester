#include "high_Net.h"
#include <iostream>
using namespace std;

int main(){
int n,m,t;
    cin>>n>>m>>t;

    Net mynet(n,m,t);
    for (int i = 0; i < n; i++)
    {
        int node_id,capacity;
        cin>>node_id>>capacity;
        mynet.buildNode(node_id,capacity);
        /* code */
    }
    for (int i = 0; i < m; i++)
    {
        int u,v;
        double pathcost,bandwidth;
        cin>>u>>v>>pathcost>>bandwidth;
        mynet.buildConnect(u,v,pathcost,bandwidth);
        /* code */
    }
    for (int i = 0; i < t; i++)
    {
        int task_id,start_node,demand;
        cin>>task_id>>start_node>>demand;
        mynet.addTask(task_id,start_node,demand);
        /* code */
    }
    mynet.assign();
    mynet.printAssign();
    return 0;
}