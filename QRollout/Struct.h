#ifndef STRUCT_H
#define STRUCT_H
//STRUCT
struct Str_Elemento
        {
        int inizio;
        int fine;
        struct Str_Elemento *next;
        };
typedef struct Str_Elemento TElem;

struct Str_Sched //questa struttura conterr�le schedulazioni per ogni macchina
        {
        int ID_job;
        int tipo;
        int inizio;
        int fine;
        int Lmax;
        int Tardy;
        int index_camp;
        struct Str_Sched *next;
        };
typedef struct Str_Sched TSchedula;

struct Str_Job
        {
        int ID;
        int tipo;
        int proc_time;
        int duedate;
        int deadline;
        int priority;
        int rel_time;
        };
typedef struct Str_Job TJob;

struct Str_Job1
        {
        int ID;
        int tipo;
        int proc_time;
        int duedate;
        int deadline;
        int priority;
        int rel_time;
        int adatto;
        int schedulato;
        };
typedef struct Str_Job1 TJob1;

struct Str_TIPO
        {
        int ID;
        int MaxOpCamp;
        };
typedef struct Str_TIPO TTipo;

struct Str_TIPO_New
        {
        int fine;
        int tipo;
        int Camp;
        };
typedef struct Str_TIPO_New TTipo_New;

struct Str_Inf
        {
        int ID_job;
        int tipo;
        int proc_time;
        int duedate;
        int deadline;
        int priority;
        int rel_time;
        int index_camp;
        int ID_heur;
        int macchina;
        int inizio;
        int fine;
        int Lmax_pers;
        int Tardy_pers;
        int Lmax;
        int Cmax;
        int Tardy;
        int feasible;
        struct Str_Inf *next;
        };
typedef struct Str_Inf TNext_Elem;

struct Str_TIPO_Funzione
        {
        int perc_utilizzo;
        int ID_heur;
        TJob * (*funz)(TJob *,int);
        };
typedef struct Str_TIPO_Funzione TStruttura_Funz;

struct Str_TIPO_Quaterna
        {
        int Lmax;
        int Cmax;
        int Tardy;
        int Feasible;
        };
typedef struct Str_TIPO_Quaterna TQuaterna;

//struttura che modella il nodo di una lista di job, la quale è usata come insieme di jobs.
struct Str_Insieme_Dinam{
        int ID;
        int tipo;
        int proc_time;
        int duedate;
        int deadline;
        int priority;
        int rel_time;
        struct Str_Insieme_Dinam *next;
};
typedef struct Str_Insieme_Dinam TInsieme_Dinamico;
#endif // STRUCT_H
