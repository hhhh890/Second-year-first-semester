#include<iostream>
#include<vector>
using namespace std;

void chooseSort(vector<int>& arr){
    int n = arr.size();
    for (int i = 0; i < n; i++)
    {
        int min = arr[n-1];
        int temp = n-1;
        for (int j = i; j < n; j++)
        {
            if (arr[j]<min)
            {
                min = arr[j];
                temp = j;
                /* code */
            }
        }
        arr[temp] = arr[i];
        arr[i] = min;
        /* code */
    }
    
}

int main(){
    vector<int> arr={2,8,4,1,9,7};
    chooseSort(arr);
    for (auto& num : arr)
    {
        cout<<num<<" ";
        /* code */
    }
    return 0;
}