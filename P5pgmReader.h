/*
 * P5pgmReader.h
 *
 *  Created on: 28.10.2015
 *      Author: fickenscher
 */

#ifndef P5PGMREADER_H_
#define P5PGMREADER_H_

typedef struct {
    int row;
    int col;
    int max_gray;
    float *matrix;
} PGMDataFloat;

float *allocate_dynamic_matrix(int row, int col);
void deallocate_dynamic_matrix(float *matrix, int row);
void SkipComments(FILE *fp);
PGMDataFloat* readPGM(const char *file_name, PGMDataFloat *data);
void writePGM(const char *filename, const PGMDataFloat *data);
void printMatrixPicutre(PGMDataFloat* pgmpicture);
PGMDataFloat* resize(PGMDataFloat* pgmpicture);
PGMDataFloat* resizeBack(PGMDataFloat* pgmpicture);
PGMDataFloat* resizeAndInvertProbabilitiesBackToPicture(PGMDataFloat* pgmpicture);
PGMDataFloat* resizeAndInvertProbabilities(PGMDataFloat* pgmpicture);

#endif /* P5PGMREADER_H_ */
