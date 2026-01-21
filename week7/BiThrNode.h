

typedef struct BiThrNode {
        char data;
        struct BiThrNode *lchild, *rchild;
        int ltag, rtag;   //1为线索，0为孩子
}BiThrNode;

BiThrNode * PreOrderFindNext(BiThrNode *node){
    if (node==nullptr)
    {
        return nullptr;
        /* code */
    }
    if (node->ltag==0 && node->lchild!=nullptr)
    {
        return node->lchild;
        /* code */
    }
    if (node->rchild!=nullptr)
    {
        return node->rchild;
        /* code */
    }
    return nullptr;
}