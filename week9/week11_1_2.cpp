#include<vector>
#include<iostream>
using namespace std;

int findSm(vector<int>& a,vector<int>& b){
    int a_start,a_end,b_start,b_end;
    int mida,midb,lena,lenb,m;
    a_start = b_start = 0;
    m = a_end = b_end = a.size();
    
    while (a_start<=a_end&&b_start<=b_end)
    {
        mida = (a_end-a_start)/2+a_start;
        midb = (b_end-b_start)/2+b_start;
        lena = mida+1;
        lenb = midb+1;
        //cout<<mida<<"a;b:"<<midb<<endl;
        if (lena+lenb<=m)
        {
            if (a[mida]<b[midb])
            {
                a_start = mida+1;
            }
            else{
                b_start=midb+1;
            }
            
        }
        else{
            if (a[mida]<b[midb])
            {
                b_end = midb-1;
                /* code */
            }
            else{
                a_end = mida-1;
            }
            
        }
    }
    if (a_start<=a_end)
    {
        return a[mida];
        /* code */
    }
    else{
        return b[midb];
    }

}


int main(){
    vector<int> A = {2,4,6,8,10};
    vector<int> B = {1,3,5,7,9};
    cout<<findSm(A,B)<<endl;
    return 0;
}