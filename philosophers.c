#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

void table(){

    //initialise the 5 chopsticks as free since none wil be eating, set as 0 if one of the is occupied
    int chopsticks[5] = {1,1,1,1,1};

    //there are 5 philosophers, 1 represents someone who eats while 0 represents someone who's thinking
    int philosophers[5];

    int isEating, left, right, current;

    for(;;){

        MPI_Recv(&current, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //normalise to 0 indexing because of rank
        current--;

        left = current;

        right = current+1;

        //circular
        if(right == 5) right = 0;

        //if someone is eating return the chopsticks and set him in the thinking state
        if(philosophers[current] == 1){

            chopsticks[left] = 1;

            chopsticks[right] = 1;

            philosophers[current] = 0;
        }

        //if someone is hungry
        else{

            isEating = chopsticks[left] * chopsticks[right];

            // if his left and right chopstick is available then eat
            if(isEating == 1){

                philosophers[current] = 1;

                chopsticks[left] = 0;

                chopsticks[right] = 0;
            }
            MPI_Send(&isEating, 1, MPI_INT, current+1, 0, MPI_COMM_WORLD);
        }
    }
}

void philosopher(int rank){

    int isEating;

    for(;;){

        MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        MPI_Recv(&isEating, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(isEating == 1){

            printf("PHILOSOPHER %d IS EATING\n",rank);

            sleep(rand() % 5);

            printf("PHILOSOPHER %d ATE\n", rank);

            MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        }

        else printf("PHILOSOPHER %d COULDNT EAT, BACK AT THINKING\n", rank);

        //make him think again untill he is available
        sleep(rand() % 5);
    }
}

int main(int argc, char *argv[]){

    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) table();

    else philosopher(rank);

    MPI_Finalize();

    return 0;
}
