#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

typedef struct FEL {
    int arrivalTime;
    int priority;
    int numSubTasks;
    int subtasks[32];
    struct FEL * next;
} FEL;

void generateInput( float, float, int, int );
double randomX ( );
double randomR( double rate );
int calcNumSubtasks( );
int calcArrivalTime ( int prevArrTime, double arrRate );
int calcServiceTime ( double mu );
FEL * felCreate( int arrivalTime, int priority ,int numSubtasks, int mu );
FEL * felPop( FEL * * head );
void felPush( FEL * * qHead, FEL * * qTail, FEL * element );


int main( int argc, char ** argv )
{
    generateInput( );

    return 0;
}

void generateInput ( double lam0, double lam1, double mu, int numTasks )
{
    int arrivalTime;
    int numSubtasks;
    int i = 0;
    int j = 0;

    FEL * felHead = NULL;
    FEL * felTail = NULL;
    FEL * element = NULL;

    for( i = 0; i <= numTasks; i++ )
    {
        //	Create the FEL for 0s
        arrivalTime = calcArrivalTime();
        numSubtasks = calcNumSubtasks();
        //	Create FEL element
        element = felCreate( arrivalTime, 0, numSubTasks, mu );
        //	Push to FEL queue
        felPush( felHead, felTail, element );
    }

    for( i = 0; i <= numTasks; i++ )
    {
        //	Create the FEL for 0s
        arrivalTime = calcArrivalTime();
        numSubtasks = calcNumSubtasks();
        //	Create FEL element
        element = felCreate( arrivalTime, 1, numSubTasks, mu );
        //	Push to FEL queue
        felPush( felHead, felTail, element );
    }

}

//--	Function to pop from FEL queue and return popped element

FEL * felPop( FEL * * head )
{
    FEL * poppedElement = NULL;
    if( (*head) != NULL )
    {
        poppedElement = (*head);
        (*head) = (*head) -> next;
        poppedElement -> next = NULL;
    }
    return poppedElement;
}

//--	Function to push element to FEL queue

void felPush( FEL * * qHead, FEL * * qTail, FEL * element )
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

//--	Function to create FEL element
FEL * felCreate( int arrivalTime, int priority ,int numSubtasks, int mu )
{
    int i = 0;
    //	Allocate memory
    FEL * element = malloc( sizeof( FEL ) );

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

//--    Generate random X

double randomX( )
{
    double x = ((double) rand() / (double) RAND_MAX );

    return x;
}

//--    Generate random R

double randomR( double rate )
{
    double r = ( (double)(-1/rate) * (double)(log( 1 - x )) );

    return r;
}

//--    Generate random numSubTasks

int calcNumSubTasks( )
{
    int numSubTasks = rand() % 32 + 1;

    return numSubTasks;
}

//--    Generate random arrival time

int calcArrivalTime( int prevArrTime, double arrRate )
{
    double r = randomR ( arrRate );

    int intArrivalTime = (int)( (arrRate) * (exp( -( arrRate * r ))) );

    int arrivalTime = prevArrTime + intArrivalTime;

    return arrivalTime
}

//-- generate random service time

int calcServiceTime ( double mu )
{
    double r = randomR ( mu );
    int serviceTime = (int)( (mu) * (exp( -( mu * r ))) );

    return serviceTime;
}
