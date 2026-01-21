#include<iostream>
using namespace std;
struct Node{
    int value;
    Node* left;
    Node* right;
    Node(int n):value(n), left(nullptr), right(nullptr) {}
};
class testTree{
private:
    Node* root;
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
public:
    testTree():root(nullptr) {}
    ~testTree(){
        destroyTree(root);
    }
    Node* getRoot(){
        return root;
    }
    void createTest1(){
        // 平衡的AVL树（复杂结构）：
        root = new Node(4);
        root->left = new Node(2);
        root->right = new Node(6);
        root->left->left = new Node(1);
        root->left->right = new Node(3);
        root->right->left = new Node(5);
        root->right->right = new Node(7);
    }
    void createTest2(){
        //不平衡的树（右重）：
        root = new Node(1);
        root->right = new Node(2);
        root->right->right = new Node(3);
        root->left = new Node(0);
        root->right->right->right = new Node(4);
    }
    void createTest3(){
        //不平衡的树（左重）：
        root = new Node(5);
        root->left = new Node(4);
        root->left->left = new Node(3);
        root->right = new Node(6);
        root->left->left->left = new Node(2);
    }

};

//判断每个子树是否为AVL树
pair<bool,int> isAVL(Node* node){
    if (node==nullptr)
    {
        return make_pair(true,0);
        /* code */
    }
    pair<bool,int> left = isAVL(node->left);
    pair<bool,int> right = isAVL(node->right);
    bool balanced = left.first && right.first && (abs(left.second - right.second)<=1);
    int height = max(left.second, right.second) + 1;
    return make_pair(balanced, height);
}
// 判断整棵树是否为AVL树
bool isAVLTree(Node* root){
    return isAVL(root).first;
}

int main(){
    testTree tree1, tree2, tree3;
    tree1.createTest1();
    tree2.createTest2();
    tree3.createTest3();

    cout << "Tree 1 is AVL: " << (isAVLTree(tree1.getRoot()) ? "Yes" : "No") << endl;
    cout << "Tree 2 is AVL: " << (isAVLTree(tree2.getRoot()) ? "Yes" : "No") << endl;
    cout << "Tree 3 is AVL: " << (isAVLTree(tree3.getRoot()) ? "Yes" : "No") << endl;

    return 0;
}