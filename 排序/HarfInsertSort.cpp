#include<iostream>
#include<vector>
using namespace std;

void halfInsertSort(vector<int>&arr){
    size_t n = arr.size();
    int temp;
    for(size_t i = 1; i < n ; i++ ){
        temp = arr[i];
        int left = 0;
        int right = i - 1;
        while(left <= right){
            int mid = left + (right - left) / 2;
            if(arr[mid] > temp){
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        for (int j = i; j > left; j--) {
            arr[j] = arr[j - 1];
        }
        arr[left] = temp;
    }
}

int main(){
    vector<int> arr={3,5,7,2,4,8};
    halfInsertSort(arr);
    for (const auto& x:arr)
    {
        cout<<x<<" ";
        /* code */
    }
    cout<<endl;
    return 0;
}