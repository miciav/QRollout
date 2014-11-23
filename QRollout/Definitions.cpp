#include "qrollout.h"
int num_job;
int num_heur ;
schedula *M1_sch,*M2_sch,*M3_sch;
job* permutazione_buffer;
job *array_job;
job *best_perm;
strutt_funz *funzioni;
int Lmax_best;
int Cmax_best;
int Tardy_best;
int Feasible_best;
int swap_lat_tard;
int Feasible; // mi dice se la schedula costruita �feasible
int num_heur_utilizz; //mantiene il numero di euristiche che si stanno utilizzando
int politica_pruning; //indica quale politica di esclusione si sta utilizzando
clock_t tempo_fine1,tempo_fine2,tempo_fine3;
clock_t tempo_inizio1,tempo_inizio2,tempo_inizio3;
clock_t tempo_inizio4;
int num_tipi ;
tipo *array_tipi;
int Cmaj_matrix[Max_tipi][Max_tipi];
int num_macchine;

int Lmax;// massima lateness
int Cmax;//Makespan
int Tardy;//numero di tardy jobs per schedula
elem *M1,*M2,*M3;
