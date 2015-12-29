#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "IntervalMap.h"
#include "P5pgmReader.h"



#define ROWS_IN_INTERVAL 8
#define DATA_ROWS 640
#define DATA_COLUMNS 8
#define EPSILON 0.00001

#define VISUAL 0



	//static IC** curICArray;
	static int curArrayWidth;
	static int curICArraySize;
	static int shiftProgress;
	static float shiftOffset;
	
	static IC** curICArray;


void printPC(PC* pc){
	//printf("PC has lateral position y: %f   ;\n xposition inside is: %f    ;\n and the state is %d   \n\n\n",pc->y,pc->xPosition,pc->state);
}

void processData(float* data, int x, int y, PC** pcArray, int col, int row){
 	//printf(".");
	float maxValue=0;
	float maxValuesX=0; //describes the xValue of the maxValue.
	for(int rowOffset=0; rowOffset<ROWS_IN_INTERVAL; rowOffset++){
	//printf(" %.1f ",data[((x+rowOffset)*col+y)]);
		if(maxValue < data[(x+rowOffset)*col+y]){
		//printf("updated");
			maxValue=data[(x+rowOffset)*col+y];   //update maxValue
			maxValuesX=x+rowOffset;
		}
	}
	int scaledX=x/ROWS_IN_INTERVAL;
	pcArray[scaledX][y].y=y;								//seems to make no sense, but: pcArray[y] will contain the weighed y value. (y wont be an integer.)
	
	if(maxValue>0.6){
		pcArray[scaledX][y].state=maxValue;	
		if(maxValue<0 || maxValue>1)printf("maxValue= %f\n\n\n",maxValue);	
		//printf("state one is put!!!!\n");
	}
	else if(maxValue<0.3)pcArray[scaledX][y].state=0;
	//else if(maxValue<0)pcArray[scaledX][y].state=0;
	else pcArray[scaledX][y].state=maxValue;
	pcArray[scaledX][y].xPosition=maxValuesX;
}
	
IC* compressPCRow(PC* pcarray,int size,IC* icOut){

	icOut = insertAfter(NULL);
	IC* runPtr=icOut;


	if(runPtr==NULL)exit(EXIT_FAILURE);
	
	int yBegin=0;

	for(int y=1;y<size;y++){
	//iterate over the whole pcarray in this row -> looking for repeating states
	//to create interval-cells

		if(pcarray[yBegin].state != pcarray[y].state){    
			
			runPtr->yStart=yBegin;

			runPtr->yEnd=y-1;
						//	printf("yEnd %.0f vs %d y-1 \n",runPtr->yEnd,y-1);
			runPtr->state=pcarray[yBegin].state;
			yBegin=y;

			runPtr=insertAfter(runPtr);
			
		}
		if(y==size-1){
			//we have to add this to the array too!
			runPtr->yStart=yBegin;
			runPtr->yEnd=y;
			runPtr->state=pcarray[yBegin].state;
			yBegin=y;

		}
	}
	return icOut;
}


void writeICtoPGM(IC** icArray, int row_size, int col_size,const char *outputFileName){
	float * file = allocate_dynamic_matrix(row_size, col_size);
	
	for(int row=0;row<row_size;row++){
		IC* runPtr=accessIC(icArray,row/ROWS_IN_INTERVAL);
		for(int z=0;z<col_size;runPtr=runPtr->next){
			if(runPtr==NULL){
				fprintf(stderr,"data structure is not dense - there is not everything ; \n col=%d   row = %d\n\n",z,row/ROWS_IN_INTERVAL);
				printList(icArray[row/ROWS_IN_INTERVAL]);
			}
			
			float dif=(runPtr->yEnd)-(runPtr->yStart);
/*							printf("runPtr->yStart %f, runPtr->yEnd= %f\n",runPtr->yStart,runPtr->yEnd);*/
			for(int y=0;y<=dif;y++){
				//for(int offset=0;offset<ROWS_IN_INTERVAL;offset++)

				file[(row)*col_size+z]=runPtr->state;
				z++;
			}
		}
	}
	PGMDataFloat* pgmOut = (PGMDataFloat*)malloc(sizeof(PGMDataFloat));
	pgmOut->col=col_size;
	pgmOut->row=row_size;
	pgmOut->matrix=file;
	pgmOut->max_gray=255;
	
	PGMDataFloat* tmp = resizeAndInvertProbabilitiesBackToPicture(pgmOut);
	
	writePGM(outputFileName, tmp);
	deallocate_dynamic_matrix(tmp->matrix,tmp->row);
	free(tmp);
	tmp=NULL;
	free(file);
	file=NULL;
	free(pgmOut);
	pgmOut=NULL;
}





IC** convertPgm(const char *inputFileName,const char *outputFileName){
	PGMDataFloat pgmpicture;
	PGMDataFloat* pgmpicture2;
	pgmpicture2 = readPGM(inputFileName, &pgmpicture);
	pgmpicture2 = resizeAndInvertProbabilities(pgmpicture2);
	float* matrix=pgmpicture2->matrix;
	//we have a matrix with collumn based storage imported from a given pgm-file
	

	
	IC** newICArray=(IC**)malloc(sizeof(IC*)*pgmpicture2->row/ROWS_IN_INTERVAL);
	if(curICArraySize==0){
		printf("Initialize!\n");
		curICArraySize=pgmpicture2->row/ROWS_IN_INTERVAL;
		curArrayWidth=pgmpicture2->col;
	}

	
	PC **pcarray = (PC**)malloc(sizeof(PC*)*pgmpicture2->row/ROWS_IN_INTERVAL);//*sizeof(PC)*DATA_COLUMNS);  
	for(int x=0; x<pgmpicture2->row; x=x+ROWS_IN_INTERVAL){
		pcarray[x/ROWS_IN_INTERVAL]=(PC*)malloc(sizeof(PC)*pgmpicture2->col);     //in pcarray[x] i want to save DATA_COLUMNS times a pc* value
		for(int y=0;y<pgmpicture2->col;y++){
			processData(matrix,x,y,pcarray,pgmpicture2->col,pgmpicture2->row);
		}
	}
	
	
	for(int x=0; x<pgmpicture2->row/ROWS_IN_INTERVAL; x++){
		newICArray[x]=compressPCRow(pcarray[x],pgmpicture2->col,newICArray[x]);
	}
	
	
//	printf("achtung: yEnd: %.0f, yStart: %.0f",accessIC(newICArray,0)->yEnd,accessIC(newICArray,0)->yStart);
//	printf("----> pointer should be : %p",newICArray[0]);

	writeICtoPGM(newICArray,pgmpicture2->row,pgmpicture2->col,outputFileName);
	
	
	for(int x=0; x<pgmpicture2->row/ROWS_IN_INTERVAL; x++){
		free(pcarray[x]);
		pcarray[x]=NULL;
		//free(newICArray[x]);
	}
	free(pcarray);
	pcarray=NULL;
	
	
	deallocate_dynamic_matrix(pgmpicture2->matrix,pgmpicture2->row);
	free(pgmpicture2);
	pgmpicture2=NULL;
	
	deallocate_dynamic_matrix(pgmpicture.matrix,pgmpicture.row);
	return newICArray;
	
}


IC* accessIC(IC** ICArray,int index){
	if(ICArray==NULL)return NULL;
	return ICArray[index];
/*	return ICArray[abs(curICArraySize+index-shiftProgress)%curICArraySize];*/
}

void setIC(IC** ICArray,int index,IC* setvalue){
	ICArray[index]=setvalue;
/*	ICArray[abs(curICArraySize+index-shiftProgress)%curICArraySize]=setvalue;*/
}



void rotatePGMData(PGMDataFloat* pgmData,float angle,int centerRow){//centerRow describes the row which will be the center of the rotation!
	int row=pgmData->row;
	int col=pgmData->col;
	float* matrix=pgmData->matrix;
	//we have to rotate this image around the middle of the bottom corner of the picture;
	//access= [r*col+c];
	float* newMatrix=allocate_dynamic_matrix(row,col);
	int dif_col=col/2;
	
	for(int r=0;r<row;r++){
		for(int c=0; c<col; c++){
			newMatrix[r*col+c]=255;
		}
	}
	
	
	for(int r=0;r<row;r++){
		for(int c=0; c<col; c++){
			int tmpC=c-dif_col;
			int tmpR=r-centerRow;
			int newC=round(cos(angle)*tmpC-sin(angle)*tmpR);
			newC=newC+dif_col;
			int newR=round(sin(angle)*tmpC+cos(angle)*tmpR);
			newR=newR+centerRow;
			//printf("c=%d   ;tmpC=%d ; newC= %d   ; cos(angle)*tmpC= %f\n",c,tmpC,newC,cos(angle)*tmpC);
			//printf("r=%d   ;tmpR=%d ; newR= %d   ; sin(angle)= %f\n",r,tmpR,newR,cos(angle));
			if(newR<row && 0<=newR && newC<col && 0<=newC){
				newMatrix[newR*col+newC]=matrix[r*col+c];
			}			
			
		}
	}
	deallocate_dynamic_matrix(pgmData->matrix,row);
	pgmData->matrix=newMatrix;
	
}


float b(float x1, float y1, float x2, float y2){
	float top=(x2*y1-x1*y2);
	float bot=(x2-x1);
	if(bot!=0){
		float ret=top/bot;
		return ret;
	}
	bot=EPSILON;
	
	fprintf(stderr,"b calculation failed! (bot=%f) , x1=%f : x2=%f \n",bot,x1,x2);
	return top/bot;
}

float m(float x1, float y1, float x2, float y2){
	float top= (y2-y1);
	float bot= (x2-x1);
	if(bot!=0){
		float ret= top/bot;
		return ret;
	}
	bot=EPSILON;
	fprintf(stderr,"m calculation failed! \n");
	return top/bot;
}

void rot(float* x, float* y, float angle, int xOffset, int yOffset){
	float tmpX=*x-xOffset;
	float tmpY=*y-yOffset;
/*	printf("tmpY=%f ;;; tmpX =%f\n",tmpY,tmpX);*/
	float newX=(cos(angle)*(tmpX)-sin(angle)*tmpY);
	*x=newX+xOffset;
	float newY=(sin(angle)*(tmpX)+cos(angle)*tmpY);
/*	printf("newy=%f ;;; newX =%f\n",newY,newX);	*/
	*y=newY+yOffset;
/*	printf("y=%f ;;; x =%f\n\n ",*y,*x);*/
	
}


IC** makeNewWhite(int size,int width){
	IC** newArray=malloc(sizeof(IC*)*size);
	if(newArray==NULL){
		fprintf(stderr,"malloc failed.\n");
		exit(EXIT_FAILURE);
	}
	for(int x=0;x<size;x++){
		newArray[x]=insertAfter(NULL);
		newArray[x]->state=0;
		newArray[x]->yStart=0;
		newArray[x]->yEnd=width-1;
	}
	return newArray;

}

typedef struct line{
	float b;
	float m;
	float yMin;
	float yMax;
}Line;


float computeStartX(Line line,int exactInterval,int width){
	float startX=width;
	//step 1: check value range 
	//1.1: wenn bei exactInterval+ROWS_IN_INTERVAL durchgeht: dann oben messen
	if(line.yMin<(exactInterval) && line.yMax>(exactInterval)){
/*		printf("\n ( %d + rowsInInterval - %f)/%f\n\n",exactInterval,line.b,line.m);*/
		startX=(exactInterval-line.b)/line.m;
/*		printf("startx= %f",startX);*/
	}
/*	printf("1. startX = %f\n",startX);*/
	//1.2: wenn es auch unten durch geht, da auch nachsehen, ob vielleicht kleiner/groeser
	if(line.yMin<(exactInterval+ROWS_IN_INTERVAL) && line.yMax>(exactInterval+ROWS_IN_INTERVAL)){
		if(startX>(exactInterval+ROWS_IN_INTERVAL-line.b)/line.m) 
			startX=(exactInterval+ROWS_IN_INTERVAL-line.b)/line.m;
	}
/*		printf("2. startX = %f\n",startX);*/
	//wenn es zwischen drin liegt: schauen ob der eckpunkt weiter drausssen liegt
	if(line.yMin>exactInterval && line.yMin<exactInterval+ROWS_IN_INTERVAL){
		if(startX>(line.yMin-line.b)/line.m)
			startX=(line.yMin-line.b)/line.m;
	}
	if(line.yMax>exactInterval && line.yMax<exactInterval+ROWS_IN_INTERVAL){
		if(startX>(line.yMax-line.b)/line.m)
			startX=(line.yMax-line.b)/line.m;
	}	
	if(startX<0)startX=0;
/*		printf("3. startX = %f\n",startX);*/
	return startX;
}
float computeEndX(Line line, int exactInterval,int width){
	float endX=0.;
	//step 1: check value range 
	//1.1: wenn bei exactInterval+ROWS_IN_INTERVAL durchgeht: dann oben messen
	if(line.yMin<(exactInterval) && line.yMax>(exactInterval)){
		endX=(exactInterval-line.b)/line.m;
	}
	//1.2: wenn es auch unten durch geht, da auch nachsehen, ob vielleicht kleiner/groeser
	if(line.yMin<(exactInterval+ROWS_IN_INTERVAL) && line.yMax>(exactInterval+ROWS_IN_INTERVAL)){
		if(endX<(exactInterval-line.b+ROWS_IN_INTERVAL)/line.m) 
			endX=(exactInterval-line.b+ROWS_IN_INTERVAL)/line.m;
	}
	//wenn es zwischen drin liegt: schauen ob der eckpunkt weiter drausssen liegt
	if(line.yMin>exactInterval && line.yMin<exactInterval+ROWS_IN_INTERVAL){
		if(endX<(line.yMin-line.b)/line.m)
			endX=(line.yMin-line.b)/line.m;
	}
	//wenn es zwischen drin liegt: schauen ob der eckpunkt weiter drausssen liegt
	if(line.yMax>exactInterval && line.yMax<exactInterval+ROWS_IN_INTERVAL){
		if(endX<(line.yMax-line.b)/line.m)
			endX=(line.yMax-line.b)/line.m;
	}
	if(endX>=width)endX=width-1;
	return endX;
}

void shiftStructure(IC** curICArray,int stepsize,int width){
	int shiftSize = stepsize/ROWS_IN_INTERVAL;
	shiftOffset=shiftOffset+(stepsize%ROWS_IN_INTERVAL)/ROWS_IN_INTERVAL;
	if(shiftOffset>1){
		shiftOffset-=1;
		shiftSize++;
	};
	deleteWholeList(curICArray[curICArraySize-1]);
	for(int shift=0;shift<shiftSize;shift++){
		for(int x=curICArraySize-1;x>=1;x--){
			curICArray[x]=curICArray[x-1];
/*				tidyUpList(curICArray[x]);*/
/*				printf("%d ",x-1);*/
/*				printList(curICArray[x]);*/
		}
/*		deleteWholeList(curICArray[0]); //free lowest row, we dont need it anymore*/
		curICArray[0]=NULL;
		curICArray[0]=insertAfter(NULL);
		curICArray[0]->yStart=0;
		curICArray[0]->yEnd=width-1;
		curICArray[0]->state=0.2;
	}
}


void rotateStructure(IC** icArray, float angle, int width, int height,IC** whiteICArray){


//TODO: USE accessIC instead of normal access!!!
	if(angle==0.){
		for(int x=0;x<curICArraySize;x++){
			free(whiteICArray[x]);
		}
		free(whiteICArray);
		whiteICArray=NULL;
		return;
	}
	
	for(int i=0;i<curICArraySize;i++){
	//	printf("durchlauf!!!!!!!\n\n");
		for(IC** runArray=icArray;accessIC(runArray,i)!=NULL;setIC(runArray,i,accessIC(runArray,i)->next)){
	//		printf("state= %f\n",runArray[i]->state);
			if(accessIC(runArray,i)->state < 0.4)continue;
			//step 1: get the corner coordinates:
			float tlX,tlY,blX,blY,trX,trY,brX,brY;  //tl=topLeft , bl=bottomLeft, tr=topright....
			
			tlX= accessIC(runArray,i)->yStart;
			tlY=i * ROWS_IN_INTERVAL;   //was curICArraySize-i)*ROWS_IN_INTERVAL
			trY= tlY;
			trX= accessIC(runArray,i)->yEnd;
			blX= tlX;
			blY= tlY+ROWS_IN_INTERVAL;
			brY= blY;
			brX= trX;
			
			//step 2: rotate those around the bottom middle point!--> width/2!
/*						printf("%f ; %f ; %f ; %f ; %f ; %f ; %f ; %f ; \n", tlX,tlY,blX,blY,trX,trY,brX,brY);*/
			rot(&tlX,&tlY,angle,round(width/2),height);
			rot(&blX,&blY,angle,round(width/2),height);
			rot(&trX,&trY,angle,round(width/2),height);
			rot(&brX,&brY,angle,round(width/2),height);
			
			//step 3: create 4 straight lines with the points and mark intervals
			Line* lines=(Line*)malloc(sizeof(Line)*4);
			if(lines==NULL){
				fprintf(stderr,"malloc failed!\n");
				exit(EXIT_FAILURE);
			}
/*			printf("%f ; %f ; %f ; %f ; %f ; %f ; %f ; %f ; \n", tlX,tlY,blX,blY,trX,trY,brX,brY);*/
			lines[0].b=b(tlX,tlY,trX,trY);
			lines[0].m=m(tlX,tlY,trX,trY);
			lines[0].yMin=fmin(tlY,trY);
			lines[0].yMax=fmax(tlY,trY);
			
			lines[1].b=b(tlX,tlY,blX,blY);
			lines[1].m=m(tlX,tlY,blX,blY);
			lines[1].yMin=fmin(tlY,blY);
			lines[1].yMax=fmax(tlY,blY);
			
			lines[2].b=b(blX,blY,brX,brY);
			lines[2].m=m(blX,blY,brX,brY);
			lines[2].yMin=fmin(blY,brY);
			lines[2].yMax=fmax(blY,brY);
			
			lines[3].b=b(brX,brY,trX,trY);
			lines[3].m=m(brX,brY,trX,trY);
			lines[3].yMin=fmin(brY,trY);
			lines[3].yMax=fmax(brY,trY);
			
			int maxY=lines[0].yMax;
			int minY=lines[0].yMin;
			for (int i = 1; i < 4; i += 1){
				if(lines[i].yMax>maxY)maxY=lines[i].yMax;
				if(lines[i].yMin<minY)minY=lines[i].yMin;
			}
			//step 3.1: look at all intervals between yMin and yMax!
			
			//TRY to make a picture with the lines in it!
			
			
			
			
/*			printf("from = %d :_ to %d\n\n",(int)fmax((minY/ROWS_IN_INTERVAL),0),(int)fmin(maxY/ROWS_IN_INTERVAL,curICArraySize) );*/
			for(int interval=fmax(floor(minY/ROWS_IN_INTERVAL-4),0);interval<fmin(ceil(maxY/ROWS_IN_INTERVAL),curICArraySize) ;interval++){
/*				printf("interval= %d\n",interval);*/
				float startX=width;
				float endX=0.;
				for(int i = 0; i < 4; i += 1){
					float tmpStartX=computeStartX(lines[i],interval*ROWS_IN_INTERVAL,width);
/*					printf("startx= %d ; at i= %d\n\n",tmpStartX,i);*/
					float tmpEndX=	computeEndX(lines[i],interval*ROWS_IN_INTERVAL,width);
					if(startX>tmpStartX)startX=tmpStartX;
					if(endX<tmpEndX)endX=tmpEndX;
				}

				if(startX<endX && startX!=endX){
					insertInto(whiteICArray[interval],startX,endX,runArray[i]->state);
				}
				tidyUpList(whiteICArray[interval]);			//check structure for slow gaps, which came from rotating everything
				if(interval==1){
					printf("                       %d ",interval);
					printList(accessIC(whiteICArray,interval));
				}
			}
			
			
			free(lines);
			lines=NULL;
		}
	}
	freeICArray(curICArray,curICArraySize);
	curICArray=whiteICArray;
	//free(whiteICArray);
	//whiteICArray=NULL;


}



IC* associateBoarder(IC** map,IC* icMeas,int curIndex){
	float startDev,endDev,stateDev;
	
	for(IC* icMap=map[curIndex];icMap!=NULL;icMap=icMap->next){
		startDev=fabs(icMap->yStart-icMeas->yStart)/curArrayWidth;
		endDev=fabs(icMap->yEnd-icMeas->yEnd)/curArrayWidth;
		stateDev= fabs(icMap->state-icMeas->state);
		if(startDev+endDev<0.1 && stateDev<0.1){
			//associated!
/*			if(curIndex==25){*/
/*			printf("startDev %f, endDev= %f,stateDev= %f\n",startDev,endDev,stateDev);*/
/*			printf("associated! ->\n icMap start: %.1f; end: %.1f; state: %.1f",icMap->yStart,icMap->yEnd,icMap->state);}*/
			return icMap;
		}
	}

/*	printf("notAssociated!\n");*/
/*	printf("startDev %f, endDev= %f,stateDev= %f\n icMeas->yStart = %f ; icMeas->yEnd= %f ; state= %f\n\n",startDev,endDev,stateDev,icMeas->yStart,icMeas->yEnd,icMeas->state);*/
	return NULL;
}

void updateBoarder(IC* icAssoc,IC* icMeas,int x){
	//we should compute the kallman filter update step!
	//but we dont have sigma or other things we need. so i'll weigh it in my own style
/*	if(x==10){*/
/*		printf("\nUPDATING : icAssoc->ystart= %f,  icMeas_>start= %f \nicAssoc->yend= %f,  icMeas_>end= %f\n",icAssoc->yStart,icMeas->yStart,icAssoc->yEnd,icMeas->yEnd);*/
/*	}*/
	if(icAssoc->yEnd!=curArrayWidth-1) icAssoc->yEnd=(icAssoc->yEnd*0.4+icMeas->yEnd*0.6);
	if(icAssoc->next!=NULL)icAssoc->next->yStart=icAssoc->yEnd;
	if(icAssoc->yStart!=0)icAssoc->yStart=(icAssoc->yStart*0.4+icMeas->yStart*0.6);	
	if(icAssoc->prev!=NULL)icAssoc->prev->yEnd=icAssoc->yStart;
	
	
/*	if(x==10){*/
/*		printf("\nIT IS NOW:: icAssoc->ystart= %f, icAssoc->yend= %f,  \n\n",icAssoc->yStart,icAssoc->yEnd);*/
/*		if(icAssoc->prev!=NULL)*/
/*		printf("\nPREV IS NOW:: icAssoc->prev->ystart= %f, icAssoc->prev->yend= %f,  \n\n",icAssoc->prev->yStart,icAssoc->prev->yEnd);*/
/*	}*/
}

void updateCell(IC* icMap,IC* icMeas){
	//we calculate the new occupancy probability!
	float pt=icMeas->state;
	float p1t=icMap->state;
	if(pt<EPSILON)pt=EPSILON;
	if(pt>1-EPSILON)pt=1-EPSILON;
	if(p1t<EPSILON)p1t=EPSILON;
	if(p1t>1-EPSILON)p1t=1-EPSILON;
	
	
	float newP=pt*p1t/((1-pt)*(1-p1t)+pt*p1t);  //equation 14 in "Interval based representation of occupancy information ...0"
	icMap->state=newP;
	
}



void assocAndUpdate(IC** map, IC** measurement){
	for(int x=0;x<curICArraySize;x++){
		IC* icMap=map[x];
/*		if(x==25){*/
/*		printf("BEFORE 25!!");*/
/*		printList(map[x]);*/
/*				printf("Meas at 25 \n\n");*/
/*		printList(measurement[x]);}*/
		for(IC* icMeas=measurement[x];icMeas!=NULL;icMeas=icMeas->next){
			IC* icAssoc=associateBoarder(map,icMeas,x);
			if(icAssoc!=NULL){
				updateBoarder(icAssoc,icMeas,x);
			}else{
/*				printf("before: ->   ");			*/
/*				printList(map[x]);*/
				icAssoc=insertInto(map[x],icMeas->yStart,icMeas->yEnd,icMeas->state);
/*				printf("inserted! -> ");*/
/*				printList(map[x]);*/
			}
			while(icMap!=NULL && icMap->yEnd<=icAssoc->yEnd){
				updateCell(icMap,icMeas);
				icMap=icMap->next;
			}
		}
/*		if(x==25){*/
/*			printf("\n\n\n%d AFTER AFTER:::",x);*/
/*			printList(map[x]);*/
/*		}*/
	}
}


void freeICArray(IC** array,int size){
	for (int i = size-1;  i >=0;  i -= 1)
		{
			deleteWholeList(array[i]);
		}
	free(array);
	array=NULL;
}

int main( int argc,char** argv){
	int stepsize=8;
	curICArray=NULL;
	curICArraySize=0;
	shiftProgress=0;
	shiftProgress=0;
	
	PGMDataFloat tmp;
	PGMDataFloat* pgmpic;

	pgmpic=readPGM("400x1280_low.pgm",&tmp);
	
	
	PGMDataFloat* pgmOut = (PGMDataFloat*)malloc(sizeof(PGMDataFloat));
	pgmOut->col=400;
	pgmOut->row=640;
	pgmOut->matrix=allocate_dynamic_matrix(640, 400);
	pgmOut->max_gray=255;

	IC** measIC;
	IC** whiteICArray;
//////////////////////////////////////////////////////SENSORSIMULATION////////////////////////////////////////////////////////////////////		
	float angleArray[30]={3,3,4,4,-4,0,-4,0,0,-8,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0};
	float angle=4.0;
	
	//create 1st pic -> start position!
	
	for (int i = 1280; i>=640; i--){
		for (int j = 0; j < pgmpic->col; ++j) {
			pgmOut->matrix[(i-640) * pgmpic->col + j]=pgmpic->matrix[i * pgmpic->col + j];
	   }
	}
	
	writePGM("tmpOut.pgm",pgmOut);
	//NOW we are at the part for interpreting the simulated "sensor"-values.
	curICArray=convertPgm("tmpOut.pgm","pgmOutPicture.pgm");
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\SENSORSIMULATION END\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\	
	
	
	
	
	
	
	
	for(int step=((pgmpic->row-640)/stepsize); step >=0;step--){
		printf("Frame %d\n",((pgmpic->row-640)/stepsize)-step);

		angle=angleArray[((pgmpic->row-640)/stepsize)-step];
/*		system("xdg-open pgmOutPicture.pgm");*/


	//1. compensate own vehicle motion
	//2. Prediction of other Objects
		//Rotation: 
		whiteICArray = makeNewWhite(curICArraySize,pgmOut->col); //tested.
		rotateStructure(curICArray,angle/360*3.142,pgmOut->col,pgmOut->row,whiteICArray);
		//Longitudinal:	
		shiftStructure(curICArray,stepsize,pgmOut->col);



		writeICtoPGM(curICArray,640,400,"afterShift.pgm");
		if(VISUAL)system("xdg-open afterShift.pgm");
		
		//////////////////////////////////////////////////////SENSORSIMULATION////////////////////////////////////////////////////////////////////		
//Pepare "Sensor"-Values for algorithm. it would not be relevant if we would get real data -> so it doesnt take time in simulation!
	//Rotation - Simulation
		rotatePGMData(pgmpic,angle/360*3.142,640+stepsize*step);
	//Longitudinal movement - simulation
		for (int i = 640+stepsize*step; i>=stepsize*step; i--){
			for (int j = 0; j < pgmpic->col; ++j) {
				pgmOut->matrix[(i-stepsize*step) * pgmpic->col + j]=pgmpic->matrix[i * pgmpic->col + j];
		   }
		}		
		writePGM("tmpOut.pgm",pgmOut);
/*		system("xdg-open tmpOut.pgm");*/
		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\SENSORSIMULATION END\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

	//3.Feature Extraction	
		//--> done with the convertPGM method 
		measIC = convertPgm("tmpOut.pgm","pgmOutPicture.pgm");
		if(VISUAL)system("xdg-open pgmOutPicture.pgm");

	//4. Association and update
		assocAndUpdate(curICArray,measIC);
		freeICArray(measIC,curICArraySize);
		writeICtoPGM(curICArray,640,400,"afterAaU.pgm");
		if(VISUAL)system("xdg-open afterAaU.pgm");
	//5. Merge 
		//would be tidyUpList, but we use it already!

		//free(curICArray);

		
			if(((pgmpic->row-640)/stepsize)-step==1){
/*				for (int i = curICArraySize-1; i >0; i -= 1)*/
/*				{*/
/*					//printf("%d  ",i);*/
/*					//printList(curICArray[i]);*/
/*					deleteWholeList(whiteICArray[i]);*/
/*				}*/
/*				free(whiteICArray);*/
				break;		
			}
		if(VISUAL)sleep(2);
	}
	//FREE all ressources
/*	freeICArray(curICArray,curICArraySize);*/
		
	for (int i = curICArraySize-1; i >=0; i -= 1)
	{
		//printf("%d  ",i);
		//printList(curICArray[i]);
		if(curICArray[i]!=NULL)	deleteWholeList(curICArray[i]);
	}
/*	freeICArray(curICArray,curICArraySize);*/
	
			
	if(curICArray!=NULL) {
		free(curICArray);
		curICArray=NULL;
	}
	//free(measIC);
	deallocate_dynamic_matrix(pgmpic->matrix, pgmpic->row);
	//free(pgmpic);
	deallocate_dynamic_matrix(pgmOut->matrix, pgmOut->row);
	free(pgmOut);
	exit(EXIT_SUCCESS);	

}



