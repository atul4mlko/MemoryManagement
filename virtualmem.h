struct seq{
	char pageNum;
	int counter;
	struct seq *next;
};

struct frames{
	char pageNum;
	int refBit;
	int refBitArray[8];
	int refBitDecimalValue;
	int nextOccurance;
	int counter;
	struct frames *prev;
	struct frames *next;
};

struct frames *head;
struct frames *tail;
struct seq *pageSeq;
struct seq *firstNode;
struct seq *firstLFUNode;
struct seq *frameQueue;
struct seq *firstNodeOfFrame;
struct frames *frame;
struct frames *clock;

char inputSeq[5000];
char input[5000];
int availableFrames = 5;
int framesLeft = 5;
char replacementPolicy[] = "FIFO";
char fileName[50] = "";
int nbOfPageReplacementByAlgo;
int nbOfPageReplacementByOptimal;


/********************************************
*****Page replacement algorithm functions****
********************************************/
void FIFOPageReplacement();
void LFUPageReplacement();
void OptimalPageReplacement();
void LRUStackPageReplacement();
void LRUClockPageReplacement();
void LRURefPageReplacement();
/********************************************/


/********************************************
********Linked list specific functions*******
********************************************/
void initializeList();
void initializeFrame();
void addPageToTailOfFrame(char);
void addPageToHeadOfFrame(char);
void removeNodeFromFrame(char);
void removeNodeFromHead();
void removeNodeFromTail();
/********************************************/


/********************************************
***********Other services functions**********
********************************************/
void showHelp();
void displayFrame();
int isFrameAvailable();
int findPage(char);
void toggleRefBitForExistingPage(char);
void iterateClock();
void replacePage(char);
int binToDecimal(int []);
void shiftBits(char);
void addShiftBits(char);
char findVictimPage();
char findPageWithMinFreq();
void incrementCounter(char);
void setCounter(char);
/********************************************/



