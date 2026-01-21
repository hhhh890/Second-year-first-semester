#include <iostream>
using namespace std;

class AVLtree{
private:
    struct Node
    {
        int value;
        Node* left;
        Node* right;
        int height;
        int balance;
        int size;
        Node(int n):value(n), left(nullptr), right(nullptr), height(1), balance(0), size(1) {}
        /* data */
    };
    Node* root;
    int getHeight(Node* node){
        if(node == nullptr) return 0;
        return node->height;
    }
    int getSize(Node* node){
        if(node == nullptr) return 0;
        return node->size;
    }
    void updateNode(Node* node){
        if(node == nullptr) return;
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        node->height = max(leftHeight, rightHeight) + 1;
        node->balance = leftHeight - rightHeight;
        node->size = getSize(node->left) + getSize(node->right) + 1;
    }
    //右旋
    void rightRotate(Node*& node){
        Node* lc = node->left;
        node->left = lc->right;
        lc->right = node;
        updateNode(node);
        updateNode(lc);
        node = lc;
    }
    //左旋
    void leftRotate(Node*& node){
        Node* rc = node->right;
        node->right = rc->left;
        rc->left = node;
        updateNode(node);
        updateNode(rc);
        node = rc;
    }
    void balanceTree(Node*& node){
        if (node==nullptr)
        {
            return;
            /* code */
        }
        if (node->balance==2 && node->left->balance>=0)
        {
            //LL
            rightRotate(node);
        }
        else if (node->balance==2 && node->left->balance<0)
        {
            //LR
            leftRotate(node->left);
            rightRotate(node);
        }
        else if (node->balance==-2 && node->right->balance<=0)
        {
            //RR
            leftRotate(node);
        }
        else if (node->balance==-2 && node->right->balance>0)
        {
            //RL
            rightRotate(node->right);
            leftRotate(node);
        }
        updateNode(node);
    }
    void insert(Node*& node, int value){
        if (node==nullptr)
        {
            node = new Node(value);
            return;
            /* code */
        }
        if (value<node->value)
        {
            insert(node->left, value);
        }
        else if(value>node->value)
        {
            insert(node->right, value);
        }
        else{
            return;
        }
        updateNode(node);
        balanceTree(node);
    }
    void deleteNode(Node*& node,int value){
        if (node==nullptr)
        {
            return;
            /* code */
        }
        if (value<node->value)
        {
            deleteNode(node->left, value);
        }
        else if (value>node->value)
        {
            deleteNode(node->right, value);
        }
        else
        {
            //如果是叶子，直接删除
            if (node->left==nullptr && node->right==nullptr)
            {
                delete node;
                node = nullptr;
            }
            else if (node->left==nullptr)
            {
                //只有右子节点
                Node* temp = node;
                node = node->right;
                delete temp;
            }
            else if (node->right==nullptr)
            {
                //只有左子节点
                Node* temp = node;
                node = node->left;
                delete temp;
            }
            else
            {
                //有两个子节点，找右子树的最小值替换
                Node* temp = node->right;
                while (temp->left!=nullptr)
                {
                    temp = temp->left;
                }
                node->value = temp->value;
                deleteNode(node->right, temp->value);
            }
        }
        if (node==nullptr)
        {
            return;
            /* code */
        }
        updateNode(node);
        balanceTree(node);
    }
    void destroyTree(Node* node){
        if (node==nullptr)
        {
            return;
            /* code */
        }
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
    Node* findth(Node* node, int k){
        if (node==nullptr)
        {
            return nullptr;
            /* code */
        }
        
        int leftSize = getSize(node->left);
        if (k==leftSize+1)
        {
            return node;
        }
        else if (k<=leftSize)
        {
            return findth(node->left, k);
        }
        else
        {
            return findth(node->right, k-leftSize-1);
        }
        
    }
public:
    AVLtree():root(nullptr) {}
    ~AVLtree(){
        destroyTree(root);
    }
    void insert(int value){
        insert(root, value);
    }
    void deleteNode(int value){
        deleteNode(root, value);
    }
    int getSize(){
        return getSize(root);
    }
    int getHeight(){
        return getHeight(root);
    }
    int findthk(int k){
        Node* result = findth(root, k);
        if (result==nullptr)
        {
            throw out_of_range("k is out of range");
            /* code */
        }
        return result->value;
    }
};

int main(){
    AVLtree tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    tree.insert(25);

    cout << "Size of AVL tree: " << tree.getSize() << endl;
    cout << "Height of AVL tree: " << tree.getHeight() << endl;

    for (int k = 1; k <= tree.getSize(); ++k) {
        cout << k << "-th smallest element: " << tree.findthk(k) << endl;
    }

    tree.deleteNode(20);
    cout << "Size of AVL tree after deleting 20: " << tree.getSize() << endl;
    cout << "Height of AVL tree after deleting 20: " << tree.getHeight() << endl;

    for (int k = 1; k <= tree.getSize(); ++k) {
        cout << k << "-th smallest element: " << tree.findthk(k) << endl;
    }

    return 0;
}