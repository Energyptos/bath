
#include <stdlib.h>
#include <stdio.h>


#include "ICList.h"


void printList(IC* head){
	for(IC* ptr=head;ptr!=NULL;ptr=ptr->next){
		int len=(ptr->yEnd-ptr->yStart);
		printf("|len: %d;st: %f; start:%.1f,end:%.1f|",len,ptr->state,ptr->yStart,ptr->yEnd);
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


IC* insertInto(IC* head,float start,float end,float state){
/*printf("InsertINTO: list: ");*/
/*printList(head);*/
/*printf(" with start= %d , end= %d ; state= %f \n",start,end,state);*/
//visualisation: out of |----runptr---------|
// is made 				|rnPtr|midl|lastHalf|
	for(IC* runPtr=head;runPtr!=NULL;runPtr=runPtr->next){
		if(runPtr->state!=state){
		float oldEnd=runPtr->yEnd;
			if(runPtr->yEnd>=end && runPtr->yStart<=start){
				IC* middleOne = insertAfter(runPtr);
				middleOne->yStart=start;
				middleOne->yEnd=end;
				runPtr->yEnd=start;
				middleOne->state=state;
				IC* lastHalf = insertAfter(middleOne); //runptr will be the middleOne if we inserted after it.		
				lastHalf->state=runPtr->state;
				lastHalf->yEnd=oldEnd;
				lastHalf->yStart=end;		
				runPtr=lastHalf;
				break;
			}
			if(runPtr->yEnd>end && runPtr->yStart==start){
				//start is the same => we only need two intervals
				IC* lastHalf = insertAfter(runPtr);
				lastHalf->state=runPtr->state;
				lastHalf->yStart=end;
				lastHalf->yEnd=oldEnd;
				runPtr->state=state;
				runPtr->yEnd=end;
				runPtr=lastHalf;
				break;
			}
			if(runPtr->yEnd==end && runPtr->yStart<start){
				//end is the same-> start is less
				IC* lastHalf = insertAfter(runPtr);
				lastHalf->yEnd=end;
				lastHalf->state=state;
				lastHalf->yStart=start;
				runPtr->yEnd=start;
				runPtr=lastHalf;
				break;
			}
			if(runPtr->yEnd==end && runPtr->yStart==start){
				exit(EXIT_FAILURE);
			}
				
		
		}
	}
	

}


void tidyUpList(IC* head){
	int min_whitespace=3;
	for(IC* runPtr=head;runPtr!=NULL;runPtr=runPtr->next){
/*		if(runPtr->yEnd-runPtr->yStart<=0){*/
/*			runPtr=runPtr->prev;*/
/*			deleteThis(runPtr->next);*/
/*			continue;	*/
/*		}*/
		if(runPtr->state<0.4){
		//whitespace!
			if(runPtr->yEnd-runPtr->yStart<min_whitespace){
				//make it disapear
				if(runPtr->prev!=NULL && runPtr->next!=NULL){
				 printf("DELETE!!!!!!!!\n\n\n");
					runPtr=runPtr->prev;
					runPtr->yEnd=runPtr->next->yEnd;
					deleteThis(runPtr->next);
					continue;
				}
			}
		}
		if(runPtr->next!=NULL && runPtr->next->state==runPtr->state){
			runPtr->yEnd=runPtr->next->yEnd;
			deleteThis(runPtr->next);
			continue;
		}
		
	}
}


void deleteAfter(IC* thisOne){
	deleteThis(thisOne->next);
}


void deleteThis(IC* thisOne){
	//printf("this %p, prev %p , next %p\n\n\n",thisOne,thisOne->prev, thisOne->next);
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


