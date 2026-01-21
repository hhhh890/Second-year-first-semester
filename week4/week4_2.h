template<typename T>
struct linknode
{
    T data;
    int priority;
    linknode* next;
    linknode(const T& d,int p,linknode* n=nullptr):data(d),priority(p),next(n){}    
};

template<typename T>
class LinkedPrQueue
{
private:
linknode<T>* head;
linknode<T>* tail;
void makeEmpty();
    /* data */
public:
    LinkedPrQueue(/* args */):head(nullptr),tail(nullptr){}
    ~LinkedPrQueue(){makeEmpty();}
    bool Enqueue(const T& x,int p);
    bool Dequeue(T &x);
    bool isEmpty();
};


template<typename T>
void LinkedPrQueue<T>::makeEmpty(){
    linknode<T>* p = head;
    while (p!=nullptr)
    {
        head = head->next;
        delete p;
        p = head;
    }
    delete head;
    delete tail;
    return;
}

template<typename T>
bool LinkedPrQueue<T>::Enqueue(const T& x,int p){
    linknode<T>* newnode = new linknode<T>(x,p,nullptr);
    if (head==nullptr)
    {
        head = tail = newnode;
        return true;
    }
    
    linknode<T>* find = head;
    if (find->priority < p)
    {
        newnode->next = head;
        head = newnode;
        return true;
        /* code */
    }
    
    while (find!=tail && find->next->priority >= p)
    {
        find = find->next;
        /* code */
    }
    newnode->next = find->next;
    find->next = newnode;
    if (find==tail)
    {
        tail = newnode;
        /* code */
    }
    return true;
}

template<typename T>
bool  LinkedPrQueue<T>::Dequeue(T &x){
    if (head == nullptr)
    {
        return false;
        /* code */
    }
    if (head==tail)
    {
        x = head->data;
        linknode<T>* p = head;
        delete p;
        head = tail = nullptr;
        return true;
        /* code */
    }
    x = head->data;
    linknode<T>* p = head;
    head = head->next;
    delete p;
    return true;
}


template<typename T>
bool LinkedPrQueue<T>::isEmpty(){
    if (head==nullptr)
    {
        return true;
        /* code */
    }
    return false;
    
}
