#include "qRolloutThread.h"
#include "Globals.h"
#include "Euristiche.h"
#include "Schedula.h"
#include <time.h>

//*************************************************************************************************
//			ROLLOUT Main function
//***********************************************************************************************
TJob *QRolloutThread::Rollout ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    TNext_Elem *job_fisso ;
    int i,iter,kk  = 0;
    int pp=0,jj=0;
    int iter_for = 0;
    int index,index1 =0;
    int cont_livelli= GNum_Job-1;
    int Pos_vincitore = 0;
    int Pos_assoluta=0;
    TJob **permutazioni;
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    int fine = 0;
    TSchedula *M1_sch_attuale;
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    TSchedula *M3_sch_attuale;
    TJob * perm_di_passaggio;
    TNext_Elem *prossimo2;
    TNext_Elem *prossimo = NULL;
    TNext_Elem *prossimo1;
    TNext_Elem *temp;
    TNext_Elem *temp_prox;
    int ID_job ;
    int macchina;
    int inizio;
    int tipo;
    int index_camp;
    int LMAX;
    int TARDY;
    int L_max;
    int C_max;
    int tardy;
    int ID_heur;
    int rel_time;
    int proc_time;
    int duedate;
    int deadline;
    int priority;

    // FINE DICHIARAZIONE DELLE VARIABILI
    //___________________________________________________________________________________________________________

    permutazioni    = new TJob*[Fnum_Heur];
    Fpermutazione_Buffer = new TJob[GNum_Job];

    M1_sch_attuale  = GMacch1_Sched;
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    // in array job attuale metto una copia di array job
    array_job_attuale = new TJob[GNum_Job];
    for ( i = 0;i<GNum_Job;i++ )
        array_job_attuale[i] = GArray_Job[i];

    job_fisso = new TNext_Elem;

    while ( cont_livelli>0 )
    {
        TNext_Elem lista_prossimi_vincitori[500]; // questa la dovrei spostare
        //____________________________________________________________________!!!!!!!!!!

        index=0;
        index1=0;
        /*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/
        for ( iter_for = 0; iter_for < GNum_Job; iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
                        devo considerare il caso di tutti i job scedulati per primi  */
            if ( array_job_attuale[iter_for].ID!=-1 ) //se e' selezionabile
            {
                job_fisso->ID_job = array_job_attuale[iter_for].ID; //job base da cui faccio partire le euristiche
                array_job_attuale[iter_for].ID =-1;// non + selezionabile
                array_job_attuale_temp = new TJob[cont_livelli];
                iter=0;
                for ( kk=0;kk<cont_livelli;kk++ )
                {
                    while ( array_job_attuale[iter].ID==-1 )
                        iter++;

                    array_job_attuale_temp[kk]=array_job_attuale[iter];
                    iter++;
                }

                iter=0;
                prossimo = NULL;
                for ( i = 0;i<Fnum_Heur;i++ ) //num_heur dovra' essere cambiato con il numero di heuristiche effettivamente usato
                {
                    AzzeraSchedule();// azzero le schedule
                    //___________________________________________________________________________________
                    // calcolo i riempimenti parziali delle schedule
                    perm_di_passaggio = new TJob[GNum_Job];

                    InizializzaPermutazioneMigliore ( perm_di_passaggio );

                    for ( pp=0;pp<GNum_Job-cont_livelli-1;pp++ )
                        perm_di_passaggio[pp] = GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];
                    prossimo2 = new TNext_Elem;
                    prossimo2->next=NULL;
                    CostruisciEValutaSchedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo2,perm_di_passaggio,GNum_Job-cont_livelli );
                    delete prossimo2;
                    //___________________________________________________________________________________

                    permutazioni[i] = NULL;
                    permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                    if ( prossimo == NULL )
                    {
                        prossimo         = new TNext_Elem;
                        prossimo->ID_job = permutazioni[i][0].ID;
                        prossimo->ID_heur= Ffunzioni[i].ID_heur;
                        prossimo->next   = NULL;
                    }
                    else
                    {
                        // 						next_elem *temp;
                        // 						next_elem *temp_prox;
                        temp = prossimo;
                        while ( temp->next!=NULL )
                        {
                            temp = temp->next;
                        }
                        temp_prox    = new TNext_Elem;
                        temp->next   = temp_prox;
                        temp = temp->next;
                        temp->ID_job = permutazioni[i][0].ID;
                        temp->ID_heur= Ffunzioni[i].ID_heur;
                        temp->next   = NULL;

                    }
                    // calcolo il valore delle schedule con la permutazione data dall'euristica i-esima
                    for ( jj = 0; pp < GNum_Job; pp++,jj++ ) // completo la permutazione di passaggio
                    {
                        perm_di_passaggio[pp] = permutazioni[i][jj];
                    }
                    AzzeraSchedule();
                    CostruisciEValutaSchedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,perm_di_passaggio,GNum_Job );

                    QString TestoASchermo = QString(" %i) %i %i %i %i num iter %i\n").arg(i).arg(FLmax).arg(FCmax).arg(FTardy).arg(Ffeasible).arg(GNum_Job-cont_livelli);
                    emit ScriviASchermo(  TestoASchermo );
                    SalvaSeMeglio ( perm_di_passaggio );
                    delete[] perm_di_passaggio;
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
                        if ( GNum_Macchine == 3 )
                            CopiaSchedule ( GMacch3_Sched,M3_sch_buffer );

                        VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                        BilanciamentoSchedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                        ValutaSchedula (M1_sch_buffer,
                                         M2_sch_buffer,
                                         M3_sch_buffer,
                                         prossimo1 );

                        EliminaSchedula ( M1_sch_buffer );
                        if ( GNum_Macchine>=2 )
                            EliminaSchedula ( M2_sch_buffer );
                        if ( GNum_Macchine==3 )
                            EliminaSchedula ( M3_sch_buffer );

                        TNext_Elem *tmp_prox;
                        tmp_prox=prossimo;
                        while ( tmp_prox->next!=NULL )
                        {
                            tmp_prox = tmp_prox->next;
                        }//trovo l'ultimo elemento.
                        if
                                (
                                 ( prossimo1->feasible>tmp_prox->feasible )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax<tmp_prox->Lmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax==tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax<tmp_prox->Cmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax==tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax==tmp_prox->Cmax )
                                     &&
                                     ( prossimo1->Tardy<tmp_prox->Cmax )

                                     )
                                 )
                        {
                            tmp_prox->Lmax      = prossimo1->Lmax;
                            tmp_prox->Cmax      = prossimo1->Cmax;
                            tmp_prox->Tardy     = prossimo1->Tardy;
                            tmp_prox->feasible  = prossimo1->feasible;
                        }

                        delete prossimo1;
                    }
                }

                /* se la schedula non e' feasible deve essere penalizzata rispetto alle altre.  *
                 *devo ridurre il numero di job che rimangono da schedulare                     *
                 *devo trovare il job con la Lateness + alta                                    *
                 *in condizioni di parita' quello con la Cmax +bassa                            *
                 *infine con il numero + basso di Tardy job                                     */

                array_job_attuale[iter_for].ID = iter_for;
                delete[] array_job_attuale_temp;

                ID_job      = prossimo->ID_job;
                macchina    = prossimo->macchina;
                fine        = prossimo->fine;
                inizio      = prossimo->inizio;
                tipo        = prossimo->tipo;
                index_camp  = prossimo->index_camp;
                LMAX        = prossimo->Lmax_pers;
                TARDY       = prossimo->Tardy_pers;
                L_max       = prossimo->Lmax;
                C_max       = prossimo->Cmax;
                tardy       = prossimo->Tardy;
                ID_heur     = prossimo->ID_heur;
                rel_time    = prossimo->rel_time;
                proc_time   = prossimo->proc_time;
                duedate     = prossimo->duedate;
                deadline    = prossimo->deadline;
                priority    = prossimo->priority;
                Ffeasible   = prossimo->feasible;

                temp = prossimo ;

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
                            ID_job      = temp->next->ID_job;
                            macchina    = temp->next->macchina;
                            tipo        = temp->next->tipo;
                            fine        = temp->next->fine;
                            inizio      = temp->next->inizio;
                            index_camp  = temp->next->index_camp;
                            LMAX        = temp->next->Lmax_pers;
                            TARDY       = temp->next->Tardy_pers;
                            L_max       = temp->next->Lmax;
                            C_max       = temp->next->Cmax;
                            tardy       = temp->next->Tardy;
                            deadline    = temp->next->deadline;
                            duedate     = temp->next->duedate;
                            proc_time   = temp->next->proc_time;
                            rel_time    = temp->next->rel_time;
                            priority    = temp->next->priority;
                            ID_heur     = temp->next->ID_heur;
                            Ffeasible   = temp->next->feasible;
                        }
                    }
                    temp=temp->next;
                }
                //ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                // e su quale macchina e' stato schedulato

                emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").arg(cont_livelli).arg(ID_heur).arg( L_max).arg(C_max).arg(tardy ) );

                // salvataggio nella lista delle info sul job prescelto in questo step
                lista_prossimi_vincitori[index].ID_job      = ID_job;
                lista_prossimi_vincitori[index].macchina    = macchina;
                lista_prossimi_vincitori[index].tipo        = tipo;
                lista_prossimi_vincitori[index].fine        = fine;
                lista_prossimi_vincitori[index].inizio      = inizio;
                lista_prossimi_vincitori[index].index_camp  = index_camp;
                lista_prossimi_vincitori[index].Lmax_pers   = LMAX;
                lista_prossimi_vincitori[index].Tardy_pers  = TARDY;
                lista_prossimi_vincitori[index].Lmax        = L_max;
                lista_prossimi_vincitori[index].Cmax        = C_max;
                lista_prossimi_vincitori[index].Tardy       = tardy;
                lista_prossimi_vincitori[index].deadline    = deadline;
                lista_prossimi_vincitori[index].duedate     = duedate;
                lista_prossimi_vincitori[index].proc_time   = proc_time;
                lista_prossimi_vincitori[index].rel_time    = rel_time;
                lista_prossimi_vincitori[index].priority    = priority;
                lista_prossimi_vincitori[index].ID_heur     = ID_heur;
                lista_prossimi_vincitori[index].feasible    = Ffeasible;
                index++;
                // fine salvataggio ___________________________________________________________


                TNext_Elem *temp_pr;
                for ( i = 0;i < Fnum_Heur;i++ )
                {
                    temp_pr = prossimo->next;
                    delete prossimo;
                    prossimo = temp_pr;
                }


                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                for ( i = 0;i < Fnum_Heur;i++ )
                    delete[] permutazioni[i];

            }

        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );

        //_____________________________________________________________________________________________________________________
        Pos_vincitore=SelezionaProssimoJob ( lista_prossimi_vincitori,cont_livelli );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta = TrovaPosizioneAssoluta ( array_job_attuale, Pos_vincitore );

        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible > Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax < Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax < Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax == Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy < Ftardy_Best )
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
            array_job_attuale[Pos_assoluta].ID  = -1;
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
            array_job_attuale[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;
       }
        cont_livelli--;
    }

    i = 0;
    while ( array_job_attuale[i].ID==-1 )
        i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale[i].rel_time;
    array_job_attuale[i].ID=-1;

    delete job_fisso;
    delete[] array_job_attuale;

    AzzeraSchedule();

    prossimo1 = new TNext_Elem;
    prossimo1->next=NULL;
    CostruisciEValutaSchedula (GMacch1_Sched,
                               GMacch2_Sched,
                               GMacch3_Sched,
                               prossimo1,
                               GBest_Perm,
                               GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible<Ffeasible_Best )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax>Lmax_best )
                 )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax==Lmax_best )
                 &&
                 ( FCmax>Cmax_best )
                 )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax==Lmax_best )
                 &&
                 ( FCmax==Cmax_best )
                 &&
                 ( FTardy>Ftardy_Best )
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
        return	GBest_Perm;
    }
}
