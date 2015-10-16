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


//output: schedula_di_lavoro1, schedula_di_lavoro2
//La funzione restituisce queste due schedule generate rispettivamente da M1_sch ed M2_sch swappando i job sch_swap1 e sch_swap2
void QRolloutThread::MossaSwap(TSchedula *pM1_sch,
                                TSchedula *pM2_sch,
                                TElem *pM1,
                                TElem *pM2,
                                TSchedula *pSch_Swap1,
                                TSchedula *pSch_Swap2,
                                TSchedula *pSchedula_di_lavoro1,
                                TSchedula *pSchedula_di_lavoro2)
{

    /*PSEUDOCODICE
1.scorro le schedule copiando gli elementi fino ai job interessati allo swap
2.salvo i 2 job da spostare job_swap1, job_swap2
3.verifica_macchina(schedulazione_di_lavoro1,M1,disponibilit� 1,setup_vett1,0,job_swap1,0)
  verifica_macchina(schedulazione_di_lavoro2,...)
4.AggiungiSchedula(schedulazione_di_lavoro1,...)
  AggiungiSchedula(schedulazione_di_lavoro2,...)
5.salvo gli ID_violati dei job_swap1 e job_swap2
6.continuo a copiare le schedule fino alla loro fine
*/	
	TSchedula *temp1;
	TSchedula *temp2;
	TSchedula *M1_sch_temp;
	TSchedula *M2_sch_temp;
	
        TJob *job_swap1=new TJob;
        TJob *job_swap2=new TJob;
        TJob *job_temp=new TJob;
        int *disponibilita1=new int;
        int *disponibilita2=new int;
        int *setup_vett1=new int;
        int *setup_vett2=new int;
	int i;

        M1_sch_temp = pM1_sch;
        M2_sch_temp = pM2_sch;
        temp1=pSchedula_di_lavoro1;
        temp2=pSchedula_di_lavoro2;
	
	//copio nella schedula di lavoro1 gli elementi fino a che non incontro quello che dovrei mettere nella schedula_di_lavoro2
        while (M1_sch_temp -> ID_job != pSch_Swap1 -> ID_job){
		if (temp1->ID_job != -3){
                        temp1->next=new TSchedula;
			temp1=temp1->next;
		}
		temp1->ID_job = M1_sch_temp->ID_job;
                temp1->tipo = M1_sch_temp->tipo;
		temp1->inizio = M1_sch_temp->inizio;
		temp1->fine = M1_sch_temp->fine;
		temp1->Lmax = M1_sch_temp->Lmax;
		temp1->Tardy = M1_sch_temp->Tardy;
		temp1->index_camp = M1_sch_temp->index_camp;
		temp1->next=NULL;
		M1_sch_temp = M1_sch_temp->next;
	}

	//copio nella schedula di lavoro2 gli elementi fino a che non incontro quello che dovrei mettere nella schedula_di_lavoro1
        while (M2_sch_temp->ID_job != pSch_Swap2->ID_job){
		if (temp2->ID_job != -3){
                        temp2->next=new TSchedula;
			temp2=temp2->next;
		}
		temp2->ID_job = M2_sch_temp->ID_job;
                temp2->tipo = M2_sch_temp->tipo;
		temp2->inizio = M2_sch_temp->inizio;
		temp2->fine = M2_sch_temp->fine;
		temp2->Lmax = M2_sch_temp->Lmax;
		temp2->Tardy = M2_sch_temp->Tardy;
		temp2->index_camp = M2_sch_temp->index_camp;
		temp2->next=NULL;
		M2_sch_temp = M2_sch_temp->next;
	}

	//fai lo swap
	i=0;
        while(i < GNum_Job)
	{
		if(M1_sch_temp->ID_job == GArray_Job[i].ID){
			job_swap1[0] = GArray_Job[i];
			break;
		}
		i++;
	}
	i=0;
	while(i<GNum_Job)
	{
		if(M2_sch_temp->ID_job == GArray_Job[i].ID){
			job_swap2[0] = GArray_Job[i];
			break;
		}
		i++;
	}

        VerificaMacchina(pSchedula_di_lavoro1,pM1,disponibilita1,setup_vett1,0,job_swap2,0);
        VerificaMacchina(pSchedula_di_lavoro2,pM2,disponibilita2,setup_vett2,0,job_swap1,0);

        Schedula::AggiungiSchedula(pSchedula_di_lavoro1,job_swap2[0],disponibilita1[0],setup_vett1[0]);
        Schedula::AggiungiSchedula(pSchedula_di_lavoro2,job_swap1[0],disponibilita2[0],setup_vett2[0]);
	
	//rischedula i job sulle schedule_di_lavoro che sono dopo i job swappati
	M1_sch_temp = M1_sch_temp->next;
	while (M1_sch_temp != NULL){
		i=0;
		while(i<GNum_Job){
			if(M1_sch_temp->ID_job == GArray_Job[i].ID){
				job_temp[0] = GArray_Job[i];
			}
			i++;
		}
                VerificaMacchina(pSchedula_di_lavoro1,pM1,disponibilita1,setup_vett1,0,job_temp,0);
                Schedula::AggiungiSchedula(pSchedula_di_lavoro1,job_temp[0],disponibilita1[0],setup_vett1[0]);
		M1_sch_temp = M1_sch_temp->next;
		//temp1=temp1->next;
	}

	M2_sch_temp = M2_sch_temp->next;
	//temp2 = temp2->next;
	while(M2_sch_temp != NULL){
		i=0;
		while(i<GNum_Job){
			if(M2_sch_temp->ID_job == GArray_Job[i].ID){
				job_temp[0] = GArray_Job[i];
			}
			i++;
		}
                VerificaMacchina(pSchedula_di_lavoro2,pM2,disponibilita2,setup_vett2,0,job_temp,0);
                Schedula::AggiungiSchedula(pSchedula_di_lavoro2,job_temp[0],disponibilita2[0],setup_vett1[0]);
		M2_sch_temp = M2_sch_temp->next;
	}
	//libero un pò di memoria
        delete(job_swap1);
        delete(job_swap2);
        delete(job_temp);
        delete(disponibilita1);
        delete(disponibilita2);
        delete(setup_vett1);
        delete(setup_vett2);
} 

//output: schedula_di_lavoro1, schedula_di_lavoro2
//Restituisce queste due schedule generate rispettivamente da InCuiInserire col job puntato da sch_insert (di InCuiLevare) e InCuiLevare senza il job puntato da sch_insert
void QRolloutThread::MossaInsert(TSchedula *pInCuiInserire,
                                  TSchedula *pInCuiLevare,
                                  TElem *pM1,
                                  TElem *pM2,
                                  TSchedula *pSch_Insert,
                                  TSchedula *pSch_Following,
                                  TSchedula *pSchedula_di_lavoro1,
                                  TSchedula *pSchedula_di_lavoro2)
{
/*PSEUDOCODICE:
1. copio nelle schedule di lavoro i primi elementi se non sono interessati all'insert
2. scorro copiando le schedule fino alle posizioni interessate all'insert
3. faccio l'insert (schedulo in schedula_di_lavoro1 il job_insert)
4. continuo a schedulare la schedula_di_lavoro1 fino alla fine
5. continuo a schedulare la schedula_di_lavoro2 fino alla fine saltando il job_insert
*/

	TSchedula *temp1;
	TSchedula *temp2;
	TSchedula *Sch1_ins_temp;
	TSchedula *Sch2_ins_temp;
	
        TJob *job_insert=new TJob;
        TJob *job_temp=new TJob;
        int *disponibilita=new int;
        int *setup_vett=new int;
	int i;

        Sch1_ins_temp = pInCuiInserire;
        Sch2_ins_temp = pInCuiLevare;
        temp1=pSchedula_di_lavoro1;
        temp2=pSchedula_di_lavoro2;

	//scorro copiando le schedule fino alle posizioni interessate all'insert
        while (Sch1_ins_temp->ID_job != pSch_Following->ID_job)
        {
                if (temp1->ID_job != -3)
                {
                        temp1->next = new TSchedula;
                        temp1 = temp1->next;
		}
		temp1->ID_job = Sch1_ins_temp->ID_job;
                temp1->tipo = Sch1_ins_temp->tipo;
		temp1->inizio = Sch1_ins_temp->inizio;
		temp1->fine = Sch1_ins_temp->fine;
		temp1->Lmax = Sch1_ins_temp->Lmax;
		temp1->Tardy = Sch1_ins_temp->Tardy;
		temp1->index_camp = Sch1_ins_temp->index_camp;
		temp1->next=NULL;
		Sch1_ins_temp = Sch1_ins_temp->next;	
	}
	
        while(Sch2_ins_temp -> ID_job != pSch_Insert -> ID_job){
		if(temp2->ID_job != -3){
                        temp2->next=new TSchedula;
			temp2=temp2->next;
		}
		temp2->ID_job = Sch2_ins_temp->ID_job;
                temp2->tipo = Sch2_ins_temp->tipo;
		temp2->inizio = Sch2_ins_temp->inizio;
		temp2->fine = Sch2_ins_temp->fine;
		temp2->Lmax = Sch2_ins_temp->Lmax;
		temp2->Tardy = Sch2_ins_temp->Tardy;
		temp2->index_camp = Sch2_ins_temp->index_camp;
		temp2->next=NULL;
		Sch2_ins_temp = Sch2_ins_temp->next;
	}
	
	//faccio l'insert
	i=0;
	while(i<GNum_Job)
	{
                if(pSch_Insert->ID_job == GArray_Job[i].ID)
                {
			job_insert[0] = GArray_Job[i];
			break;
		}
		i++;
	}
        VerificaMacchina(pSchedula_di_lavoro1,pM1,disponibilita,setup_vett,0,job_insert,0);
        Schedula::AggiungiSchedula(pSchedula_di_lavoro1,job_insert[0],disponibilita[0],setup_vett[0]);

	//continuo a schedulare la schedula_di_lavoro1 fino alla fine
	while(Sch1_ins_temp != NULL){
		i=0;
		while(i<GNum_Job){
			if(Sch1_ins_temp -> ID_job == GArray_Job[i].ID){
				job_temp[0]=GArray_Job[i];
				break;
			}
			i++;
		}
                VerificaMacchina(pSchedula_di_lavoro1,pM1,disponibilita,setup_vett,0,job_temp,0);
                Schedula::AggiungiSchedula(pSchedula_di_lavoro1,job_temp[0],disponibilita[0],setup_vett[0]);
		Sch1_ins_temp=Sch1_ins_temp->next;
	}

	//continuo a schedulare la schedula_di_lavoro2 fino alla fine
	Sch2_ins_temp=Sch2_ins_temp->next;
	while(Sch2_ins_temp != NULL){
		i=0;
                while(i < GNum_Job){
			if(Sch2_ins_temp -> ID_job == GArray_Job[i].ID){
				job_temp[0]=GArray_Job[i];
				break;
			}
			i++;
		}
                VerificaMacchina(pSchedula_di_lavoro2,pM2,disponibilita,setup_vett,0,job_temp,0);
                Schedula::AggiungiSchedula(pSchedula_di_lavoro2,job_temp[0],disponibilita[0],setup_vett[0]);
		Sch2_ins_temp=Sch2_ins_temp->next;
	}
        //faccio un pò di delete
        delete(job_insert);
        delete(job_temp);
        delete(disponibilita);
        delete(setup_vett);
}


//esegue le VNS separatamente su M1_sch_buffer, M2_sch_buffer, M3_sch_buffer
int QRolloutThread::VnsRicercaLocale(TSchedula *PM1_sch,
                                     TSchedula *PM2_sch,
                                     TSchedula *PM3_sch,
                                     TElem *pM1,
                                     TElem *pM2,
                                     TElem *pM3)
{
    int ris = 0;
    ris += VnsPerMacchina(PM1_sch,pM1);
    if(GNum_Macchine >= 2)
        ris += VnsPerMacchina(PM2_sch,pM2);
    if(GNum_Macchine == 3)
        ris += VnsPerMacchina(PM3_sch,pM3);
    return ris;
}

//output: sch_coda
//Restituisce in sch_coda la schedula generata da M_sch ma col job in_coda spostato in coda
void QRolloutThread::MossaSpostaInCoda(TSchedula *pM_sch,
                                       TElem *pM,
                                       TSchedula *pIn_Coda,
                                       TSchedula *pSch_Coda)
{
    TSchedula *temp, *M_sch_temp;
    int i;
    int *disponibilita  = new int;
    int *setup_vett     = new int;
    TJob *job_temp      = new TJob;

    //copio i primi elementi della schedula fino a che non incontro quello da mettere in coda
    temp = pSch_Coda;
    M_sch_temp = pM_sch;
    while(M_sch_temp->ID_job != pIn_Coda->ID_job)
    {
        if (temp->ID_job != -3)
        {
            temp->next  = new TSchedula;
            temp        = temp->next;
        }
        temp->ID_job = M_sch_temp->ID_job;
        temp->tipo = M_sch_temp->tipo;
        temp->inizio = M_sch_temp->inizio;
        temp->fine = M_sch_temp->fine;
        temp->Lmax = M_sch_temp->Lmax;
        temp->Tardy = M_sch_temp->Tardy;
        temp->index_camp = M_sch_temp->index_camp;
        temp->next=NULL;
        M_sch_temp=M_sch_temp->next;
    }

    //scavalco l'elemento in coda
    M_sch_temp=M_sch_temp->next;
    //continuo a schedulare fino alla fine
    while (M_sch_temp != NULL){
        i=0;
        while(i<GNum_Job){
            if(M_sch_temp->ID_job == GArray_Job[i].ID){
                job_temp[0] = GArray_Job[i];
            }
            i++;
        }
        VerificaMacchina(pSch_Coda,pM,disponibilita,setup_vett,0,job_temp,0);
        Schedula::AggiungiSchedula(pSch_Coda,job_temp[0],disponibilita[0],setup_vett[0]);
        M_sch_temp = M_sch_temp->next;	}

    //metto in coda l'elemento in coda
    i=0;
    while(i<GNum_Job){
        if(pIn_Coda->ID_job == GArray_Job[i].ID){
            job_temp[0] = GArray_Job[i];
        }
        i++;
    }
    VerificaMacchina(pSch_Coda, pM, disponibilita, setup_vett, 0, job_temp, 0);
    Schedula::AggiungiSchedula(pSch_Coda, job_temp[0], disponibilita[0], setup_vett[0]);
    //libera memoria
    delete(disponibilita);
    delete(setup_vett);
    delete(job_temp);
}

//output: schedula_di_lavoro1, schedula_di_lavoro2
//Restituisce gli output con le schedule generate rispettivamente da InCuiInserire con il job puntato da sch_insert (di InCuiLevare) messo in coda e da InCuiLevare senza il job puntato da sch_insert
void QRolloutThread::MossaInsertCoda(TSchedula *pInCuiInserire,
                                     TSchedula *pInCuiLevare,
                                     TElem *pM1,
                                     TElem *pM2,
                                     TSchedula *pSch_Insert,
                                     TSchedula *pSchedula_di_lavoro1,
                                     TSchedula *pSchedula_di_lavoro2)
{
    TSchedula *sch1_temp;
    TSchedula *sch2_temp;
    TSchedula *temp1;
    TSchedula *temp2;
    TJob *job_insert= new TJob;
    TJob *job_temp  = new TJob;
    int i;
    int *disponibilita=new int;
    int *setup_vett=new int;

    sch1_temp = pInCuiInserire;
    sch2_temp = pInCuiLevare;
    temp1 = pSchedula_di_lavoro1;
    temp2 = pSchedula_di_lavoro2;

    //copio in schedula_di_lavoro1 scorrendo InCuiInserire per copiare i valori
    while (sch1_temp != NULL){
        if (temp1->ID_job != -3)
        {
            temp1->next = new TSchedula;
            temp1 = temp1->next;
        }
        temp1->ID_job       = sch1_temp->ID_job;
        temp1->tipo         = sch1_temp->tipo;
        temp1->inizio       = sch1_temp->inizio;
        temp1->fine         = sch1_temp->fine;
        temp1->Lmax         = sch1_temp->Lmax;
        temp1->Tardy        = sch1_temp->Tardy;
        temp1->index_camp   = sch1_temp->index_camp;
        temp1->next         = NULL;
        sch1_temp = sch1_temp->next;
    }

    //copio in schedula_di_lavoro2 scorrendo InCuiLevare fino al job interessato all'insert che dovr�  essere rimosso da questa schedula
    while(sch2_temp->ID_job != pSch_Insert->ID_job)
    {
        if (temp2->ID_job != -3)
        {
            temp2->next = new TSchedula;
            temp2       = temp2->next;
        }
        temp2->ID_job       = sch2_temp->ID_job;
        temp2->tipo         = sch2_temp->tipo;
        temp2->inizio       = sch2_temp->inizio;
        temp2->fine         = sch2_temp->fine;
        temp2->Lmax         = sch2_temp->Lmax;
        temp2->Tardy        = sch2_temp->Tardy;
        temp2->index_camp   = sch2_temp->index_camp;
        temp2->next         = NULL;
        sch2_temp   =   sch2_temp->next;
    }

    //continuo a schedulare la schedula_di_lavoro2 fino alla fine saltanto il job dell'insert
    sch2_temp=sch2_temp->next;
    while(sch2_temp != NULL)
    {
        i = 0;
        while(i < GNum_Job)
        {
            if(sch2_temp -> ID_job == GArray_Job[i].ID)
            {
                job_temp[0]=GArray_Job[i];
                break;
            }
            i++;
        }
        VerificaMacchina(pSchedula_di_lavoro2,pM2,disponibilita,setup_vett,0,job_temp,0);
        Schedula::AggiungiSchedula(pSchedula_di_lavoro2,job_temp[0],disponibilita[0],setup_vett[0]);
        sch2_temp=sch2_temp->next;
    }

    //faccio l'insert in coda a schedula_di_lavoro1
    i = 0;
    while(i < GNum_Job)
    {
        if(pSch_Insert->ID_job == GArray_Job[i].ID)
        {
            job_insert[0] = GArray_Job[i];
            break;
        }
        i++;
    }
    VerificaMacchina(pSchedula_di_lavoro1,pM1,disponibilita,setup_vett,0,job_insert,0);
    Schedula::AggiungiSchedula(pSchedula_di_lavoro1,job_insert[0],disponibilita[0],setup_vett[0]);
    //faccio un pò di delete
    delete(job_insert);
    delete(job_temp);
    delete(disponibilita);
    delete(setup_vett);
}



/*ricerca locale tra macchine utilizzando come mosse swap e insert puri.
INSERT: di tutti i job della macchina critica in tutte le posizioni delle altre schedule
SWAP: di tutti i job della macchina critica con tutti quelli delle altre macchine
*/
int QRolloutThread::RicercaLocaleTraMacchine(TSchedula *pM1_sch,
                                             TSchedula *pM2_sch,
                                             TSchedula *pM3_sch)
{

    TSchedula *temp1 = NULL;
    TSchedula *temp2 = NULL;
    TSchedula *schedula_di_lavoro1 = NULL;
    TSchedula *schedula_di_lavoro2 = NULL;
    TQuaterna *quaterna_di_lavoro  = NULL;
    TQuaterna *quaterna_migliore   = NULL;
    TNext_Elem *prossimo = NULL;
    TSchedula **arraySch = NULL;
    TElem **arrayM = NULL;
    int ris = 0;
    if(GNum_Macchine == 2)
    {
        prossimo            =   new TNext_Elem;
        quaterna_migliore   =   new TQuaterna;
        ValutaSchedula(pM1_sch,pM2_sch,NULL,prossimo);

        quaterna_migliore->Feasible = prossimo->feasible;
        quaterna_migliore->Lmax     = prossimo->Lmax;
        quaterna_migliore->Cmax     = prossimo->Cmax;
        quaterna_migliore->Tardy    = prossimo->Tardy;
        quaterna_di_lavoro  = new TQuaterna;
        arraySch    =   new TSchedula*[2];
        arrayM      =   new TElem*[2];
        OrdinaCandidati(arraySch,arrayM);
        temp2 = arraySch[0];
        while(temp2 != NULL)
        {
            temp1 = arraySch[1];
            while (temp1 != NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaInsert(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,temp1,schedula_di_lavoro1,schedula_di_lavoro2);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
	
        temp2=arraySch[0];
        while(temp2 != NULL)
        {
            temp1 = arraySch[1];
            while(temp1 != NULL)
            {
                schedula_di_lavoro1 = new TSchedula;
                schedula_di_lavoro2 = new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaSwap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax  <   quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax ==  quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax ==  quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax ==  quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<   quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                       Schedula::EliminaSchedula(schedula_di_lavoro1);
                       Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1 = temp1->next;
            }
            temp2 = temp2->next;
        }
    }
    else if(GNum_Macchine==3){
        prossimo=new TNext_Elem;
        quaterna_migliore=new TQuaterna;
        ValutaSchedula(pM1_sch, pM2_sch, pM3_sch, prossimo);
        quaterna_migliore->Feasible=prossimo->feasible;
        quaterna_migliore->Lmax=prossimo->Lmax;
        quaterna_migliore->Cmax=prossimo->Cmax;
        quaterna_migliore->Tardy=prossimo->Tardy;
        quaterna_di_lavoro=new TQuaterna;
        arraySch=new TSchedula*[3];  ////// CONTROLLARE
        arrayM=new TElem*[2];        ////CONTROLLARE
        OrdinaCandidati(arraySch, arrayM);
        //ciclo di insert tra prima e seconda schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[1];
            while(temp1 !=NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaInsert(arraySch[1], arraySch[0], arrayM[1], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert4 conveniente    %d   %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax,quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo di insert tra prima e terza schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[2];
            while(temp1 !=NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaInsert(arraySch[2], arraySch[0], arrayM[2], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                ValutaSchedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2,arraySch[1], schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert5 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&
                              (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2,arraySch[1], schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert6 conveniente    %d    %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax,quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo di swap tra prima e seconda schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[1];
            while(temp1 != NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaSwap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo di swap tra prima e terza schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[2];
            while(temp1 != NULL){
                schedula_di_lavoro1 = new TSchedula;
                schedula_di_lavoro2 = new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaSwap(arraySch[0], arraySch[2], arrayM[0], arrayM[2], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                ValutaSchedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchine(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1 = temp1->next;
            }
            temp2 = temp2->next;
        }
    }
    delete(quaterna_di_lavoro);
    delete(quaterna_migliore);
    Schedula::EliminaSchedula(schedula_di_lavoro1);
    Schedula::EliminaSchedula(schedula_di_lavoro2);

    delete(arraySch);
    delete(arrayM);
    delete(prossimo);

    return ris;
}




/*
ordina le tre schedule in un array a seconda dell'ordine di come andranno esaminate nella ricerca locale.
Quindi in arraySch[0] avra' la schedula critica
*/
void QRolloutThread::OrdinaCandidati(TSchedula **arraySch, TElem **arrayM)
{
        TQuaterna *quaterna1;
	TQuaterna *quaterna2;
	TQuaterna *quaterna3;

        quaterna1 = Schedula::ValutaSingolaSchedula(GMacch1_Sched);
        quaterna2 = Schedula::ValutaSingolaSchedula(GMacch2_Sched);
	if(GNum_Macchine==2){
		if (quaterna1->Lmax >= quaterna2->Lmax){
			arraySch[0]=GMacch1_Sched;
			arraySch[1]=GMacch2_Sched;
			arrayM[0]=GMacch1;
			arrayM[1]=GMacch2;
		}
		else {
			arraySch[0]=GMacch2_Sched;
			arraySch[1]=GMacch1_Sched;
			arrayM[0]=GMacch2;
			arrayM[1]=GMacch1;
		}
                delete(quaterna1);
                delete(quaterna2);
	}
	if(GNum_Macchine == 3){
                quaterna3 = Schedula::ValutaSingolaSchedula(GMacch3_Sched);
		if(quaterna1->Lmax > quaterna2->Lmax && quaterna1->Lmax > quaterna3->Lmax){
			arraySch[0]=GMacch1_Sched;
			arraySch[1]=GMacch2_Sched;
			arraySch[2]=GMacch3_Sched;
			arrayM[0]=GMacch1;
			arrayM[1]=GMacch2;
			arrayM[2]=GMacch3;
		}
		else if(quaterna2->Lmax > quaterna1->Lmax && quaterna2->Lmax > quaterna3->Lmax){
			arraySch[0]=GMacch2_Sched;
			arraySch[1]=GMacch1_Sched;
			arraySch[2]=GMacch3_Sched;
			arrayM[0]=GMacch2;
			arrayM[1]=GMacch1;
			arrayM[2]=GMacch3;
		}
		else{
			arraySch[0]=GMacch3_Sched;
			arraySch[1]=GMacch1_Sched;
			arraySch[2]=GMacch2_Sched;
			arrayM[0]=GMacch3;
			arrayM[1]=GMacch1;
			arrayM[2]=GMacch2;
		}
                delete(quaterna1);
                delete(quaterna2);
                delete(quaterna3);
	}
	
	
}

/*funzione che viene chiamata nel caso di miglioramenti nella ricerca locale tra macchine.
Ricopia le schedule_di_lavoro nelle M_sch.
N.B. Le schedule_di_lavoro vengono messe nell'ordine in cui hanno preso le schedule di arraySch
*/
void QRolloutThread::SostituisciSchedule(TSchedula **pArraySch, TSchedula *pSchedula1, TSchedula *pSchedula2, TSchedula *pschedula3)
{
    if(GNum_Macchine == 2)
    {
        if(pArraySch[0]==GMacch1_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1 , GMacch1_Sched);
            Schedula::CopiaSchedule(pSchedula2 , GMacch2_Sched);
        }
        else{
            Schedula::CopiaSchedule(pSchedula2, GMacch1_Sched);
            Schedula::CopiaSchedule(pSchedula1, GMacch2_Sched);
        }
    }
    else if (GNum_Macchine==3){
        if(pArraySch[0]==GMacch1_Sched && pArraySch[1]==GMacch2_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1, GMacch1_Sched);
            Schedula::CopiaSchedule(pSchedula2, GMacch2_Sched);
            Schedula::CopiaSchedule(pschedula3, GMacch3_Sched);
        }
        if(pArraySch[0]==GMacch1_Sched && pArraySch[1]==GMacch3_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1, GMacch1_Sched);
            Schedula::CopiaSchedule(pSchedula2, GMacch3_Sched);
            Schedula::CopiaSchedule(pschedula3, GMacch2_Sched);
        }
        if(pArraySch[0]==GMacch2_Sched && pArraySch[1]==GMacch1_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1, GMacch2_Sched);
            Schedula::CopiaSchedule(pSchedula2, GMacch1_Sched);
            Schedula::CopiaSchedule(pschedula3, GMacch3_Sched);
        }
        if(pArraySch[0]==GMacch2_Sched && pArraySch[1]==GMacch3_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1, GMacch2_Sched);
            Schedula::CopiaSchedule(pSchedula2, GMacch3_Sched);
            Schedula::CopiaSchedule(pschedula3, GMacch1_Sched);
        }
        if(pArraySch[0]==GMacch3_Sched && pArraySch[1]==GMacch1_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1, GMacch3_Sched);
            Schedula::CopiaSchedule(pSchedula2, GMacch1_Sched);
            Schedula::CopiaSchedule(pschedula3, GMacch2_Sched);
        }
        if(pArraySch[0]==GMacch3_Sched && pArraySch[1]==GMacch2_Sched)
        {
            Schedula::CopiaSchedule(pSchedula1, GMacch3_Sched);
            Schedula::CopiaSchedule(pSchedula2, GMacch2_Sched);
            Schedula::CopiaSchedule(pschedula3, GMacch1_Sched);
        }
    }
}

/*ricerca locale tra macchine utilizzando come mosse swap e insert puri e poi facendo sempre la VND su ogni macchine
INSERT: di tutti i job della macchina critica in tutte le posizioni delle altre schedule
SWAP: di tutti i job della macchina critica con tutti quelli delle altre macchine
*/
int QRolloutThread::RicercaLocaleTraMacchineVND(TSchedula *pM1_sch, TSchedula *pM2_sch, TSchedula *pM3_sch)
{
    TSchedula *temp1 = NULL;
    TSchedula *temp2 = NULL;
    TSchedula *schedula_di_lavoro1  = NULL;
    TSchedula *schedula_di_lavoro2  = NULL;
    TQuaterna *quaterna_di_lavoro   = NULL;
    TQuaterna *quaterna_migliore    = NULL;
    TNext_Elem *prossimo    = NULL;
    TSchedula **arraySch    = NULL;
    TElem **arrayM = NULL;
    int ris = 0;
    if(GNum_Macchine == 2)
    {
        prossimo=new TNext_Elem;
        quaterna_migliore=new TQuaterna;
        ValutaSchedula(pM1_sch,pM2_sch,NULL,prossimo);
        quaterna_migliore->Feasible = prossimo->feasible;
        quaterna_migliore->Lmax = prossimo->Lmax;
        quaterna_migliore->Cmax = prossimo->Cmax;
        quaterna_migliore->Tardy = prossimo->Tardy;
        quaterna_di_lavoro = new TQuaterna;
        arraySch = new TSchedula*[3];
        arrayM   = new TElem*[2];
        OrdinaCandidati(arraySch,arrayM);
        temp2=arraySch[0];
        while(temp2 != NULL)
        {
            temp1=arraySch[1];
            while (temp1 != NULL)
            {
                schedula_di_lavoro1 = new TSchedula;
                schedula_di_lavoro2 = new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaInsert(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,temp1,schedula_di_lavoro1,schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[1], arrayM[0], NULL);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax  <   quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax  ==  quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax  ==  quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax   ==  quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy  <   quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1 = temp1->next;
            }
            temp2 = temp2->next;
        }
	
        temp2=arraySch[0];
        while(temp2 != NULL)
        {
            temp1=arraySch[1];
            while(temp1 != NULL)
            {
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaSwap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);
                quaterna_di_lavoro->Feasible = prossimo->feasible;
                quaterna_di_lavoro->Lmax  = prossimo->Lmax;
                quaterna_di_lavoro->Cmax  = prossimo->Cmax;
                quaterna_di_lavoro->Tardy = prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible >= quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris += RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
    }
    else if(GNum_Macchine==3)
    {
        prossimo=new TNext_Elem;
        quaterna_migliore=new TQuaterna;
        ValutaSchedula(pM1_sch, pM2_sch, pM3_sch, prossimo);
        quaterna_migliore->Feasible=prossimo->feasible;
        quaterna_migliore->Lmax=prossimo->Lmax;
        quaterna_migliore->Cmax=prossimo->Cmax;
        quaterna_migliore->Tardy=prossimo->Tardy;
        quaterna_di_lavoro=new TQuaterna;
        arraySch=new TSchedula*[3];
        arrayM=new TElem*[2];
        OrdinaCandidati(arraySch, arrayM);
        //ciclo di insert tra prima e seconda schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[1];
            while(temp1 !=NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaInsert(arraySch[1], arraySch[0], arrayM[1], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[1], arrayM[0], NULL);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert4 conveniente   %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo di insert tra prima e terza schedula

        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[2];
            while(temp1 !=NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaInsert(arraySch[2], arraySch[0], arrayM[2], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[2], arrayM[0], NULL);
                ValutaSchedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2,arraySch[1], schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert5 conveniente     %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&
                              (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro2, arraySch[1], schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto un'insert6 conveniente       %d       %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo di swap tra prima e seconda schedula

        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[1];
            while(temp1 != NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaSwap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap4 conveniente    %d    %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo di swap tra prima e terza schedula

        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[2];
            while(temp1 != NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                MossaSwap(arraySch[0], arraySch[2], arrayM[0], arrayM[2], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[2], NULL);
                ValutaSchedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap5 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        ris++;
                        //printf("Ho fatto uno swap6 conveniente    %d     %d   %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);

                        ris+=RicercaLocaleTraMacchineVND(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                    }
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
    }
    delete(quaterna_di_lavoro);
    delete(quaterna_migliore);
    Schedula::EliminaSchedula(schedula_di_lavoro1);
    Schedula::EliminaSchedula(schedula_di_lavoro2);
    delete(arraySch);
    delete(arrayM);
    delete(prossimo);

    return ris;
}

/*ricerca locale tra macchine utilizzando come mosse swap_coda e insert_coda.
INSERT: di tutti i job della macchina critica in coda alle altre schedule che poi (col nuovo job) subiscono la VND
SWAP: tra tutti i job della macchina critica con tutti quelli delle altre schedule, mettendoli in coda ed eseguendo le due VND su entrambe le schedule
*/
int QRolloutThread::RicercaLocaleTraMacchineCoda(TSchedula *pM1_sch,
                                                 TSchedula *pM2_sch,
                                                 TSchedula *pM3_sch)
{
    TSchedula *temp1 = NULL;
    TSchedula *temp2 = NULL;
    TSchedula *schedula_di_lavoro1 = NULL;
    TSchedula *schedula_di_lavoro2 = NULL;
    TSchedula *sch_coda1 = NULL;
    TSchedula *sch_coda2 = NULL;
    TSchedula *coda1 = NULL;
    TSchedula *coda2 = NULL;
    TQuaterna *quaterna_di_lavoro = NULL;
    TQuaterna *quaterna_migliore  = NULL;
    TNext_Elem *prossimo = NULL;
    TSchedula **arraySch = NULL;
    TElem **arrayM = NULL;
    int ris = 0;

    if (GNum_Macchine == 2)
    {
        prossimo=new TNext_Elem;
        quaterna_migliore=new TQuaterna;
        ValutaSchedula(pM1_sch,pM2_sch,NULL,prossimo);
        quaterna_migliore->Feasible=prossimo->feasible;
        quaterna_migliore->Lmax=prossimo->Lmax;
        quaterna_migliore->Cmax=prossimo->Cmax;
        quaterna_migliore->Tardy=prossimo->Tardy;
        quaterna_di_lavoro=new TQuaterna;
        arraySch=new TSchedula*[3];;
        arrayM=new TElem*[2];
        OrdinaCandidati(arraySch,arrayM);
        temp2=arraySch[0];
        //ciclo di insert
        while (temp2 != NULL){
            schedula_di_lavoro1=new TSchedula;
            schedula_di_lavoro2=new TSchedula;
            Schedula::InizializzaSchedula(schedula_di_lavoro1);
            Schedula::InizializzaSchedula(schedula_di_lavoro2);
            MossaInsertCoda(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,schedula_di_lavoro1,schedula_di_lavoro2);
            VnsPerMacchina(schedula_di_lavoro1, arrayM[1]);
            ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);
            quaterna_di_lavoro->Feasible=prossimo->feasible;
            quaterna_di_lavoro->Lmax=prossimo->Lmax;
            quaterna_di_lavoro->Cmax=prossimo->Cmax;
            quaterna_di_lavoro->Tardy=prossimo->Tardy;
            if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
            {
                if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                {
                    quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                    quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                    quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                    quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                    SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    ris++;
                    delete(arraySch);
                    delete(arrayM);
                    delete(quaterna_di_lavoro);
                    delete(quaterna_migliore);
                    delete(prossimo);
                    ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                    return 1;
                    break;
                }
                else if
                        ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                         ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                         ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                          (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                          (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                {
                    quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                    quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                    quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                    quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                    SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    ris++;
                    delete(arraySch);
                    delete(arrayM);
                    delete(quaterna_di_lavoro);
                    delete(quaterna_migliore);
                    delete(prossimo);
                    ris += RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                    return 1;
                    break;
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
            }
            else{
                Schedula::EliminaSchedula(schedula_di_lavoro1);
                Schedula::EliminaSchedula(schedula_di_lavoro2);
            }
            temp2=temp2->next;
        }
        //ciclo di swap
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[1];
            while(temp1 != NULL){

                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                sch_coda1=new TSchedula;
                sch_coda2=new TSchedula;
                Schedula::InizializzaSchedula(sch_coda1);
                Schedula::InizializzaSchedula(sch_coda2);
                MossaSpostaInCoda(arraySch[0], arrayM[0], temp2, sch_coda1);
                MossaSpostaInCoda(arraySch[1], arrayM[1], temp1, sch_coda2);
                //coda1, coda2 puntano agli elementi della schedula da swappare, ovvero temp1, temp2 che ora sono stati messi in coda
                coda1=sch_coda1;
                coda2=sch_coda2;
                if(coda1 != NULL){
                    while(coda1->next != NULL){
                        coda1=coda1->next;
                    }
                }
                if(coda2 != NULL){
                    while(coda2->next != NULL){
                        coda2=coda2->next;
                    }
                }
                //sposta i temp in coda alle rispettive schedule
                MossaSwap(sch_coda1, sch_coda2, arrayM[0], arrayM[1], coda1, coda2, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                        ris++;
                        //printf("Ho fatto uno swap1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);
                        ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                        ris++;
                        //printf("Ho fatto uno swap2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);
                        ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    Schedula::EliminaSchedula(sch_coda1);
                    Schedula::EliminaSchedula(sch_coda2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
    }
    else if (GNum_Macchine==3){
        prossimo=new TNext_Elem;
        quaterna_migliore=new TQuaterna;
        quaterna_di_lavoro=new TQuaterna;
        ValutaSchedula(pM1_sch, pM2_sch, pM3_sch, prossimo);
        quaterna_migliore->Feasible=prossimo->feasible;
        quaterna_migliore->Lmax=prossimo->Lmax;
        quaterna_migliore->Cmax=prossimo->Cmax;
        quaterna_migliore->Tardy=prossimo->Tardy;
        arraySch=new TSchedula*[3];;
        arrayM=new TElem*[2];
        OrdinaCandidati(arraySch,arrayM);
        temp2=arraySch[0];
        //ciclo di insert tra la prima e la seconda macchina
        while(temp2 != NULL){
            schedula_di_lavoro1=new TSchedula;
            schedula_di_lavoro2=new TSchedula;
            Schedula::InizializzaSchedula(schedula_di_lavoro1);
            Schedula::InizializzaSchedula(schedula_di_lavoro2);
            MossaInsertCoda(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,schedula_di_lavoro1,schedula_di_lavoro2);
            VnsPerMacchina(schedula_di_lavoro1, arrayM[1]);
            ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
            quaterna_di_lavoro->Feasible=prossimo->feasible;
            quaterna_di_lavoro->Lmax=prossimo->Lmax;
            quaterna_di_lavoro->Cmax=prossimo->Cmax;
            quaterna_di_lavoro->Tardy=prossimo->Tardy;
            if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
            {
                if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                {
                    quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                    quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                    quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                    quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                    SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    ris++;
                    //printf("Ho fatto un'insert3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                    delete(arraySch);
                    delete(arrayM);
                    delete(quaterna_di_lavoro);
                    delete(quaterna_migliore);
                    delete(prossimo);
                    ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                    return 1;
                    break;
                }
                else if
                        ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                         ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                         ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                          (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                          (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                {
                    quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                    quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                    quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                    quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                    SostituisciSchedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    ris++;
                    delete(arraySch);
                    delete(arrayM);
                    delete(quaterna_di_lavoro);
                    delete(quaterna_migliore);
                    delete(prossimo);
                    ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                    return 1;
                    break;
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
            }
            else{
                Schedula::EliminaSchedula(schedula_di_lavoro1);
                Schedula::EliminaSchedula(schedula_di_lavoro2);
            }
            temp2=temp2->next;
        }
        //ciclo di insert tra la prima e la terza schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            schedula_di_lavoro1=new TSchedula;
            schedula_di_lavoro2=new TSchedula;
            Schedula::InizializzaSchedula(schedula_di_lavoro1);
            Schedula::InizializzaSchedula(schedula_di_lavoro2);
            MossaInsertCoda(arraySch[2],arraySch[0],arrayM[2],arrayM[0],temp2,schedula_di_lavoro1,schedula_di_lavoro2);
            VnsPerMacchina(schedula_di_lavoro1, arrayM[2]);
            ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[1], prossimo);
            quaterna_di_lavoro->Feasible=prossimo->feasible;
            quaterna_di_lavoro->Lmax=prossimo->Lmax;
            quaterna_di_lavoro->Cmax=prossimo->Cmax;
            quaterna_di_lavoro->Tardy=prossimo->Tardy;
            if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
            {
                if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                {
                    quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                    quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                    quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                    quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                    SostituisciSchedule(arraySch, schedula_di_lavoro2, arraySch[1], schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    ris++;
                    //printf("Ho fatto un'insert5 conveniente");
                    delete(arraySch);
                    delete(arrayM);
                    delete(quaterna_di_lavoro);
                    delete(quaterna_migliore);
                    delete(prossimo);
                    ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                    return 1;
                    break;
                }
                else if
                        ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                         ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                         ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                          (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                          (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                {
                    quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                    quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                    quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                    quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                    SostituisciSchedule(arraySch, schedula_di_lavoro2, arraySch[1], schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    ris++;
                    //printf("Ho fatto un'insert6 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                    delete(arraySch);
                    delete(arrayM);
                    delete(quaterna_di_lavoro);
                    delete(quaterna_migliore);
                    delete(prossimo);
                    ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                    return 1;
                    break;
                }
                else
                {
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                }
            }
            else{
                Schedula::EliminaSchedula(schedula_di_lavoro1);
                Schedula::EliminaSchedula(schedula_di_lavoro2);
            }
            temp2=temp2->next;
        }
        //ciclo swap tra la prima e la seconda schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[1];
            while(temp1 != NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                sch_coda1=new TSchedula;
                sch_coda2=new TSchedula;
                Schedula::InizializzaSchedula(sch_coda1);
                Schedula::InizializzaSchedula(sch_coda2);
                MossaSpostaInCoda(arraySch[0], arrayM[0], temp2, sch_coda1);
                MossaSpostaInCoda(arraySch[1], arrayM[1], temp1, sch_coda2);
                //coda1, coda2 puntano agli elementi della schedula da swappare, ovvero temp1, temp2 che ora sono stati messi in coda
                coda1=sch_coda1;
                coda2=sch_coda2;
                if(coda1 != NULL){
                    while(coda1->next != NULL){
                        coda1=coda1->next;
                    }
                }
                if(coda2 != NULL){
                    while(coda2->next != NULL){
                        coda2=coda2->next;
                    }
                }
                //sposta i temp in coda alle rispettive schedule
                MossaSwap(sch_coda1, sch_coda2, arrayM[0], arrayM[1], coda1, coda2, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
                ValutaSchedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                        ris++;
                        //printf("Ho fatto uno swap3 conveniente");
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);
                        ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                        ris++;
                        //printf("Ho fatto uno swap4 conveniente    %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);
                        ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    Schedula::EliminaSchedula(sch_coda1);
                    Schedula::EliminaSchedula(sch_coda2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
        //ciclo swap tra la prima e la terza schedula
        temp2=arraySch[0];
        while(temp2 != NULL){
            temp1=arraySch[2];
            while(temp1 != NULL){
                schedula_di_lavoro1=new TSchedula;
                schedula_di_lavoro2=new TSchedula;
                Schedula::InizializzaSchedula(schedula_di_lavoro1);
                Schedula::InizializzaSchedula(schedula_di_lavoro2);
                sch_coda1=new TSchedula;
                sch_coda2=new TSchedula;
                Schedula::InizializzaSchedula(sch_coda1);
                Schedula::InizializzaSchedula(sch_coda2);
                MossaSpostaInCoda(arraySch[0], arrayM[0], temp2, sch_coda1);
                MossaSpostaInCoda(arraySch[2], arrayM[2], temp1, sch_coda2);
                //coda1, coda2 puntano agli elementi della schedula da swappare, ovvero temp1, temp2 che ora sono stati messi in coda
                coda1=sch_coda1;
                coda2=sch_coda2;
                if(coda1 != NULL){
                    while(coda1->next != NULL){
                        coda1=coda1->next;
                    }
                }
                if(coda2 != NULL){
                    while(coda2->next != NULL){
                        coda2=coda2->next;
                    }
                }
                //sposta i temp in coda alle rispettive schedule
                MossaSwap(sch_coda1, sch_coda2, arrayM[0], arrayM[2], coda1, coda2, schedula_di_lavoro1, schedula_di_lavoro2);
                VnsRicercaLocale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[2], NULL);
                ValutaSchedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
                quaterna_di_lavoro->Feasible=prossimo->feasible;
                quaterna_di_lavoro->Lmax=prossimo->Lmax;
                quaterna_di_lavoro->Cmax=prossimo->Cmax;
                quaterna_di_lavoro->Tardy=prossimo->Tardy;
                if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
                {
                    if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                        ris++;
                        //printf("Ho fatto uno swap5 conveniente    %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);
                        ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else if
                            ((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
                             ((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
                              (quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
                              (quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
                    {
                        quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                        quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                        quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                        quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
                        SostituisciSchedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                        ris++;
                        //printf("Ho fatto uno swap6 conveniente     %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
                        delete(arraySch);
                        delete(arrayM);
                        delete(quaterna_di_lavoro);
                        delete(quaterna_migliore);
                        delete(prossimo);
                        ris+=RicercaLocaleTraMacchineCoda(pM1_sch, pM2_sch, pM3_sch);
                        return 1;
                        break;
                    }
                    else
                    {
                        Schedula::EliminaSchedula(schedula_di_lavoro1);
                        Schedula::EliminaSchedula(schedula_di_lavoro2);
                        Schedula::EliminaSchedula(sch_coda1);
                        Schedula::EliminaSchedula(sch_coda2);
                    }
                }
                else{
                    Schedula::EliminaSchedula(schedula_di_lavoro1);
                    Schedula::EliminaSchedula(schedula_di_lavoro2);
                    Schedula::EliminaSchedula(sch_coda1);
                    Schedula::EliminaSchedula(sch_coda2);
                }
                temp1=temp1->next;
            }
            temp2=temp2->next;
        }
    }
    Schedula::EliminaSchedula(schedula_di_lavoro1);
    Schedula::EliminaSchedula(schedula_di_lavoro2);
    delete(quaterna_migliore);
    delete(quaterna_di_lavoro);
    delete(arraySch);
    delete(arrayM);
    delete(prossimo);
    return ris;
}
