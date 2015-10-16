#ifndef SCHEDULA_H
#define SCHEDULA_H
#include "Struct.h"
class Schedula {
public:

static void AggiungiSchedula(TSchedula *pSched, TJob pTask,int pTime,int pSetup_Vett);
static void InizializzaSchedula ( TSchedula *pSched );
static void EliminaSchedula(TSchedula *pSched);
static void CopiaSchedule(TSchedula *pSched1,TSchedula *pSched2);
static void AzzeraSchedule(void);
static void DistruggiSchedule(int pNum_Macchine);
static int CmaxSchedula(TSchedula *pSched);
static int MaxCmaxSchedula(TSchedula **pVett_Schedule,int pNum_Macchine);
static int MinCmaxSchedula(TSchedula **pVett_Schedule,int pNum_Macchine);
static TQuaterna *ValutaSingolaSchedula(TSchedula *M_sch);
static int Costruisci_E_Valuta_Schedula(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale,TNext_Elem *prossimo,TJob *perm,int dim_job);
};
#endif // SCHEDULA_H
