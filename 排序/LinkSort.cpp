#include<iostream>
#include<vector>

using namespace std;
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
void printList(ListNode* head) {
    ListNode* cur = head;
    while (cur != nullptr) {
        cout << cur->val << " ";
        cur = cur->next;
    }
    cout << endl;
}
int getDigit(int num, int digit) {
    return (num / digit) % 10;
}
int findMax(ListNode* head){
    if (head==nullptr)
    {
        return 0;
    }
    int maxVal = head->val;
    ListNode* curr = head->next;
    while (curr!=nullptr)
    {
        if (maxVal<curr->val)
        {
            maxVal = curr->val;
        }
        curr = curr->next;
    }
    return maxVal;
    
}
void freeList(ListNode* head) {
    ListNode* cur = head;
    while (cur != nullptr) {
        ListNode* next = cur->next;
        delete cur;
        cur = next;
    }
}

ListNode* radixSort(ListNode* head){
    if (head==nullptr||head->next==nullptr)
    {
        return head;
    }
    int maxVal =findMax(head);
    int digit = 1;
    while (maxVal/digit>0)
    {
        ListNode* buckets[10] = {nullptr};
        ListNode* busketTail[10] = {nullptr};
        ListNode* curr = head;
        while (curr!=nullptr)
        {
            int idx = getDigit(curr->val, digit);
            ListNode* nextNode = curr->next;
            curr->next = nullptr;
            if (buckets[idx] ==nullptr)
            {
                buckets[idx] = curr;
                busketTail[idx] = curr;
            }
            else{
                busketTail[idx]->next = curr;
                busketTail[idx] = curr;
            }
            curr = nextNode;
        }
        ListNode* newHead = nullptr;
        ListNode* newTail = nullptr;
        for (int i = 0; i < 10; ++i) {
            if (buckets[i] != nullptr) {
                if (newHead == nullptr) {
                    newHead = buckets[i];
                    newTail = busketTail[i];
                } else {
                    newTail->next = buckets[i];
                    newTail = busketTail[i];
                }
            }
        }

        // 更新头节点和当前处理的位数
        head = newHead;
        digit *= 10;

    }
    
    return head;
}
// 测试主函数
int main() {
    // 构建测试链表：39 → 6 → 28 → 15 → 9 → 102
    ListNode* head = new ListNode(39);
    head->next = new ListNode(6);
    head->next->next = new ListNode(28);
    head->next->next->next = new ListNode(15);
    head->next->next->next->next = new ListNode(9);
    head->next->next->next->next->next = new ListNode(102);

    printList(head);

    // 基数排序
    head = radixSort(head);

    printList(head);

    // 释放内存
    freeList(head);
    return 0;
}