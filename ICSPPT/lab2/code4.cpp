#include <iostream>
int func4(int a, int b) {
    if (a<=0)
    {
        return 0;
        /* code */
    }
    if (a==1)
    {
        return b;
        /* code */
    }
    return func4(a-1,b)+b+func4(a-2,b);
    
}

int main() {
    int result = func4(9,4);
    std::cout<<"when rsp[0]==4 ,Rsp[1]: " << result << std::endl;
    return 0;
}