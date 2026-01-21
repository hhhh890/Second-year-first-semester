#include"BinNode.h"
#include<iostream>
using namespace std;

void buildtree(BinNode<int>& head){
    BinNode<int>* nownode = &head;
    for (int i = 1; i < 10; i++)
    {
        BinNode<int>* lch = new BinNode<int>{2*i+1,nullptr,nullptr};
        BinNode<int>* rch = new BinNode<int>{2*i,nullptr,nullptr};
        nownode->lchild = lch;
        nownode->rchild = rch;
        nownode = lch;
        /* code */
    }
    
}
void printtree(BinNode<int>* tree){
    if (tree == nullptr)
    {
        return;
        /* code */
    }
    cout<<tree->data<<" ";
    printtree(tree->lchild);
    printtree(tree->rchild);
    
}

void freetree(BinNode<int>* tree){
    if (tree==nullptr)
    {
        return;
        /* code */
    }
    if (tree->lchild==nullptr&&tree->rchild == nullptr)
    {
        delete tree;
        return;
        /* code */
    }
    freetree(tree->lchild);
    freetree(tree->rchild);
    delete tree;
    return;
    
}

int main(){
    BinNode<int> mytree={1,nullptr,nullptr};
    buildtree(mytree);
    printtree(&mytree);
    BinNode<int>* fathernode = getLCA(&mytree,17,12);
    cout<<"getLCA: "<<fathernode->data<<endl;
    int maxdistance = maxDistance(&mytree);
    cout<<"max distance:"<<maxdistance<<endl;
    freetree(&mytree);
}