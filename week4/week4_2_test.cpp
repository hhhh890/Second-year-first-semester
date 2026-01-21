#include "week4_2.h"
#include <iostream>

int main(){
    LinkedPrQueue<int> lpq;
    lpq.Enqueue(1,1);
    lpq.Enqueue(2,3);
    lpq.Enqueue(3,2);
    lpq.Enqueue(4,2);
    lpq.Enqueue(6,2);
    int x;
    while (lpq.Dequeue(x))
    {
        std::cout<<x<<" ";
        /* code */
    }
    std::cout<<std::endl;
    return 0;
}