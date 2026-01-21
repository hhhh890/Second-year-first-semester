#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;


template <class T> 
struct GenListNode { 	   
    int utype;		                 //＝0 / 1 / 2 
    GenListNode<T> * tlink;      //下一结点指针
    union {				
        int ref;	                         //utype=0, 存放引用计数
        T value;	                         //utype=1, 存放整数值
        GenListNode<T> *hlink;	
                                //utype =2, 存放指向子表的指针
    } info;
    GenListNode ( ) : utype (0), tlink (nullptr), info{0} { }
    //构造函数
    GenListNode (GenListNode<T>& RL) { 
            utype = RL.utype; tlink = RL.tlink; info = RL.info;
    }	   //复制构造函数
};
template <class T>  struct Items {
    int utype;	            //＝0 / 1 / 2 
    union { 	            //联合       
       int ref;                  //utype=0, 存放引用计数
       T value;	            //utype=1, 存放数值
       GenListNode<T>* hlink;   
                                   //utype =2, 存放指向子表的指针
    } info;
    Items(): utype(0), info(0) { }
    Items(Items<T>& RL) {utype = RL.utype; info = RL.info;}
};
template <class T> class GenList {  	
private:
    GenListNode<T> *first;                   //广义表头指针
    GenListNode <T> *copy ( GenListNode <T> *ls );       //复制 ls 指示的无共享非递归表
    int length ( GenListNode<T> *ls );	     //计算由 ls 指示的非递归表的长度
    int depth ( GenListNode <T> *ls ); 	     //计算由 ls 指示的非递归表的深度
    void Remove (GenListNode <T> *ls );      //释放以 ls 为表头结点的广义表
    void Createlist (istream& in, GenListNode <T> *ls );
public:
    GenList ( );                      //构造函数
    ~GenList ( ){Remove(first);}		       //析构函数 
    bool Head (Items<T> &x);       //返回表头元素
    bool Tail (GenList<T> &lt );	 //返回表尾
    GenListNode<T>  *First ( ); //返回表头指针
    GenListNode<T> *Next ( GenListNode<T> *elem ){return elem->tlink;}    //返回表元素elem的直接后继元素
    void Copy(const GenList<T>& R){R = copy(first);}           //复制广义表
    bool Equal (GenListNode<T> *s, GenListNode<T> *t){
        GenListNode<T>* sn,*tn;
        sn = s->tlink; tn = t->tlink;
        while (sn!=nullptr&&tn!=nullptr)
        {
            if (sn->utype!=tn->utype)
            {
                return false;
                /* code */
            }
            else if (sn->utype==1)
            {
                if (sn->info.value!=tn->info.value)
                {
                    return false;
                    /* code */
                }
                
                /* code */
            }
            else if (sn->utype==2)
            {
                if (!Equal(sn->info.hlink,tn->info.hlink))
                {
                    return false;
                    /* code */
                }
                
            }
            sn = sn->tlink;
            tn = tn->tlink;
            /* code */
        }
        return (sn==nullptr)&&(tn==nullptr);
        
    }   //比较以s和t为表头的两表是否相等
    int Length ( ){return length(first);}         //计算长度
    int Depth ( ){return depth(first);}           //计算深度
    friend istream& operator >> (istream& in, GenList<T>& L){
        L.Createlist(in,L.first);
        if (in.fail()) {  // 若输入失败，清除状态但提示
            cerr << "输入错误，已重置流状态" << endl;
            in.clear();
        }
        return in;
    } 
};

template <class T>
GenList<T>::GenList(){
    first = new GenListNode<T> ;
    if (first==nullptr)
    {
        std::cerr<<"error in Genlist"<<endl;
        /* code */
    }
    first->utype=0;
    first->info.ref=1;
}
template<class T>
bool GenList<T>::Head(Items<T> &x){
    if (first->tlink==nullptr)
    {
        return false;
        /* code */
    }
    else{
        x->utype=first->tlink->utype;
        x->info = first->tlink->info;
    }
    return true;
    
}
template<class T>
GenListNode<T>*  GenList<T>::First ( ){
    return first;
}
template<class T>
bool GenList<T>::Tail (GenList<T> &lt ){
    if (first->tlink==nullptr)
    {
        return false;
        /* code */
    }
    lt.first->info.ref =0 ;
    lt.first->tlink = Copy(first->tlink->tlink);
    lt.first->utype = 0;
    return true;
}
template<class T>
void GenList<T>::Remove (GenListNode <T> *ls ){
    ls->info.ref--;
    if (ls->info.ref<=0)
    {
        GenListNode<T>* q ;
        while (ls->tlink!=nullptr)
        {
            q = ls->tlink;
            if (q->utype==2)
            {
                Remove(q->info.hlink);
                /* code */
            }
            ls->tlink = q->tlink;
            delete q;
            /* code */
        }
        delete ls;    
        /* code */
    }
}
template<class T>
GenListNode <T> * GenList<T>::copy ( GenListNode <T> *ls ){
    GenListNode<T>* q =nullptr;
    if (ls!=nullptr)
    {
        q = new GenListNode<T>;
        q->utype = ls->utype;
        switch ( ls->utype ) {			
            case 0:  q->info.ref = ls->info.ref;   break;
            case 1:  q->info.value = ls->info.value;   break;
            case 2:  q->info.hlink = copy (ls->info.hlink) ;   break;
        }      
        q->tlink = copy(ls->tlink);
        /* code */
    }
    return q;
}
template<class T>
int GenList<T>:: length ( GenListNode<T> *ls ){
    int res;
    while (ls!=nullptr)
    {
        res++;
        ls = ls->tlink;
        /* code */
    }
    return ls;
}
template<class T>
int GenList<T>::depth ( GenListNode <T> *ls ){
    if (ls->tlink==nullptr)
    {
        return 1;
        /* code */
    }
    int deep = 0;
    GenListNode<T>* q = ls->tlink;
    while (q!=nullptr)
    {
        if (q->utype==2)
        {
            int n= depth(q->info.hlink);
            if (n>deep)
            {
                deep = n;
                /* code */
            }
        }
        q = q->tlink;
    }
    return deep+1;
    
}
template<class T>
void GenList<T>::Createlist (istream& in, GenListNode <T> *ls ){
    //假设广义表的元素类型是char，每个原子元素的值为英文字母。广义表的输入形式为以'#‘结束的字符串，例如：
    //" ((a,b),(u,(x,y,z)),())#"
    char ch;
    in>>ch;
    if (ch=='#')
    {
        return;
        /* code */
    }
    while (isspace(ch))
    {
        in>>ch;
        /* code */
    }
    if (ch=='(')
    {
        in>>ch;
        /* code */
    }
    GenListNode<T>* now = ls;
    while (ch!='#')
    {   
        if (ch==')')
        {
            break;
            /* code */
        }
         if(ch==','||isspace(ch))
        {
            in>>ch;
            continue;
            /* code */
        }
        
        GenListNode<T>* tail = new GenListNode<T>;
        if (ch=='(')
        {
            tail->utype=2;
            GenListNode<T> * q = new GenListNode<T>;
            q->info.ref = 1;
            in.unget();
            Createlist(in,q);
            tail->info.hlink = q;
            /* code */
        } 
        else
        {
            //放回元素：
            in.unget();
            T value;
            in>>value;
            tail->utype = 1;
            tail->info.value = value;
            /* code */
        }
        now->tlink = tail;
        now = tail;
        in>>ch;
        /* code */
    }
    return;
}


