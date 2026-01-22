//姓名：陈莹；学号：24300240197
#define _POSIX_C_SOURCE 200809L
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct  cache_line
{
    int valid;
    int tag;
    int timestamp;
    /* data */
}CacheLine;
typedef struct  mycache
{
    int S;
    int E;
    int B;
    CacheLine **lines;
    /* data */
}Cache;

Cache *cache;
int hit_count ;
int miss_count ;
int eviction_count ;
int verbose ;
char t[1000];

void initCache(int s,int b,int e){
    int S_ = 1<<s;
    int B_ = 1<<b;
    cache = (Cache*)malloc(sizeof(Cache));
    cache->S = S_;
    cache->B = B_;
    cache->E = e;
    cache->lines = (CacheLine**)malloc(sizeof(CacheLine*)*S_);
    for (int i = 0; i < S_; i++)
    {
        cache->lines[i] = (CacheLine*)malloc(sizeof(CacheLine)*e);
        for (int j = 0; j < e; j++)
        {
            cache->lines[i][j].valid = 0;
            cache->lines[i][j].tag = -1;
            cache->lines[i][j].timestamp=0;
            /* code */
        }
        /* code */
    }
}
void update(int idx,int op_s,int op_tag){
    cache->lines[op_s][idx].valid = 1;
    cache->lines[op_s][idx].tag = op_tag;
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[op_s][i].valid)
        {
            cache->lines[op_s][i].timestamp++;
            /* code */
        }
        /* code */
    }
    cache->lines[op_s][idx].timestamp = 0;
}

int findLRU(int op_s){
    //to del the max time
    int max_idx = 0;
    int max_time = 0;
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[op_s][i].timestamp>max_time)
        {
            max_time = cache->lines[op_s][i].timestamp;
            max_idx = i;
            /* code */
        }
        /* code */
    }
    return max_idx;   
}

int findEmpty(int op_s){
    //to write in empty idx
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[op_s][i].valid==0)
        {
            return i;
            /* code */
        }
        /* code */
    }
    return -1;
}

int findIdx(int op_s,int op_tag){
    //hit MISS return -1;
    //hit return idx
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[op_s][i].valid==1 && cache->lines[op_s][i].tag==op_tag)
        {
            return i;
            /* code */
        }
        /* code */
    }
    return -1;
}

void updateCache(int op_s,int op_tag){
    //
    int idx = findIdx(op_s,op_tag);
    if (idx==-1)
    {
        //miss
        miss_count++;
        if (verbose)
        {
            printf(" miss");
            /* code */
        }
        int empty_idx = findEmpty(op_s);
        //why miss
        if (empty_idx==-1){
            //conflict miss
            eviction_count++;
            if (verbose)
            {
                printf(" eviction");
                /* code */
            }
            empty_idx = findLRU(op_s);
        }
        //cold miss
        update(empty_idx,op_s,op_tag);
        /* code */
    }
    else
    {
        //hit
        hit_count++;
        if (verbose)
        {
            printf(" hit");
            /* code */
        }
        update(idx,op_s,op_tag);
        /* code */
    }
}

void traceFile(int s,int E,int b){
    FILE* pf;
    pf = fopen(t,"r");
    if (pf == NULL)
    {
        exit(1);
        /* code */
    }
    char identy;
    unsigned address;
    int size;
    while (fscanf(pf," %c %x,%d",&identy,&address,&size)>0)
    {
        int op_tag = address>>(s+b);
        int op_s = (address>>b) & ((unsigned)(-1)>>(8 * sizeof(unsigned) - s));
        switch (identy)
        {
        case 'L':
            updateCache(op_s,op_tag);
            /* code */
            break;
        case 'M':
            updateCache(op_s,op_tag);
            updateCache(op_s,op_tag);
            /* code */
            break;
        case 'S':
            updateCache(op_s,op_tag);
            /* code */
            break;
        default:
            break;
        }
        /* code */
    }
}

void freeCache(){
    for (int i = 0; i < cache->S; i++)
    {
        free(cache->lines[i]);
        /* code */
    }
    free(cache->lines);
    free(cache);
}
int main(int argc, char* argv[])
{
    char opt;
    int s,e,b;
    hit_count = miss_count = eviction_count = verbose = 0;
    while (-1!= (opt = getopt(argc, argv, "s:E:b:t:h")))
    {
        switch (opt)
        {
        case 'h':
            printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
            exit(0);
        case 'v':
            verbose = 1;
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            e = atoi(optarg);
            break;
        case 't':
            strcpy(t,optarg);
            break;
        default:
            break;
        }
        /* code */
    }
    
    initCache(s,b,e);
    traceFile(s,e,b);
    freeCache();
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
