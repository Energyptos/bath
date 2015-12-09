#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#include "P5pgmReader.h"

float *allocate_dynamic_matrix(int row, int col)
{
	float *ret_val;
    //int i;

    ret_val = (float *)malloc(sizeof(float *) * row * col);
    if (ret_val == NULL) {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }

    /*for (i = 0; i < row; ++i) {
        ret_val[i] = (float *)malloc(sizeof(float) * col);
        if (ret_val[i] == NULL) {
            perror("memory allocation failure");
            exit(EXIT_FAILURE);
        }
    }*/

    return ret_val;
}


void deallocate_dynamic_matrix(float *matrix, int row)
{
    int i;

    /*for (i = 0; i < row; ++i)
        free(matrix[i]);*/
    free(matrix);
}


void SkipComments(FILE *fp)
{
    int ch;
    char line[100];

    while ((ch = fgetc(fp)) != EOF && isspace(ch))
        ;
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        SkipComments(fp);
    } else
        fseek(fp, -1, SEEK_CUR);
}


PGMDataFloat* readPGM(const char *file_name, PGMDataFloat *data)
{
    FILE *pgmFile;
    char version[3];
    int i, j;
    int temp;

    pgmFile = fopen(file_name, "rb");
    if (pgmFile == NULL) {
        perror("cannot open file to read");
        exit(EXIT_FAILURE);
    }

    fgets(version, sizeof(version), pgmFile);
    if (strcmp(version, "P2")) {
        fprintf(stderr, "Wrong file type!\n");
        exit(EXIT_FAILURE);
    }

    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->col);
    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->row);
    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->max_gray);
    fgetc(pgmFile);

    data->matrix = allocate_dynamic_matrix(data->row, data->col);
        for (i = 0; i < data->row; ++i){
            for (j = 0; j < data->col; ++j) {
                fscanf(pgmFile, "%d", &temp);
                //printf("%d\n", temp);
                data->matrix[i * data->col + j] = temp;
            }
            //printf("\n");
		}
    fclose(pgmFile);
    return data;
}


/*and for writing*/
void writePGM(const char *filename, const PGMDataFloat *data)
{
    FILE *pgmFile;
    int i, j;
    int temp;

    pgmFile = fopen(filename, "wb");
    if (pgmFile == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }

    fprintf(pgmFile, "P2 ");
    fprintf(pgmFile, "%d %d ", data->col, data->row);
    fprintf(pgmFile, "%d ", data->max_gray);

    for (i = 0; i < data->row; ++i){
    	for (j = 0; j < data->col; ++j) {
    		temp = data->matrix[i * data->col + j];
    		fprintf(pgmFile, "%d\n", temp);
       }
    }

    fclose(pgmFile);
    //deallocate_dynamic_matrix(data->matrix, data->row);
}

void printMatrixPicutre(PGMDataFloat* pgmpicture){
	int i, j;
	float lo;
    for (i = 0; i < pgmpicture->row; ++i){
        for (j = 0; j < pgmpicture->col; ++j) {
            lo = pgmpicture->matrix[i * pgmpicture->col + j];
            printf("%f ", lo);
        }
        printf("\n");
    }
	printf("\n\n\n");
}

PGMDataFloat* resize(PGMDataFloat* pgmpicture){
	//PGMDataFloat *pgmFloatTemp = {pgmpicture->row, pgmpicture->col, pgmpicture->max_gray,
		//							allocate_dynamic_matrix(pgmpicture->row, pgmpicture->col)};
	PGMDataFloat* pgmFloatTemp = (PGMDataFloat*)malloc(sizeof(PGMDataFloat));
	pgmFloatTemp->col = pgmpicture->col;
	pgmFloatTemp->row = pgmpicture->row;
	pgmFloatTemp->max_gray = pgmpicture->max_gray;
	pgmFloatTemp->matrix = allocate_dynamic_matrix(pgmpicture->row, pgmpicture->col);
	int i, j;
	int lo;
    for (i = 0; i < pgmpicture->row; ++i){
        for (j = 0; j < pgmpicture->col; ++j) {
        	lo = pgmpicture->matrix[i * pgmpicture->col + j];
        	//printf("%f ", lo);
        	float temp = (float)lo / 255;
        	//printf("%f ", temp);
        	pgmFloatTemp->matrix[i * pgmpicture->col + j] =  temp;
        	//printf("%d ", pgmpicture->matrix[i * data->col + j]);
        }
        //printf("\n");
    }
    return pgmFloatTemp;
}


PGMDataFloat* resizeBack(PGMDataFloat* pgmpicture){
	PGMDataFloat* pgmFloatTemp = (PGMDataFloat*)malloc(sizeof(PGMDataFloat));
	pgmFloatTemp->col = pgmpicture->col;
	pgmFloatTemp->row = pgmpicture->row;
	pgmFloatTemp->max_gray = pgmpicture->max_gray;
	pgmFloatTemp->matrix = allocate_dynamic_matrix(pgmFloatTemp->row, pgmFloatTemp->col);
	int i, j;
	float lo;
    for (i = 0; i < pgmpicture->row; ++i){
        for (j = 0; j < pgmpicture->col; ++j) {
        	lo = pgmpicture->matrix[i * pgmpicture->col + j];
        	//printf("%f ", lo);
        	float temp = (float)lo * 255;
        	//printf("%f ", temp);
        	pgmFloatTemp->matrix[i * pgmpicture->col + j] =  temp;
        	//printf("%d ", pgmpicture->matrix[i * pgmpicture->col + j]);
        }
        //printf("\n");
    }
    return pgmFloatTemp;
}


PGMDataFloat* resizeAndInvertProbabilities(PGMDataFloat* pgmpicture){
	PGMDataFloat* pgmFloatTemp = (PGMDataFloat*)malloc(sizeof(PGMDataFloat));
	pgmFloatTemp->col = pgmpicture->col;
	pgmFloatTemp->row = pgmpicture->row;
	pgmFloatTemp->max_gray = pgmpicture->max_gray;
	pgmFloatTemp->matrix = allocate_dynamic_matrix(pgmFloatTemp->row, pgmFloatTemp->col);
	int i, j;
	int lo;
    for (i = 0; i < pgmpicture->row; ++i){
        for (j = 0; j < pgmpicture->col; ++j) {
        	lo = pgmpicture->matrix[i * pgmpicture->col + j];
        	lo = 255 - lo;
        	//printf("%f ", lo);
        	float temp = (float)lo / 255;
        	//printf("%f ", temp);
        	pgmFloatTemp->matrix[i * pgmFloatTemp->col + j] =  temp;
        	//printf("%d ", pgmpicture->matrix[i * pgmpicture->col + j]);
        }
        //printf("\n");
    }
    return pgmFloatTemp;
}


PGMDataFloat* resizeAndInvertProbabilitiesBackToPicture(PGMDataFloat* pgmpicture){
	PGMDataFloat* pgmFloatTemp = (PGMDataFloat*)malloc(sizeof(PGMDataFloat));
	pgmFloatTemp->col = pgmpicture->col;
	pgmFloatTemp->row = pgmpicture->row;
	pgmFloatTemp->max_gray = pgmpicture->max_gray;
	pgmFloatTemp->matrix = allocate_dynamic_matrix(pgmFloatTemp->row, pgmFloatTemp->col);
	int i, j;
	float lo;
    for (i = 0; i < pgmpicture->row; ++i){
        for (j = 0; j < pgmpicture->col; ++j) {
        	lo = pgmpicture->matrix[i * pgmpicture->col + j];
        	lo = 255 * lo;
        	//printf("%f ", lo);
        	float temp = 255 - lo;
        	//printf("%f ", temp);
        	pgmFloatTemp->matrix[i * pgmFloatTemp->col + j] =  temp;
        	//printf("%d ", pgmpicture->matrix[i * pgmpicture->col + j]);
        }
        //printf("\n");
    }
    return pgmFloatTemp;
}

/*int main(int argc, char **argv){
	//PGMDataFloat pgmpicture = {321, 481, 255, NULL};
	PGMDataFloat pgmpicture;
	PGMDataFloat* pgmpicture2;
	pgmpicture2 = readPGM("init.pgm", &pgmpicture);
	//printMatrixPicutre(pgmpicture2);
	//writePGM("testOut.pgm", pgmpicture2);
	//printMatrixPicutre(pgmpicture2);
	//pgmpicture2 = resizeAndInvertProbabilities(pgmpicture2);
	pgmpicture2 = resize(pgmpicture2);
	printMatrixPicutre(pgmpicture2);
 	//pgmpicture2 = resizeAndInvertProbabilitiesBackToPicture(pgmpicture2);
	pgmpicture2 = resizeBack(pgmpicture2);
 	printMatrixPicutre(pgmpicture2);
}*/
