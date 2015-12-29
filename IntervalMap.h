#include "ICList.h"
typedef struct{
	float y;		//lateral position
	float xPosition;	//position inside the interval (longitudinal direciton)
	float state;  //0=free; 0.5=Unkown; 1=Occupied;	
}PC;


IC* accessIC(IC** ICArray,int index);
void setIC(IC** ICArray,int index,IC* setvalue);
void printPC(PC* pc);//currently not in use
void processData(float* data, int x, int y, PC** pcArray, int col, int row);

IC* compressPCRow(PC* pcarray,int size,IC* icOut); //returns Head of row!

void printIC(IC** icArray,int size); //not in use
void writeICtoPGM(IC** icArray, int row_size, int col_size,const char *outputFileName);
IC** convertPgm(const char *inputFileName,const char *outputFileName);  //does the whole process
void freeICArray(IC** array,int size);





