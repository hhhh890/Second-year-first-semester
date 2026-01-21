#include <algorithm>
template <class T>
struct BinNode {
    T data;   BinNode <T > *lchild;      BinNode <T > *rchild;  
  };

template<class T>
BinNode <T> * getLCA(BinNode <T> * root, T X, T Y){
    if (root == nullptr) {
        return nullptr;
    }
    if (root->data == X || root->data == Y) {
        return root;
    }

    BinNode<T>* leftLCA = getLCA(root->lchild, X, Y);
    BinNode<T>* rightLCA = getLCA(root->rchild, X, Y);

    if (leftLCA && rightLCA) {
        return root;
    }

    return (leftLCA != nullptr) ? leftLCA : rightLCA;
}

template<class T>
int rootDistance(BinNode <T> * root,int& max){
    if (root->lchild==nullptr&&root->rchild==nullptr)
    {
        return 0;
        /* code */
    }
    if (root->lchild==nullptr)
    {
        int n = 1+rootDistance(root->rchild,max);
        if(max< n ){
            max = n;
        }
        return n;
        /* code */
    }
    if (root->rchild==nullptr)
    {
        int n = 1+rootDistance(root->lchild,max);
        if(max< n ){
            max = n;
        }
        return n;
        /* code */
    }
    int rt = 1+rootDistance(root->rchild,max);
    int lt = 1+rootDistance(root->lchild,max);
    int n = rt+lt;
    if (max<n)
    {
        max = n;
        /* code */
    }
    return rt>lt?rt:lt;
    
}

template<class  T>

int maxDistance(BinNode <T> * root){
    int max = 0;
    rootDistance(root,max);
    return max;
}