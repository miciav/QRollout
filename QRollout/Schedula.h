#ifndef SCHEDULA_H
#define SCHEDULA_H
#include "Struct.h"

void AggiungiSchedula(TSchedula *pSched, TJob pTask,int pTime,int pSetup_Vett);
void InizializzaSchedula ( TSchedula *pSched );
void EliminaSchedula(TSchedula *pSched);
void CopiaSchedule(TSchedula *pSched1,TSchedula *pSched2);
void AzzeraSchedule(void);
void DistruggiSchedule(int pNum_Macchine);
int CmaxSchedula(TSchedula *pSched);
int MaxCmaxSchedula(TSchedula **pVett_Schedule,int pNum_Macchine);
int MinCmaxSchedula(TSchedula **pVett_Schedule,int pNum_Macchine);
TQuaterna *ValutaSingolaSchedula(TSchedula *M_sch);
int Costruisci_E_Valuta_Schedula(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale,TNext_Elem *prossimo,TJob *perm,int dim_job);
#endif // SCHEDULA_H
