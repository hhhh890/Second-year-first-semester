#include<iostream>
#include<unordered_map>
#include<cstring>

using namespace std;

char firstSingle(char * str){
    unordered_map<char, int> charCount;
    for (size_t i = 0; i < strlen(str); i++)
    {
        charCount[str[i]]++;
    }
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (charCount[str[i]] == 1)
        {
            return str[i];
        }
    }
    return '\0';
}

int main(){
    char str[]="database";
    cout<<firstSingle(str)<<endl;
    return 0;
}