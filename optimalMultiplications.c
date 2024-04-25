#include <stdio.h>
#include<stdlib.h>
#include<limits.h>


int** generateRandomMatrix(int rows , int cols){
    printf("%d , %d" , rows , cols);
    printf("\n");
    int** matrix = (int**)malloc(rows * sizeof(int*));
    for(int i = 0; i < rows ; i++)
        matrix[i] = (int*)malloc(cols * sizeof(int));

    for(int i = 0 ; i < rows ; i++){
        for(int j = 0 ; j < cols ; j++){
            matrix[i][j] = (rand() % 10) + 1;
        }
    }

    return matrix;
}

void printMatrix(int** matrix , int rows , int cols){
    for(int i = 0 ; i < rows ; i++){
        for(int j = 0 ; j < cols ; j++){
            printf("%d " , matrix[i][j]);
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


int main() {

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

    int** matrix[numMatrices];
    for(int i = 0 ; i < numMatrices ; i++) {
       matrix[i] = generateRandomMatrix(dimensions[i] , dimensions[i+1]);
       printMatrix(matrix[i] , dimensions[i] , dimensions[i+1] );
    }

    MatrixChain(numMatrices, dimensions);
    printf("\n");


}
