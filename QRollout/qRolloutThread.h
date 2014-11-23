#ifndef QROLLOUT_H
#define QROLLOUT_H

#include <time.h>
#include <QThread>
#include <QObject>
#include "Struct.h"
#include <stdint.h>






void VerificaMacchina(TSchedula *temp, TElem  *temp1, int *disponibilita, int *setup_vett,int p,TJob *perm,int i);


class  QRolloutThread : public QThread {
Q_OBJECT
public:    

    void run();
    int QRolloutThreadInicialize(QString pFileConfig, QString pFileSolution);
    int getNumJob();
    QStringList *GetListaAlgoritmos();
    QStringList *GetListaEuristicas();
    QStringList *GetListaRicercheLocali();
    QStringList *GetListaPolitiche();
    //
signals:
    void ScriviASchermo(QString pTexto);

private:
    TNext_Elem *Fprossimo;
    FILE *Fistanza;// file di testo formattato contenente l'istanza da eseguire.
    FILE *FfileOut;// file di testo in cui verra' salvato l'output
    FILE *FFile_Out_Lmax;
    FILE *FFile_Out_Cmax;
    FILE *FFile_Out_Tardy;
    FILE *FFile_Out_Feas;
    FILE *FFile_Out_Tempo;
    TJob* FPermutazione_Finale;
    time_t FTempo_Sec_Inizio1;
    time_t FTempo_Sec_Fine1;
    int FNum_Heur;
    int FEuristica; //indica quale delle euristiche bisogna lanciare nel caso non si usi il rollout
    int FLocal_Search_Mode;
    int FTempo;
    int FRollout_Or_Heuristic;
    char FInstance_File_Name[256];
    char FOutput_File[256]; // Nome file di output della configurazione
    int Fforce;
    int Ftipo_Eur;
    int Ftipo_Rollout;
    char *Fout_Lmax;
    char *Fout_Cmax;
    char *Fout_Tardy;
    char *Fout_Feas;
    char *Fout_Tempo;
    char Fstr_Temp[100];
    int Fnum_Heur ;
    TJob* Fpermutazione_Buffer;
    TStruttura_Funz *Ffunzioni;
    int Lmax_best;
    int Cmax_best;
    int Ftardy_Best;
    int Ffeasible_Best;
    int Fswap_Lat_Tard;
    int Ffeasible; // mi dice se la schedula costruita e' feasible
    int FNum_Heur_Used; //mantiene il numero di euristiche che si stanno utilizzando
    int FPolitica_Pruning; //indica quale politica di esclusione si sta utilizzando
    clock_t FTempo_Fine1;
    clock_t FTempo_Fine2;
    clock_t FTempo_Fine3;
    clock_t FTempo_Inizio1;
    clock_t FTempo_Inizio2;
    clock_t FTempo_Inizio3;
    clock_t FTempo_Inizio4;

    //_______________________________________________________________________________
    int FLmax;// massima lateness
    int FCmax;//Makespan
    int FTardy;//numero di tardy jobs per schedula
    //________________________________________________________________________________

    void CaricaFileConfigurazione(char *path_file_conf);

    //______________________________________________________________________________

    int CaricaIndisponibilita(FILE *pIstanza);
    void DistruggiIndisponibilita(int pNum_Macchine);
    void EliminaLista(TElem *pPunt_Lista);
    int CaricaListaJob(FILE * pIstanza);

    TJob *Rollout(int pForce);
    TJob *Rollout_Old(int pForce);
    TJob *Rollout_Modificato1(int pForce);
    TJob *Rollout_Modificato2(int pForce);
    TJob *Rollout_Modificato3(int pForce);
    TJob *Rollout_Modificato4(int pForce);
    TJob *Rollout_Modificato5(int pForce);
    TJob *Rollout_Modificato6(int pForce);
    TJob *Rollout_Time(int pForce,int pTempo);
    TJob *Rollout_Heuristic_Pruning(int pForce);
    TJob *Rollout_Dynamic_Job_Choosing ( int pForce);
    TJob *Rollout_Dynamic ( int pForce );
    int64_t GetCpuTime(void);
    int Round(double pValue);

    void StampaRisultatiAVideo(int flag);
    void StampaRisultatiSuFile(FILE *FfileOut,char *FInstance_File_Name,int flag,int Fforce);

    int Min(int a, int b);
    int Max(int a, int b);

    void StampaPercentualiUtilizzoAVideo(void);
    void StampaPermutazioni(TJob *permutazione,int dim);
    void VerificaCambiamentoMacchina(int *primo_passo_M1,int *primo_passo_M2,int *primo_passo_M3);
    void StampaSequenzaMacchina(TSchedula *M);
    void VerificaSeElementiUguali(TJob *perm_di_passaggio,int GNum_Job);



    int *TrovaMigliori(TNext_Elem *lista_prossimi_vincitori,int num_next_job);
    int *TrovaProssimiMigliori(TNext_Elem *pLista_Prox_Vincitori,int pNum_Next_Job );
    int *PurificaListaJobPrescelti(int *lista_migliori_passo_precedente,int dim_lista, int *prossima_dimensione);



    int Calcola_Numero_Euristiche(int Ftipo_Eur);
    void Inizializza_Struttura_Euristiche(int Ftipo_Eur);

    /*Funzioni per le ricerche locali*/
    void MossaSwap(TSchedula *pM1_sch,
                    TSchedula *pM2_sch,
                    TElem *pM1,
                    TElem *pM2,
                    TSchedula *pSch_Swap1,
                    TSchedula *pSch_Swap2,
                    TSchedula *pSchedula_di_lavoro1,
                    TSchedula *pSchedula_di_lavoro2);
    void MossaInsert(TSchedula *pInCuiInserire,
                      TSchedula *pInCuiLevare,
                      TElem *pM1,
                      TElem *pM2,
                      TSchedula *pSch_Insert,
                      TSchedula *pSch_Following,
                      TSchedula *pSchedula_di_lavoro1,
                      TSchedula *pSchedula_di_lavoro2);

    int RicercaLocaleTraMacchine(TSchedula *pM1_sch,
                                 TSchedula *pM2_sch,
                                 TSchedula *pM3_sch);

    int RicercaLocaleTraMacchineVND(TSchedula *pM1_sch,
                                    TSchedula *pM2_sch,
                                    TSchedula *pM3_sch);

    int RicercaLocaleTraMacchineCoda(TSchedula *pM1_sch,
                                     TSchedula *pM2_sch,
                                     TSchedula *pM3_sch);
    int VnsRicercaLocale(TSchedula *PM1_sch,
                         TSchedula *PM2_sch,
                         TSchedula *PM3_sch,
                         TElem *pM1,
                         TElem *pM2,
                         TElem *pM3);

    void MossaSpostaInCoda(TSchedula *pM_sch,
                           TElem *pM,
                           TSchedula *pIn_Coda,
                           TSchedula *pSch_Coda);
    void MossaInsertCoda(TSchedula *pInCuiInserire,
                         TSchedula *pInCuiLevare,
                         TElem *pM1,
                         TElem *pM2,
                         TSchedula *pSch_Insert,
                         TSchedula *pSchedula_di_lavoro1,
                         TSchedula *pSchedula_di_lavoro2);

    TInsieme_Dinamico *AggiungiDinamicamente(TInsieme_Dinamico *insieme, TJob job_insert);
    TInsieme_Dinamico *Copia_Dinamicamente(TInsieme_Dinamico *insieme, TInsieme_Dinamico *insieme_next, TJob job_insert);
    int VNS(TSchedula *M1_sch_buffer,TSchedula *M2_sch_buffer,TSchedula *M3_sch_buffer);
    int BilanciamentoSchedule(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale);
    float *CalcolaStatistiche();

    //POLITICHE (CREARE ALTRI 2 FILE)

    void PoliticaNeverWin(TStruttura_Funz *pFunzioni);
    void PoliticaLess_3perc(TStruttura_Funz *pFunzioni);
    void PoliticaLess_5perc(TStruttura_Funz *pFunzioni);
    void PoliticaLess_7perc(TStruttura_Funz *pFunzioni);
    void PoliticaLess_3percReset(TStruttura_Funz *pFunzioni);
    void PoliticaLess_5percReset(TStruttura_Funz *pFunzioni);
    void PoliticaLess_7percReset(TStruttura_Funz *pFunzioni);
    void PoliticaLinearSpread(TStruttura_Funz *pFunzioni, int cont_livelli);
    void PoliticaLinearSpreadReset(TStruttura_Funz *pFunzioni, int cont_livelli);
    void PoliticaNeverWinOneAtATime(TStruttura_Funz *pFunzioni);

    void EscludiEuristica(TStruttura_Funz *pFunzioni, int politica, int cont_livelli);


    TSchedula *Mossa(TSchedula *M_sch, TElem *M, int pos_iniziale, int pos_finale);
    void OrdinaCandidati(TSchedula **arraySch, TElem **arrayM);
    int VnsPerMacchina( TSchedula *pMacch_sch,TElem *pElem );

    void Aggiorna_Temp_Prox(int k,TJob perm,TSchedula *M_sch,TNext_Elem *temp_prox);
    int CostruisciEValutaSchedula(TSchedula *pM1_Sch,
                                  TSchedula *pM2_Sch,
                                  TSchedula *pM3_Sch,
                                  TNext_Elem *pProx_Elem,
                                  TJob *pPerm,
                                  int pDim_job );

    void ValutaSchedula(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale,TNext_Elem *prossimo);
    void SostituisciSchedule(TSchedula **arraySch, TSchedula *schedula_di_lavoro1, TSchedula *schedula_di_lavoro2, TSchedula *schedula_di_lavoro3);

    int SelezionaProssimoJob(TNext_Elem *lista_prossimi_vincitori,int cont_livelli);
    void SalvaSeMeglio(TJob* permutazioni);
    int TrovaPosizioneAssoluta(TJob *array,int pos_rel);

};

#endif // QROLLOUT_H
