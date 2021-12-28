#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <libgen.h>
#include <fcntl.h> // Definition of 0_* constants.

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>

#define MATRIX_DIMENSION_XY 10

using namespace std;

// Function Prototypes.
void set_matrix_elem(float *M,int x,int y,float f);
int quadratic_matrix_compare(float *A,float *B);
void quadratic_matrix_print(float *C);
void quadratic_matrix_multiplication(float *A,float *B,float *C);
void synch(int par_id,int par_count,int *ready);


//SEARCH FOR TODO
//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M,int x,int y,float f)
{
M[x + y*MATRIX_DIMENSION_XY] = f;
}
//************************************************************************************************************************
// lets see it both are the same
int quadratic_matrix_compare(float *A,float *B)
{
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
       if(A[a +b * MATRIX_DIMENSION_XY]!=B[a +b * MATRIX_DIMENSION_XY]) 
        return 0;
   
return 1;
}
//************************************************************************************************************************
//print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    {
    printf("\n");
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        printf("%.2f,",C[a + b* MATRIX_DIMENSION_XY]);
    }
printf("\n");
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A,float *B,float *C)
{
	//nullify the result matrix first
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        C[a + b*MATRIX_DIMENSION_XY] = 0.0;
//multiply
for(int a = 0;a<MATRIX_DIMENSION_XY;a++) // over all cols a
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
        for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
            {
                C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
            }
}
//************************************************************************************************************************
void synch(int par_id,int par_count,int *ready)
{
    //TODO: synch algorithm. make sure, ALL processes get stuck here until all ARE here
    int j = 0, synch_ID = ready[par_count] + 1;
    ready[par_id] = synch_ID;

    bool b = false;

    while(true)
    {
        b = true;

        while(j < par_count)
        {
            if(ready[j] < synch_ID)
            {
                b = false;
                break;
            }
            else if(b = true)
            {
                ready[par_count] = synch_ID;
                break;
            }
            j++;
        }
    }
}
//************************************************************************************************************************
int main(int argc, char *argv[])
{
    int par_id = 0; // the parallel ID of this process
    int par_count = 1; // the amount of processes
    float *A,*B,*C; //matrices A,B and C
    int *ready; //needed for synch

    if(argc!=3)
    {
        printf("no shared\n");
    }
    else
    {
        par_id= atoi(argv[1]);
        par_count= atoi(argv[2]);
        // strcpy(shared_mem_matrix,argv[3]);
    }

    if(par_count == 1)
    {
        printf("only one process\n");
    }

    int fd[4];
    
    if(par_id==0)
    {
        //TODO: init the shared memory for A,B,C, ready. shm_open with C_CREAT here! then ftruncate! then mmap.
        fd[0] = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
        fd[1] = shm_open("matrixB", O_RDWR | O_CREAT, 0777);
        fd[2] = shm_open("matrixC", O_RDWR | O_CREAT, 0777);
        fd[3] = shm_open("synchobject", O_RDWR | O_CREAT, 0777);

        A = (float*) (mmap(0, sizeof(float)*100, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0));
        B = (float*) (mmap(0, sizeof(float)*100, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0));
        C = (float*) (mmap(0, sizeof(float)*100, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0));
        ready = (int*)(mmap(0, sizeof(int)*(par_count + 1), PROT_READ | PROT_WRITE, MAP_SHARED,fd[3], 0));

        int j = 0;

        while(j <= par_count)
        {
            ready[i] = 0;
            j++;
        }
    }
    else
    {
    	//TODO: init the shared memory for A,B,C, ready. shm_open withOUT C_CREAT here! NO ftruncate! but yes to mmap
        sleep(2); //needed for initalizing synch
        fd[0] = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
        fd[1] = shm_open("matrixB", O_RDWR | O_CREAT, 0777);
        fd[2] = shm_open("matrixC", O_RDWR | O_CREAT, 0777);
        fd[3] = shm_open("synchobject", O_RDWR | O_CREAT, 0777);

        A = (float*)(mmap(0, sizeof(int) * 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0));
        B = (float*)(mmap(0, sizeof(int) * 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0));
        C = (float*)(mmap(0, sizeof(int) * 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0));
        ready = (int*)(mmap(0, sizeof(int) * (par_count + 1), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0));
        ready[par_id] = 0;
    }


    synch(par_id,par_count,ready);

    if(par_id == 0)
    {
	   //TODO: initialize the matrices A and B.
       int x = 0, y = 0;

       while(x < 100)
       {
          while(y < 100)
          {
            set_matrix_elem(A, x, y, 1);
            set_matrix_elem(B, x, y, 1);
            y++;
          }
          x++;
       }
       start = clock();
    }

    synch(par_id,par_count,ready);

    //TODO: quadratic_matrix_multiplication_parallel(par_id, par_count,A,B,C, ...);
    quadratic_matrix_multiplication(par_id, par_count, A, B, C);
    	
    synch(par_id,par_count,ready);

    if(par_id == 0)
    {
        stop = clock();
        double time_took = ((double)(stop - start)) / (CLOCKS_PER_SEC);
        quadratic_matrix_print(C);
        flush(nullptr);
    }

    synch(par_id, par_count, ready);

    //lets test the result:
    float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
    quadratic_matrix_multiplication(A, B, M);

    if (quadratic_matrix_compare(C, M))
    {
    	print("full points!\n");
        flush(nullptr);
    }
    else
    {
    	print("buuug!\n");
        flush(nullptr);
    }


    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");

    return 0;    
}