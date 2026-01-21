#include<iostream>
#include<vector>
#include<stack>
#include<unordered_map>

using namespace std;

int akm(int m,int n){
    if(m==0) return n+1;
    if (m>=4&&n>=2)
    {
        cerr<<"overflow"<<endl;
        return -1;
    }
    
    if(m!=0 && n==0) return akm(m-1,1);
    if (m<0 || n<0)
    {
        return -1;
    }
    if (m!=0 && n!=0)
    {
        return akm(m-1,akm(m,n-1));
    }
    return -1;
}
int akm2(int m, int n){
    if(m<0||n<0){
        return -1;
    }
    stack<pair<int,int>> s;
    s.push({m,n});
    
    int res = -1;
    while (!s.empty())
    {
        m = s.top().first;
        n = s.top().second;
        s.pop();
        if (n==-1)
        {
            s.push({m,res});
        }
        else if (m==0)
        {
            res = n+1;
        }
        else if (n==0)
        {
            s.push({m-1,1});
            /* code */
        }
        
        else if (n!=0&&m!=0)
        {
            s.push({m-1,-1});
            s.push({m,n-1});
            /* code */
        }
        
    }
    return res;
    
}


int main(){
    int m,n;
    cin>>m>>n;
    cout<<"using (1):"<<"akm("<<m<<","<<n<<") = "<<akm(m,n)<<endl;
    cout<<"using (2):"<<"akm2("<<m<<","<<n<<") = "<<akm2(m,n)<<endl;
    return 0;
}