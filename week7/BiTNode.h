#include<stack>
#include<stdio.h>
using namespace std;
typedef struct BiTNode {
    char data;
    struct BiTNode *lchild, *rchild;
}BiTNode;

void PreOrder(BiTNode *root){
    stack<BiTNode*> stk;
    stk.push(root);
    BiTNode* nowroot;
    while (!stk.empty())
    {
        nowroot = stk.top();
        stk.pop();
        putchar(nowroot->data);
        if (nowroot->rchild!=nullptr)
        {
            stk.push(nowroot->rchild);
            /* code */
        }
        if (nowroot->lchild!=nullptr)
        {
            stk.push(nowroot->lchild);
            /* code */
        }
        
    }
    
}