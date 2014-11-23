#include "qRolloutThread.h"
#include "Globals.h"
#include "Euristiche.h"
#include "Schedula.h"
#include <time.h>

TJob *QRolloutThread::Rollout_Time(int force,int Tempo)
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo
    // i due vettori che inizializzan e tengono conto del tempo corrente
    time_t *diinizio,*corrente;
    TNext_Elem *job_fisso ;
    int i,iter,kk,diff  = 0;
    int pp=0,jj=0;
    int iter_for = 0;
    int index,index1 =0;
    int cont_livelli= GNum_Job-1;
    int Pos_vincitore = 0;
    int Pos_assoluta=0;
    TJob **permutazioni;
    permutazioni= new TJob*[Fnum_Heur];
    TJob *job_da_inserire;

    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_attuale;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    Fpermutazione_Buffer=new TJob[GNum_Job] ;
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if(GNum_Macchine >= 2)
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if(GNum_Macchine == 3)
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for(i = 0;i<GNum_Job;i++)
        array_job_attuale[i] = GArray_Job[i];

    job_fisso = new TNext_Elem;
    diinizio = 0;
    corrente = 0;
    time(diinizio);
    TJob * perm_di_passaggio;
    while(cont_livelli>0)
    {
        time(corrente);
        diff = difftime(*corrente,*diinizio);
        if (diff < Tempo){
            break;

        }
        TNext_Elem lista_prossimi_vincitori[500];
        index = 0;
        index1 = 0;
        /*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/
        for(iter_for=0;iter_for<GNum_Job;iter_for++)
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if (array_job_attuale[iter_for].ID==-1)//se e' non selezionabile
            {
            }
            else //se �selezionabile
            {
                job_fisso->ID_job = array_job_attuale[iter_for].ID;
                job_da_inserire = new TJob;// da eliminare
                job_da_inserire[0] = array_job_attuale[iter_for];
                array_job_attuale[iter_for].ID =-1;
                array_job_attuale_temp = new TJob[cont_livelli];
                iter=0;
                for(kk = 0;kk < cont_livelli; kk++)
                {
                    while(array_job_attuale[iter].ID == -1)
                        iter++;

                    array_job_attuale_temp[kk]=array_job_attuale[iter];
                    iter++;
                }
                iter=0;
                delete(job_da_inserire);
                TNext_Elem *prossimo = NULL;
                TNext_Elem *prossimo1;
                for (i = 0;i < Fnum_Heur; i++)//num_heur dovra' essere cambiato con il numero di heuristiche effettivamente usato
                {
                    Azzera_Schedule();
                    perm_di_passaggio = new TJob[GNum_Job];
                    Inizializza_Permutazione_Migliore(perm_di_passaggio);
                    for(pp = 0; pp < GNum_Job-cont_livelli-1; pp++)
                        perm_di_passaggio[pp] = GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[(GNum_Job-cont_livelli-1)]=GArray_Job[iter_for];
                    TNext_Elem *prossimo2;
                    prossimo2 =  new TNext_Elem;
                    prossimo2->next = NULL;
                    Costruisci_E_Valuta_Schedula(GMacch1_Sched,
                                                 GMacch2_Sched,
                                                 GMacch3_Sched,
                                                 prossimo2,
                                                 perm_di_passaggio,
                                                 GNum_Job);
                    delete(prossimo2);
                    Azzera_Schedule();
                    permutazioni[i]=NULL;
                    permutazioni[i] = Ffunzioni[i].funz(array_job_attuale_temp,cont_livelli);
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
                        {
                            temp = temp->next;
                        }
                        temp_prox= new TNext_Elem;
                        temp->next = temp_prox;
                        temp = temp->next;
                        temp->ID_job = permutazioni[i][0].ID;
                        temp->ID_heur= Ffunzioni[i].ID_heur;
                        temp->next = NULL;

                    }
                    for(jj = 0; pp < GNum_Job; pp++, jj++)
                        perm_di_passaggio[pp] = permutazioni[i][jj];

                    Azzera_Schedule();
                    Costruisci_E_Valuta_Schedula(GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,perm_di_passaggio,GNum_Job);
                    ////printf(" %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli);
                    Salva_Se_Meglio(perm_di_passaggio);
                    delete(perm_di_passaggio);
                    //devo riportare la macchina nelle condizioni orginarie
                    if(force == 1)
                    {
                        prossimo1 = new TNext_Elem;
                        M1_sch_buffer= new TSchedula;
                        M2_sch_buffer=new TSchedula;
                        M3_sch_buffer=new TSchedula;
                        Copia_Schedule(GMacch1_Sched,M1_sch_buffer);
                        if(GNum_Macchine>=2)
                        {
                            Copia_Schedule(GMacch2_Sched,M2_sch_buffer);
                        }
                        if(GNum_Macchine==3)
                        {
                            Copia_Schedule(GMacch3_Sched,M3_sch_buffer);
                        }
                        VNS(M1_sch_buffer,M2_sch_buffer,M3_sch_buffer);
                        Bilanciamento_Schedule(M1_sch_buffer,M2_sch_buffer,M3_sch_buffer);//bilancio
                        Valuta_Schedula(M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1);
			
                        Elimina_Schedula(M1_sch_buffer);
                        if(GNum_Macchine>=2)
                        {
                            Elimina_Schedula(M2_sch_buffer);
                        }
                        if(GNum_Macchine==3)
                        {

                            Elimina_Schedula(M3_sch_buffer);
                        }
                        TNext_Elem *tmp_prox;
                        tmp_prox=prossimo;
                        while(tmp_prox->next!=NULL)
                        {
                            tmp_prox = tmp_prox->next;
                        }//trovo l'ultimo elemento.
                        if
                                (
                                 (prossimo1->feasible>tmp_prox->feasible)
                                 ||
                                 (
                                     (prossimo1->feasible==tmp_prox->feasible)
                                     &&
                                     (prossimo1->Lmax<tmp_prox->Lmax)
                                     )
                                 ||
                                 (
                                     (prossimo1->feasible==tmp_prox->feasible)
                                     &&
                                     (prossimo1->Lmax==tmp_prox->Lmax)
                                     &&
                                     (prossimo1->Cmax<tmp_prox->Cmax)
                                     )
                                 ||
                                 (
                                     (prossimo1->feasible==tmp_prox->feasible)
                                     &&
                                     (prossimo1->Lmax==tmp_prox->Lmax)
                                     &&
                                     (prossimo1->Cmax==tmp_prox->Cmax)
                                     &&
                                     (prossimo1->Tardy<tmp_prox->Cmax)

                                     )
                                 )
                        {
                            tmp_prox->Lmax=prossimo1->Lmax;
                            tmp_prox->Cmax=prossimo1->Cmax;
                            tmp_prox->Tardy=prossimo1->Tardy;
                            tmp_prox->feasible=prossimo1->feasible;
                        }

                        delete(prossimo1);
                    }
                }
                //se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                //devo ridurre il numero di job che rimangono da schedulare
                //devo trovare il job con la Lateness + alta
                //in condizioni di parita' quello con la Cmax +bassa
                //infine con il numero + basso di Tardy job
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
                    }
                    temp = temp->next;
                }
                // ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                // e su quale macchina e' stato schedulato

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

                TNext_Elem *temp_pr;
                for(i = 0; i < Fnum_Heur; i++)
                {
                    temp_pr = prossimo->next;
                    delete(prossimo);
                    prossimo = temp_pr;
                }

                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                for(i = 0; i < Fnum_Heur; i++)
                    delete(permutazioni[i]);
            }
        }
        //_____________________________________________________________________________________________________________________
        Pos_vincitore=Seleziona_Prossimo_Job(lista_prossimi_vincitori,cont_livelli);//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta=Trova_Posizione_Assoluta(array_job_attuale,Pos_vincitore);
        // 		//printf("\nla posizione_vincitore �: %i Pos_assoluta �: %i",Pos_vincitore,Pos_assoluta);
        // 		//printf("\n l'id del job successore a quello da inserire �:%i \n",lista_prossimi_vincitori[Pos_vincitore].ID_job);
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
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
        cont_livelli--;
    }
    i=0;
    while(array_job_attuale[i].ID==-1)
        i++;
    GBest_Perm[GNum_Job-1].ID=array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo=array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time=array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate=array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline=array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority=array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time=array_job_attuale[i].rel_time;
    array_job_attuale[i].ID=-1;
    delete(job_fisso);
    delete(array_job_attuale);
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1= new TNext_Elem;
    prossimo1->next=NULL;
    Costruisci_E_Valuta_Schedula(GMacch1_Sched,
                                 GMacch2_Sched,
                                 GMacch3_Sched,
                                 prossimo1,
                                 GBest_Perm,
                                 GNum_Job);
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
        // 		//printf("\n Uso La permutazione_buffer\n\a");
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




