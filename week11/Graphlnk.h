/*
实现本PPT中的无向图的邻接表类（ class Graphlnk ）中的方法
 bool removeEdge ( int v1,  int v2 ) 
 删除连接i和j的边，这里i、j是顶点在顶点表中的下标。
 （该函数不要考虑各种错误处理，
 即假设函数的输入参数正确，
 程序中用到的各种数据也没有错误。
 程序中的Graphlnk类定义只要包含RemoveEdge(int i, int j)需要用到的属性和方法的定义即可）

*/
#include <iostream>
using namespace std;
template <class T> struct Edge {//边结点
    int dest;			//目标顶点下标	
    int cost;    	                    //边上的权值
    Edge<T> * link;        //下一边链接指针
    Edge ( ) { }                    //构造函数
    Edge ( int num, int weight ) : dest (num), cost (weight), link (nullptr) { } 
    bool operator != ( Edge<T>& R ) const  { return dest != R.dest; } 
} ;

template <class T> struct Vertex { 
    T data;    		  	//顶点数据
    Edge <T> *adj;        //边链表头指针
};

template <class T> class Graphlnk {
private:
    Vertex<T> * VexList;   //顶点表
    int numVertices;       //当前顶点数
    int maxVertices;       //最大顶点数
    int numEdges;          //当前边数
public:
    Graphlnk ( int maxV = 10 ): maxVertices(maxV), numVertices(0), numEdges(0), VexList(new Vertex<T>[maxV]) {
     }        //构造函数
    ~Graphlnk ( ) {
        for ( int i = 0; i < numVertices; i++ ) {
            Edge<T> * p = VexList[i].adj;
            while ( p != nullptr ) {
                Edge<T> * q = p;
                p = p->link;
                delete q;
            }
        }
        delete [] VexList;
    }                //析构函数
    bool RemoveEdge ( int v1, int v2 ) ; //删除边
    int getVertexPos ( T vertex ) {
        for ( int i = 0; i < numVertices; i++ ) {
            if ( VexList[i].data == vertex )
                return i;
        }
        return -1;
    }    //返回顶点在顶点表中的位置
    int getNumVertices ( ) const { return numVertices; }
    void buildGraph ( ) {
        cout << "请输入顶点和边的个数：" << endl;
        cin >>  numVertices >> numEdges;
        cout << "请输入各顶点信息：" << endl;
        for ( int i = 0; i < numVertices; i++ ) {
            cin >> VexList[i].data;
            VexList[i].adj = nullptr; 
        }
        cout << "请输入各边信息（格式：vi vj weight）：" << endl;
        for ( int k = 0; k < numEdges; k++ ) {
            T v1, v2; int weight;
            cin >> v1 >> v2 >> weight;
            int i = getVertexPos(v1);
            int j = getVertexPos(v2);
            Edge<T> * e1 = new Edge<T>(j, weight);
            e1->link = VexList[i].adj;
            VexList[i].adj = e1;
            Edge<T> * e2 = new Edge<T>(i, weight);
            e2->link = VexList[j].adj;
            VexList[j].adj = e2;
            cout << "已添加边：" << v1 << " - " << v2 << " 权值：" << weight << endl;
        }
    } //建立图
    void printGraph ( ) {
        cout << "图的邻接表表示如下：" << endl;
        for ( int i = 0; i < numVertices; i++ ) {
            cout << VexList[i].data << " : ";
            Edge<T> * p = VexList[i].adj;
            while ( p != nullptr ) {
                cout << " -> " << VexList[p->dest].data << "(" << p->cost << ")";
                p = p->link;
            }
            cout << endl;
        }
    } //打印图

};

template <class T>
bool Graphlnk<T>::RemoveEdge ( int v1, int v2 ) {
    if (v1>numVertices||v1<0||v2>numVertices||v2<0)
    {
        return false;
        /* code */
    }
    Edge<T>* p=VexList[v1].adj;
    Edge<T>* prev=nullptr;
    while (p!=nullptr)
    {
        if(p->dest==v2)
        {
            if(prev==nullptr)
            {
                VexList[v1].adj=p->link;
                delete p;
                break;
            }
            else
            {
                prev->link=p->link;
                delete p;
                break;
            }
        }
        prev=p;
        p=p->link;
    }
    p=VexList[v2].adj;
    prev=nullptr;
    while (p!=nullptr)
    {
        if(p->dest==v1)
        {
            if(prev==nullptr)
            {
                VexList[v2].adj=p->link;
                delete p;
                break;
            }
            else
            {
                prev->link=p->link;
                delete p;
                break;
            }
        }
        prev=p;
        p=p->link;
    }
}