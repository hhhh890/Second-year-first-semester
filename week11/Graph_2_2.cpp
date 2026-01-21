#include "Graphlnk.h"

int main(){
    Graphlnk<char> G;
    G.buildGraph(); // 输入图的顶点和边信息: 5 5
    //顶点：a b c d e
    //边：a b 1 a c 1 b c 1 b d 4 b e 1
    G.printGraph();
    cout<<"删除边a-c和d-e后："<<endl;
    int v1=G.getVertexPos('a');
    int v2=G.getVertexPos('c');
    G.RemoveEdge(v1,v2);
    G.printGraph();
    v1=G.getVertexPos('d');
    v2=G.getVertexPos('e');
    G.RemoveEdge(v1,v2);
    G.printGraph();
    return 0;
}