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

//Esegue il rollout col pruning delle euristiche.
//Restituisce la permutazione dei job trovata
TJob *QRolloutThread::Rollout_Heuristic_Pruning(int force)
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

    permutazioni= new TJob*[Fnum_Heur];


    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_attuale;
    M1_sch_attuale = GMacch1_Sched;
    Fpermutazione_Buffer = new TJob[GNum_Job];
    TSchedula *M2_sch_attuale; //puntatore all'ultimo elemento della schedula in costruzione su M2

    if(GNum_Macchine >= 2)
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if(GNum_Macchine == 3)
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];


    for(i = 0;i<GNum_Job;i++)
        array_job_attuale[i] = GArray_Job[i];

    job_fisso = new TNext_Elem;
    TJob * perm_di_passaggio;
    //WHILE-STADI-ROLLOUT
    while(cont_livelli>0)
    {
        TNext_Elem lista_prossimi_vincitori[500];
        index=0;
        index1=0;
        /*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/
        for(iter_for=0;iter_for<GNum_Job;iter_for++)
        {
            if (array_job_attuale[iter_for].ID==-1)//se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                array_job_attuale[iter_for].ID = -1;
                array_job_attuale_temp = new TJob[cont_livelli];
                iter = 0;
                //mette in array_job_attuale_temp i primi cont_livelli job selezionabili, cioè i job che devono essere ancora schedulati
                for(kk = 0;kk < cont_livelli; kk++)
                {
                    while(array_job_attuale[iter].ID==-1)
                        iter++;

                    array_job_attuale_temp[kk]=array_job_attuale[iter];
                    iter++;
                }
                iter=0;
                TNext_Elem *prossimo = NULL;
                for (i = 0;i<Fnum_Heur;i++)//num_heur dovr�essere cambiato con il numero di heuristiche effettivamente usato
                {
                    if (Ffunzioni[i].perc_utilizzo !=-1)
                    {
                        Schedula::AzzeraSchedule();
                        perm_di_passaggio=new TJob[GNum_Job];
                        Heuristics::InizializzaPermutazioneMigliore(perm_di_passaggio);
                        for(pp = 0; pp < GNum_Job - cont_livelli - 1;pp++)
                            perm_di_passaggio[pp] = GBest_Perm[pp];

                        pp++;
                        perm_di_passaggio[(GNum_Job-cont_livelli-1)]=GArray_Job[iter_for];
                        TNext_Elem *prossimo2;
                        prossimo2= new TNext_Elem;
                        prossimo2->next=NULL;
                        CostruisciEValutaSchedula(GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo2,perm_di_passaggio,GNum_Job);
                        //prossimo2 contiene le informazioni su questa schedulazione con perm_di_passaggio
                        delete(prossimo2);
                        Schedula::AzzeraSchedule();
                        permutazioni[i] = Ffunzioni[i].funz(array_job_attuale_temp,cont_livelli);
                        //si scrive nelle righe di permutazioni la permutazione trovata dalla i-esima euristica
                        //se prossimo e' NULL si inizializza come testa della lista altrimenti si aggiunge in cosa un nuovo elemento temp
                        //si aggiunge un next_elem col primo job della permutazione trovata dall'euristica e relativi dati della schedula parziale
                        if(prossimo == NULL)
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
                            while(temp->next!=NULL)
                                temp = temp->next;
                            temp_prox=new TNext_Elem;
                            temp->next = temp_prox;
                            temp = temp->next;
                            temp->ID_job = permutazioni[i][0].ID;
                            temp->ID_heur= Ffunzioni[i].ID_heur;
                            temp->next = NULL;

                        }
                        //completo la fine perm_di_passaggio con la permutazione appena trovata dall'euristica
                        for(jj = 0;pp < GNum_Job;pp++,jj++)
                            perm_di_passaggio[pp] = permutazioni[i][jj];
                        Schedula::AzzeraSchedule();
                        //costruisce e valuta la schedula ottenuta dalla perm_di_passaggio salvando i risultati ottenuti in prossimo
                        CostruisciEValutaSchedula(GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,perm_di_passaggio,GNum_Job);

                        SalvaSeMeglio(perm_di_passaggio);
                        delete[] perm_di_passaggio;
                        //devo riportare la macchina nelle condizioni orginarie
                    }
                    else
                    {
                        //se la euristica e' stata esclusa in precedenza...
                    }
                }//FINE for i<num_heur
                //se la schedula non �feasible deve essere penalizzata rispetto alle altre.
                //devo ridurre il numero di job che rimangono da schedulare
                //devo trovare il job con la Lateness + alta
                //in condizioni di parit�quello con la Cmax +bassa
                // infine con il numero + basso di Tardy job

                //imposto i valori di scheduling (var globali) con i valori di prossimo (della testa della lista), cioè gli inizializzo prima dello scorrimento della lista
                array_job_attuale[iter_for].ID =iter_for;
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
                int c=1; //variabile che conta per sapere i vincitori locali
                int vinc_loc=ID_heur; //variabile che tiene traccia del vincitore locale;
                if (c == FNum_Heur_Used)
                    Ffunzioni[vinc_loc].perc_utilizzo++;

                //WHILE-VINCITORI-EURISTICHE
                while(temp->next!=NULL)
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
                            c++;
                            vinc_loc=ID_heur;
                            if (c==FNum_Heur_Used){
                                Ffunzioni[vinc_loc].perc_utilizzo++;
                                //printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
                                c=0;
                            }
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
                            c++;
                            vinc_loc=ID_heur;
                            if (c==FNum_Heur_Used){
                                Ffunzioni[vinc_loc].perc_utilizzo++;
                                //printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
                                c=0;
                            }
                        }
                        else{
                            if (c!=FNum_Heur_Used)
                                c++;
                            else {
                                c=1;
                                Ffunzioni[vinc_loc].perc_utilizzo++;
                                //printf("La funzione %i rivince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
                                //se tutte le euristiche del turno non portano benefici si considera vincitore l'ultimo che è stato
                            }
                        }
                    }
                    else {
                        if (c!=FNum_Heur_Used)
                            c++;
                        else {
                            c=1;
                            Ffunzioni[vinc_loc].perc_utilizzo++;
                            //printf("La funzione %i rivince e la sua perc e': %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
                            //se tutte le euristiche del turno sono non feasible si considera vincitore l'ultimo che è stato
                        }
                    }
                    temp=temp->next;
                }//FINE WHILE-VINCITORI-EURISTICHE:scorre temp (vecchio prossimo) salvando di volta in volta valori migliori tra tutte le schedule contenute in prossimo, che erano quelle trovate da tutte le euristiche tenendo questo job come fisso
                //mio codice per vincitori locali
                if (c==FNum_Heur_Used && FNum_Heur_Used !=1){
                    Ffunzioni[vinc_loc].perc_utilizzo++;
                    //printf("La funzione %i vince e la sua perc e': %i \n",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
                }
                //ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                // e su quale macchina �stato schedulato

                //printf("\n(%i) %i %i %i %i",cont_livelli, ID_heur, L_max, C_max,tardy);
                //printf("\n");
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

                //libero prossimo e permutazioni per il prossimo ciclo
                TNext_Elem *temp_pr;
                while (prossimo->next != NULL){
                    temp_pr = prossimo->next;
                    delete(prossimo);
                    prossimo = temp_pr;
                }

                for(i=0;i<Fnum_Heur;i++)
                {
                    if (Ffunzioni[i].perc_utilizzo != -1)
                        delete(permutazioni[i]);
                }
            }
        } //FINE for con iter_for (quello che fa partire il rollout da tutti i job)
        //printf(" %i____________________\n",cont_livelli);
        //____________________________________________________________________________________________________________________

        Pos_vincitore=SelezionaProssimoJob(lista_prossimi_vincitori,cont_livelli);
        //Pos_vincitore indica la posizione relativa del job da schedulare
        //scelto fra tutti quelli vincitori in questo stadio
        Pos_assoluta=TrovaPosizioneAssoluta(array_job_attuale,Pos_vincitore);

        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        //poi inserisco in best_perm il job migliore fra tutti i vincitori dello stadio di rollout
        if
                (
                 (lista_prossimi_vincitori[Pos_vincitore].feasible>Ffeasible_Best)
                 ||
                 (
                     (lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best)
                     &&
                     (lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best)
                     )
                 ||
                 (
                     (lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best)
                     &&
                     (lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best)
                     &&
                     (lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best)
                     )
                 ||
                 (
                     (lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best)
                     &&
                     (lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best)
                     &&
                     (lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best)
                     &&
                     (lista_prossimi_vincitori[Pos_vincitore].Tardy<Ftardy_Best)
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
            //il job schedulato viene segnato come non selezionabile (non rischedulabile)
            array_job_attuale[Pos_assoluta].ID=-1;
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
            array_job_attuale[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;
        }
        //ora viene esclusa una euristica dall'insieme delle euristiche a seconda della politica di pruning scelta.
        EscludiEuristica(Ffunzioni,FPolitica_Pruning,cont_livelli);

        cont_livelli--;
    } //FINE WHILE-STADI-ROLLOUT

    i=0;
    while(array_job_attuale[i].ID==-1)
    {
        i++;
    }
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


