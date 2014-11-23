#ifndef EURISTICHE_H
#define EURISTICHE_H

#include "Struct.h"
//PERMUTAZIONI
TJob *PermutazioneEdd_1Tipo(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneEdd_2Tipo(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneBase(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_2(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_3(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_5(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_10(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_15(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_20(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_25(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *Permutazione_delta_30(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_35(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_40(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDelta_50(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneSPTSemplice(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneFittizia(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneLLFDelta_10(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneLLFDelta_5(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneLLFDelta_2(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneLLFDeltaBasica(TJob *pArray_Job_Attuale, int pDim_Job, int pDelta);
TJob *PermutazioneLLF(TJob *pArray_Job_Attuale, int pDim_Job);
//_______________________________________________________________________________
TJob *PermutazioneDelta_7ore(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *Permutazione_delta_24ore(TJob *pArray_Job_Attuale,int pDim_Job);
TJob *PermutazioneDelta_3ProcMedio(TJob *pArray_Job_Attuale,int pDim_Job);
TJob *PermutazioneDeltaProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneDeltaMezzoProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
//_______________________________________________________________________________

TJob *PermutazioneLLFDelta_7ore(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *Permutazione_LLF_delta_24ore(TJob *pArray_Job_Attuale,int pDim_Job);
TJob *PermutazioneLLFDelta_3ProcMedio(TJob *pArray_Job_Attuale,int pDim_Job);
TJob *PermutazioneLLFDeltaProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
TJob *PermutazioneLLFDeltaMezzoProcMedio(TJob *pArray_Job_Attuale, int pDim_Job);
//_______________________________________________________________________________

TTipo_New CaricaTempo(TSchedula *M_sch, TElem *M);
int CalcolaProcTimeMedio(void);
int AggiungiJobPermDelta (TJob1 *pArray_Job_Attuale,int pDim_Job,TSchedula **Schedule_Locali,int pDelta);
void InizializzaPermutazioneMigliore(TJob *pPerm_Di_Passaggio);
int TrovaEdgeIndisp(TElem *M,int minimum);
int AggiungiJobPerm (TJob1 *pArray_Job_Attuale,int pDim_Job,TSchedula **Schedule_Locali);

#endif // EURISTICHE_H
