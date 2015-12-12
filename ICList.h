

typedef struct ic{
	float yStart;   //lateral start position
	float yEnd;		//lateral endposition
	float state;			//state: 0=free; 0.5=Unkown; 1=Occupied;
	float xsigmaYEnd;	//standard deviation of the end
	float x;		//lateral position
	float p;		//P(0|z^(1:t)) -> occupancy probability
	int nAge;		//age of a cell
	int objectID;		//ID of the associated dynamic object
	struct ic* next;
	struct ic* prev;
}IC;


void deleteThis(IC* thisOne);
void deleteWholeList(IC* start);


IC* insertBefore(IC* thisOne); //returns inserted Pointer
IC* insertAfter(IC* thisOne); //returns inserted Pointer
IC* insertInto(IC* head,float start,float end,float state);
void deleteAfter(IC* thisOne);

void printList(IC* head); //prints the list

void tidyUpList(IC* head);
