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
void subPush( sub * subElement, sub * * processQ );
sub * subPopMiddle( sub * * head );
sub * subPop( sub * * head );
int subQueueIsEmpty( sub * head );
void subQPrint( sub * qHead, char * string );
Q * scanQueue( Q * * head, int avProc );
void decreaseDuration( sub * * head );
void scanAndPop( sub * * head, int * );

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

    //~~~~  SIMULATION BEGINS HERE
    while( ( !queueIsEmpty( allTaskHead ) || !queueIsEmpty( q0head ) || !queueIsEmpty( q1head ) || !subQueueIsEmpty( processQ ) ) && time < 25 )
    {
        printf( "\nTime: %d\n\n", time );
        subQPrint( processQ, "processQ at beginning");

        if( !subQueueIsEmpty( processQ ) )
        {
            //	Check if anything needs to be popped
            scanAndPop( &processQ, &availProc );
        }

        if( !queueIsEmpty( allTaskHead ) )
        {
            //	Take element from allTaskQueue
            while( allTaskHead != NULL && allTaskHead -> arrivalTime == time )
            {
                //	Pop and put to q1 or q0
                if ( allTaskHead -> priority == 0 )
                {
                    qPush( &q0head, &q0tail, qPop( &allTaskHead ) );
                }
                else if ( allTaskHead -> priority == 1 )
                {
                    qPush( &q1head, &q1tail, qPop( &allTaskHead ) );
                }
            }
        }

        //  While avProc > 0 keep scanning q1 and q0 and not

        if( !queueIsEmpty( q0head ) )
        {
            //	Take element from q0head
            task = scanQueue( &q0head, availProc );

            while ( task != NULL )
            {
                // create subtasks
                for ( i = 0; i < task -> numSubTasks; i++ )
                {
                    sub * subTask = subCreate( task -> subtasks[i], time );

                    //push subtasks
                    subPush( subTask, &processQ );
                }

                // update availProc
                availProc -= task -> numSubTasks;

                task = scanQueue( &q0head, availProc );
            }

        }

        if( !queueIsEmpty( q1head ) )
        {
            //	Take element from q1head
            //	Take element from q0head
            task = scanQueue( &q1head, availProc );

            while ( task != NULL )
            {
                // create subtasks
                for ( i = 0; i < task -> numSubTasks; i++ )
                {
                    sub * subTask = subCreate( task -> subtasks[i], time );

                    //push subtasks
                    subPush( subTask, &processQ );
                }

                // update availProc
                availProc -= task -> numSubTasks;

                task = scanQueue( &q0head, availProc );
            }
        }

        printf( "\nTime: %d\n\n", time );
        printf( "\nNumber of available processors: %d\n", availProc );
        //qPrint( allTaskHead, "alltask" );
        //qPrint( q0head, "q0" );
        //qPrint( q1head, "q1" );
        subQPrint( processQ, "processQ at end");

        time++;

        //  Decrease duration in processQ HERE
        decreaseDuration( &processQ );
        //~~~	WHILE LOOP ENDS HERE
    }
}

//--	To decremened duration at every second

void decreaseDuration( sub * * head )
{
    sub * temp = ( * head );
    while( temp != NULL )
    {
        temp -> subDuration--;
        temp = temp -> next;
    }
}

//--	To scan the entire processQ to check if anything needs to be popped

void scanAndPop( sub * * head, int * availProc )
{
    if( (*head) != NULL && (*head) -> subDuration <= 0 )
    {
        //	Pop
        sub * element = subPop( head );
        ( * availProc )++;
        printf( "\n~~~~1st pop Popping~~~~~\n");
        subQPrint( element, "Process element" );
    }
    sub * temp = ( * head );
    while( temp != NULL && temp -> next != NULL )
    {
        if( temp -> next -> subDuration <= 0 )
        {
            //	Pop
            sub * element = subPopMiddle( &temp );
            ( * availProc )++;
            printf( "\n~~~~~~Popping~~~~~~\n");
            subQPrint( element, "Process element" );
        }
        else
        {
            temp = temp -> next;
        }
    }
}

//--    To scan queue for elements to be processed

Q * scanQueue( Q * * head, int availProc )
{
    Q * elementToBeServed = NULL;
    Q * temp = (*head);
    if ( temp != NULL && temp -> numSubTasks <= availProc )
    {
        //	Pop element from queue and serve
        elementToBeServed = qPop( &(*head) );
    }
    while( temp != NULL && temp -> next != NULL )
    {
        if ( temp -> next -> numSubTasks <= availProc )
        {
            //	Pop element from queue and serve
            qPopMiddle( &temp );
        }
        else
        {
            temp = temp -> next;
        }
    }

    return elementToBeServed;
}

//--	To create the sub task element

sub * subCreate ( int duration, int time )
{
    sub * subElement = malloc( sizeof(sub) );
    subElement -> subArrTime = time;
    subElement -> subDuration = duration;

    subElement -> next = NULL;

    return subElement;
}

//--	To push element to subQ

void subPush ( sub * subElement, sub * * processQ )
{
    if ( ( * processQ ) != NULL )
    {
        subElement -> next = ( * processQ );
        ( * processQ ) = subElement;
    }

    else
    {
        ( * processQ ) = subElement;
    }
}

sub * subPop( sub * * head )
{
    sub * poppedElement = NULL;
    if( (*head) != NULL )
    {
        poppedElement = (*head);
        (*head) = (*head) -> next;
        poppedElement -> next = NULL;
    }
    return poppedElement;
}

//--	To pop element from processQ

sub * subPopMiddle ( sub * * head )
{
    //	Copy head in temp
    sub * temp = ( * head);
    sub * poppedElement = NULL;
    //	Set popped element to temp -> next
    if( temp -> next != NULL )
    {
        poppedElement = temp -> next;
    }
    // 	Set temp to next next
    if( temp -> next != NULL && temp -> next -> next != NULL )
    {
        temp = temp -> next -> next;
    }
    else
    {
        temp = NULL;
    }
    //	Set head -> next to temp
    ( * head) -> next = temp;
    //	Set poppedElement next to null and return
    if( poppedElement != NULL )
    {
        poppedElement -> next = NULL;
    }
    return poppedElement;
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
        return 1;
    }
    else
    {
        return 0;
    }
}

//--	To create the task queue

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
    }
    return element;
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

//--	To pop an element from the middle of a queue

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

//--	Get queue Length

int qLength( Q * qHead )
{
    if( qHead != NULL )
    {
        Q * temp = qHead;
        int count = 1;
        while( temp -> next != NULL )
        {
            temp = temp -> next;
            count++;
        }
        return count;
    }
    return 0;
}


//--	Increase element time for each element in queue
//--	by 1 as time incremends

void incTime( Q * qHead )
{
    if( qHead != NULL )
    {
        Q * temp = qHead;
        while( temp -> next != NULL )
        {
            temp -> elementTotalTime++;
            temp = temp -> next;
        }
        temp -> elementTotalTime++;
    }
}

void subQPrint( sub * qHead, char * string )
{
    int counter = 0;
    int i = 0;
    if ( qHead != NULL )
    {
        sub * temp = qHead;
        counter = 1;
        printf( "QUEUE\n____%s___\n", string);
        do
        {
            printf( "%d : %d %d\n", counter, temp -> subArrTime, temp -> subDuration );
            //for( i = 0; i < temp -> numSubTasks; i++ )
            //{
            //    printf( "%d ", temp -> subtasks[i] );
            //}
            //printf("\n");
            counter++;
            if( temp -> next != NULL )
            {
                temp = temp -> next;
            }
            else
            {
                temp = NULL;
            }
        }
        while( temp != NULL );
    }
    else
    {
        printf("\nQueue %s is empty!!!\n\n", string);
    }
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
            {
                temp = temp -> next;
            }
        }
        while( temp -> next != NULL );
    }
    else
    {
        printf("\nQueue %s is empty!!!\n\n", string);
    }
}
