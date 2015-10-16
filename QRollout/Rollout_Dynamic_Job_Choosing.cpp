/***************************************************************************
 *   Copyright (C) 2005 by Antonio Maccioni                                *
 *   antonio.maccioni@gmail.com                                            *
 *                                                                         *
 *   This program is delete software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the delete Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   delete Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "qRolloutThread.h"
#include "Euristiche.h"
#include "Globals.h"
#include "Schedula.h"
#include <iostream>

/*
Esegue il rollout con la scelta dinamica dei job ad ogni stadio di rollout
Restituisce una permutazione dei job
*/
TJob *QRolloutThread::Rollout_Dynamic_Job_Choosing ( int pForce )
{
    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo
    int i,iter,kk  = 0;
    int pp = 0;
    int cont_livelli = GNum_Job - 1;
    TJob **permutazioni = NULL;
    permutazioni = new TJob*[Fnum_Heur];
    TInsieme_Dinamico *insieme = NULL;
    TInsieme_Dinamico *insieme_next = NULL;
    TInsieme_Dinamico *insieme_temp = NULL;
    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale = NULL;
    TJob *array_job_attuale_temp = NULL;
    TSchedula *M1_sch_attuale = NULL;
    TNext_Elem *lista_vincitori = NULL; //contiene i vincitori dei singoli turni dello stadio di rollout
    M1_sch_attuale = GMacch1_Sched;
    Fpermutazione_Buffer = new TJob[GNum_Job];

    TSchedula *M2_sch_attuale; //puntatore all'ultimo elemento della schedula in costruzione su M2
    if(GNum_Macchine >= 2)
        M2_sch_attuale = GMacch2_Sched;
    TSchedula *M3_sch_attuale;
    if(GNum_Macchine == 3)
        M3_sch_attuale = GMacch3_Sched;


    array_job_attuale = new TJob[GNum_Job];
    insieme      = NULL;
    insieme_temp = NULL;
    insieme_next = NULL;

    for(i = 0; i < GNum_Job; i++)
        array_job_attuale[i] = GArray_Job[i];

    //inizializzo insieme_temp perchè all'inizio del rollout deve contenere tutti i job
    for(i = 0; i < GNum_Job; i++)
        insieme = AggiungiDinamicamente(insieme, array_job_attuale[i]);


    TJob * perm_di_passaggio = NULL;

    //WHILE-STADI-ROLLOUT
    while(cont_livelli > 0)
    {
        insieme_temp = insieme;
        while(insieme_temp != NULL)
        {
            /* considero tutti i job di insieme_temp schedulati per primi
             * devo usare
             * costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * per valutare l'inserimento di un job in una macchina     */
            if (insieme_temp->ID == -1)//se e' non selezionabile
            {
                //il caso di avere un job non selezionabile in insieme_temp non esiste con questa implementazione basic del job choosing
            }
            else //se e' selezionabile
            {
                array_job_attuale_temp = new TJob[cont_livelli];
                //metto in array_job_attuale_temp i job ancora da schedulare che mi servono per essere permutati
                iter=0;
                for(kk = 0; kk < cont_livelli; kk++)
                {
                    while(array_job_attuale[iter].ID==-1 || array_job_attuale[iter].ID == insieme_temp->ID)
                        iter++;

                    array_job_attuale_temp[kk] = array_job_attuale[iter];
                    iter++;
                }
                TNext_Elem *prossimo = NULL;
                for (i = 0; i < Fnum_Heur; i++)
                {
                    //inizializza la perm_di_passaggio con la best_perm, cioè con la schedulazione gi  fatta
                    Schedula::AzzeraSchedule();
                    perm_di_passaggio = new TJob[GNum_Job];
                    Heuristics::InizializzaPermutazioneMigliore(perm_di_passaggio);
                    for(pp=0;pp<GNum_Job-cont_livelli-1;pp++)
                        perm_di_passaggio[pp]=GBest_Perm[pp];

                    pp++;
                    //mette nella perm di passaggio il job fisso
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)].ID=insieme_temp->ID;
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)].proc_time=insieme_temp->proc_time;
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)].duedate=insieme_temp->duedate;
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)].deadline=insieme_temp->deadline;
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)].priority=insieme_temp->priority;
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)].rel_time=insieme_temp->rel_time;

                    TNext_Elem *prossimo2 = new TNext_Elem;					prossimo2->next=NULL;
                    CostruisciEValutaSchedula(GMacch1_Sched,
                                                 GMacch2_Sched,
                                                 GMacch3_Sched,
                                                 prossimo2,
                                                 perm_di_passaggio,
                                                 GNum_Job);
                    //prossimo2 contiene le informazioni su questa schedulazione con perm_di_passaggio
                    delete(prossimo2);
                    Schedula::AzzeraSchedule();
                    permutazioni[i] = Ffunzioni[i].funz(array_job_attuale_temp,cont_livelli);

                    //si scrive nelle righe di permutazioni la permutazione trovata dalla i-esima euristica
                    //se prossimo è NULL si inizializza come testa della lista altrimenti si aggiunge in cosa un nuovo elemento temp
                    //si aggiunge un next_elem col primo job della permutazione trovata dall'euristica e relativi dati della schedula parziale
                    if(prossimo == NULL)
                    {
                        prossimo =new TNext_Elem;
                        prossimo->ID_job = insieme_temp->ID;
                        prossimo->ID_heur= Ffunzioni[i].ID_heur;
                        prossimo->next = NULL;
                        insieme_next = AggiungiDinamicamente (insieme_next, permutazioni[i][0]);
                    }
                    else
                    {
                        TNext_Elem *temp;
                        TNext_Elem *temp_prox;
                        temp = prossimo;
                        while(temp->next != NULL)
                            temp = temp->next;

                        temp_prox       = new TNext_Elem;
                        temp->next      = temp_prox;
                        temp            = temp->next;
                        temp->ID_job    = insieme_temp->ID;
                        temp->ID_heur   = Ffunzioni[i].ID_heur;
                        temp->next      = NULL;
                        insieme_next = AggiungiDinamicamente (insieme_next, permutazioni[i][0]);

                    }
                    //completo la fine perm_di_passaggio con la permutazione appena trovata dall'euristica
                    for( int jj = 0; pp < GNum_Job; pp++, jj++)
                        perm_di_passaggio[pp] = permutazioni[i][jj];

                    Schedula::AzzeraSchedule();
                    //costruisce e valuta la schedula ottenuta dalla perm_di_passaggio salvando i risultati ottenuti in prossimo
                    CostruisciEValutaSchedula(GMacch1_Sched,
                                                 GMacch2_Sched,
                                                 GMacch3_Sched,
                                                 prossimo,
                                                 perm_di_passaggio,
                                                 GNum_Job);
                   SalvaSeMeglio(perm_di_passaggio);
                    delete[] perm_di_passaggio;
                }//FINE for i<num_heur
                // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                // devo ridurre il numero di job che rimangono da schedulare
                // devo trovare il job con la Lateness + alta
                // in condizioni di parita' quello con la Cmax +bassa
                // infine con il numero + basso di Tardy job

                delete(array_job_attuale_temp);
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

                //WHILE-VINCITORI-EURISTICHE
                while(temp->next != NULL)
                {
                    if (Ffeasible <= temp->next->feasible)
                    {
                        if
                                (
                                 (Ffeasible ==0)
                                 &&
                                 (temp->next->feasible==1)
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
                                 (L_max > temp->next->Lmax)
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max > temp->next->Cmax)
                                     )
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max == temp->next->Cmax)
                                     &&
                                     (tardy > temp->next->Tardy)
                                     )
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max == temp->next->Cmax)
                                     &&
                                     (tardy == temp->next->Tardy)
                                     &&
                                     (LMAX > temp->next->Lmax_pers)
                                     )
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max == temp->next->Cmax)
                                     &&
                                     (tardy == temp->next->Tardy)
                                     &&
                                     (LMAX > temp->next->Lmax_pers)
                                     &&
                                     (
                                         (temp->next->duedate !=0)
                                         &&
                                         (duedate > temp->next->duedate)
                                         )
                                     )
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max == temp->next->Cmax)
                                     &&
                                     (tardy == temp->next->Tardy)
                                     &&
                                     (LMAX > temp->next->Lmax_pers)
                                     &&
                                     (duedate == temp->next->duedate)
                                     &&
                                     (
                                         (temp->next->deadline !=0)
                                         &&
                                         (deadline > temp->next->deadline)
                                         )
                                     )
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max == temp->next->Cmax)
                                     &&
                                     (tardy == temp->next->Tardy)
                                     &&
                                     (LMAX > temp->next->Lmax_pers)
                                     &&
                                     (duedate == temp->next->duedate)
                                     &&
                                     (deadline == temp->next->deadline)
                                     &&
                                     (proc_time > temp->next->proc_time)
                                     )
                                 ||
                                 (
                                     (L_max == temp->next->Lmax)
                                     &&
                                     (C_max == temp->next->Cmax)
                                     &&
                                     (tardy == temp->next->Tardy)
                                     &&
                                     (LMAX > temp->next->Lmax_pers)
                                     &&
                                     (duedate == temp->next->duedate)
                                     &&
                                     (deadline == temp->next->deadline)
                                     &&
                                     (proc_time == temp->next->proc_time)
                                     &&
                                     (rel_time > temp->next->proc_time)
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
                        else{

                        }
                    }
                    else {

                    }
                    temp = temp->next;
                }//FINE WHILE-VINCITORI-EURISTICHE:scorre temp (prossimo) salvando di volta in volta valori migliori tra tutte le schedule contenute in prossimo, che erano quelle trovate da tutte le euristiche tenendo questo job come fisso

                //ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                //e su quale macchina e' stato schedulato
                // salvataggio nella lista delle info sul job prescelto in questo step
                if (lista_vincitori == NULL){
                    lista_vincitori=new TNext_Elem;
                    lista_vincitori->ID_job = ID_job;
                    lista_vincitori->macchina = macchina;
                    lista_vincitori->tipo =  tipo;
                    lista_vincitori->fine = fine;
                    lista_vincitori->inizio = inizio;
                    lista_vincitori->index_camp = index_camp;
                    lista_vincitori->Lmax_pers = LMAX;
                    lista_vincitori->Tardy_pers =TARDY;
                    lista_vincitori->Lmax = L_max;
                    lista_vincitori->Cmax = C_max;
                    lista_vincitori->Tardy = tardy;
                    lista_vincitori->deadline = deadline;
                    lista_vincitori->duedate = duedate;
                    lista_vincitori->proc_time = proc_time;
                    lista_vincitori->rel_time = rel_time;
                    lista_vincitori->priority = priority;
                    lista_vincitori->ID_heur = ID_heur;
                    lista_vincitori->feasible = Ffeasible;
                    lista_vincitori->next=NULL;
                }
                else{

                    TNext_Elem *vincitori_temp;
                    TNext_Elem *vincitori_temp_prox;
                    vincitori_temp = lista_vincitori;
                    while(vincitori_temp->next != NULL)
                        vincitori_temp = vincitori_temp->next;

                    vincitori_temp_prox=new TNext_Elem;
                    vincitori_temp->next = vincitori_temp_prox;
                    vincitori_temp = vincitori_temp->next;
                    vincitori_temp->ID_job = ID_job;
                    vincitori_temp->macchina = macchina;
                    vincitori_temp->tipo =  tipo;
                    vincitori_temp->fine = fine;
                    vincitori_temp->inizio = inizio;
                    vincitori_temp->index_camp = index_camp;
                    vincitori_temp->Lmax_pers = LMAX;
                    vincitori_temp->Tardy_pers =TARDY;
                    vincitori_temp->Lmax = L_max;
                    vincitori_temp->Cmax = C_max;
                    vincitori_temp->Tardy = tardy;
                    vincitori_temp->deadline = deadline;
                    vincitori_temp->duedate = duedate;
                    vincitori_temp->proc_time = proc_time;
                    vincitori_temp->rel_time = rel_time;
                    vincitori_temp->priority = priority;
                    vincitori_temp->ID_heur = ID_heur;
                    vincitori_temp->feasible = Ffeasible;
                    vincitori_temp->next = NULL;
                }
                // fine salvataggio

                //libero prossimo e permutazioni per il prossimo ciclo
                TNext_Elem *temp_pr;

                while (prossimo->next != NULL){
                    temp_pr = prossimo->next;
                    delete(prossimo);
                    prossimo = temp_pr;
                }
                delete(prossimo);
                prossimo = NULL;

                for( i = 0; i < Fnum_Heur; i++)
                    delete(permutazioni[i]);



            }
            insieme_temp=insieme_temp->next;

        } //FINE while(insieme_temp->testa != NULL) (quello che fa partire il rollout da tutti i job)

        int ID_job = lista_vincitori->ID_job;
        Ffeasible = lista_vincitori->feasible;
        TNext_Elem *migliore = lista_vincitori;
        TNext_Elem *vincitori_temp = lista_vincitori;
        while(vincitori_temp->next != NULL){
            if (Ffeasible <= vincitori_temp->next->feasible)
            {
                if
                        (
                         (Ffeasible ==0)
                         &&
                         (vincitori_temp->next->feasible==1)
                         )
                {
                    migliore=vincitori_temp;
                    ID_job = vincitori_temp->next->ID_job;
                    Ffeasible = vincitori_temp->next->feasible;
                }
                else if
                        (
                         (migliore->Lmax > vincitori_temp->next->Lmax)
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax > vincitori_temp->next->Cmax)
                             )
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax == vincitori_temp->next->Cmax)
                             &&
                             (migliore->Tardy > vincitori_temp->next->Tardy)
                             )
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax == vincitori_temp->next->Cmax)
                             &&
                             (migliore->Tardy == vincitori_temp->next->Tardy)
                             &&
                             (migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
                             )
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax == vincitori_temp->next->Cmax)
                             &&
                             (migliore->Tardy == vincitori_temp->next->Tardy)
                             &&
                             (migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
                             &&
                             (
                                 (vincitori_temp->next->duedate !=0)
                                 &&
                                 (migliore->duedate > vincitori_temp->next->duedate)
                                 )
                             )
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax == vincitori_temp->next->Cmax)
                             &&
                             (migliore->Tardy == vincitori_temp->next->Tardy)
                             &&
                             (migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
                             &&
                             (migliore->duedate == vincitori_temp->next->duedate)
                             &&
                             (
                                 (vincitori_temp->next->deadline !=0)
                                 &&
                                 (migliore->deadline > vincitori_temp->next->deadline)
                                 )
                             )
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax == vincitori_temp->next->Cmax)
                             &&
                             (migliore->Tardy == vincitori_temp->next->Tardy)
                             &&
                             (migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
                             &&
                             (migliore->duedate == vincitori_temp->next->duedate)
                             &&
                             (migliore->deadline == vincitori_temp->next->deadline)
                             &&
                             (migliore->proc_time > vincitori_temp->next->proc_time)
                             )
                         ||
                         (
                             (migliore->Lmax == vincitori_temp->next->Lmax)
                             &&
                             (migliore->Cmax == vincitori_temp->next->Cmax)
                             &&
                             (migliore->Tardy == vincitori_temp->next->Tardy)
                             &&
                             (migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
                             &&
                             (migliore->duedate == vincitori_temp->next->duedate)
                             &&
                             (migliore->deadline == vincitori_temp->next->deadline)
                             &&
                             (migliore->proc_time == vincitori_temp->next->proc_time)
                             &&
                             (migliore->rel_time > vincitori_temp->next->proc_time)
                             )
                         )
                {
                    migliore    = vincitori_temp;
                    ID_job      = vincitori_temp->next->ID_job;
                    Ffeasible   = vincitori_temp->next->feasible;
                }
                else{

                }
            }
            else {

            }
            vincitori_temp = vincitori_temp->next;
        }

        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        //poi inserisco in best_perm il job migliore fra tutti i vincitori dello stadio di rollout
        if
                (
                 (migliore->feasible>Ffeasible_Best)
                 ||
                 (
                     (migliore->feasible==Ffeasible_Best)
                     &&
                     (migliore->Lmax<Lmax_best)
                     )
                 ||
                 (
                     (migliore->feasible==Ffeasible_Best)
                     &&
                     (migliore->Lmax==Lmax_best)
                     &&
                     (migliore->Cmax<Cmax_best)
                     )
                 ||
                 (
                     (migliore->feasible==Ffeasible_Best)
                     &&
                     (migliore->Lmax==Lmax_best)
                     &&
                     (migliore->Cmax==Cmax_best)
                     &&
                     (migliore->Tardy<Ftardy_Best)
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID=array_job_attuale[ID_job].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo=array_job_attuale[ID_job].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time=array_job_attuale[ID_job].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate=array_job_attuale[ID_job].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline=array_job_attuale[ID_job].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority=array_job_attuale[ID_job].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time=array_job_attuale[ID_job].rel_time;
            insieme=Copia_Dinamicamente(insieme, insieme_next, array_job_attuale[ID_job]);
            //il job schedulato viene segnato come non selezionabile (non rischedulabile)
            array_job_attuale[ID_job].ID=-1;
        }
        else
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo=Fpermutazione_Buffer[GNum_Job-cont_livelli-1].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time=Fpermutazione_Buffer[GNum_Job-cont_livelli-1].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate=Fpermutazione_Buffer[GNum_Job-cont_livelli-1].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline=Fpermutazione_Buffer[GNum_Job-cont_livelli-1].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority=Fpermutazione_Buffer[GNum_Job-cont_livelli-1].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time=Fpermutazione_Buffer[GNum_Job-cont_livelli-1].rel_time;
            insieme=Copia_Dinamicamente(insieme, insieme_next, Fpermutazione_Buffer[GNum_Job-cont_livelli-1]);
            //il job schedulato viene segnato come non selezionabile (non rischedulabile)
            array_job_attuale[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;;

        }
        //se insieme resta vuoto (coincide l'unico job di insieme_next col successivo della permutazione_buffer)
        if (insieme == NULL){
            insieme=AggiungiDinamicamente(insieme, array_job_attuale[ID_job]);
        }
        insieme_next=NULL;
        while (lista_vincitori->next != NULL){
            vincitori_temp = lista_vincitori;
            lista_vincitori=lista_vincitori->next;
            delete(vincitori_temp);
        }
        delete(lista_vincitori);
        lista_vincitori=NULL;
        cont_livelli--;
    } //FINE WHILE-STADI-ROLLOUT

    //devo schedulare l'ultimo job
    i=0;
    while(array_job_attuale[i].ID==-1) i++;

    GBest_Perm[GNum_Job-1].ID=array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo=array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time=array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate=array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline=array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority=array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time=array_job_attuale[i].rel_time;
    array_job_attuale[i].ID=-1;
    //libero memoria che non mi serve più
    delete(array_job_attuale);
    Schedula::AzzeraSchedule();
    TNext_Elem *prossimo1;
    prossimo1= new TNext_Elem;
    prossimo1->next=NULL;
    //costruisci e valuta la permutazione finale, confronto con quella buffer e restituisco la permutazione trovata dall'intero rollout
    CostruisciEValutaSchedula(GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo1,GBest_Perm,GNum_Job);
    delete(prossimo1);
    if
            (
             (Ffeasible<Ffeasible_Best)
             ||
             (
                 (Ffeasible==Ffeasible_Best)
                 &&
                 (FLmax>Lmax_best)
                 )
             ||
             (
                 (Ffeasible==Ffeasible_Best)
                 &&
                 (FLmax==Lmax_best)
                 &&
                 (FCmax>Cmax_best)
                 )
             ||
             (
                 (Ffeasible==Ffeasible_Best)
                 &&
                 (FLmax==Lmax_best)
                 &&
                 (FCmax==Cmax_best)
                 &&
                 (FTardy>Ftardy_Best)
                 )
             )
    {
        delete(permutazioni);
        return Fpermutazione_Buffer;
    }
    else
    {
        delete(permutazioni);
        delete(Fpermutazione_Buffer);
        return	GBest_Perm;
    }
}


