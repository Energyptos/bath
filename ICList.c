
#include <stdlib.h>
#include <stdio.h>


#include "ICList.h"


void printList(IC* head){
	for(IC* ptr=head;ptr!=NULL;ptr=ptr->next){
		int len=(ptr->yEnd-ptr->yStart);
		printf("|len: %d;st: %f|",len,ptr->state);
	}
	printf("\n");

}


IC* insertBefore(IC* thisOne){
	IC* insertOne = (IC*)malloc(sizeof(IC));
		if(insertOne==NULL){
			fprintf(stderr,"malloc failed. \n");
		}
	insertOne->next=NULL;
	insertOne->prev=NULL;
	if(thisOne==NULL){
	//create all new list --> only return 1 element
		return insertOne;
	}
	if(thisOne->prev!=NULL){
		 insertOne->prev = thisOne->prev;	//adjust nextPointer of new Element (if there is one)
		 	(insertOne->prev)->next=insertOne;  			//adjust prev Pointer 
	}
	thisOne->prev=insertOne;	  			//adjust next pointer of thisOne


	return insertOne;
}



IC* insertAfter(IC* thisOne){
	IC* insertOne =(IC*) malloc(sizeof(IC));
		if(insertOne==NULL){
			fprintf(stderr,"malloc failed. \n");
		}
	insertOne->next=NULL;
	insertOne->prev=NULL;
	if(thisOne==NULL){
	//create all new list --> only return 1 element
		return insertOne;
	}
	if(thisOne->next!=NULL){
		insertOne->next = thisOne->next;	//adjust nextPointer of new Element (if there is one) 
	//TODO: why are we landing here!!?!??
	//printf("next?! thisOne->next yend:%p",thisOne->next);
		//insertOne->next->prev=insertOne;
	}
	thisOne->next=insertOne;	  			//adjust next pointer of thisOne
	insertOne->prev=thisOne;  			//adjust prev Pointer
	
	return insertOne;
}


IC* insertInto(IC* head,int start,int end,float state){
/*printf("InsertINTO: list: ");*/
/*printList(head);*/
/*printf(" with start= %d , end= %d ; state= %f \n",start,end,state);*/
//visualisation: out of |----runptr---------|
// is made 				|rnPtr|midl|lastHalf|
	for(IC* runPtr=head;runPtr!=NULL;runPtr=runPtr->next){
		if(runPtr->state!=state){
			IC* middleOne;
			IC* lastHalf;
			if(runPtr->yStart<start){
				//only here we need a middleOne, because if those starts are equal, we would leave an empty interval!
				middleOne = insertAfter(runPtr);
				middleOne->state=state;
				middleOne->yStart=start;
				middleOne->yEnd=end;
				runPtr->yEnd=start;
				runPtr=middleOne;
			}
			if(runPtr->yEnd>end){
				//ony if this is the case, we need 3 blocks: run,midl and lastHalf
				lastHalf = insertAfter(runPtr); //runptr will be the middleOne if we inserted after it.
				lastHalf->state=runPtr->state;
				lastHalf->yEnd=runPtr->yEnd;
				lastHalf->yStart=end;
				runPtr=lastHalf;
			}
		}
		
	}
}




void deleteAfter(IC* thisOne){
	deleteThis(thisOne->next);
}


void deleteThis(IC* thisOne){
	if(thisOne==NULL)return;
	if(thisOne->next!=NULL && thisOne->prev!=NULL){
		thisOne->prev->next=thisOne->next;
		thisOne->next->prev=thisOne->prev;
	}
	free(thisOne);
}

void itemsInList(IC* head){
	IC* tmp=head;
	int ctr=0;
	while(tmp!=NULL){
		ctr++;
		tmp=tmp->next;
	}
	printf("list has %d entries\n",ctr);
}

void deleteWholeList(IC* start){
	while(start!=NULL){
		IC* tmp =start->next;
		//printf("d%.1f ",start->state);
		free(start);
		//deleteThis(start);
		start=tmp;
	}
}


