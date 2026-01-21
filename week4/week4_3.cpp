#include<iostream>
#include<vector>

using namespace std;

bool issafe(vector<int>& path,int deep,int hang){
    int k = deep+hang;
    int k2 = deep - hang;
    for (int i = 0; i < (int)path.size(); i++)
    {
        if (i+path[i]==k|| i-path[i] == k2||i == deep || path[i]==hang)
        {
            return false;
        }
    }
    return true;
    
}

void backtracking(vector<vector<int>>& res,int n,int deep,vector<int>& path )
{
    if (deep==n)
    {
        res.push_back(path);
        return;
        /* code */
    }
    for (int i = 0; i < n; i++)
    {
        if (issafe(path,deep,i))
        {
            path.push_back(i);
            backtracking(res,n,deep+1,path);
            path.pop_back();
        }
        
        /* code */
    }
    return;
    
    
}
int main(){
    int n;
    cout<<"please input n"<<endl;
    cin>>n;
    vector<vector<int>> res;
    vector<int> path;
    backtracking(res,n,0,path);
    for (size_t i = 0; i < res.size(); i++)
    {
        int flag = 0;
        for (auto it:res[i])
        {
            if (flag)
            {
                cout<<',';
            }
            cout<<it;
            flag = 1;
            /* code */
        }
        cout<<";"<<endl;
        
        /* code */
    }
        
}
