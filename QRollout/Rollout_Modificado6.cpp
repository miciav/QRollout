#include "qRolloutThread.h"
#include "Globals.h"
#include "Euristiche.h"
#include "Schedula.h"
#include <time.h>
#include <math.h>
#include <stdio.h>

//*************************************************************************************************
//       ROLLOUT Main function modified 9-8-2006
// 	inserira' nella lista dei job papabili solo quelli che hanno release date inferiore al minimo
// 	tempo di completamento delle schedule attuali.
//***********************************************************************************************
TJob *QRolloutThread::Rollout_Modificato6 ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
    int pp=0,jj=0;
    int iter_for = 0;
    int index,index1 =0;
    int cont_livelli= GNum_Job-1;
    int Pos_vincitore = 0;
    int Pos_assoluta=0;
    TJob **permutazioni;
    permutazioni= new TJob*[Fnum_Heur];
    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TJob *array_job_attuale_swap;//questo array conterrï¿½  una copia del array_job_attuale al passo precedente per poterlo ripristinare
    TSchedula *M1_sch_attuale;
    TSchedula **vett_schedule_parziali; //questo vettore conterrï¿½  i puntatori alle schedule parziali via via costruite dal rollout.
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job=1;
    int *punt_job_scelti;
    int *dimensione_lista_purificata;
    dimensione_lista_purificata = new int;
    Fpermutazione_Buffer = new TJob[GNum_Job];
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0; i < GNum_Job; i++ )
        array_job_attuale[i] = GArray_Job[i];

    TJob * perm_di_passaggio;
    num_job_daschedulare = GNum_Job;
    vett_schedule_parziali = new TSchedula*[GNum_Macchine];
    array_job_attuale_swap = new TJob[GNum_Job];
    while ( cont_livelli > 0 )
    {
        punt_job_scelti=NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        index=0;
        index1=0;

        // Per prima cosa devo calcolare il max tempo di completamento delle schedule parziali.
        int tempo_minimo_di_completamento=0; //in questo filtro non tengo conto delle indisponibilitï¿½  delle macchine.
        int tempo_minimo_di_release=10000000;//ï¿½? un numero arbitrariamente grande
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perÃ² devo verificare che il tempo di rilascio sia inferiore al max dei tempi di completamento delle schedule parziali.

        // devo quindi salvare in una struttura tutti i job validi e in un'altra tutti quegli elementi che pur non essendo ancora stati selezionati non verificano il criterio del filtro.
        if ( cont_livelli==GNum_Job-1 ) //
        {
            vett_schedule_parziali[0]=GMacch1_Sched;
            if ( GNum_Macchine==2 )
            {
                vett_schedule_parziali[1]=GMacch2_Sched;
            }
            if ( GNum_Macchine==3 )
            {
                vett_schedule_parziali[1]=GMacch2_Sched;
                vett_schedule_parziali[2]=GMacch3_Sched;
            }
            tempo_minimo_di_completamento=MinCmaxSchedula ( vett_schedule_parziali,GNum_Macchine );

            for ( i = 0;i<GNum_Job;i++ )
            {
                array_job_attuale_swap[i] = array_job_attuale[i];
            }
            for ( i=0;i<GNum_Job;i++ )
            {
                if ( array_job_attuale[i].ID !=-1 )
                {
                    if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                    {
                        tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                    }
                }
            }
            tempo_minimo_di_completamento=Max ( tempo_minimo_di_completamento,tempo_minimo_di_release );
            for ( i = 0; i < GNum_Job; i++ )
                if ( array_job_attuale[i].ID != -1 &&
                     array_job_attuale[i].rel_time > tempo_minimo_di_completamento )
                    array_job_attuale[i].ID = -1;//non schedulabile


        }
        else
        {
            AzzeraSchedule();
            perm_di_passaggio = new TJob[GNum_Job-cont_livelli-1];
            for ( pp = 0; pp < GNum_Job-cont_livelli-1; pp++ )
                perm_di_passaggio[pp] = GBest_Perm[pp];

            pp++;
            TNext_Elem *prossimo2;
            prossimo2 = new TNext_Elem;
            prossimo2->next = NULL;
            CostruisciEValutaSchedula (GMacch1_Sched,
                                          GMacch2_Sched,
                                          GMacch3_Sched,
                                          prossimo2,
                                          perm_di_passaggio,
                                          GNum_Job-cont_livelli-1 );
            delete prossimo2;
            delete[] perm_di_passaggio;
            vett_schedule_parziali[0]=GMacch1_Sched;
            if ( GNum_Macchine == 2 )
                vett_schedule_parziali[1] = GMacch2_Sched;
            if ( GNum_Macchine == 3 )
            {
                vett_schedule_parziali[1] = GMacch2_Sched;
                vett_schedule_parziali[2] = GMacch3_Sched;
            }
            tempo_minimo_di_completamento = MinCmaxSchedula ( vett_schedule_parziali,GNum_Macchine );
            for ( i = 0; i < GNum_Job; i++ )
                array_job_attuale[i] = array_job_attuale_swap[i];// ho modificato l'array_job_attuale_swap in modo che indichi come non schedulabile il job scelto al passo precedente.

            tempo_minimo_di_release = 10000000;//e' un numero arbitrariamente grande
            for ( i = 0; i < GNum_Job; i++ )
            {
                if ( array_job_attuale[i].ID !=-1 )
                {
                    if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                    {
                        tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                    }
                }
            }
            tempo_minimo_di_completamento=Max ( tempo_minimo_di_completamento,tempo_minimo_di_release );
            for ( i = 0; i < GNum_Job; i++ )
                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_minimo_di_completamento )
                    array_job_attuale[i].ID=-1;//non schedulabile


        }
        for ( iter_for = 0; iter_for < GNum_Job; iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if ( array_job_attuale[iter_for].ID == -1 ) //se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                array_job_attuale_swap[iter_for].ID =-1;

                array_job_attuale_temp = new TJob[cont_livelli];
                iter = 0;
                for ( kk=0;kk<cont_livelli;kk++ )
                {
                    while ( array_job_attuale_swap[iter].ID == -1 ) iter++;

                    array_job_attuale_temp[kk] = array_job_attuale_swap[iter];
                    iter++;
                }
                iter = 0;
                TNext_Elem *prossimo = NULL;
                TNext_Elem *prossimo1;
                for ( i = 0; i < Fnum_Heur; i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                {
                    // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                    AzzeraSchedule();
                    perm_di_passaggio = new TJob[GNum_Job];
                    InizializzaPermutazioneMigliore ( perm_di_passaggio );
                    for ( pp = 0; pp < GNum_Job-cont_livelli-1; pp++ )
                        perm_di_passaggio[pp]=GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];// aggiungo in coda il job prescelto

                    // _____________________________________________________________________________________________________________________________
                    AzzeraSchedule();
                    permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                    permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                    if ( prossimo == NULL )
                    {
                        prossimo = new TNext_Elem;
                        prossimo->ID_job = permutazioni[i][0].ID;
                        prossimo->ID_heur= Ffunzioni[i].ID_heur;
                        prossimo->next = NULL;
                    }
                    else
                    {
                        TNext_Elem *temp;
                        TNext_Elem *temp_prox;
                        temp = prossimo;
                        while ( temp->next!=NULL )
                            temp = temp->next;

                        temp_prox=new TNext_Elem;
                        temp->next = temp_prox;
                        temp = temp->next;
                        temp->ID_job = permutazioni[i][0].ID;
                        temp->ID_heur= Ffunzioni[i].ID_heur;
                        temp->next = NULL;
                    }
                    for ( jj = 0;pp < GNum_Job; pp++, jj++ )
                        perm_di_passaggio[pp] = permutazioni[i][jj];

                    //aggiungo i job proposti dall'euristica
                    AzzeraSchedule();
                    CostruisciEValutaSchedula (GMacch1_Sched,
                                                  GMacch2_Sched,
                                                  GMacch3_Sched,
                                                  prossimo,
                                                  perm_di_passaggio,
                                                  GNum_Job );
                    emit ScriviASchermo(  QString(" %1) %2 %3 %4 %5 num iter %6\n").
                                        arg(i).
                                        arg(FLmax).
                                        arg(FCmax).
                                        arg(FTardy).
                                        arg(Ffeasible).
                                        arg(GNum_Job-cont_livelli) );
                    SalvaSeMeglio ( perm_di_passaggio);
                    delete[] perm_di_passaggio;

                    //_____________________________________________________________________________________________________________________
                    //devo riportare la macchina nelle condizioni orginarie
                    if ( force == 1 )
                    {
                        prossimo1       = new TNext_Elem;
                        M1_sch_buffer   = new TSchedula;
                        M2_sch_buffer   = new TSchedula;
                        M3_sch_buffer   = new TSchedula;
                        CopiaSchedule ( GMacch1_Sched,M1_sch_buffer );
                        if ( GNum_Macchine >= 2 )
                            CopiaSchedule ( GMacch2_Sched,M2_sch_buffer );
                        if ( GNum_Macchine==3 )
                            CopiaSchedule ( GMacch3_Sched,M3_sch_buffer );

                        VNS (M1_sch_buffer,
                             M2_sch_buffer,
                             M3_sch_buffer );
                        BilanciamentoSchedule (M1_sch_buffer,
                                                M2_sch_buffer,
                                                M3_sch_buffer );//bilancio
                        ValutaSchedula (M1_sch_buffer,
                                         M2_sch_buffer,
                                         M3_sch_buffer,
                                         prossimo1 );

                        EliminaSchedula ( M1_sch_buffer );
                        if ( GNum_Macchine >= 2 ) {EliminaSchedula ( M2_sch_buffer );}
                        if ( GNum_Macchine == 3 ) {EliminaSchedula ( M3_sch_buffer );}
                        TNext_Elem *tmp_prox;
                        tmp_prox = prossimo;
                        while ( tmp_prox->next!=NULL )
                            tmp_prox = tmp_prox->next;

                        //trovo l'ultimo elemento.
                        if
                                (
                                 ( prossimo1->feasible > tmp_prox->feasible )
                                 ||
                                 (
                                     ( prossimo1->feasible == tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax < tmp_prox->Lmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible == tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax == tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax < tmp_prox->Cmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible == tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax == tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax == tmp_prox->Cmax )
                                     &&
                                     ( prossimo1->Tardy < tmp_prox->Cmax )
                                     )
                                 )
                        {
                            tmp_prox->Lmax=prossimo1->Lmax;
                            tmp_prox->Cmax=prossimo1->Cmax;
                            tmp_prox->Tardy=prossimo1->Tardy;
                            tmp_prox->feasible=prossimo1->feasible;
                        }
                        delete prossimo1;
                    }
                }
                // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                // devo ridurre il numero di job che rimangono da schedulare
                // devo trovare il job con la Lateness + alta
                // in condizioni di parita' quello con la Cmax +bassa
                // infine con il numero + basso di Tardy job
                array_job_attuale_swap[iter_for].ID = iter_for;
                delete[] array_job_attuale_temp;
                int ID_job = prossimo->ID_job;
                int macchina = prossimo->macchina;
                int fine = prossimo->fine;
                int inizio = prossimo->inizio;
                int tipo = prossimo->tipo;
                int index_camp = prossimo->index_camp;
                int LMAX = prossimo->Lmax_pers;
                int TARDY = prossimo->Tardy_pers;
                int L_max = prossimo->Lmax;
                int C_max = prossimo->Cmax;
                int tardy = prossimo->Tardy;
                int ID_heur = prossimo->ID_heur;
                int rel_time = prossimo->rel_time;
                int proc_time = prossimo->proc_time;
                int duedate = prossimo->duedate;
                int deadline = prossimo->deadline;
                int priority = prossimo->priority;
                Ffeasible = prossimo->feasible;
                TNext_Elem *temp = prossimo ;
                while ( temp->next!=NULL )
                {
                    if ( Ffeasible <= temp->next->feasible )
                    {
                        if
                                (
                                 ( Ffeasible ==0 )
                                 &&
                                 ( temp->next->feasible==1 )
                                 )
                        {
                            ID_job = temp->next->ID_job;
                            macchina = temp->next->macchina;
                            tipo = temp->next->tipo;
                            fine = temp->next->fine;
                            inizio = temp->next->inizio;
                            index_camp = temp->next->index_camp;
                            LMAX = temp->next->Lmax_pers;
                            TARDY = temp->next->Tardy_pers;
                            L_max = temp->next->Lmax;
                            C_max = temp->next->Cmax;
                            tardy = temp->next->Tardy;
                            deadline = temp->next->deadline;
                            duedate = temp->next->duedate;
                            proc_time = temp->next->proc_time;
                            rel_time = temp->next->rel_time;
                            priority = temp->next->priority;
                            ID_heur = temp->next->ID_heur;
                            Ffeasible = temp->next->feasible;
                        }
                        else if
                                (
                                 ( L_max > temp->next->Lmax )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max > temp->next->Cmax )
                                     )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max == temp->next->Cmax )
                                     &&
                                     ( tardy > temp->next->Tardy )
                                     )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max == temp->next->Cmax )
                                     &&
                                     ( tardy == temp->next->Tardy )
                                     &&
                                     ( LMAX > temp->next->Lmax_pers )
                                     )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max == temp->next->Cmax )
                                     &&
                                     ( tardy == temp->next->Tardy )
                                     &&
                                     ( LMAX > temp->next->Lmax_pers )
                                     &&
                                     (
                                         ( temp->next->duedate !=0 )
                                         &&
                                         ( duedate > temp->next->duedate )
                                         )
                                     )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max == temp->next->Cmax )
                                     &&
                                     ( tardy == temp->next->Tardy )
                                     &&
                                     ( LMAX > temp->next->Lmax_pers )
                                     &&
                                     ( duedate == temp->next->duedate )
                                     &&
                                     (
                                         ( temp->next->deadline !=0 )
                                         &&
                                         ( deadline > temp->next->deadline )
                                         )
                                     )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max == temp->next->Cmax )
                                     &&
                                     ( tardy == temp->next->Tardy )
                                     &&
                                     ( LMAX > temp->next->Lmax_pers )
                                     &&
                                     ( duedate == temp->next->duedate )
                                     &&
                                     ( deadline == temp->next->deadline )
                                     &&
                                     ( proc_time > temp->next->proc_time )
                                     )
                                 ||
                                 (
                                     ( L_max == temp->next->Lmax )
                                     &&
                                     ( C_max == temp->next->Cmax )
                                     &&
                                     ( tardy == temp->next->Tardy )
                                     &&
                                     ( LMAX > temp->next->Lmax_pers )
                                     &&
                                     ( duedate == temp->next->duedate )
                                     &&
                                     ( deadline == temp->next->deadline )
                                     &&
                                     ( proc_time == temp->next->proc_time )
                                     &&
                                     ( rel_time > temp->next->proc_time )
                                     )
                                 )
                        {
                            ID_job = temp->next->ID_job;
                            macchina = temp->next->macchina;
                            tipo = temp->next->tipo;
                            fine = temp->next->fine;
                            inizio = temp->next->inizio;
                            index_camp = temp->next->index_camp;
                            LMAX = temp->next->Lmax_pers;
                            TARDY = temp->next->Tardy_pers;
                            L_max = temp->next->Lmax;
                            C_max = temp->next->Cmax;
                            tardy = temp->next->Tardy;
                            deadline = temp->next->deadline;
                            duedate = temp->next->duedate;
                            proc_time = temp->next->proc_time;
                            rel_time = temp->next->rel_time;
                            priority = temp->next->priority;
                            ID_heur = temp->next->ID_heur;
                            Ffeasible = temp->next->feasible;
                        }
                    }
                    temp=temp->next;
                }//ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                //       e su quale macchina e' stato schedulato
                emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                    arg(cont_livelli).
                                    arg( ID_heur).
                                    arg( L_max).
                                    arg( C_max).
                                    arg(tardy ));
                // salvataggio nella lista delle info sul job prescelto in questo step
                lista_prossimi_vincitori[index].ID_job = ID_job;
                lista_prossimi_vincitori[index].macchina = macchina;
                lista_prossimi_vincitori[index].tipo =  tipo;
                lista_prossimi_vincitori[index].fine = fine;
                lista_prossimi_vincitori[index].inizio = inizio;
                lista_prossimi_vincitori[index].index_camp = index_camp;
                lista_prossimi_vincitori[index].Lmax_pers = LMAX;
                lista_prossimi_vincitori[index].Tardy_pers =TARDY;
                lista_prossimi_vincitori[index].Lmax = L_max;
                lista_prossimi_vincitori[index].Cmax = C_max;
                lista_prossimi_vincitori[index].Tardy = tardy;
                lista_prossimi_vincitori[index].deadline = deadline;
                lista_prossimi_vincitori[index].duedate = duedate;
                lista_prossimi_vincitori[index].proc_time = proc_time;
                lista_prossimi_vincitori[index].rel_time = rel_time;
                lista_prossimi_vincitori[index].priority = priority;
                lista_prossimi_vincitori[index].ID_heur = ID_heur;
                lista_prossimi_vincitori[index].feasible = Ffeasible;
                index++;
                // fine salvataggio ___________________________________________________________

                //qui salvo i job che si sono dimostrati i migliori.C_max

                TNext_Elem *temp_pr;
                for ( i = 0; i < Fnum_Heur; i++ )
                {
                    temp_pr = prossimo->next;
                    delete prossimo;
                    prossimo = temp_pr;
                }


                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                for ( i = 0; i < Fnum_Heur; i++ )
                    delete[] permutazioni[i];

            }
        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );

        Pos_vincitore=SelezionaProssimoJob ( lista_prossimi_vincitori,num_next_job );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta=TrovaPosizioneAssoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible>Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Ftardy_Best )
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = array_job_attuale[Pos_assoluta].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = array_job_attuale[Pos_assoluta].rel_time;
            array_job_attuale_swap[Pos_assoluta].ID = -1;
        }
        else
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].rel_time;
            array_job_attuale_swap[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;
        }
        cont_livelli--;
        if ( punt_job_scelti != NULL )
            delete[] punt_job_scelti;
    }


    i = 0;
    while ( array_job_attuale_swap[i].ID==-1 ) i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale_swap[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale_swap[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale_swap[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale_swap[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale_swap[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale_swap[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale_swap[i].rel_time;
    array_job_attuale[i].ID = -1;

    delete[] array_job_attuale;
    delete[] array_job_attuale_swap;
    AzzeraSchedule();
    TNext_Elem *prossimo1;
    prossimo1= new TNext_Elem;
    prossimo1->next = NULL;
    CostruisciEValutaSchedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;


    //  TODO questo si dovrebbe riscrivere. Molte cose sono fatte male.
    if
            (
             ( Ffeasible < Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax > Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax > Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy > Ftardy_Best )
                 )
             )
    {
        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return   GBest_Perm;
    }
}


