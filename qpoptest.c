#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

//--	Queue element Structure

typedef struct Q {
    int arrivalTime;		//	arrival time of the task to the queue
    int priority;			//	priority of 0 or 1
    int numSubTasks;		//	number of subtasks
    int subtasks[32];	    //	durations for each subtask
    int elementProcessTime;	//	max duration of subtasks
    int elementTotalTime;	//	total time that task is in the system
    struct Q * next;
} Q;

//--	Process queue element structure

typedef struct sub {
    int subArrTime;
    int subDuration;
    struct sub * next;
} sub;

//--	Function Definitions

void createTaskQueue( Q * * head, Q * * tail, char * filename );
void runSimulation( char * filename );
int qLength( Q * qHead );
void qPrint( Q * qHead, char * string );
Q * qCreate( int arrivalTime, int priority, int duration, int * subtasks );
void qPush( Q * * qHead, Q * * qTail, Q * element );
Q * qPop( Q * * head );
Q * qPopMiddle( Q * * head );
int queueIsEmpty( Q * head );
sub * subCreate( int duration, int time );
int subQueueIsEmpty( sub * head );
Q * scanQueue( Q * * head, int avProc );

//--	Main

int main (int argc, char ** argv)
{
    runSimulation( "test1.txt" );

    return 0;
}

//--	Fuctino to run simulations

void runSimulation( char * filename )
{
    int processors = 64;        //  Total num processors
    int availProc = 64;    //  Available Processors
    int totalTime0 = 0;         //  Total used time by 0 tasks
    int toatlTime1 = 0;         //  Total used time by 1 tasks
    int TotalServiceTime0 = 0;
    int TotalServiceTime1 = 0;
    int waitTime0 = 0;
    int waitTime1 = 0;
    int avgWaitTime0 = 0;
    int avgWaitTime1 = 0;
    int avgQLength = 0;
    int time = 1;
    int lastExitTime = -1;
    int lastEntryTime = 0;
    int totalLength = 0;
    int i;
    float cpu = 0;

    //	Intializing 0 and 1 task queues

    Q * allTaskHead = NULL;
    Q * allTaskTail = NULL;
    Q * q0head = NULL;
    Q * q1head = NULL;
    Q * q0tail = NULL;
    Q * q1tail = NULL;
    sub * processQ = NULL;

    Q * task = NULL;


    /*
    Read file line by line
    *	Put elements in apt queue ( 0 / 1 )
    *	Check if num processors avail
    * 	If yes, serve task, else wait
    */

    //  Create task queue
    createTaskQueue( &allTaskHead, &allTaskTail, filename );
    qPrint( allTaskHead, "alltask" );

    Q * temp = allTaskHead;
    Q * pop = NULL;
    while( temp -> next != NULL )
    {
        if( temp -> next -> arrivalTime == 10 )
        {
            pop = qPopMiddle( &temp );
        }
        temp = temp -> next;
    }
    qPrint( allTaskHead, "alltask" );
    if( pop != NULL )
        printf( "Popped Element:\n%d %d %d\n\n", pop -> arrivalTime, pop -> priority, pop -> numSubTasks );
}

//--    To check if the queue is empty

int queueIsEmpty( Q * head )
{
    if ( head == NULL )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//--    To check if the queue is empty

int subQueueIsEmpty( sub * head )
{
    if ( head == NULL )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void createTaskQueue( Q * * head, Q * * tail, char * filename )
{
    int arrivalTime;
    int priority;
    int numSubTasks;
    int subtasks[32];           //  To store subtasks
    int i;

    FILE * fptr;		//	To store the file ptr

    fptr = fopen( filename, "r" );	//	Open the file to be read
    //	Check if file opened successfully!
    if ( fptr == NULL )
    {
        printf( "File: %s did not open successfully\n", filename );
        return;
    }

    while( !feof(fptr) )
    {
        //	Read Line and put into all task queue
        //int read = fscanf( fptr, "%d %d %d", &arrivalTime, &priority, &duration );
        fscanf( fptr, "%d %d %d", &arrivalTime, &priority, &numSubTasks );
        for( i = 0; i < numSubTasks; i++ )
        {
            fscanf( fptr, "%d", &subtasks[i] );
        }
        //  Pushing to task queue
        Q * element = qCreate( arrivalTime, priority, numSubTasks, subtasks );
        qPush( &(*head), &(*tail), element );
    }

}

//--	To create the queue element

Q * qCreate( int arrivalTime, int priority, int numSubTasks, int * subtasks )
{
    int i = 0;
    Q * element = malloc( sizeof(Q) );
    element -> arrivalTime = arrivalTime;
    element -> priority = priority;
    element -> numSubTasks = numSubTasks;
    for( i = 0; i < numSubTasks; i++ )
    {
        element -> subtasks[i] = subtasks[i];
        element -> next = NULL;
        element -> elementTotalTime = 0;

        return element;
    }
}

//--    Push queue element

void qPush( Q * * qHead, Q * * qTail, Q * element )
{
    if ( (*qHead) != NULL )
    {
        (*qTail) -> next = element;
        (*qTail) = (*qTail) -> next;
    }
    else
    {
        (*qTail) = element;
        (*qHead) = (*qTail);
    }
}

//--	Pop queue element

Q * qPop( Q * * head )
{
    Q * poppedElement = NULL;
    if( (*head) != NULL )
    {
        poppedElement = (*head);
        (*head) = (*head) -> next;
        poppedElement -> next = NULL;
    }
    return poppedElement;
}

Q * qPopMiddle( Q * * head )
{
    //	Copy head in temp
    Q * temp = (*head);
    Q * poppedElement = NULL;
    //	Set popped element to temp -> next
    poppedElement = temp -> next;
    // 	Set temp to next next
    temp = temp -> next -> next;
    //	Set head -> next to temp
    (*head) -> next = temp;
    //	Set poppedElement next to null and return
    poppedElement -> next = NULL;
    return poppedElement;
}

//--	Print queue

void qPrint( Q * qHead, char * string )
{
    int counter = 0;
    int i = 0;
    if ( qHead != NULL )
    {
        Q * temp = qHead;
        counter = 1;
        printf( "QUEUE\n____%s___\n", string);
        do
    {
        printf( "%d : %d %d %d\n", counter, temp -> arrivalTime, temp -> priority, temp -> numSubTasks );
        //for( i = 0; i < temp -> numSubTasks; i++ )
        //{
        //    printf( "%d ", temp -> subtasks[i] );
        //}
        //printf("\n");
        counter++;
        if( temp -> next != NULL )
            temp = temp -> next;
        }
        while( temp -> next != NULL );
    }
    else
    {
        printf("Queue is empty!!!\n\n");
    }
}
