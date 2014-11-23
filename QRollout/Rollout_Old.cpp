#include "qRolloutThread.h"
#include "Globals.h"
#include "Euristiche.h"
#include "Schedula.h"
#include <time.h>

//*************************************************************************************************
//			ROLLOUT OLD Main function
//***********************************************************************************************

TJob *QRolloutThread::Rollout_Old ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter  = 0;
    int j = 0;
    TJob *permutazioni[Fnum_Heur];
    int primo_passo_M1 = 0;
    int primo_passo_M2 = 0;
    int primo_passo_M3 = 0;//non ho fatto ancora nessuna assegnazione alla macchina 3
    TJob *array_job_attuale;
    TSchedula *M1_sch_attuale =NULL;
    TSchedula *M1_sch_buffer = NULL;
    TSchedula *M2_sch_buffer = NULL;
    TSchedula *M3_sch_buffer = NULL;
    M1_sch_attuale = GMacch1_Sched;
    TSchedula *temp1 = NULL;
    TSchedula *temp2 = NULL;
    int fine = 0;

    TSchedula *M2_sch_attuale = NULL;//puntatore all'ultimo elemento della schedula in costruzione su M2

    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale = NULL;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];
    for ( i = 0; i < GNum_Job; i++ )
        array_job_attuale[i] = GArray_Job[i];

    iter = 0;

    while ( iter < GNum_Job )
    {
        TNext_Elem *prossimo = NULL;
        TNext_Elem *prossimo1;

        for ( i = 0; i < Fnum_Heur; i++ ) //num_heur dovra' essere cambiato con il numero di heuristiche effettivamente usato
        {

            permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale,GNum_Job-iter );

            prossimo1 = new TNext_Elem;
            if ( prossimo == NULL )
            {
                prossimo            = new TNext_Elem;
                prossimo->ID_job    = permutazioni[i][0].ID;
                prossimo->ID_heur   = Ffunzioni[i].ID_heur;
                prossimo->next      = NULL;
            }
            else
            {
                TNext_Elem *temp_prox;
                TNext_Elem *temp = prossimo;
                while ( temp->next != NULL )
                    temp = temp->next;

                temp_prox       = new TNext_Elem;
                temp->next      = temp_prox;
                temp            = temp->next;
                temp->ID_job    = permutazioni[i][0].ID;
                temp->ID_heur   = Ffunzioni[i].ID_heur;
                temp->next      = NULL;

            }
            CostruisciEValutaSchedula (GMacch1_Sched,
                                          GMacch2_Sched,
                                          GMacch3_Sched,
                                          prossimo,
                                          permutazioni[i],
                                          GNum_Job - iter );

            //devo riportare la macchina nelle condizioni orginarie
            if ( force == 1 )
            {
                M1_sch_buffer = new TSchedula;
                M2_sch_buffer = new TSchedula;
                M3_sch_buffer = new TSchedula;

                CopiaSchedule ( GMacch1_Sched, M1_sch_buffer );

                if ( GNum_Macchine >= 2 )
                    CopiaSchedule ( GMacch2_Sched, M2_sch_buffer );

                if ( GNum_Macchine == 3 )
                    CopiaSchedule ( GMacch3_Sched, M3_sch_buffer );

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
                if ( GNum_Macchine >= 2 )
                    EliminaSchedula ( M2_sch_buffer );

                if ( GNum_Macchine == 3 )
                    EliminaSchedula ( M3_sch_buffer );

                TNext_Elem *tmp_prox;
                tmp_prox = prossimo;
                while ( tmp_prox->next != NULL )
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
                    tmp_prox->Lmax    = prossimo1->Lmax;
                    tmp_prox->Cmax    = prossimo1->Cmax;
                    tmp_prox->Tardy   = prossimo1->Tardy;
                    tmp_prox->feasible= prossimo1->feasible;
                }
                delete prossimo1 ;
            }
            if
                    (
                     ( M1_sch_attuale == GMacch1_Sched )
                     &&
                     ( primo_passo_M1 == 0 ) //nn ho ancora schedulato niente su M1
                     )
            {
                fine = 0;
                while ( !fine )
                {
                    temp1 = M1_sch_attuale;
                    if ( temp1->next == NULL )
                    {
                        delete temp1;
                        break;
                    }
                    while ( temp1->next != NULL )
                    {
                        temp2 = temp1;
                        temp1 = temp1->next;
                    }
                    delete temp1;
                    temp2->next = NULL;
                }
                GMacch1_Sched = new TSchedula;
                {
                    GMacch1_Sched->ID_job       = -3;
                    GMacch1_Sched->tipo         = 0;
                    GMacch1_Sched->inizio       = 0;
                    GMacch1_Sched->fine         = 0;
                    GMacch1_Sched->index_camp   = 0;
                    GMacch1_Sched->Lmax         = 0;
                    GMacch1_Sched->Tardy        = 0;
                    GMacch1_Sched->next         = NULL;
                    M1_sch_attuale = GMacch1_Sched;
                }
            }
            else
            {
                fine = 0;
                while ( !fine )
                {
                    temp1 = M1_sch_attuale;
                    if ( temp1->next==NULL )
                    {
                        fine = 1;
                        break;
                    }
                    while ( temp1->next!=NULL )
                    {
                        temp2 = temp1;
                        temp1 = temp1->next;
                    }
                    delete temp1;
                    temp2->next = NULL;
                }
            }
            if ( GNum_Macchine >= 2 )
            {
                if
                        (
                         ( M2_sch_attuale == GMacch2_Sched )
                         &&
                         ( primo_passo_M2 == 0 ) //nn ho ancora schedulato niente su M2
                         )
                {
                    fine = 0;
                    while ( !fine )
                    {
                        temp1 = M2_sch_attuale;
                        if ( temp1->next == NULL )
                        {
                            delete temp1;
                            break;
                        }
                        while ( temp1->next != NULL )
                        {
                            temp2 = temp1;
                            temp1 = temp1->next;
                        }
                        delete temp1;
                        temp2->next = NULL;
                    }
                    GMacch2_Sched = new TSchedula;
                    {
                        GMacch2_Sched->ID_job       = -3;
                        GMacch2_Sched->tipo         = 0;
                        GMacch2_Sched->inizio       = 0;
                        GMacch2_Sched->fine         = 0;
                        GMacch2_Sched->Lmax         = 0;
                        GMacch2_Sched->Tardy        = 0;
                        GMacch2_Sched->index_camp   = 0;
                        GMacch2_Sched->next         = NULL;
                        M2_sch_attuale = GMacch2_Sched;
                    }
                }
                else
                {
                    fine = 0;
                    while ( !fine )
                    {
                        temp1 = M2_sch_attuale;
                        if ( temp1->next == NULL )
                        {
                            fine = 1;
                            break;
                        }
                        while ( temp1->next != NULL )
                        {
                            temp2 = temp1;
                            temp1 = temp1->next;
                        }
                        delete temp1;
                        temp2->next = NULL;
                    }
                }
            }
            if ( GNum_Macchine == 3 )
            {
                if
                        (
                         ( M3_sch_attuale == GMacch3_Sched )
                         &&
                         ( primo_passo_M3 == 0 ) //nn ho ancora schedulato niente su M3
                         )
                {
                    fine = 0;
                    while ( !fine )
                    {
                        temp1 = M3_sch_attuale;
                        if ( temp1->next==NULL )
                        {
                            delete temp1;
                            break;
                        }
                        while ( temp1->next!=NULL )
                        {
                            temp2=temp1;
                            temp1=temp1->next;
                        }
                        delete temp1;
                        temp2->next = NULL;
                    }
                    GMacch3_Sched = new TSchedula;
                    {
                        GMacch3_Sched->ID_job       = -3;
                        GMacch3_Sched->tipo         = 0;
                        GMacch3_Sched->inizio       = 0;
                        GMacch3_Sched->fine         = 0;
                        GMacch3_Sched->Lmax         = 0;
                        GMacch3_Sched->Tardy        = 0;
                        GMacch3_Sched->index_camp   = 0;
                        GMacch3_Sched->next         = NULL;
                        M3_sch_attuale = GMacch3_Sched;
                    }
                }
                else
                {
                    fine = 0;
                    while ( !fine )
                    {
                        temp1 = M3_sch_attuale;
                        if ( temp1->next == NULL )
                        {
                            fine = 1;
                            break;
                        }
                        while ( temp1->next != NULL )
                        {
                            temp2 = temp1;
                            temp1 = temp1->next;
                        }
                        delete temp1;
                        temp2->next = NULL;
                    }
                }
            }
        }
        // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
        // devo ridurre il numero di job che rimangono da schedulare
        // devo trovare il job con la Lateness + alta
        // in condizioni di parita' quello con la Cmax +bassa
        // infine con il numero + basso di Tardy job
        int ID_job      = prossimo->ID_job;
        int macchina    = prossimo->macchina;
        int fine        = prossimo->fine;
        int inizio      = prossimo->inizio;
        int tipo        = prossimo->tipo;
        int index_camp  = prossimo->index_camp;
        int LMAX        = prossimo->Lmax_pers;
        int TARDY       = prossimo->Tardy_pers;
        int L_max       = prossimo->Lmax;
        int C_max       = prossimo->Cmax;
        int tardy       = prossimo->Tardy;
        int ID_heur     = prossimo->ID_heur;
        int rel_time    = prossimo->rel_time;
        int proc_time   = prossimo->proc_time;
        int duedate     = prossimo->duedate;
        int deadline    = prossimo->deadline;
        int priority    = prossimo->priority;
        Ffeasible       = prossimo->feasible;
        TNext_Elem *temp = prossimo ;
        while ( temp->next != NULL )
        {
            if ( Ffeasible <= temp->next->feasible )
            {
                if
                        (
                         ( Ffeasible == 0 )
                         &&
                         ( temp->next->feasible == 1 )
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
            temp = temp->next;
        }
        //ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
        // e su quale macchina e' stato schedulato
        emit ScriviASchermo( QString( "\n (%1) %2 %3 %4 %5").
                            arg(iter).
                            arg( ID_heur).
                            arg( L_max).
                            arg( C_max).
                            arg(tardy) );
        switch ( macchina )
        {
        case 1:
        {
            temp1 = M1_sch_attuale;
            if ( temp1->ID_job != -3 )
            {
                while ( temp1->next != NULL )
                    temp1 = temp1->next;

                temp1->next = new TSchedula;
                M1_sch_attuale = temp1->next;
                M1_sch_attuale->next = NULL;
            }//sposto il valore di M3_sch_attuale
            M1_sch_attuale->ID_job = ID_job;
            M1_sch_attuale->tipo = tipo;
            M1_sch_attuale->inizio = inizio;
            M1_sch_attuale->fine = fine;
            M1_sch_attuale->Lmax = LMAX;
            M1_sch_attuale->Tardy = TARDY;
            M1_sch_attuale->index_camp = index_camp;
            primo_passo_M1 = 1;//ho fatto almeno una assegnazione
            break;
        }
        case 2:
        {
            temp1 = M2_sch_attuale;
            if ( temp1->ID_job != -3 )
            {
                while ( temp1->next!=NULL )
                    temp1 = temp1->next;

                temp1->next = new TSchedula;
                M2_sch_attuale = temp1->next;
                M2_sch_attuale->next = NULL;
            }//sposto il valore di M2_sch_attuale
            M2_sch_attuale->ID_job = ID_job;
            M2_sch_attuale->tipo = tipo;
            M2_sch_attuale->inizio = inizio;
            M2_sch_attuale->fine = fine;
            M2_sch_attuale->Lmax = LMAX;
            M2_sch_attuale->Tardy = TARDY;
            M2_sch_attuale->index_camp = index_camp;
            primo_passo_M2 = 1;//ho fatto almeno una assegnazione
            break;
        }
        case 3:
        {
            temp1 = M3_sch_attuale;
            if ( temp1->ID_job != -3 )
            {
                while ( temp1->next!=NULL )
                    temp1 = temp1->next;

                temp1->next = new TSchedula;
                M3_sch_attuale = temp1->next;
                M3_sch_attuale->next = NULL;
            }//sposto il valore di M3_sch_attuale
            M3_sch_attuale->ID_job = ID_job;
            M3_sch_attuale->tipo = tipo;
            M3_sch_attuale->inizio = inizio;
            M3_sch_attuale->fine = fine;
            M3_sch_attuale->Lmax = LMAX;
            M3_sch_attuale->Tardy = TARDY;
            M3_sch_attuale->index_camp = index_camp;
            primo_passo_M3 = 1;//ho fatto almeno una assegnazione
            break;
        }
        }
        for ( i = 0; i < ( GNum_Job-iter ); i++ )
        {
            GBest_Perm[iter+i].ID       = permutazioni[ID_heur][i].ID;
            GBest_Perm[iter+i].tipo     = permutazioni[ID_heur][i].tipo;
            GBest_Perm[iter+i].proc_time= permutazioni[ID_heur][i].proc_time;
            GBest_Perm[iter+i].duedate  = permutazioni[ID_heur][i].duedate;
            GBest_Perm[iter+i].deadline = permutazioni[ID_heur][i].deadline;
            GBest_Perm[iter+i].priority = permutazioni[ID_heur][i].priority;
            GBest_Perm[iter+i].rel_time = permutazioni[ID_heur][i].rel_time;
        }
        //       faccio l'update dell'utilizzo delle singole funzioni
        Ffunzioni[ID_heur].perc_utilizzo++;
        //devo ora eliminare il job selezionato dall'insieme dei job schedulabili
        i = 0;
        j = 0;
        while ( j < GNum_Job-iter )
        {
            if ( array_job_attuale[j].ID!=ID_job )
            {
                array_job_attuale[i] = array_job_attuale[j];
                i++;
            }
            j++;
        }
        //marco i job nn utilizzabili
        for ( j=i; j < GNum_Job; j++ )
            array_job_attuale[j].ID = -1;



        TNext_Elem *temp_pr;
        for ( i=0; i < Fnum_Heur; i++ )
        {
            temp_pr = prossimo->next;
            delete prossimo;//questa si deve fare meglio, molto meglio. Magari mi ricordassi perchè. :)
            prossimo = temp_pr;
        }
        iter++;
        for ( i=0;i<Fnum_Heur;i++ )
            delete permutazioni[i];

    }


    return   GBest_Perm;
}

