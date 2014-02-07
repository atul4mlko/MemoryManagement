#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/time.h>
#include<math.h>
#include "virtualmem.h"

int main(int argc , char *argv[]){
	int i;
	struct timeval tv;
	char nameOfAlgo[20];
	unsigned int timeBeforeAlgo, timeAfterAlgo, timeBeforeOptimal, timeAfterOptimal, timeTakenByOptimal, timeTakenByAlgo;
	for (i=1; i < argc; i++){
		if (strcmp(argv[i],"-h") == 0){
			showHelp();
		}else
		if (strcmp(argv[i],"-f") == 0){
			availableFrames = framesLeft = atoi(argv[++i]);
		}else
		if (strcmp(argv[i],"-r") == 0){
			strcpy(replacementPolicy,argv[++i]);
		}else
		if (strcmp(argv[i],"-i") == 0){
			strcpy(fileName,argv[++i]);
		}else
			printf("%s is a wrong option. Please enter the correct option",argv[i]);
	}
	FILE *fp;
	if ((fp = fopen(fileName,"r")) != NULL){
		int c;
		i = 0;
		while ((c = getc(fp)) != EOF){
			if (isspace(c) == 0)
				inputSeq[i++] = c;
		}
		inputSeq[i] = '\0';
		fclose(fp);
	}
	else{
		printf("Enter the sequence!!\n");
		fgets(inputSeq,sizeof inputSeq, stdin);
	}
	if (strcmp(replacementPolicy,"LFU") == 0){
		strcpy(nameOfAlgo,"LFU");
		gettimeofday(&tv, NULL);
		timeBeforeAlgo = (uint)tv.tv_usec;
		LFUPageReplacement();
		gettimeofday(&tv, NULL);
		timeAfterAlgo = (uint)tv.tv_usec;
		timeTakenByAlgo = (uint) (timeAfterAlgo - timeBeforeAlgo);
	}
	else if (strcmp(replacementPolicy,"LRU-STACK") == 0){
		strcpy(nameOfAlgo,"LRU-STACK");
		gettimeofday(&tv, NULL);
		timeBeforeAlgo = (uint)tv.tv_usec;
		LRUStackPageReplacement();
		gettimeofday(&tv, NULL);
		timeAfterAlgo = (uint)tv.tv_usec;
		timeTakenByAlgo = (uint) (timeAfterAlgo - timeBeforeAlgo);
	}
	else if (strcmp(replacementPolicy,"LRU-CLOCK") == 0){
		strcpy(nameOfAlgo,"LRU-CLOCK");
		gettimeofday(&tv, NULL);
		timeBeforeAlgo = (uint)tv.tv_usec;
		LRUClockPageReplacement();
		gettimeofday(&tv, NULL);
		timeAfterAlgo = (uint)tv.tv_usec;
		timeTakenByAlgo = (uint) (timeAfterAlgo - timeBeforeAlgo);
	}
	else if (strcmp(replacementPolicy,"LRU-REF8") == 0){
		strcpy(nameOfAlgo,"LRU-REF8");
		gettimeofday(&tv, NULL);
		timeBeforeAlgo = (uint)tv.tv_usec;
		LRURefPageReplacement();
		gettimeofday(&tv, NULL);
		timeAfterAlgo = (uint)tv.tv_usec;
		timeTakenByAlgo = (uint) (timeAfterAlgo - timeBeforeAlgo);
	}
	else{
		strcpy(nameOfAlgo,"FIFO");
		gettimeofday(&tv, NULL);
		timeBeforeAlgo = (uint)tv.tv_usec;
		initializeList();
		initializeFrame();
		FIFOPageReplacement();	
		gettimeofday(&tv, NULL);
		timeAfterAlgo = (uint)tv.tv_usec;
		timeTakenByAlgo = (uint) (timeAfterAlgo - timeBeforeAlgo);
	}
	timeBeforeOptimal = (uint)tv.tv_usec;
	initializeList();
	initializeFrame();
	OptimalPageReplacement();
	timeAfterOptimal = (uint)tv.tv_usec;
	timeTakenByOptimal = (uint) (timeAfterOptimal - timeBeforeOptimal);
	float percentPage = ((nbOfPageReplacementByAlgo-nbOfPageReplacementByOptimal*1.00)/nbOfPageReplacementByOptimal)*100;
	float percentTime = ((timeAfterOptimal-timeTakenByAlgo*1.00)/timeAfterOptimal)*100;
	printf("Percentage page replacement penalty using %s \t: %2.2f percent\n\n",nameOfAlgo,percentPage);
	printf("Total time to run %s algorithm \t: %u microseconds\nTotal time to run Optimal algorithm  \t: %u microseconds\n",nameOfAlgo,timeTakenByAlgo,timeAfterOptimal);
	printf("%s is %2.2f percent faster than Optimal algorithm.\n\n",nameOfAlgo,percentTime);
	if(nbOfPageReplacementByOptimal < 0)
		printf("!! Any negative value should be considered as 0.\n\n");
	return 0;
}

/************************************************************
*This function will display the help options.
*************************************************************/
void showHelp(){
	printf("\n---------------------------------------------------------------------\n");
	printf("-h\t:Print the usage summary with all the options.");
	printf("\n-f\t:Set the number of available frames. By default it should be 5.");
	printf("\n-r\t:Set the page replacement policy. It can be either-");
	printf("\n\t\tFIFO (First-in-first-out)");
	printf("\n\t\tLFU (Least-frequently-used)");
	printf("\n\t\tLRU-STACK (Least-recently-used stack implementation)");
	printf("\n\t\tLRU-CLOCK ((Least-recently-used clock implementation – second chance alg.)");
	printf("\n\t\tLRU-REF8 (Least-recently-used Reference-bit Implementation, using 8 reference bits)");
	printf("\n\tThe default will be FIFO.");
	printf("\n---------------------------------------------------------------------\n\n");
	exit(0);
}


/*************************************************************
**********Optimal Page Replacement Algorithm******************
*************************************************************/
void OptimalPageReplacement(){
	int i=0, numberOfPageFaults=0;
	char pageToBeRemoved;
	struct seq *pageSeqList;
	struct seq *referenceFrame;
	pageSeqList = firstNode;
	while (pageSeqList->next != NULL){
		referenceFrame = firstNodeOfFrame;
		while (referenceFrame->next != NULL){
			if (pageSeqList->pageNum == referenceFrame->pageNum){
				break;
			}
			referenceFrame = referenceFrame->next;
		}
		if (referenceFrame->next == NULL && isFrameFull() == 0){
			numberOfPageFaults += 1;
			referenceFrame = firstNodeOfFrame;
			while (referenceFrame->pageNum != '\0'){
				referenceFrame = referenceFrame->next;
			}
			referenceFrame->pageNum = pageSeqList->pageNum;
		}else if (referenceFrame->next == NULL && isFrameFull() == 1){
			numberOfPageFaults += 1;
			struct seq *currentPos;
			referenceFrame = firstNodeOfFrame;
			while (referenceFrame->next != NULL){
				currentPos = pageSeqList;
				referenceFrame->counter = 0;
				while (currentPos->next != NULL){
					if (referenceFrame->pageNum != currentPos->pageNum){
						referenceFrame->counter += 1;
					}else
						break;
					currentPos = currentPos->next;
				}
				referenceFrame = referenceFrame->next;
			}
			referenceFrame = firstNodeOfFrame;
			int minOccurance = referenceFrame->counter;
			char minOccuredPage = referenceFrame->pageNum;
			while (referenceFrame->next != NULL){
				if (referenceFrame->counter > minOccurance){
					minOccurance = referenceFrame->counter;
					minOccuredPage = referenceFrame->pageNum;
				}
				referenceFrame = referenceFrame->next;
			}
			referenceFrame = firstNodeOfFrame;
			while (referenceFrame->next != NULL){
				if (referenceFrame->pageNum == minOccuredPage){
					referenceFrame->pageNum = pageSeqList->pageNum;
					break;
				}
				referenceFrame = referenceFrame->next;
			}
		}
		pageSeqList = pageSeqList->next;
	}
	printf("# of page replacements with Optimal = %d\n",numberOfPageFaults-availableFrames);
	nbOfPageReplacementByOptimal = numberOfPageFaults-availableFrames;
}

/*************************************************************
************** FIFO Page Replacement Algorithm ***************
*************************************************************/
void FIFOPageReplacement(){
	int i=0, numberOfPageFaults=0;
	int toBeRemoved = 0;
	struct seq *pageSeqList;
	struct seq *referenceFrame;
	pageSeqList = firstNode;
	while (pageSeqList->next != NULL){
		referenceFrame = firstNodeOfFrame;
		while (referenceFrame->next != NULL){
			if (pageSeqList->pageNum == referenceFrame->pageNum){
				break;
			}
			referenceFrame = referenceFrame->next;
		}
		if (referenceFrame->next == NULL && isFrameFull() == 0){
			numberOfPageFaults += 1;
			referenceFrame = firstNodeOfFrame;
			while (referenceFrame->pageNum != '\0'){
				referenceFrame = referenceFrame->next;
			}
			referenceFrame->pageNum = pageSeqList->pageNum;
		}else if (referenceFrame->next == NULL && isFrameFull() == 1){
			numberOfPageFaults += 1;
			referenceFrame = firstNodeOfFrame;
			toBeRemoved += 1;
			int j,k;
			if (toBeRemoved%availableFrames == 0)
				k = availableFrames;
			else
				k = toBeRemoved%availableFrames;
			for (j=1; j < k; j++){
				referenceFrame = referenceFrame->next;
			}
			referenceFrame->pageNum = pageSeqList->pageNum;
		}
		pageSeqList = pageSeqList->next;
	}
	printf("\n# of page replacements with FIFO = %d\n",numberOfPageFaults-availableFrames);
	nbOfPageReplacementByAlgo = numberOfPageFaults-availableFrames;
}

/*************************************************************
************** LRU Page Replacement Algorithm ****************
*************************************************************/
void LRURefPageReplacement(){
	int numberOfPageFaults = 0,j;
	frame = malloc(sizeof(struct frames));
	head = malloc(sizeof(struct frames));
	tail = malloc(sizeof(struct frames));
	head = frame;
	tail = frame;
	frame->next = NULL;
	frame->prev = head;
	frame->pageNum = inputSeq[0];
	for (j=0;j<8;j++)	
		frame->refBitArray[j] = 0;
	int i, isPageAlreadyExists = 0;
	frame->refBitDecimalValue = binToDecimal(frame->refBitArray);
	numberOfPageFaults += 1;
	for (i=0; i<strlen(inputSeq); i++){
		if (isspace(inputSeq[i]) == 0){
			isPageAlreadyExists = findPage(inputSeq[i]);
			if (isPageAlreadyExists == 1){
				shiftBits(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 1){
				numberOfPageFaults += 1;
				framesLeft -= 1;
				addPageToTailOfFrame(inputSeq[i]);
				addShiftBits(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 0){
				numberOfPageFaults += 1;
				char victimPage = findVictimPage();
				removeNodeFromFrame(victimPage);
				addPageToTailOfFrame(inputSeq[i]);
				addShiftBits(inputSeq[i]);
			}
		}
	}
	printf("\n# of page replacements with LRU-REF8 = %d\n",numberOfPageFaults-availableFrames);
	nbOfPageReplacementByAlgo = numberOfPageFaults-availableFrames;
}

/*************************************************************
************** LFU Page Replacement Algorithm ****************
*************************************************************/
void LFUPageReplacement(){
	int numberOfPageFaults = 0;
	frame = malloc(sizeof(struct frames));
	head = malloc(sizeof(struct frames));
	tail = malloc(sizeof(struct frames));
	head = frame;
	tail = frame;
	frame->next = NULL;
	frame->prev = head;
	frame->pageNum = inputSeq[0];
	frame->counter = 1;
	numberOfPageFaults += 1;
	int i, isPageAlreadyExists = 0;
	for (i=1; i<strlen(inputSeq); i++){
		if (isspace(inputSeq[i]) == 0){
			isPageAlreadyExists = findPage(inputSeq[i]);
			if (isPageAlreadyExists == 1){
				incrementCounter(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 1){
				numberOfPageFaults += 1;
				framesLeft -= 1;
				addPageToTailOfFrame(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 0){
				numberOfPageFaults += 1;
				char victim = findPageWithMinFreq();
				removeNodeFromFrame(victim);
				addPageToTailOfFrame(inputSeq[i]);
			}
		}
	}
	printf("\n# of page replacements with LFU = %d\n",numberOfPageFaults-availableFrames);
	nbOfPageReplacementByAlgo = numberOfPageFaults-availableFrames;
}

/*************************************************************
************ LRU-CLOCK Page Replacement Algorithm ************
*************************************************************/
void LRUClockPageReplacement(){
	int numberOfPageFaults = 0;
	frame = malloc(sizeof(struct frames));
	head = malloc(sizeof(struct frames));
	tail = malloc(sizeof(struct frames));
	head = frame;
	tail = frame;
	clock = frame;
	frame->next = NULL;
	frame->prev = head;
	frame->pageNum = inputSeq[0];
	frame->refBit = 1;
	numberOfPageFaults += 1;
	int i, isPageAlreadyExists = 0;
	for (i=1; i<strlen(inputSeq); i++){
		if (isspace(inputSeq[i]) == 0){
			isPageAlreadyExists = findPage(inputSeq[i]);
			if (isPageAlreadyExists == 1){
				toggleRefBitForExistingPage(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 1){
				numberOfPageFaults += 1;
				framesLeft -= 1;
				addPageToTailOfFrame(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 0){
				numberOfPageFaults += 1;
				iterateClock();
				replacePage(inputSeq[i]);
			}
		}
	}
	printf("\n# of page replacements with LRU-CLOCK = %d\n",numberOfPageFaults-availableFrames);
	nbOfPageReplacementByAlgo = numberOfPageFaults-availableFrames;
}

/*************************************************************
************ LRU-STACK Page Replacement Algorithm ************
*************************************************************/
void LRUStackPageReplacement(){
	int numberOfPageFaults = 0;
	frame = malloc(sizeof(struct frames));
	head = malloc(sizeof(struct frames));
	tail = malloc(sizeof(struct frames));
	head = frame;
	tail = frame;
	frame->next = NULL;
	frame->prev = head;
	frame->pageNum = inputSeq[0];
	numberOfPageFaults += 1;
	int i, isPageAlreadyExists = 0;
	for (i=1; i<strlen(inputSeq); i++){
		if (isspace(inputSeq[i]) == 0){
			isPageAlreadyExists = findPage(inputSeq[i]);
			if (isPageAlreadyExists == 1){
				removeNodeFromFrame(inputSeq[i]);
				addPageToTailOfFrame(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 1){
				numberOfPageFaults += 1;
				framesLeft -= 1;
				addPageToTailOfFrame(inputSeq[i]);
			}else if (isPageAlreadyExists == 0 && isFrameAvailable() == 0){
				numberOfPageFaults += 1;
				addPageToTailOfFrame(inputSeq[i]);
				removeNodeFromHead();
			}
		}
	}
	printf("\n# of page replacements with LRU-STACK = %d\n",numberOfPageFaults-availableFrames);
	nbOfPageReplacementByAlgo = numberOfPageFaults-availableFrames;
}

/************************************************************
*This function creates a linked list of the input sequence.
This list will be used in Optimal and FIFO pagere placement 
algorithms.
*************************************************************/
void initializeList(){
	pageSeq = malloc(sizeof(struct seq));
	firstNode = pageSeq;
	int i;
	for (i=0; i < strlen(inputSeq); i++){
		if (isspace(inputSeq[i]) == 0){
			pageSeq->next = malloc(sizeof(struct seq));
			pageSeq->pageNum = inputSeq[i];
			pageSeq = pageSeq->next;
		}
	}
	pageSeq->next = NULL;	
}

/************************************************************
*This function initializes a linked list of the frames.
This list will be used in Optimal and FIFO pagere placement 
algorithms.
*************************************************************/
void initializeFrame(){
	frameQueue = malloc(sizeof(struct seq));
	firstNodeOfFrame = frameQueue;
	int i;
	for (i=0; i<availableFrames; i++){
		frameQueue->next = malloc(sizeof(struct seq));
		frameQueue->pageNum = '\0';
		frameQueue->counter = 0;
		frameQueue = frameQueue->next;
	}
	frameQueue->next = NULL;
}

/************************************************************
* - returns 1 if the list of frames is completely filled.
* - returns 0 if the list of frames is not completely filled.
*************************************************************/
int isFrameFull(){
	int alreadyFilled = 0;
	struct seq *referenceFrame;
	referenceFrame = firstNodeOfFrame;
	while (referenceFrame->pageNum != '\0'){
		alreadyFilled += 1;
		referenceFrame = referenceFrame->next;
	}
	if (alreadyFilled == availableFrames)
		return 1;
	else
		return 0;
}

/************************************************************
* returns the victim page that is to be removed. This will be
* used bu LRU algorithm.
*************************************************************/
char findVictimPage(){
	struct frames *refFrame;
	refFrame = head;
	while (refFrame->next != NULL){
		refFrame->refBitDecimalValue = binToDecimal(refFrame->refBitArray);
		refFrame = refFrame->next;
	}
	refFrame = head;
	int minimumVal = refFrame->refBitDecimalValue;
	char victim = refFrame->pageNum;
	while (refFrame->next != NULL){
		if (minimumVal > refFrame->refBitDecimalValue){
			minimumVal = refFrame->refBitDecimalValue;
			victim = refFrame->pageNum;
		}
		refFrame = refFrame->next;
	}
	return victim;
}

/************************************************************
* This will associate the bit sequence to the newly added
* page and shifts the bit sequences of other pages as per
* the LRU-REF8 algorithm.
*************************************************************/
void addShiftBits(char pgNum){
	struct frames *refFrame;
	refFrame = head;
	while (refFrame->next != NULL){
		int i;
		if (refFrame->pageNum == pgNum){
			for (i=1;i<8;i++){
				refFrame->refBitArray[i] = 0;
			}
			refFrame->refBitArray[0] = 1;
		}else{
			for (i=7;i>0;i--){
				refFrame->refBitArray[i] = refFrame->refBitArray[i-1];
			}
			refFrame->refBitArray[0] = 0;
		}
		refFrame = refFrame->next;
	}
}

/************************************************************
* converts an array of integers, in which each array entry 
* represents a binary digit, i.e 0 or 1, into the corresponding
* decimal value.
*************************************************************/
int binToDecimal(int binArray[]){
	int i=0,decimalValue = 0;
	for (i=0; i<8; i++){
		decimalValue += pow(2,7-i) * binArray[i];
	}
	return decimalValue;
}

/************************************************************
* shifts the bits of the pages as per the LRU-REF8
* algorithm.
*************************************************************/
void shiftBits(char pgNum){
	struct frames *refFrame;
	refFrame = head;
	while (refFrame->next != NULL){
		int i;
		if (refFrame->pageNum == pgNum){
			for (i=7;i>0;i--){
				refFrame->refBitArray[i] = refFrame->refBitArray[i-1];
			}
			refFrame->refBitArray[0] = 1;
		}else{
			for (i=7;i>0;i--){
				refFrame->refBitArray[i] = refFrame->refBitArray[i-1];
			}
			refFrame->refBitArray[0] = 0;
		}
		refFrame = refFrame->next;
	}
}

/************************************************************
* toggles the referrence bit of an existing page. This will
* be used by LRU-CLOCK algorithm.
*************************************************************/
void toggleRefBitForExistingPage(char pgNum){
	struct frames *refSeq;
	refSeq = head;
	while(refSeq != tail){
		if (refSeq->pageNum == pgNum){
			if (refSeq->pageNum == 0)
				refSeq->pageNum = 1;
		}
		refSeq = refSeq->next;
	}
}

/************************************************************
* It iterates a circular linked list using a global variable
* clock. This will keep track of the clock hand. This will
* be used by LRU-CLOCK algorithm.
*************************************************************/
void iterateClock(){
	while (clock->refBit != 0 && clock->next != NULL){
		clock->refBit = 0;
		clock = clock->next;
	}
}

/************************************************************
* Simply replaces the page number of the frame. This will
* be used by LRU-CLOCK algorithm.
*************************************************************/
void replacePage(char pgNum){
	clock->pageNum = pgNum;
	clock->refBit = 1;
}

/************************************************************
* Displays the current content of the frame. Currently not
* being anywhere.
*************************************************************/
void displayFrame(){
	struct frames *start = head;
	struct frames *stop = tail;int i=1;
	while (start != NULL){
		printf("%c ",start->pageNum, start->refBit);
		start = start->next;	
	}
}

/************************************************************
* returns 1 if empty frames are available.
* returns 0 if empty frames are not available.
*************************************************************/
int isFrameAvailable(){
	if (framesLeft == 1)
		return 0;
	else
		return 1;
}

/************************************************************
* returns 1 if empty pgNum is in the frame list.
* returns 0 if empty pgNum is not there in the frame list.
*************************************************************/
int findPage(char pgNum){
	struct frames *refSeq;
	refSeq = head;
	while(refSeq->next != NULL){
		if (refSeq->pageNum == pgNum)
			return 1;
		refSeq = refSeq->next;
	}
	return 0;
}

/************************************************************
* adds a new frame with page number = pgNum to the tail of 
* the frame list.
*************************************************************/
void addPageToTailOfFrame(char pgNum){
	struct frames *newEntry;
	newEntry = malloc(sizeof(struct frames));
	newEntry->next = malloc(sizeof(struct frames));
	newEntry->prev = malloc(sizeof(struct frames));
	newEntry->pageNum = pgNum;
	newEntry->refBit = 1;
	newEntry->counter = 1;
	newEntry->refBitDecimalValue = 0;
	tail->next = newEntry;
	newEntry->prev = tail;
	newEntry->next = NULL;
	tail = newEntry;
}

/************************************************************
* adds a new frame with page number = pgNum to the head of 
* the frame list.
*************************************************************/
void addPageToHeadOfFrame(char pgNum){
	struct frames *newEntry;
	newEntry = malloc(sizeof(struct frames));
	newEntry->next = malloc(sizeof(struct frames));
	newEntry->prev = malloc(sizeof(struct frames));
	newEntry->pageNum = pgNum;
	newEntry->next = head;
	newEntry->prev = NULL;
	head = newEntry;
}

/************************************************************
* removes a frame with page number = pgNum from the frame list.
*************************************************************/
void removeNodeFromFrame(char pgNum){
	struct frames *refSeq;
	refSeq = head;
	while(refSeq->pageNum != pgNum){
		refSeq = refSeq->next;
	}
	if (refSeq == head)
		removeNodeFromHead();
	else if (refSeq == tail)
		removeNodeFromTail();
	else{
		struct frames *tmp;
		tmp = malloc(sizeof(struct frames));
		tmp->prev = malloc(sizeof(struct frames));
		tmp = refSeq;
		refSeq->prev->next = refSeq->next;
		refSeq->next->prev = tmp->prev;
		free(tmp);
	}
}

/************************************************************
* removes a frame with page number = pgNum from the head the 
* frame list.
*************************************************************/
void removeNodeFromHead(){
	struct frames *tmp;
	tmp = head;
	head = head->next;
	head->prev = NULL;
}

/************************************************************
* removes a frame with page number = pgNum from the tail the 
* frame list.
*************************************************************/
void removeNodeFromTail(){
	struct frames *tmp;
	tmp = tail;
	tail = tmp->prev;
	tail->next = NULL;
}

/************************************************************
* increases the counter attribute of the page with page
* number = pgNum. This will be used by LFU algorithm.
*************************************************************/
void incrementCounter(char pgNum){
	struct frames *refFrame;
	refFrame = head;
	while (refFrame->next != NULL){
		if (refFrame->pageNum == pgNum)
			refFrame->counter += 1;
		refFrame = refFrame->next;
	}
}

/************************************************************
* returns the page with the minimum number of references.
* Used by LFU algorithm.
*************************************************************/
char findPageWithMinFreq(){
	struct frames *refFrame;
	refFrame = head;
	char victim = refFrame->pageNum;
	char minFreq = refFrame->counter;
	while (refFrame->next != NULL){
		if (minFreq > refFrame->counter){
			victim = refFrame->pageNum;
			minFreq = refFrame->counter;			
		}
		refFrame = refFrame->next;
	}
	return victim;
}
