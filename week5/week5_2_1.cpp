#include<cstring>
#include<iostream>
using namespace std;

void Del(char *s,char ch){
    char *p=s;
    char *q = s;
    while (*q!='\0')
    {
        if (*q!=ch)
        {
            *p = *q;
            p++;
            /* code */
        }
        q++;
        /* code */
    }
    *p = '\0';
}

int main(){
    char s[100];
    char ch;
    cin.getline(s,100);
    cin>>ch;
    Del(s,ch);
    cout<<s<<endl;
    return 0;
}  