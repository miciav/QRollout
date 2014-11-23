#ifndef GLOBALS_H
#define GLOBALS_H
#include "Struct.h"
// secondi prima che la funzione clock cominci a tornare indietro
#define MAX_SECONDS 2147
#define MAX_TIPI 40

extern int GNum_Job;
extern int GNum_Tipi;
extern int GCmaj_Matrix[MAX_TIPI][MAX_TIPI];
extern int GNum_Macchine;
extern TElem *GMacch1;
extern TElem *GMacch2;
extern TElem *GMacch3;
extern TSchedula *GMacch1_Sched;
extern TSchedula *GMacch2_Sched;
extern TSchedula *GMacch3_Sched;
extern TJob *GBest_Perm;
extern TJob *GArray_Job;
extern TTipo *GArray_Tipi;

#endif // GLOBALS_H
