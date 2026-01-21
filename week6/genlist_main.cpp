#include "GenList.h"
#include <iostream>

using namespace std;

int main(){
    GenList<int> s,t;
    cout<<"first:"<<endl;
    cout<<"eg:(1,(2,3),4)"<<endl;
    cin>>s;
    cout<<"second:"<<endl;
    cout<<"eg:(1,(2,3),4)"<<endl;
    cin>>t;
    cout<<"bool:"<<s.Equal(s.First(),t.First())<<endl;
    
    return 0;
}