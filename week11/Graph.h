#include <iostream>
#include <stdexcept>
using namespace std;

// 核心修正1：友元模板参数改用U/V，避免与类的T/E重名
template <class T>
class Graphmtx {

private:
    T* VerticesList;
    int** Edge;
    const int maxWeight; // 声明顺序1：maxWeight
    int numVertices;   // 声明顺序2：numVertices
    int maxVertices;   // 声明顺序3：maxVertices
    int numEdges;      // 声明顺序4：numEdges

    int getVertexPos(T vertex) {
        for (int i = 0; i < numVertices; i++)
            if (VerticesList[i] == vertex) return i;
        return -1;
    }

public:
    // 核心修正2：初始化列表顺序匹配成员声明顺序（消除-Wreorder警告）
    Graphmtx(int sz = 10) 
        : maxWeight(65535),  // 1. 先初始化const成员maxWeight
          numVertices(0),    // 2. 再初始化numVertices
          maxVertices(sz),   // 3. 然后maxVertices
          numEdges(0)        // 4. 最后numEdges
    {
        Edge = (int**)new int * [maxVertices];
        VerticesList = new T[maxVertices];

        for (int i = 0; i < sz; i++) {
            Edge[i] = new int[maxVertices];
        }

        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                Edge[i][j] = (i == j) ? 0 : maxWeight;
            }
        }
    }

    ~Graphmtx() {
        delete[] VerticesList;
        for (int i = 0; i < maxVertices; i++) {
            delete[] Edge[i];
        }
        delete[] Edge;
    }

    T getValue(int i) {
        if (i >= 0 && i < numVertices) {
            return VerticesList[i];
        }
        throw out_of_range("Vertex index out of bounds");
    }

    int getWeight(int v1, int v2) {
        return (v1 != -1 && v2 != -1) ? Edge[v1][v2] : 0;
    }

    int getFirstNeighbor(int v);
    int getNextNeighbor(int v, int w);
    bool insertVertex(const T vertex);
    bool insertEdge(int v1, int v2, int cost);
    bool removeVertex(int v) { return false; }
    bool removeEdge(int v1, int v2) { return false; }
    int getNumVertices() const {
        return numVertices;
    }
    void buildGraph(){
        cout << "请输入顶点和边个数" << endl;
        cin>> numVertices >> numEdges;
        cout << "请输入各顶点信息" << endl;
        for(int i=0;i<numVertices;i++){
            cin>> VerticesList[i];
        }
        cout << "请输入各边信息" << endl;
        for(int k=0;k<numEdges;k++){
            T v1,v2;
            int weight;
            cin>> v1 >> v2 >> weight;
            int i=getVertexPos(v1);
            int j=getVertexPos(v2);
            Edge[i][j]=weight;
            cout<<"边："<<v1<<"-"<<v2<<" 权值："<<weight<<endl;
        }
        
    }
    void printGraph(){
        cout << "图的邻接矩阵表示如下：" << endl;
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                if (Edge[i][j] == maxWeight)
                    cout << "∞" << "\t";
                else
                    cout << Edge[i][j] << "\t";
            }
            cout << endl;
        }
    }
};



template <class T>
int Graphmtx<T>::getFirstNeighbor(int v) {
    if (v < 0 || v >= numVertices) return -1;
    for (int i = 0; i < numVertices; i++)
        if (Edge[v][i] != maxWeight && Edge[v][i] != 0) return i;
    return -1;
}

template <class T>
int Graphmtx<T>::getNextNeighbor(int v, int w) {
    if (v < 0 || v >= numVertices || w < 0 || w >= numVertices) return -1;
    for (int i = w + 1; i < numVertices; i++)
        if (Edge[v][i] != maxWeight && Edge[v][i] != 0) return i;
    return -1;
}

template <class T>
bool Graphmtx<T>::insertVertex(const T vertex) {
    if (numVertices == maxVertices) return false;
    VerticesList[numVertices++] = vertex;
    return true;
}

template <class T>
bool Graphmtx<T>::insertEdge(int v1, int v2, int cost) {
    if (v1 < 0 || v1 >= numVertices || v2 < 0 || v2 >= numVertices) return false;
    Edge[v1][v2] = cost;
    numEdges++;
    return true;
}

// 左子女-右兄弟链表节点
template <class T>
struct Node {
    T vertex;
    Node* lchild;
    Node* rchild;
    Node(T v) : vertex(v), lchild(nullptr), rchild(nullptr) {}
};

// 生成森林结构
template <class T>
struct Forest {
    Node<T>* roots;
    int numTrees;

    void destroy(Node<T>* r) {
        if (r != nullptr) {
            destroy(r->lchild);
            destroy(r->rchild);
            delete r;
        }
    }

    Forest() : roots(nullptr), numTrees(0) {}
    ~Forest() { destroy(roots); }
};

// DFS递归函数
template <class T>
Node<T>* dfs(Graphmtx<T>& G, int v, bool visited[]) {
    visited[v] = true;
    Node<T>* currNode = new Node<T>(G.getValue(v));

    Node<T>* lastChild = nullptr;
    for (int w = G.getFirstNeighbor(v); w >= 0; w = G.getNextNeighbor(v, w)) {
        if (!visited[w]) {
            Node<T>* childNode = dfs(G, w, visited);
            if (currNode->lchild == nullptr) {
                currNode->lchild = childNode;
                lastChild = childNode;
            } else {
                lastChild->rchild = childNode;
                lastChild = childNode;
            }
        }
    }
    return currNode;
}

// DFS生成森林（仅2个参数）
template <class T>
void DFS_AdjMtx(Graphmtx<T>& G, Forest<T>& F) {
    bool* visited = new bool[G.getNumVertices()];
    for (int i = 0; i < G.getNumVertices(); i++)
        visited[i] = false;

    Node<T>* lastRoot = nullptr;
    for (int i = 0; i < G.getNumVertices(); i++) {
        if (!visited[i]) {
            Node<T>* newRoot = dfs(G, i, visited);
            F.numTrees++;

            if (F.roots == nullptr) {
                F.roots = newRoot;
                lastRoot = newRoot;
            } else {
                lastRoot->rchild = newRoot;
                lastRoot = newRoot;
            }
        }
    }

    delete[] visited;
}

