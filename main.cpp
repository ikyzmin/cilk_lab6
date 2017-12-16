#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

const int SERIAL = 0;
const int CILK_SPAWN = 1;
const int CILK_FOR = 2;
const int OPENMP =3;

void work(int **dist,int k, int i,int size)
{

    for (int j = 0; j < size; ++j)
        if ((dist[i][k] * dist[k][j] != 0) && (i != j))
            if ((dist[i][k] + dist[k][j] < dist[i][j]) ||
                (dist[i][j] == 0))
                dist[i][j] = dist[i][k] + dist[k][j];
}

void floyd_warshall(int **dist,int size) {
    for (int k = 0; k < size; ++k)
        for (int i = 0; i < size; ++i)
            work(dist,k,i,size);
}

void floyd_warshall_cilk_spawn(int **dist,int size) {
    for (int k = 0; k < size; ++k)
        for (int i = 0; i < size; ++i)
            cilk_spawn work(dist,k,i,size);
}

void floyd_warshall_cilk_for(int **dist,int size) {
    cilk_for (int k = 0; k < size; ++k)
    for (int i = 0; i < size; ++i)
        work(dist,k,i,size);
}

void floyd_warshall_openmp(int **dist,int size) {
#pragma omp parallel num_threads(4)
#pragma omp single
    for (int k = 0; k < size; ++k)
        for (int i = 0; i < size; ++i)
#pragma omp task
                work(dist,k,i,size);
}

void startExperiment(int TYPE){
    srand(time(NULL));
    int **dist;
    double avg = 0;
    int lengths[] = {100, 200, 500, 1000, 2000, 3000, 4000, 5000};
    switch (TYPE) {
        case SERIAL:
            printf("Serial___________\n");
            break;
        case CILK_SPAWN:
            printf("Cilk Spawn_______\n");
            break;
        case CILK_FOR:
            printf("Cilk For_________\n");
            break;
        case OPENMP:
            printf("OpenMp___________\n");
            break;
        default:
            printf("Experiment type undefined");
            return;
    }
    for (int i = 0; i < 8; ++i) {
        avg = 0;
        dist = (int **) calloc(lengths[i], sizeof(int *));
        for (int j = 0; j < lengths[i]; ++j) {
            dist[j] = (int *) calloc(lengths[i], sizeof(int));
            for (int k = 0; k < lengths[i]; k++) {
                {
                    if (j != k)
                        dist[j][k] = rand() % 130;
                    else
                        dist[j][k] = 0;
                }
            }
        }
        for (int j = 0; j < 5; j++) {
        double st = omp_get_wtime();
            switch (TYPE) {
                case SERIAL:
                    floyd_warshall(dist, lengths[i]);
                    break;
                case CILK_SPAWN:
                    floyd_warshall_cilk_spawn(dist,lengths[i]);
                    break;
                case CILK_FOR:
                    floyd_warshall_cilk_for(dist,lengths[i]);
                    break;
                case OPENMP:
                    floyd_warshall_openmp(dist,lengths[i]);
                    break;
            }
        double fn = omp_get_wtime();
        avg += (fn-st);
        }
        avg/=5;
        printf("Average time is %f seconds\n", avg);
    }
}

int main(int argc, char *argv[]) {
    __cilkrts_set_param("nworkers","4");
    startExperiment(SERIAL);
    startExperiment(CILK_SPAWN);
    startExperiment(CILK_FOR);
    startExperiment(OPENMP);
}