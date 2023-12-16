#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "utils.h"
#include "Limits.h"

typedef struct Thread {
    pthread_t thread;
    long id;
    pthread_barrier_t *barrier;
    pthread_mutex_t *mutex;
    Q queue;
    int MAP;
    int RED;
    L** sets_list;
    L* R_set;
}Th, *TH;

/*
        Functie repartizeaza pentru fiecare thread de tip MAP un fisier pe
    care sa il prelucreze
*/
char* assignFiles(Th thread_info) {
    char *name = NULL;

    pthread_mutex_lock(thread_info.mutex);

    if (!is_empty(thread_info.queue)) {
        name = dequeue(thread_info.queue); 
    }

    pthread_mutex_unlock(thread_info.mutex);

    return name;
}

/*
        Functia pe care actioneaza threadurile de tip MAP
*/
void funcMAP(Th thread_info, char* name, L* sets) {
    
    long number;

    // Reading numbers from file and adding them
    FILE* fp = fopen(name, "r");
    fscanf(fp, "%ld\n", &number); //citesc numarul de numere
    while (fscanf(fp, "%ld\n", &number) != EOF) {
        if (number > 1) {
            for (int exponent = thread_info.RED + 1; exponent >= 2; exponent--) {
                long double logarithm = log2l(number) / exponent;
                Limits lim = find_limits(logarithm, 2);

                for (long baza = lim->limit_down; baza <= lim->limit_up; baza++) {
                    if (raise_to_power(exponent, baza) == number) {
                        append(sets[exponent - 2], number);
                        free(lim);
                        break;
                    }
                }
            }

        } else {
            if (number == 1) {
                for (int index = 0; index < thread_info.RED; index++) {
                    append(sets[index], number);
                }
            }
        }
    }

    fclose(fp);
    return;
}

/*
        Functia pe care actioneaza threadurile de tip RED
*/
void funcRED(Th thread_info, L** sets_list, L* R_set) {
    int count = 0;
    R_set[thread_info.id - thread_info.MAP] = init_list();
    for (int index = 0; index < thread_info.MAP; index++) {
        mergeLists(R_set[thread_info.id - thread_info.MAP], 
                        sets_list[index][thread_info.id - thread_info.MAP]);
    }
    insertSort(&R_set[thread_info.id - thread_info.MAP]->head);

    Nodei tmp = R_set[thread_info.id - thread_info.MAP]->head;
    if (tmp != NULL) {
        Nodei tmp_next = tmp->next;
        while (tmp != NULL) {
            if (tmp_next == NULL) {
                count++;
            } else {
                if (tmp->var != tmp_next->var) {
                    count++;
                }
                tmp_next = tmp_next->next;
            }
            tmp = tmp->next;
        }
    }
 
    int number = (int)(thread_info.id - thread_info.MAP + 2);
    char name[9];
    strcpy(name, "out1.txt");
    name[3] = (char)(number + '0');

    FILE* fp = fopen(name, "w");
    fprintf(fp, "%d", count);
    fclose(fp);

    
    destroy_list(R_set[thread_info.id - thread_info.MAP]);

    return;
}

/*
        Functia principala a tuturor thread-urilor
*/
void *func(void *arg) {

    Th thread_info = *(Th *)arg;
    
    if (thread_info.id < thread_info.MAP) {

        char *name = NULL;
        thread_info.sets_list[(int)thread_info.id] = 
                                    (L *)malloc(thread_info.RED * sizeof(L));
        for (int index = 0; index < thread_info.RED; index++) {
            thread_info.sets_list[(int)thread_info.id][index] = init_list();
        }

        while (!is_empty(thread_info.queue)) {
            name = assignFiles(thread_info);
            funcMAP(thread_info, name, 
                                thread_info.sets_list[(int)thread_info.id]);
            if (name != NULL) {
                free(name);
            }
        }
    }

    pthread_barrier_wait(thread_info.barrier);

    if (thread_info.id >= thread_info.MAP) {
        funcRED(thread_info, thread_info.sets_list, thread_info.R_set);
    }

    pthread_exit(NULL);
}

/*
        Functia care citeste din fisierul de input de unde se citeste numarul
    de fisiere si numele fisierelor din care vor citi threadurile de tip MAP
*/
void readFromFile(FILE *inputFile, char*** fileNames, int* numberOfFiles) {

    char line[30];
    
    fscanf(inputFile, "%d\n", numberOfFiles);
    
    char** files = (char **)calloc(*numberOfFiles, sizeof(char *));

    for (int index = 0; index < *numberOfFiles; index++) {
        fscanf(inputFile, "%s\n", line);
        files[index] = strdup(line);
    }

    *fileNames = files;
    return;
}

/*
        Functia care parseaza datele date la intrare
*/
void get_args(int *MAP, int *RED, FILE* inputFile, 
            char*** fileNames, int *numberOfFiles, int argc, char* argv[]) {

    if (argc != 4) {
        printf("Incorrect number of parameters!\n");
        exit(-1);
    }

    *MAP = atoi(argv[1]);
    *RED = atoi(argv[2]);   
    
    inputFile = fopen(argv[3], "r");
    readFromFile(inputFile, fileNames, numberOfFiles);
    fclose(inputFile);

    return;
}

/*
        Functia care initializeaza threadurile de tip MAP si RED
*/
void init(int nr_threads, TH threads) {

    // Initializam toate threadurile
    for (int index = 0; index < nr_threads; index++) {
        threads[index].id = index;
        pthread_create(&threads[index].thread, NULL, func, &threads[index]);
    }

    // Asteptam toate threadurile
    for (int index = 0; index < nr_threads; index++) {
        pthread_join(threads[index].thread, NULL);
    }

    return;
}

/*
        Functie de eliberare a memoriei ocupata de numele fisierelor
*/
void unaloccFiles(char** fileNames, int numberOfFiles) {

    for (int index = 0; index < numberOfFiles; index++) {
        free(fileNames[index]);
    }

    free(fileNames);

    return;
}

int main(int argc, char *argv[]) {
    
    int MAP, RED, numberOfFiles, status;
    char **fileNames = NULL;
    FILE *inputFile = NULL;
    pthread_barrier_t barrier;
    pthread_mutex_t mutex;
    Q queue = (Q)malloc(sizeof(q));
    init_queue(queue);
    get_args(&MAP, &RED, inputFile, &fileNames, &numberOfFiles, argc, argv);
    L** sets_list = (L **)malloc(MAP * sizeof(L *));
    L* R_set = (L *)malloc(RED * sizeof(L));

    Th threads[MAP + RED];

    status = pthread_barrier_init(&barrier, NULL, (unsigned)MAP + RED);
    if (status != 0) {
        printf("Cannot create barrier!\n");
    }

    status = pthread_mutex_init(&mutex, NULL);
    if (status != 0) {
        printf("Cannot create mutex!\n");
    }

    for (int index = 0; index < numberOfFiles; index++) {
        enqueue(queue, fileNames[index]);
    }

    for (int index = 0; index < MAP + RED; index++) {
        threads[index].id = index;
        threads[index].barrier = &barrier;
        threads[index].mutex = &mutex;
        threads[index].queue = queue;
        threads[index].MAP = MAP;
        threads[index].RED = RED;
        threads[index].sets_list = sets_list;
        threads[index].R_set = R_set;
    }

    init(MAP + RED, threads);


    // eliberare memorie folosita
    free(R_set);

    for (int index = 0; index < MAP; index++) {
        for (int num = 0; num < RED; num++) {
            destroy_list(sets_list[index][num]);
        }
        free(sets_list[index]);
    }
    free(sets_list);
    destroy_queue(queue);
    unaloccFiles(fileNames, numberOfFiles);
    status = pthread_barrier_destroy(&barrier);
    if (status != 0) {
        printf("Cannot destroy barrier!\n");
    }
    status = pthread_mutex_destroy(&mutex);
    if (status != 0) {
        printf("Cannot destroy mutex!\n");
    }

    return 0;
}