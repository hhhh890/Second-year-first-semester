#include<iostream>
#include<vector>
using namespace std;

void shellSort(vector<int>& arr){
    int n = arr.size();
    for (int d = n/2; d >= 1; d/=2)
    {
        for (int i = d; i < n; i++)
        {
            if (arr[i]<arr[i-d])
            {
                int temp = arr[i];
                int j = i - d;
                do{
                    arr[j+d] = arr[j];
                    j-=d;
                } while (j>=0&&arr[j]>temp);
                /* code */
                arr[j+d] = temp;
            }
        }
    }
    
}

int main(){
    vector<int> arr = {2,5,1,8,6,3};
    shellSort(arr);
    for (const auto& x:arr)
    {
        cout<<x<<" ";
        /* code */
    }
    return 0;
    
}