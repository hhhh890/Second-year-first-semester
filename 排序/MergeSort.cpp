#include<iostream>
#include<vector>
using namespace std;
void merge(vector<int>& arr1,vector<int>& arr2,int left,int mid,int right){
    for (int i = left; i <= right; i++)
    {
        arr2[i] = arr1[i];
    }
    int s1 = left , s2 = mid + 1 , t = left;
    while(s1 <= mid && s2 <= right)
        if ( arr2[s1] <= arr2[s2] ) arr1[t++] = arr2[s1++];
        else arr1[t++] = arr2[s2++];
    while(s1 <= mid) arr1[t++] = arr2[s1++];
    while(s2 <= right) arr1[t++] = arr2[s2++];
}
void mergeSort(vector<int>& arr1,vector<int>& arr2,int left,int right){
    if (left>=right)
    {
        return;
    }
    int mid = left+(right-left)/2;
    mergeSort(arr1,arr2,left,mid);
    mergeSort(arr1,arr2,mid+1,right);
    merge(arr1,arr2,left,mid,right);
}

int main(){
    vector<int> arr = {8,3,6,4,1,9};
    vector<int> res(6,0);
    mergeSort(arr,res,0,5);
    for(auto& num:arr){
        cout<<num<<' ';
    }
    return 0;
}