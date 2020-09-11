#include <stdio.h>
#include <stdlib.h>

double** transpose(double** a, double** b, int rows, int cols){     //tranpose function, just switch indexes
    int i,j;
    for(i=0;i<cols;i++){
        for(j=0;j<rows;j++){
            b[i][j]=a[j][i];
        }
    }
    return b;
}


double** multiply(double** a, double** b, double** multiplied, int arows, int acols, int brows, int bcols){
    int i,j,k;
    for(i=0;i<arows;i++){
        for(j=0;j<bcols;j++){
            multiplied[i][j]=0;                         //reset couter to 0 for next iteration
            for(k=0;k<brows;k++){
               multiplied[i][j]+=a[i][k]*b[k][j];       //keep adding the multiplied value until end of row/col
            }
            

        }
    }
    return multiplied;
}

double** invert(double** inverted, double** finalmatrix,int size){
    int i,j,k;
    double factor;
//----------------------------------------------------------
     for(i=0;i<size;i++)                                    //augmented matrix is already passed in, so we only need to perform row operations
		 {
			  for(j=0;j<size;j++)
			  {
				   if(i!=j)
				   {
					    factor = inverted[j][i]/inverted[i][i];     //find factor in order to create ones where necessary
					    for(k=0;k<2*size;k++)
					    {
					     	inverted[j][k] = inverted[j][k] - factor*inverted[i][k];        
					    }
				   }
			  }
		 }
		 for(i=0;i<size;i++)
		 {
			  for(j=size;j<2*size;j++)
			  {
			   	inverted[i][j] = inverted[i][j]/inverted[i][i];             
              }
         }
//-------------------------------------------------------------------
     for(i=0;i<size;i++){
         for(j=0;j<size;j++){
             finalmatrix[i][j]=inverted[i][j+size];                 //return only the second half of the augmented matrix
         }
     }    
			
    return finalmatrix;
}

int main(int argc, char *argv[]){
    double** datamatrix;                            //create all matrices to be used later on
    double** transposed;
    double** transposetimesx;
    double** inverted;
    double** inversetimestranspose;
    double** prices;
    double** wmatrix;
    double** augmentedmatrix;
    int attributes;                                 //create values to hold number of values, number of training sets, and iterators
    int sets;
    int i,j;
    FILE* trainingfile = fopen(argv[1], "r"); 
    fscanf(trainingfile, "%d", &attributes); 
	fscanf(trainingfile, "%d", &sets);

    datamatrix = (double**)malloc(sizeof(double*) * (sets));                                                //allocate memory for rows of all matrices
    prices = (double**)malloc(sizeof(double*) * (sets));
    transposed = (double**)malloc(sizeof(double*) * (attributes+1));
    transposetimesx = (double**)malloc(sizeof(double*) * (attributes+1));
    inverted = (double**) malloc(sizeof(double*) * (attributes+1));
    inversetimestranspose = (double**) malloc(sizeof(double*) * (attributes+1));
    wmatrix = (double**) malloc(sizeof(double*) * (attributes+1));
    augmentedmatrix = (double**) malloc(sizeof(double*) * (attributes+1));


        for (i = 0; i < sets; i++) {                                                                        //allocate memory for columns of data holding matrices
            datamatrix[i] = malloc((attributes+1)*sizeof(double));
            prices[i] = malloc(sizeof(double));
         }

         for(i=0;i<attributes+1;i++){                                                                       //allocate memory for colums of all other matrices
             transposed[i] = malloc((sets) *sizeof(double));
             transposetimesx[i] = malloc((attributes+1) *sizeof(double));
             inverted[i] = malloc((attributes+1) *sizeof(double));
             inversetimestranspose[i] = malloc((sets) * sizeof(double));
             wmatrix[i] = malloc(sizeof(double));
             augmentedmatrix[i] = malloc(((attributes+1)*2) * sizeof(double));
         }

    int size = (attributes+1)*(sets);                                       //this is a horrible way to do this but it work
    double numbers[size];                                                   //load all possible values into an array
    for (i = 0; i < size; i++){                                             //go through and put every (attribute+1) value into prices
        fscanf(trainingfile, "%lf,", &numbers[i] );                         //put the rest into another array
    }                                                                       //fill the datamatrix with that array
    int index = 0;
    for(int i=0;i<size;i++){
        if(i%(attributes+1)==0){
            prices[index][0]=numbers[i];
            index++;
        }
    }
    int numcounter=0;
    double shit[size-sets];
    for(i=0;i<size;i++){
        if(i%(attributes+1)==0){
            continue;
        }
        else{
            shit[numcounter]=numbers[i];
            numcounter++;
        }

    }
    int counter=0;
    for(i=0;i<sets;i++){
        for(j=0;j<attributes+1;j++){
            if(j==0){
                datamatrix[i][j]=1;
            }
            else{
                datamatrix[i][j]=shit[counter];
                counter++;
            }
        }
    }

    transposed = transpose(datamatrix, transposed, sets,attributes+1);                      //calculate the transpose of the first matrix
    transposetimesx = multiply(transposed,datamatrix,transposetimesx,attributes+1, sets, sets, attributes+1);       //multiply it by itself
    int size2=(attributes+1);                                                                 //create the augmented matrix by adding ones and zeros where needed
     for(i=0;i<size2;i++){
        for(j=0;j<size2;j++){
            augmentedmatrix[i][j]=transposetimesx[i][j];
        }
    }  

    for(int i=0;i<size2;i++){
        for(int j=0;j<size2*2;j++){
            if(j==(i+size2)){
                augmentedmatrix[i][j]=1;
            }
        }
    }
    inverted = invert(augmentedmatrix,inverted,attributes+1);                           //invert the augmented matrix
    inversetimestranspose = multiply(inverted,transposed,inversetimestranspose, attributes + 1, attributes + 1, attributes + 1, sets);  //multiply it by the transpose
    wmatrix = multiply(inversetimestranspose,prices,wmatrix,attributes+1, sets, sets, 1);           //calculate the weights

    FILE* file2 = fopen(argv[2], "r");
    int examples;
	fscanf(file2, "%d", &examples);
    double wtimex=0;
    double scanned;
    for(i=0;i<examples;i++){                                                                //we will need to do this (examples) number of times
        wtimex=wmatrix[0][0];                                                               //w0 has no coefficient in the equation
        for(j=1;j<attributes+1;j++){
            fscanf(file2,"%lf,",&scanned);                                                  //loop through the weights matrix and multiply and add every attributes+1 times
            wtimex+=wmatrix[j][0]*scanned;
        }
        printf("%0.0lf\n", wtimex);                                                         //print the calculate value in the given format
    }

	 for (i = 0; i < sets; i++) {                                                           //free all matrices opposite the way they were allocated
        free(datamatrix[i]);
		free(prices[i]);
  }

	for (i = 0; i < attributes+1; i++) {
		free(transposed[i]);
		free(transposetimesx[i]);
		free(inverted[i]);
		free(inversetimestranspose[i]);
		free(wmatrix[i]);
        free(augmentedmatrix[i]);
	}

    free(datamatrix);
    free(prices);
    free(transposed);
	free(transposetimesx);
	free(inverted);
	free(inversetimestranspose);
	free(wmatrix);
    free(augmentedmatrix);


    fclose(trainingfile);                                               //close both files
    fclose(file2);


    
    return 0;
}