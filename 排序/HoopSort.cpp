#include<iostream>
#include<vector>
using namespace std;

void shiftUp(vector<int>& hoop,int start){
    int i = start,j=(start-1)/2;
    while (i>0)
    {
        if (hoop[i]<hoop[j])
        {
            swap(hoop[i],hoop[j]);
            i = j;
            j = (i-1)/2;
        }else{
            break;
        }
    }
}
void shiftDown(vector<int>& hoop,int start,int size){
    int i = start, j = 2*i+1;
    while (j<size)
    {
        if (j+1<size&&hoop[j+1]<hoop[j])
        {
            j = j+1;
        }
        if (hoop[i]>hoop[j])
        {
            swap(hoop[i],hoop[j]);
            i = j;
            j = 2*i+1;
        }else{
            break;
        }
    }
}
int popMin(vector<int>& hoop){
    int res = hoop[0];
    hoop[0] = hoop[hoop.size()-1];
    hoop.pop_back();
    shiftDown(hoop,0,hoop.size());
    return res;
}
int main(){
    vector<int> hoop;
    hoop.push_back(5);
    shiftUp(hoop,hoop.size()-1);
    hoop.push_back(3);
    shiftUp(hoop,hoop.size()-1);
    hoop.push_back(8);
    shiftUp(hoop,hoop.size()-1);
    hoop.push_back(1);
    shiftUp(hoop,hoop.size()-1);
    hoop.push_back(4);
    shiftUp(hoop,hoop.size()-1);

    while (hoop.size()>0)
    {
        cout<<popMin(hoop)<<" ";
    }
}