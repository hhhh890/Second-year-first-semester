#include<iostream>
#include<string>
#include<vector>
using namespace std;

int ascii(string& s){
    int size = s.size();
    int res =0;
    for (int i = 0; i < size; i++)
    {
        res+=s[i]-'A';
        /* code */
    }
    return res%23;
}

int main(){
    vector<string> ss = {"CHA", "CAI", "LAN", "WEN", "LONG", "ZHAO","WU", "LIU", "CHEN", "LI", "WANG", "CAO", "YUN", "CHANG", "YANG"};
    for (size_t i = 0; i < ss.size(); i++)
    {
        int res = ascii(ss[i]);
        cout<<res<<" //";
        /* code */
    }
    
}