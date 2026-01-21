#include<iostream>
#include<vector>
#include<stack>

using namespace std;

bool isusestake(vector<int>& a,vector<int>& b){
    stack<int> temp;
    int j = 0;
    for (size_t i = 0; i < a.size(); i++)
    {
        // cout<<" b[j]:"<<b[j]<<"a[i]:"<<a[i]<<endl;
        if (temp.empty())
        {
            
            temp.push(a[i]); 
            // cout<<" b[j]:"<<b[j]<<" t: " << temp.top()<<"a[i]:"<<a[i]<<endl;
        }
        else if (temp.top()==b[j])
        {
            temp.pop();
            // cout<<"# "<<j<<"$"<<a[i]<<endl;
            j++;
            i--;
        }
        else{
            temp.push(a[i]);
        }
        
    }
    while (!temp.empty())
    {
        if (temp.top()!=b[j])
        {
            return false;
        }
        else
        {
            // cout<<"# "<<j<<"$"<<temp.top()<<endl;
            temp.pop();
            j++;
        }
    }
    if ((size_t)j!=b.size())
    {
        return false;
    }
    
    return true;
    
}

int main(){
    vector<int> a,b;
    cout<<"请输入没有重复数字的顺序表A:(注意：输入的数字间用空格，换行结束)"<<endl;
    int in;
    while (cin>>in)
    {
        a.push_back(in);
        if (cin.peek()=='\n')
        {
            break;
            /* code */
        }
        /* code */
    }
    cout<<"请输入没有重复数字的顺序表B:"<<endl;
    for (size_t i = 0; i < a.size(); i++)
    {
        cin>>in;
        b.push_back(in);
    }

    bool res =isusestake(a,b);
    if (res)
    {
        cout<<"可以用栈转化,success"<<endl;
        /* code */
    }
    else
    {
        cout<<"不可以用栈转化,fail"<<endl;
    }
    
    


    return 0;
}