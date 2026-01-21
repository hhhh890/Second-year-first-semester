#include<iostream>
#include<vector>
using namespace std;

void traveback(int start,vector<vector<int> >& res,vector<int>& zuhe,int k,int m){
    if(zuhe.size() == (size_t)k){
        res.push_back(zuhe);
        return;
    }
    for (int i = start; i < m+1; i++)
    {
        zuhe.push_back(i);
        traveback(i+1,res,zuhe,k,m);
        zuhe.pop_back();
    }
    
    return;
}

int main(){
    int m,k;
    cout<<"input m and k"<<endl;
    cin>>m>>k;
    if (m<k)
    {
        cout<<"m<k,没有组合"<<endl;
        return 0;
        /* code */
    }
    
    vector<vector<int> > res;
    // vector<int> used(m,0);
    vector<int> zuhe;
    traveback(1,res,zuhe,k,m);
    for (size_t i = 0; i < res.size(); i++)
    {
        cout<<"{";
        for (size_t j = 0; j < res[i].size(); j++)
        {
            if (j)
            {
                cout<<',';
                /* code */
            }
            cout<<res[i][j];
            
            /* code */
        }
        cout<<"}"<<endl;
        /* code */
    }
    return 0;
}