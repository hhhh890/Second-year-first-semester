#include<iostream>
#include<vector>
using namespace std;

void insertSort(vector<int>& arr){
    size_t n = arr.size();
    int temp;
    for (size_t i = 0; i < n; i++)
    {
        temp = arr[i];
        size_t j = i;
        while (j > 0 && arr[j - 1] > temp)
        {
            arr[j] = arr[j - 1];
            j--;
        }
        
        arr[j] = temp;
    }
}

int main(){
    vector<int> arr = {5, 2, 9, 1, 5, 6};
    insertSort(arr);
    for (const auto& num : arr) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}