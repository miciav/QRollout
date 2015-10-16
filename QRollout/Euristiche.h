#ifndef EURISTICHE_H
#define EURISTICHE_H

#include "Struct.h"

/*
 * This is a static class containing static heuristic methods
*/
class Heuristics
{
private:
 static void initializeLocalSchedules(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale);
 static TSchedula** initializeArrayLocalSchedules(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale);
 static TJob1* copyArrayJob(TJob *from, int pDimJob);
 static void eliminaSchedule(TSchedula *M1_sch_locale, TSchedula *M2_sch_locale, TSchedula *M3_sch_locale);
 static TTipo_New *getStartingTimes(TSchedula **schedule);
 static int getMinimumScheduleTime(TTipo_New *tempo);
 static int divideJobs(TJob1 *array_job_locale, int dim_job, int minimum, int *deadline_vett, int *duedate_vett, int *else_vett);
 static int getElemPosinVett(int *vett, int dim_job);
 static int getMinInVett(int startingPos, int *vett, int dim_job);
 static int updateMachineUnaval(TTipo_New *tempo, TElem **indisp, int pMin);
 static void filterJobs(int *vett, int minimus, TJob1 *array_job_locale, int dim_job, int *set, int *tipo_job);
 static void updateTipoCampagna(int minimum, int *tipo_macchine, int *campagna_macchine, TTipo_New *tempo);
 static TJob *assignJobToPerm(int *set, int i);
 static void minMatrix(int dim_job, int **matrix, int pos_i,int pos_k);
 static TJob *assignJobToPermWithSetup(int **set_matrix, int pos_i, int pos_k);
 static int updateMachineUnavalVer2(TTipo_New *tempo, TElem **indisp, int pMin, int minimum);
 static int divideJobsVer2(TJob1 *array_job_locale, int dim_job, int minimum, int *deadline_vett, int *duedate_vett, int *else_vett);
 static int filterJobsDelta(int *vett, int minimus,TJob1 *pArray_jobs, int delta, int dim_job, int *set, int *tipo_job);
 static void filterJobs(int *vett, TJob1 *array_job_locale, int dim_job, int *set, int *tipo_job);
public:
 //PERMUTAZIONI
static TJob *PermutazioneEdd_1Tipo(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneEdd_2Tipo(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneBase(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_2(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_3(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_5(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_10(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_15(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_20(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_25(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *Permutazione_delta_30(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_35(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_40(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDelta_50(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneSPTSemplice(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneFittizia(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneLLFDelta_10(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneLLFDelta_5(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneLLFDelta_2(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneLLFDeltaBasica(TJob *pArray_Job_Attuale, int pDim_Job, int pDelta);
static TJob *PermutazioneLLF(TJob *pArray_Job_Attuale, int pDim_Job);
//_______________________________________________________________________________
static TJob *PermutazioneDelta_7ore(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *Permutazione_delta_14ore(TJob *pArray_Job_Attuale,int pDim_Job);
static TJob *PermutazioneDelta_3ProcMedio(TJob *pArray_Job_Attuale,int pDim_Job);
static TJob *PermutazioneDeltaProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneDeltaMezzoProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
//_______________________________________________________________________________

static TJob *PermutazioneLLFDelta_7ore(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneLLFdelta_14ore(TJob *pArray_Job_Attuale,int pDim_Job);
static TJob *PermutazioneLLFDelta_3ProcMedio(TJob *pArray_Job_Attuale,int pDim_Job);
static TJob *PermutazioneLLFDeltaProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
static TJob *PermutazioneLLFDeltaMezzoProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
//_______________________________________________________________________________

static TTipo_New CaricaTempo(TSchedula *M_sch, TElem *M);
static int CalcolaProcTimeMedioArrayJob(void);
static int AggiungiJobPermDelta (TJob1 *pArray_Job_Attuale,int pDim_Job,TSchedula **Schedule_Locali,int pDelta);
static void InizializzaPermutazioneMigliore(TJob *pPerm_Di_Passaggio);
static int TrovaEdgeIndisp(TElem *M,int minimum);
static int AggiungiJobPerm (TJob1 *pArray_Job_Attuale,int pDim_Job,TSchedula **Schedule_Locali);

};
#endif // EURISTICHE_H
