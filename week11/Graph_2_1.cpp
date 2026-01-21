#include "Graph.h"

//测试DFS_AdjMtx

//先序遍历建立生成树的左子女-右兄弟链表
void printTree(Node<int>* r) {
    if (r != NULL) {
        cout << r->vertex << " ";
        printTree(r->lchild);
        printTree(r->rchild);
    }
}

int main() {
    Graphmtx<int> G;
    G.buildGraph(); // 输入图的顶点和边信息:
    //顶点：0 1 2 3 4 
    //边：0 1 1 0 2 1 1 2 1 3 4 1 1 4 3
    G.printGraph();
    Forest<int> F;
    DFS_AdjMtx(G, F);
    cout << "生成森林的树的数目为：" << F.numTrees << endl;
    cout << "各生成树的左子女-右兄弟链表表示如下：" << endl;
    Node<int>* temp = F.roots;
    printTree(temp);
    return 0;
}

//测试图：
/*
          0
         / \
        1---2
         
          3
           \
            4
*/
/*
0 1 1
0 2 1
1 2 1
3 4 1

*/