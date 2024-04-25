#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>
#include<limits.h>
#include<iostream>
using namespace std;

MPI_Status status;
int processId;

double** generateRandomMatrix(int rows , int cols){
    printf("%d , %d" , rows , cols);
    printf("\n");
    double** matrix = (double**)malloc(rows * sizeof(double*));
    for(int i = 0; i < rows ; i++)
        matrix[i] = (double*)malloc(cols * sizeof(double));

    for(int i = 0 ; i < rows ; i++){
        for(int j = 0 ; j < cols ; j++){
            matrix[i][j] = (rand() % 10) + 1;
        }
    }

    return matrix;
}

void printMatrix(double** matrix , int rows , int cols ){
    for(int i = 0 ; i < rows ; i++){
        for(int j = 0 ; j < cols ; j++){
            printf("%f " , matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void PrintOptimalParenthesization(int** S , int i , int j) {
	if (i == j)
		printf("A%d", i);
	else {
		printf("(");
		PrintOptimalParenthesization(S, i, S[i][j]);
		PrintOptimalParenthesization(S, S[i][j] + 1 , j);
		printf(")");
	}

}

void MatrixChain(int numMatrices , int* Dimensions) {

	// Two 2D Arrays for Cost(M) and Parenthesizing(S)
    int** M = (int**)malloc((numMatrices+ 1) * sizeof(int*));
    int** S = (int**)malloc((numMatrices+ 1) * sizeof(int*));
    for(int i = 0; i < numMatrices + 1 ; i++){
        M[i] = (int*)malloc((numMatrices + 1) * sizeof(int));
        S[i] = (int*)malloc((numMatrices + 1) * sizeof(int));
    }

	// Arrays Initialized
	for (int i = 0; i < numMatrices + 1; i++) {
		for (int j = 0; j < numMatrices + 1 ; j++) {
			M[i][j] = 0; S[i][j] = 0;
		}
	}

	int j , Min , q;
	for (int diff = 1; diff < numMatrices ; diff++) { // Loop to calculate difference 
		for (int i = 1; i < ( numMatrices + 1) - diff ; i++ ) {
			j = i + diff; 
			
			 Min = INT_MAX;
			for (int k = i; k <= j - 1; k++) {
				q = M[i][k] + M[k + 1][j] + (Dimensions[i - 1] * Dimensions[k] * Dimensions[j]);

				if (q < Min) {
					Min = q;
					S[i][j] = k;
				}
			}

			M[i][j] = Min;
		}

	}

	// Printing Results
	printf("Optimal Dynamic Array : \n");
	for (int i = 1; i < numMatrices + 1; i++) {
		for (int j = 1; j < numMatrices + 1; j++) {
			printf("%d " , M[i][j]);
		}
		printf("\n");
	}
		
    printf("\n");
	printf("Matrix for Parenthesizing : \n");
	for (int i = 1; i < numMatrices + 1 ; i++) {
		for (int j = 1; j < numMatrices + 1 ; j++) {
			printf("%d " , S[i][j]);
		}
		printf("\n");
	}
	
	printf("\nOptimal Number of Multiplications : %d" ,  M[1][numMatrices]);
	printf("\nOptimal Parenthesizing : "); PrintOptimalParenthesization(S, 1, numMatrices);


	// Free up the memory
	for (int i = 0; i < numMatrices + 1; ++i) {
		free(M[i]);
		free(S[i]);
	}

	free(M);
	free(S);
}

void parallelMultiply( int p , int q , int r ){
  int processCount, slaveTaskCount, source, dest, rows, offset;
  struct timeval start, stop;
  MPI_Comm_rank(MPI_COMM_WORLD, &processId);
  MPI_Comm_size(MPI_COMM_WORLD, &processCount);

// Number of slave tasks will be assigned to variable -> slaveTaskCount
  slaveTaskCount = processCount - 1;
  double matrix_a[p][q],matrix_b[q][r] ,matrix_c[p][r];
  // double matrix_c[q][r];

  // double** matrix_d = new double * [p];
  // for(int i = 0 ; i < p ; i++ )
  //   matrix_d[i] = new double[r];



  if (processId == 0) {

    for(int i = 0 ; i < p ; i++) {
      for(int j = 0 ;  j < q ; j++){
        matrix_a[i][j] = (rand()%10) +1;
        // matrix_a[i][j] = 1;
      }
    }


    for(int i = 0 ; i < q ; i++){
      for(int j = 0 ;  j < r ; j++){
        matrix_b[i][j] = (rand()%10) + 1;
        // matrix_b[i][j] = 1;
      }
    }
	
  printf("\n\t\tMatrix Multiplication using MPI\n");

// Print Matrix A
    printf("\nMatrix A\n\n");
    for (int i = 0; i<p; i++) {
      for (int j = 0; j<q; j++) {
        printf("%.0f\t", matrix_a[i][j]);
      }
	    printf("\n");
    }
  // printMatrix(p , q , matrix_a);
// Print Matrix B
    printf("\nMatrix B\n\n");
    for (int i = 0; i<q; i++) {
      for (int j = 0; j<r; j++) {
        printf("%.0f\t", matrix_b[i][j]);
      }
	    printf("\n");
    }
  // printMatrix(q , r ,matrix_b);s
    // Determine number of rows of the Matrix A, that is sent to each slave process
    rows = p/slaveTaskCount;
    // printf("rows : %d" , offset);
    int rem = p%slaveTaskCount;
    // printf("Remainder : %d" , offset);
    // Offset variable determines the starting point of the row which sent to slave process
    offset = 0;

// Calculation details are assigned to slave tasks. Process 1 onwards;
// Each message's tag is 1
    for (dest=1; dest <= slaveTaskCount; dest++) {
      // Acknowledging the offset of the Matrix A
      MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      // Acknowledging the number of rows
      MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      // Send rows of the Matrix A which will be assigned to slave process to compute
      MPI_Send(&matrix_a[offset][0], rows*q, MPI_DOUBLE,dest,1, MPI_COMM_WORLD);
      // Matrix B is sent
      MPI_Send(&matrix_b, q*r, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
      
      // Offset is modified according to number of rows sent to each process
      offset = offset + rows;
      // printf("offset : %d\n" , offset);
    }

// Root process waits untill the each slave proces sent their calculated result with message tag 2
    for(int i = 1; i <= slaveTaskCount; i++) {
      source = i;
      // Receive the offset of particular slave process
      MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      // Receive the number of rows that each slave process processed
      MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      // Calculated rows of the each process will be stored int Matrix C according to their offset and
      // the processed number of rows
      MPI_Recv(&matrix_c[offset][0], rows*r, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
    }

    // // Print the result matrix
    printf("\nResult Matrix C = Matrix A * Matrix B:\n\n");
    for (int i = 0; i<p; i++) {
      for (int j = 0; j<r; j++)
        printf("%.0f\t", matrix_c[i][j]);
      printf ("\n");
    }
    // printf("matrix  \n");
    // for( int i = 0 ; i < p ; i++){
    //     for(int j = 0 ;  j < r  ; j++){
    //       cout << matrix_c[i][j] << " ";
    //     }
    //     cout << endl;
    //   }

  }
// Slave Processes 
  if (processId > 0) {
    // Source process ID is defined
    source = 0;

    // Slave process waits for the message buffers with tag 1, that Root process sent
    // Each process will receive and execute this separately on their processes

    // The slave process receives the offset value sent by root process
    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives number of rows sent by root process  
    MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives the sub portion of the Matrix A which assigned by Root 
    MPI_Recv(&matrix_a, rows*q, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives the Matrix B
    MPI_Recv(&matrix_b, q*r, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

    // Matrix multiplication

    for (int k = 0; k<r; k++) {
      for (int i = 0; i<rows; i++) {
        // Set initial value of the row summataion
        matrix_c[i][k] = 0.0;
        // Matrix A's element(i, j) will be multiplied with Matrix B's element(j, k)
        for (int j = 0; j<p; j++)
          matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
      }
    }

    // Calculated result will be sent back to Root process (process 0) with message tag 2
    
    // Offset will be sent to Root, which determines the starting point of the calculated
    // value in matrix C 
    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // Number of rows the process calculated will be sent to root process
    MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // Resulting matrix with calculated rows will be sent to root process
    MPI_Send(&matrix_c, rows*r, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
  }
  //  return (double**)matrix_c;
}

int main(int argc, char **argv) {
  double ** res ;
  if(processId == 0){
    res = new double *[20];
    for(int i=0;i<20;i++)
    {
      res[i] = new double [10];
    }
  }

  MPI_Init(&argc, &argv);
  if(processId == 0){
    FILE *myFile;
    myFile = fopen("dimensions.txt", "r");
    int numMatrices = 5;
    int* dimensions = (int*)calloc(numMatrices + 1, sizeof(int));

    for(int i = 0; i < numMatrices + 1; i++) {
        fscanf(myFile, "%d", &dimensions[i]);
    }

    printf("\nDimensions : ");
    for(int i = 0; i < numMatrices + 1; i++) {
        printf("%d  ", dimensions[i]);
    }

    double** matrix[numMatrices];
    for(int i = 0 ; i < numMatrices ; i++) {
        matrix[i] = generateRandomMatrix(dimensions[i] , dimensions[i+1]);
        printMatrix(matrix[i] , dimensions[i] , dimensions[i+1] );
    }

    MatrixChain(numMatrices, dimensions);
    parallelMultiply( dimensions[0] , dimensions[1] , dimensions[2] );
    printf("\n");
  }
  //printMatrix(20 , 10 , res);
  MPI_Finalize();


// if(processId == 0){
//   for( int i = 0 ; i < 20 ; i++){
//     for(int j = 0 ;  j < 10  ; j++){
//       cout << res[i][j] << " ";
//     }
//     cout << endl;
//   }
// }

}
