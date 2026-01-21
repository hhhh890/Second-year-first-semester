#include"BiTNode.h"


void buildTree(BiTNode* root){
    BiTNode* nowpt = root;
    for (int i = 1; i < 5; i++)
    {
        BiTNode* newlf = new BiTNode{(char)('a'+i),nullptr,nullptr};
        BiTNode* newrt = new BiTNode{(char)('A'+i),nullptr,nullptr};
        nowpt->lchild = newlf;
        nowpt->rchild = newrt;
        if (i%2==0)
        {
            nowpt = newrt;
        }
        else{
            nowpt = newlf;
        }

    }
    
}
void freetree(BiTNode* root){
    if (root==nullptr)
    {
        return;
    }
    
    if (root->lchild==nullptr && root->rchild==nullptr)
    {
        delete root;
        return;
        /* code */
    }
    freetree(root->lchild);
    freetree(root->rchild);
    delete root;

}
int main(){
    BiTNode* root = new BiTNode{'a',nullptr,nullptr};
    buildTree(root);
    PreOrder(root);
    freetree(root);

    return 0;
}