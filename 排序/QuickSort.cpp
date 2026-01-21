#include<iostream>
#include<vector>

using namespace std;

int partition(vector<int>& arr,int l,int r){
    int pivot = arr[l];
    while (l<r)
    {
        while (l<r&&arr[r]>=pivot)
        {
            r--;
        }
        arr[l] = arr[r];
        while (l<r&&arr[l]<=pivot)
        {
            l++;
        }
        arr[r] =arr[l]; 
    }
    arr[l] = pivot;
    return l;
}

void quickSort(vector<int>& arr, int l,int r){
    if (l<r)
    {
        int pivotPos = partition(arr,l,r);
        quickSort(arr,l,pivotPos-1);
        quickSort(arr,pivotPos+1,r); 
        /* code */
    }
}

int main(){
    vector<int> arr = {3,9,6,2,8,1};
    int n = arr.size();
    quickSort(arr,0,n-1);
    for (const auto& num : arr) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}