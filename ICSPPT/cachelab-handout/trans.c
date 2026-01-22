//姓名：陈莹；学号：24300240197
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
void trans_divide(int M, int N, int A[N][M], int B[M][N]);
void trans_divide4(int M, int N, int A[N][M], int B[M][N]);
void trans_divide8(int M, int N, int A[N][M], int B[M][N]);
void trans_div4_store(int M, int N, int A[N][M], int B[M][N]);
void trans_div17_store(int M, int N, int A[N][M], int B[M][N]);
int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 &&N==32)
    {
        trans_divide8(M,N,A,B);
        /* code */
    }
    else if(M==64 && N==64)
    {
        trans_div4_store(M,N,A,B);
        /* code */
    }
    else{
        trans_div17_store(M,N,A,B);
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char trans_divide_desc[] = "Divide and Conquer Transpose";
void trans_divide(int M, int N, int A[N][M], int B[M][N])
{
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < M; j += 8)
        {
            for (int k = 0; k < 8 && i+k<N; k++)
            {
                for (int t = 0; t < 8 && j+t<M; t++)
                {
                    B[j + t][i + k] = A[i + k][j + t];
                    /* code */
                }
                
                /* code */
            }
        }
    }
}
char trans_divide4_desc[] = "Divide and Conquer 4 Transpose";
void trans_divide4(int M, int N, int A[N][M], int B[M][N])
{
    int a0,a1,a2,a3;
    for (int i = 0; i < N; i += 4) {
        for (int j = 0; j < M; j += 4)
        {
            for (int k = 0; k < 4 && i+k<N; k++)
            {
                a0 = A[i + k][j + 0];
                a1 = A[i + k][j + 1];
                a2 = A[i + k][j + 2];
                a3 = A[i + k][j + 3];
                B[j + 0][i + k] = a0;
                B[j + 1][i + k] = a1;
                B[j + 2][i + k] = a2;
                B[j + 3][i + k] = a3;
                /* code */
            }
        }
    }

}
char trans_divide8_store_desc[] ="Divide and Conquer 8 Transpose with temp store";
void trans_divide8(int M,int N,int A[N][M],int B[M][N])
{
    int a0,a1,a2,a3,a4,a5,a6,a7;
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < M; j += 8)
        {
            for (int k = 0; k < 8 && i+k < N ; k++)
            {
                a0 = A[i + k][j + 0];
                a1 = A[i + k][j + 1];
                a2 = A[i + k][j + 2];
                a3 = A[i + k][j + 3];
                a4 = A[i + k][j + 4];
                a5 = A[i + k][j + 5];
                a6 = A[i + k][j + 6];
                a7 = A[i + k][j + 7];
                B[j + 0][i + k] = a0;
                B[j + 1][i + k] = a1;
                B[j + 2][i + k] = a2;
                B[j + 3][i + k] = a3;
                B[j + 4][i + k] = a4;
                B[j + 5][i + k] = a5;
                B[j + 6][i + k] = a6;
                B[j + 7][i + k] = a7;
                /* code */
            }
        }
    }
}
char trans_div4_store_desc[] ="Divide and Conquer 4 Transpose with temp";
void trans_div4_store(int M,int N, int A[N][M],int B[M][N]){
    int a0,a1,a2,a3,a4,a5,a6,a7;
    for (int  i = 0; i < N; i+=8)
    {
        for (int j = 0; j < M; j+=8)
        {
            for (int k = 0; k < 4 && i+k<N; k++)
            {
                a0 = A[i + k][j + 0];
                a1 = A[i + k][j + 1];
                a2 = A[i + k][j + 2];
                a3 = A[i + k][j + 3];
                a4 = A[i + k][j + 4];
                a5 = A[i + k][j + 5];
                a6 = A[i + k][j + 6];
                a7 = A[i + k][j + 7];
                B[j + 0][i + k] = a0;
                B[j + 1][i + k] = a1;
                B[j + 2][i + k] = a2;
                B[j + 3][i + k] = a3;
                B[j + 0][i + k + 4] = a4;
                B[j + 1][i + k + 4] = a5;
                B[j + 2][i + k + 4] = a6;
                B[j + 3][i + k + 4] = a7;

                /* code */
            }
            for (int k = 0; k < 4 && j+k<M; k++)
            {
                a0 = B[j + k][i + 4];
                a1 = B[j + k][i + 5];
                a2 = B[j + k][i + 6];  
                a3 = B[j + k][i + 7];
                a4 = A[i + 4][j + k];
                a5 = A[i + 5][j + k];
                a6 = A[i + 6][j + k];
                a7 = A[i + 7][j + k];

                B[j + k][i + 4] = a4;
                B[j + k][i + 5] = a5;
                B[j + k][i + 6] = a6;
                B[j + k][i + 7] = a7;

                B[j + k + 4][i + 0] = a0;
                B[j + k + 4][i + 1] = a1;
                B[j + k + 4][i + 2] = a2;
                B[j + k + 4][i + 3] = a3;
                /* code */
            }
            for (int k = 0; k < 4 && i+k+4<N ; k++)
            {
                a4 = A[i + k + 4][j + 4];
                a5 = A[i + k + 4][j + 5];
                a6 = A[i + k + 4][j + 6];
                a7 = A[i + k + 4][j + 7];
                B[j + 4][i + k + 4] = a4;
                B[j + 5][i + k + 4] = a5;
                B[j + 6][i + k + 4] = a6;
                B[j + 7][i + k + 4] = a7;
                /* code */
            }
            
            
            /* code */
        }
        
        /* code */
    }
    
}
char trans_div15_store_desc[]="Divide and Conquer 15 Transpose with temp";
void trans_div15_store(int M,int N,int A[N][M],int B[M][N]){
    for (int i = 0; i < N; i+=15)
    {
        for (int j = 0; j < M; j+=15)
        {
            for (int k = 0;k < 15 && i+k<N; k++)
            {
                for (int t = 0; t <15 && j+t<M; t++)
                {
                    B[j + t][i + k] = A[i + k][j + t];
                    /* code */
                }
                /* code */
            }
        }
        /* code */
    } 
}
char trans_div16_store_desc[]="Divide and Conquer 16 Transpose with temp";
void trans_div16_store(int M,int N,int A[N][M],int B[M][N]){
    for (int i = 0; i < N; i+=16)
    {
        for (int j = 0; j < M; j+=16)
        {
            for (int k = 0;k < 16 && i+k<N; k++)
            {
                for (int t = 0; t <16 && j+t<M; t++)
                {
                    B[j + t][i + k] = A[i + k][j + t];
                    /* code */
                }
                /* code */
            }
        }
        /* code */
    } 
}
char trans_div17_store_desc[]="Divide and Conquer 17 Transpose with temp";
void trans_div17_store(int M,int N,int A[N][M],int B[M][N]){
    for (int i = 0; i < N; i+=17)
    {
        for (int j = 0; j < M; j+=17)
        {
            for (int k = 0;k < 17 && i+k<N; k++)
            {
                for (int t = 0; t <17 && j+t<M; t++)
                {
                    B[j + t][i + k] = A[i + k][j + t];
                    /* code */
                }
                /* code */
            }
        }
        /* code */
    } 
}
char trans_div18_store_desc[]="Divide and Conquer 18 Transpose with temp";
void trans_div18_store(int M,int N,int A[N][M],int B[M][N]){
    for (int i = 0; i < N; i+=18)
    {
        for (int j = 0; j < M; j+=18)
        {
            for (int k = 0;k < 18 && i+k<N; k++)
            {
                for (int t = 0; t <18 && j+t<M; t++)
                {
                    B[j + t][i + k] = A[i + k][j + t];
                    /* code */
                }
                /* code */
            }
        }
        /* code */
    } 
}
/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
    registerTransFunction(trans_divide,trans_divide_desc);
    registerTransFunction(trans_divide4,trans_divide4_desc);
    registerTransFunction(trans_divide8,trans_divide8_store_desc);
    registerTransFunction(trans_div4_store,trans_div4_store_desc);

    registerTransFunction(trans_div15_store,trans_div15_store_desc);
    registerTransFunction(trans_div16_store,trans_div16_store_desc);
    registerTransFunction(trans_div17_store,trans_div17_store_desc);
    registerTransFunction(trans_div18_store ,trans_div18_store_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

