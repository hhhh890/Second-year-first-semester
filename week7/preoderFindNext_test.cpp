#include"BiThrNode.h"
#include<stdio.h>
using namespace std;

BiThrNode* pre = nullptr;  // 全局/引用传递，跟踪先序前驱
BiThrNode* node = nullptr;
void PreThread(BiThrNode* current) {
    if (current == nullptr) return;

    if (current->lchild == nullptr) {
        current->ltag = 1;
        current->lchild = pre;
    }

    if (pre != nullptr && pre->rchild == nullptr) {
        pre->rtag = 1;
        pre->rchild = current;  
    }

    pre = current;


    if (current->ltag == 0) {  
        PreThread(current->lchild);
    }


    if (current->rtag == 0) {  
        PreThread(current->rchild);
    }
}
void buildThrTree(BiThrNode*root){
    pre = nullptr;
    if (root != nullptr) {
        PreThread(root);
        pre->rtag = 1;  // 最后一个节点的右线索指向nullptr（原代码未显式处理）
        pre->rchild = nullptr;
    }
}
void buildTree(BiThrNode* root){
    BiThrNode* nowpt = root;
    for (int i = 1; i < 5; i++)
    {
        BiThrNode* newlf = new BiThrNode{(char)('a'+i),nullptr,nullptr,0,0};
        BiThrNode* newrt = new BiThrNode{(char)('A'+i),nullptr,nullptr,0,0};
        nowpt->lchild = newlf;
        nowpt->rchild = newrt;
        if (i%2==0)
        {
            nowpt = newrt;
        }
        else{
            nowpt = newlf;
        }
        if (i==3)
        {
            node = newlf;
            /* code */
        }
        
    }
    
}
void freetree(BiThrNode* root) {
    if (root == nullptr) return;
    // 仅当左/右指针是孩子时才递归释放（线索不代表孩子）
    if (root->ltag == 0) {
        freetree(root->lchild);
    }
    if (root->rtag == 0) {
        freetree(root->rchild);
    }
    delete root;  // 释放当前节点
}

int main(){
    BiThrNode* root = new BiThrNode{'a',nullptr,nullptr,0,0};
    buildTree(root);
    buildThrTree(root);
    PreOrderFindNext(node);
    putchar(node->data);
    freetree(root);
    return 0;
}