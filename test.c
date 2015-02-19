/* ****************************
**
**  ECE 368 Project 1 Part 2
**
**  Title: 64 bit processor simulation
**
**  Student 1: Harika Thatukuru
**  username: hthatuku
**
**  Student 2: Tatparya Shankar
**  username: tshankar
**
***************************** */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

//--	Queue element Structure

typedef struct Q {
    int arrivalTime;		//	arrival time of the task to the queue
    int priority;			//	priority of 0 or 1
    int numSubTasks;		//	number of subtasks
    int subtasks[32];	    //	durations for each subtask
    struct Q * next;
} Q;

//--	Process queue element structure

typedef struct sub {
    int subArrTime;
    int subDuration;
    struct sub * next;
} sub;

//--	Function Definitions

void generateInputQueue (  Q * * head, Q * * tail, double lam0, double lam1, double mu, int numTasks );
void createTaskQueue( Q * * head, Q * * tail, char * filename );
void runSimulation( char * filename, int mode, char ** argv );
void qPrint( Q * qHead, char * string );
Q * qCreateFEL( int arrivalTime, int priority, int numSubTasks, double mu );
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

Q * FEL_sort(Q * list, int (compar)(int, int));
Q * FEL_merge(Q * lhs, Q * rhs, int (compar)(int, int));
int compar (int a, int b);
int qLength (Q * list);

double randomX ( );
double randomR( double rate );
int calcNumSubtasks( );
int calcArrivalTime ( int prevArrTime, double arrRate );
int calcServiceTime ( double mu );

//--	Main

int main (int argc, char ** argv)
{
    int mode = 0;
    if( argc == 5 )
    {
        printf( "In Mode 1:\n\n" );
        mode = 1;
    }
    else if( argc == 2 )
    {
        printf( "In Mode 2:\n\n" );
        mode = 2;
    }
    else
    {
        printf( "Invalid number of arguments\n" );
        return 0;
    }

    time_t t;
    srand((unsigned) time(&t));

    runSimulation( "test3.txt", mode, argv );

    return 0;
}

//--	Fuctino to run simulations

void runSimulation( char * filename, int mode, char ** argv )
{
    int processors = 64;        //  Total num processors
    int availProc = 64;    //  Available Processors
    int totalTime0 = 0;         //  Total used time by 0 tasks
    int toatlTime1 = 0;         //  Total used time by 1 tasks
    int TotalServiceTime0 = 0;
    int TotalServiceTime1 = 0;
    int totalNumTasks0 = 0;
    int totalNumTasks1 = 0;
    int waitTime0 = 0;
    int waitTime1 = 0;
    float avgWaitTime0 = 0;
    float avgWaitTime1 = 0;
    float avgQLength = 0;
    int time = 0;
    int lastExitTime = -1;
    int lastEntryTime = 0;
    int totalLength = 0;
    int i;
    int q0length = 0;
    int q1length = 0;
    int totalNumTasks;
    float cpu = 0;
    float avgLBF = 0;
    float avgCPU = 0;
    float avgMew = 0;
    float mewMin = 0;
    float mewMax = 1000000000000;
    float totalNumSubtasks = 0;

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

    //  Check for mode
    if( mode == 1 )
    {
        double lam0 = atof(argv[1]);
        double lam1 = atof(argv[2]);
        double mu = atof(argv[3]);
        int numTasks = atoi(argv[4]);
        //  Create task queue for MODE 1
        generateInputQueue( &allTaskHead, &allTaskTail, lam0, lam1, mu, numTasks );
        allTaskHead = FEL_sort( allTaskHead, compar );
    }

    else if( mode == 2 )
    {
        char * filename = argv[1];
        //  Create task queue for MODE 2
        createTaskQueue( &allTaskHead, &allTaskTail, filename );
    }

    //qPrint( allTaskHead, "alltask" );

    totalNumTasks = qLength( allTaskHead );

    //~~~~  SIMULATION BEGINS HERE
    while( ( !queueIsEmpty( allTaskHead ) || !queueIsEmpty( q0head ) || !queueIsEmpty( q1head ) || !subQueueIsEmpty( processQ ) ) )
    {
        //printf( "\nTime: %d\n\n", time );
        //subQPrint( processQ, "processQ at beginning");

        if( !subQueueIsEmpty( processQ ) )
        {
            //	Check if anything needs to be popped
            scanAndPop( &processQ, &availProc );
        }

        //printf( "\nTime: %d\n\n", time );
        //printf( "\nNumber of available processors: %d\n", availProc );

        if( !queueIsEmpty( allTaskHead ) )
        {
            //	Take element from allTaskQueue
            while( allTaskHead != NULL && allTaskHead -> arrivalTime == time )
            {

                q0length += qLength( q0head );
                q1length += qLength( q1head );
                //	Pop and put to q1 or q0
                if ( allTaskHead -> priority == 0 )
                {
                    totalNumTasks0++;
                    qPush( &q0head, &q0tail, qPop( &allTaskHead ) );
                }
                else if ( allTaskHead -> priority == 1 )
                {
                    totalNumTasks1++;
                    qPush( &q1head, &q1tail, qPop( &allTaskHead ) );
                }
            }
        }

        //printf( "\nBefore serving\n" );
        //qPrint( q0head, "q0" );
        //qPrint( q1head, "q1" );

        //  While avProc > 0 keep scanning q1 and q0 and not

        if( !queueIsEmpty( q0head ) )
        {
            //	Take element from q0head
            task = scanQueue( &q0head, availProc );

            while ( task != NULL )
            {
                // create subtasks
                waitTime0 += time - task -> arrivalTime;
                for ( i = 0; i < task -> numSubTasks; i++ )
                {
                    sub * subTask = subCreate( task -> subtasks[i], time );
                    //push subtasks
                    cpu += subTask -> subDuration;
                    totalNumSubtasks++;
                    if( subTask -> subDuration > mewMin )
                    {
                        mewMin = subTask -> subDuration;
                    }
                    if( subTask -> subDuration < mewMax )
                    {
                        mewMax = subTask -> subDuration;
                    }
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
            task = scanQueue( &q1head, availProc );

            while ( task != NULL )
            {
                // create subtasks
                waitTime1 += time - task -> arrivalTime;
                for ( i = 0; i < task -> numSubTasks; i++ )
                {
                    sub * subTask = subCreate( task -> subtasks[i], time );
                    //push subtasks
                    cpu += subTask -> subDuration;
                    totalNumSubtasks++;
                    if( subTask -> subDuration > mewMin )
                    {
                        mewMin = subTask -> subDuration;
                    }
                    if( subTask -> subDuration < mewMax )
                    {
                        mewMax = subTask -> subDuration;
                    }
                    //push subtasks
                    subPush( subTask, &processQ );
                }

                // update availProc
                availProc -= task -> numSubTasks;

                task = scanQueue( &q1head, availProc );
            }
        }

        //printf( "Printing here!!!!\n");
        //printf( "Wait time = %d %d\nQueuelength = %d\nTotal time = %d\n\n", waitTime0, waitTime1, q0length + q1length, time );

        //printf( "\nTime: %d\n\n", time );
        //printf( "\nNumber of available processors: %d\n", availProc );
        //qPrint( allTaskHead, "alltask" );
        //qPrint( q0head, "q0" );
        //qPrint( q1head, "q1" );
        //subQPrint( processQ, "processQ at end");

        time++;

        //  Decrease duration in processQ HERE
        decreaseDuration( &processQ );
        //~~~	WHILE LOOP ENDS HERE
    }

    avgQLength = (( q0length + q1length ) / (time));
    avgWaitTime0 = (float)waitTime0 / totalNumTasks0;
    avgWaitTime1 = (float)waitTime1 / totalNumTasks1;
    avgCPU = cpu / ( 64 * time );
    avgMew = cpu / totalNumSubtasks;
    avgLBF = ( mewMin - mewMax ) / avgMew;

    //printf( "Wait time = %d %d\nQueuelength = %d\nTotal time = %d\n", waitTime0, waitTime1, q0length + q1length, time );
    //printf( "totalNumTasks0 = %d\ntotalNumTasks1 = %d\n", totalNumTasks0, totalNumTasks1 );
    //printf( "Total time cpus = %f\nTotal time / 64 = %f\n", cpu, cpu/64 );
    //printf( "Total time cpus = %f\nTotal num subtasks = %f\n", cpu, totalNumSubtasks );
    //printf( "mewMin = %f\nmewMax = %f\navgmew = %f\n", mewMin, mewMax, avgMew );

    printf( "Average Wait Time 0: %.2f\n", avgWaitTime0 );
    printf( "Average Wait Time 1: %.2f\n", avgWaitTime1 );
    printf( "Average Queue Length: %.2f\n", avgQLength );
    printf( "Average Processor Utilization: %.2f\n", avgCPU );
    printf( "Average Load Balancing factor: %.2f\n", avgLBF );

}

//--	Get queue Length

int qLength (Q * list)
{
    int length;

    if(list == NULL)
    {
        length = 0;
    }

    else

    {
        length = qLength (list -> next) + 1;
    }

    return length;
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
        //printf( "\n~~~~1st pop Popping~~~~~\n");
        //subQPrint( element, "Process element" );
    }
    sub * temp = ( * head );
    while( temp != NULL && temp -> next != NULL )
    {
        if( temp -> next -> subDuration <= 0 )
        {
            //	Pop
            sub * element = subPopMiddle( &temp );
            ( * availProc )++;
            //printf( "\n~~~~~~Popping~~~~~~\n");
            //subQPrint( element, "Process element" );
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
    if( temp != NULL )
    {
        //printf( "numSubTasks = %d and availProc = %d\n", temp -> numSubTasks, availProc );
    }
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

//--	Gererate task queue using

void generateInputQueue (  Q * * head, Q * * tail, double lam0, double lam1, double mu, int numTasks )
{
    int arrivalTime = 0;
    int numSubtasks = 0;
    int prevArrTime = 0;
    int i = 0;
    Q * element = NULL;

    for( i = 1; i <= numTasks; i++ )
    {
        //	Create the Q for 0s
        arrivalTime = calcArrivalTime( prevArrTime, lam0 );
        prevArrTime = arrivalTime;
        numSubtasks = calcNumSubtasks( );
        //	Create Q element
        element = qCreateFEL( arrivalTime, 0, numSubtasks, mu );
        //	Push to Q queue
        qPush( &(*head), &(*tail), element );
    }

    prevArrTime = 0;

    for( i = 1; i <= numTasks; i++ )
    {
        //	Create the Q for 0s
        arrivalTime = calcArrivalTime( prevArrTime, lam1 );
        prevArrTime = arrivalTime;
        numSubtasks = calcNumSubtasks( );
        //	Create Q element
        element = qCreateFEL( arrivalTime, 1, numSubtasks, mu );
        //	Push to Q queue
        qPush( &(*head), &(*tail), element );
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
    //	Allocate memory
    Q * element = malloc( sizeof(Q) );

    //	Fill in data
    element -> arrivalTime = arrivalTime;
    element -> priority = priority;
    element -> numSubTasks = numSubTasks;

    //	Create the subTask array
    for( i = 0; i < numSubTasks; i++ )
    {
        element -> subtasks[i] = subtasks[i];
    }

    element -> next = NULL;

    return element;
}

//--	To create the queue element

Q * qCreateFEL( int arrivalTime, int priority, int numSubTasks, double mu )
{
    int i = 0;
    //	Allocate memory
    Q * element = malloc( sizeof( Q ) );

    //	Fill in data
    element -> arrivalTime = arrivalTime;
    element -> priority = priority;
    element -> numSubTasks = numSubTasks;

    //	Create the subTask array
    for( i = 0; i < numSubTasks; i++ )
    {
        //  Calc service time for each element in array
        element -> subtasks[i] = calcServiceTime( mu );
    }

    element -> next = NULL;

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

//--    Printing processQ

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

//--	Compare function for merge sort

int compar (int a, int b)
{
    return ( a - b );
}

//--	Merge function for merge sort

Q * FEL_merge(Q * lhs, Q * rhs, int (compar)(int, int))
{
    //special case
    if (lhs == NULL)
    {
        return rhs;
    }
    if (rhs == NULL)
    {
        return lhs;
    }

    //normal case

    Q * head = NULL;
    Q * tail = NULL;

    while (lhs != NULL && rhs != NULL)
    {
        int cmp = compar ((lhs -> arrivalTime), (rhs -> arrivalTime));
        //left smaller than right
        if (cmp <= 0)
        {

            if(tail == NULL)
            {

                head = lhs;
                tail = lhs;
            }
            else
            {
                tail -> next = lhs;
                tail = lhs;
            }

            lhs = lhs -> next;
            tail -> next = NULL;
        }


        //right smaller than left
        else
        {
            if (tail == NULL)
            {
                head = rhs;
                tail = rhs;
            }
            else
            {
                tail -> next = rhs;
                tail = rhs;
            }

            rhs = rhs -> next;
            tail -> next = NULL;
        }
    }

    if(lhs == NULL)
    {
        tail -> next = rhs;
    }
    else
    {
        tail -> next = lhs;
    }
    return head;
}

//--	To sort out the allTask queue

Q * FEL_sort(Q * list, int (compar)(int, int))
{
    int length = qLength ( list );

    if (length == 0 || length == 1)
    {
        return list;
    }

    //split into 2 lists
    int half_length = length / 2;

    Q * left_list = list;

    Q * temp_list = list;
    while ( (--half_length) > 0 )
    {
        temp_list = temp_list -> next;
    }

    Q * right_list = temp_list -> next;
    temp_list -> next = NULL;

    //recursive call
    left_list = FEL_sort(left_list, compar);
    right_list = FEL_sort(right_list, compar);

    //merge lists
    return FEL_merge (left_list, right_list, compar);
}

//-- generate random X

double randomX ( )
{
    double x = ((double) rand() / (double) RAND_MAX );

    return x;
}

//-- generate random R

double randomR ( double rate )
{
    double x = randomX ();
    double r = ( (double)(-1/rate) * (double)(log( 1 - x )) );

    return r;
}

//-- generate random numSubTasks

int calcNumSubtasks ( )
{
    int numSubTasks = rand() % 32 + 1;

    return numSubTasks;
}

//-- generate random arrival time

int calcArrivalTime ( int prevArrTime, double arrRate )
{
    double r = randomR ( arrRate );

    int intArrivalTime = (int)(ceil(( 10 * (arrRate) * (exp( -( arrRate * r ))) )));

    int arrivalTime = prevArrTime + intArrivalTime;

    return arrivalTime;
}

//-- generate random service time

int calcServiceTime ( double mu )
{
    double r = randomR ( mu );
    int serviceTime = (int)(ceil(( 10 * (mu) * (exp( -( mu * r ))) )));

    return serviceTime;
}
