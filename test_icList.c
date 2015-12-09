

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ICList.h"

IC* list;

void rot(int* x, int* y, float angle, int xOffset){
	int tmpX=*x;
	int tmpY=*y;
	*x=round(cos(angle)*(tmpX-xOffset)-sin(angle)*tmpY);
	*x=*x-xOffset;
	
	*y=round(sin(angle)*(tmpX-xOffset)+cos(angle)*tmpY);
	
	
}




int main (int argc, char *argv[])
{
	int x=1;
	int y=3;
	
	rot(&x,&y,3.14,0);
	printf("x=%d,  y=%d; \n",x,y);





/*	list= insertAfter(NULL);*/
/*	IC* head=list;*/
/*	list->state=1;*/
/*	for(int x=0;x<101;x++){*/
/*		list=insertAfter(list);*/
/*		printf("%p should be different\n",list);*/
/*		list->state=x;*/
/*		list->yStart=x%17;*/
/*		list->yEnd=x%19;*/
/*	}*/

/*list=head;*/
/*	list=insertAfter(list);*/
/*	list->yEnd=123123123;*/
/*	list=head;*/
/*	for (;list!=NULL;)*/
/*	{*/
/*		printf("%f ",list->yEnd);*/
/*		list=list->next;*/
/*	}*/
/*	deleteWholeList(head);*/
/*	printf("\n\n\n success!!\n");*/
	return 0;
}
