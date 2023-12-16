#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#define ERROR_SCALE 100
#define PRECISION 50

typedef struct Limits{
    long limit_down;
    long limit_up;
}limits, *Limits;

/*
        Functie de ridicat la putere cu un exponent numar INTREG  
*/
long raise_to_power(int exponent, long base) {
    long pow = 1;
    while (exponent) {
        pow *= base;
        exponent--;
    }

    return pow;
}


/*
        Functie care calculeaza limita superioara si inferioara a unui numar
    ridicat la o putere de tip numar REAL
*/
Limits find_limits(long double exponent, long baza) {
    Limits lim = (Limits)malloc(sizeof(limits)); 
    long double index_jos = 1, index_sus = 1;
    long double fractiune = 0.5, radical = sqrt(baza);
    int iteratii = PRECISION;
    long double Floor = floor(exponent), Ceil = ceil(exponent), limita_jos = 0;
    long double limita_sus = Ceil - Floor;
    exponent -= Floor;

    while(iteratii) {
        if (exponent - limita_jos < limita_sus - exponent) {
            limita_sus -= fractiune;
            index_sus *= radical;
        } else {
            limita_jos += fractiune;
            index_jos *= radical;
        }

        iteratii--;
        fractiune /= 2;
        radical = sqrt(radical);
    }

    lim->limit_down = 
            (long)floor(raise_to_power(Floor, baza) * index_jos) - ERROR_SCALE;

    lim->limit_up = 
            (long)ceil(raise_to_power(Ceil, baza) / index_sus) + ERROR_SCALE;

    return lim;
}