#include<iostream>
#include<vector>
using namespace std;

void bubbleSort(vector<int>& arr){
    int n = arr.size();
    for (int i = 0; i < n; i++)
    {
        for (int j = n-1; j > i; j--)
        {
            if (arr[j]<arr[j-1])
            {
                int temp = arr[j];
                arr[j] = arr[j-1];
                arr[j-1] = temp;
                /* code */
            }
            /* code */
        }
        /* code */
    }
}

int main(){
    vector<int> arr={1,8,4,2,7,9};
    bubbleSort(arr);
    for (const auto& x:arr)
    {
        cout<<x<<" ";
        /* code */
    }
    return 0;
}
