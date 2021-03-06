/***************************************************************************
 *   Copyright (C) 2006 by Michele Ciavotta   *
 *   ciavotta@dia.uniroma3.it   *
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
#include "Globals.h"
#include "Euristiche.h"
#include "Schedula.h"
#include <math.h>
#include <stdlib.h>
using namespace std;


int Heuristics::CalcolaProcTimeMedioArrayJob(void)
{
//     questa funzione calcola il proc. time medio

    int totale = 0;
    int temp;
    for(int i = 0; i < GNum_Job; i++)
        totale += GArray_Job[i].proc_time;

    temp = ceil((float)totale/GNum_Job);
    return temp;
}

void Heuristics::InizializzaPermutazioneMigliore(TJob *pPerm)
{
    for(int i = 0; i < GNum_Job; i++) pPerm[i] = GArray_Job[i];
}

TTipo_New Heuristics::CaricaTempo(TSchedula *pM_sch, TElem *pM)
{
    TTipo_New temp;
    int fine = 0;
    while(pM_sch->next != NULL) pM_sch = pM_sch->next;

    fine = pM_sch->fine;
    while( (pM != NULL) && (fine > pM->inizio) ) pM = pM->next;

    if( (pM == NULL) || (fine < pM->inizio) )
    {
        temp.fine = fine;
        temp.tipo = pM_sch->tipo;
        temp.Camp = pM_sch->index_camp;
        return temp;
    }
    else
    {
        temp.fine = pM->fine;
        temp.tipo = pM_sch->tipo;
        temp.Camp = pM_sch->index_camp;
        return temp;
    }//assumo che nn possano esserci due interruzioni consecutive senza un periodo di attivita' nel mezzo

}
/****************************************************************/
/* questa funzione verifica che minimum non si trovi 		*/
/* all'interno di un periodo di indisponibilita' ed in tal caso */
/* restituisce l'estremo superiore di tale periodo		*/
/****************************************************************/
int Heuristics::TrovaEdgeIndisp(TElem *M,int minimum)
{
    TElem *temp = NULL;
    temp = M;
    while(temp->next != NULL)
    {
        if (temp->inizio > minimum)      return minimum;
        else if (temp->fine >= minimum)  return temp->fine;
        else                             temp = temp->next;
    }
    return minimum;
    // nel caso in cui non ci siano indisponibilita'
    // successive a minimum
}



int Heuristics::AggiungiJobPerm (TJob1 *array_job_locale,int dim_job,TSchedula **schedule_locali)
{

    TTipo_New* tempo = getStartingTimes(schedule_locali);
    int *disponibilita;
    int disp = 0;
    TJob *perm = NULL;
    int i,k,minimum;
    int minimum_alternativo;
    int minimus = 0;
    TElem **indisp = NULL;
    int *deadline_vett = NULL;
    int *duedate_vett = NULL;
    int *else_vett = NULL;
    int *set = NULL;//insieme dei job possibilmente schedulabili (minimi nella duedate o deadline)
    int *tipo_job = NULL;
    int *tipo_macchine = NULL;
    int *campagna_macchine = NULL;
    int **setup_matrix = NULL;
    int **set_matrix = NULL;
    int *setup_vett = NULL;
    int deadline = -1;
    int duedate = -1;
    int nothing = -1;
    int fine=0;
    int j = 0;
    int st_vt = 0;
    int kk;
    int pos_i = 0;
    int pos_k = 0;
    disponibilita = &disp;
    indisp              = new TElem*[GNum_Macchine]; //array indisponibilita'
    deadline_vett       = new int[dim_job];
    duedate_vett        = new int[dim_job];
    else_vett           = new int[dim_job];
    set                 = new int[dim_job];
    tipo_job            = new int[dim_job];
    tipo_macchine       = new int[GNum_Macchine];
    campagna_macchine   = new int[GNum_Macchine];
    setup_matrix        = new int*[dim_job];
    set_matrix          = new int*[dim_job];

    for(i = 0;i < dim_job; i++)
    {
        setup_matrix[i]= new int[GNum_Macchine];
        set_matrix[i]  = new int[GNum_Macchine];
    }
    // 	indisp e' un vettore di puntatori alle liste di indisponibilita' delle macchine
    indisp[0] = GMacch1;
    indisp[1] = GMacch2;
    if(GNum_Macchine == 3) indisp[2] = GMacch3;

    // ___________________________________________________________________________________
    // 	inizializzazione delle strutture dati da utilizzare
    for(i = 0; i < dim_job; i++)
    {
        for(k = 0; k < GNum_Macchine; k++)
        {
            setup_matrix[i][k]= -1;
            set_matrix[i][k]  = -1;
            if(i == 0)
            {
                tipo_macchine[k] = -1;
                campagna_macchine[k] = -1;
            }
        }
        deadline_vett[i]    = -1;
        duedate_vett[i]     = -1;
        else_vett[i]        = -1;
        set[i]              = -1;
        tipo_job[i]         = -1;
    }

    // ___________________________________________________________________________________
    // 	ora conosco i tempi minimi di schedulazione sulle singole macchine
    // 	a questo punto devo solo cercare il minimo di questi tempi e dato questo stabilire il set di job con release date
    // 	anteriore a tale data.

    minimum = getMinimumScheduleTime(tempo);

    //ora conosco il minimo
    //_____________________________________________________________________________________

    // 	devo costruire il set dei job released
    minimum_alternativo = divideJobs(array_job_locale,dim_job,minimum,deadline_vett,duedate_vett,else_vett);
   //ora ho segnato i job che posso schedulare xche' rispettano la release date.

    // _______________________________________________________________________________________

    // 	cerco ora un elemento con deadline != 0;
    k = getElemPosinVett(deadline_vett,dim_job);

    //devo cercare il job con il valore + piccolo di deadline
    minimus = 0;
    if(k >= 0)// se ho trovato almeno un elemento con valore di deadline maggiore di 0
    {
        minimus = getMinInVett(k, deadline_vett,dim_job);
        fine = 1;// flag che mi avverte che posso terminare
        deadline = 1;// segno che ho trovato una deadline
    }
    else// se nn ho trovato nessuna deadline cerco le duedate
    {
        k = getElemPosinVett(duedate_vett,dim_job);

        minimus = 0;
        if(k>=0)// se ho trovato almeno una duedate
        {
            minimus = getMinInVett(k, duedate_vett,dim_job);
            fine = 1;//flag che mi dice che posso terminare la ricerca
            duedate = 1;// ho trovato una duedate
        }
        else //cerco un job privo di duedate e deadline
        {
            k=getElemPosinVett(else_vett,dim_job);
            if(k>=0)//un job qualsiasi va bene
            {
                fine = 1;
                nothing = 1;
            }

        }
    }

    if(fine == 0)//non posso aggiungere job perche' nessun job ha release date inferiore o uguale al minimo tempo disponibile sulle macchine.
    {
        //  per ogni macchina verifico che minimum non si trovi in un periodo di
        // 	indisponibilita', nel caso fosse inserisco in tempi_ind il valore
        // 	dell'estremo superiore di tale periodo
        minimum = updateMachineUnaval(tempo, indisp, minimum_alternativo) ;

        divideJobs(array_job_locale,dim_job,minimum,deadline_vett,duedate_vett,else_vett);

        // come sopra devo individuare il job che deadline o duedate minima
        k = getElemPosinVett(duedate_vett,dim_job);
        minimus = 0;
        if(k >= 0)
        {
            minimus = getMinInVett(k, deadline_vett,dim_job);
            fine = 1;
            deadline = 1;// segno che ho trovato una deadline
        }
        else// se nn ho trovato nessuna deadline cerco le duedate
        {
            k = getElemPosinVett(duedate_vett,dim_job);
            minimus = 0;
            if(k>=0)
            {
                minimus = getMinInVett(k, duedate_vett,dim_job);
                fine = 1;
                duedate = 1;
            }
            else //cerco un job privo di duedate e deadline
            {
                k=getElemPosinVett(else_vett,dim_job);
                if(k>=0)
                {
                    fine = 1;
                    nothing = 1;
                }

            }
        }
        //                 fine adesso dovrebbe essere uguale ad 1
    }
    if(fine==1)// dovrebbe essere sempre vera questa condizione a questo punto
    {
        if (deadline == 1)
        {//minimus indica il valore + basso della deadline
            filterJobs(deadline_vett,minimus,array_job_locale,dim_job,set,tipo_job);
        }
        else if (duedate == 1)
        {
            //minimus indica il valore + basso della duedate
            filterJobs(duedate_vett,minimus,array_job_locale,dim_job,set,tipo_job);
        }
        else if (nothing == 1)
        {
            // 			salvo tutti i job che non hanno ne' duedate ne' deadline
            filterJobs(else_vett,array_job_locale,dim_job,set,tipo_job);
        }
        // _________________________________________________________________________________
        // 		per ogni macchina salvo le informazioni sul tipo di job e sullo
        // 		stado della campagna

        updateTipoCampagna(minimum,tipo_macchine,campagna_macchine,tempo);

        //__________________________________________________________________________________

        for(i=0;i<dim_job;i++)
        {
            for(k=0;k<GNum_Macchine;k++)
            {
                if
                        (
                         (set[i]!=-1)// se il job e' elegibile
                         &&
                         (tipo_macchine[k]!=-1)// se la macchina non e' vuota
                         )
                {
                    if(tipo_job[i] == tipo_macchine[k])// se il job e' dello stesso tipo del precedente
                    {
                        j = 0;
                        for(j=0;j<GNum_Tipi;j++)
                        {
                            if(GArray_Tipi[j].ID==tipo_macchine[k])// cerco la campagna massima
                            {
                                if(campagna_macchine[k] < GArray_Tipi[j].MaxOpCamp)
                                {
                                    // 								eureka! posso shedulare questo job immediatamente
                                    // 								la macchina k-esima verra'assegnata al job

                                    perm = assignJobToPerm(set,i);
                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    VerificaMacchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    Schedula::AggiungiSchedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
                                    kk = set[i];
                                    delete(indisp);
                                    delete(deadline_vett);
                                    delete(duedate_vett);
                                    delete(else_vett);
                                    delete(set);
                                    delete(tipo_job);
                                    delete(tipo_macchine);
                                    delete(campagna_macchine);
                                    for(i=0;i<dim_job;i++)
                                    {
                                        delete(setup_matrix[i]);
                                        delete(set_matrix[i]);

                                    }
                                    delete(setup_matrix);
                                    delete(set_matrix);
                                    return kk;
                                }
                                else
                                {
                                    setup_matrix[i][k]= GCmaj_Matrix[campagna_macchine[k]-1][tipo_job[i]-1];
                                    set_matrix[i][k] = set[i];
                                }
                            }
                        }
                    }
                    else
                    {
                        if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gia' schedulati
                        {
                            // 	nn devo pagare setup
                            st_vt = 0;
                            setup_vett =&st_vt;
                            perm = assignJobToPerm(set,i);
                            VerificaMacchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            Schedula::AggiungiSchedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
                            kk = set[i];
                            delete(indisp);
                            delete(deadline_vett);
                            delete(duedate_vett);
                            delete(else_vett);
                            delete(set);
                            delete(tipo_job);
                            delete(tipo_macchine);
                            delete(campagna_macchine);
                            for(i = 0;i < dim_job; i++)
                            {
                                delete(setup_matrix[i]);
                                delete(set_matrix[i]);
                            }
                            delete(setup_matrix);
                            delete(set_matrix);
                            return kk;
                        }
                        else
                        {
                            setup_matrix[i][k]= GCmaj_Matrix[campagna_macchine[k]-1][tipo_job[i]-1];
                            set_matrix[i][k] = set[i];
                        }
                    }
                }
            }
        }
    }
    else
    {
        exit(0);
    }
    // 	 a questo punto ho la matrice dei setup e devo scegliere quello a costo minimo.
    minMatrix(dim_job,setup_matrix,pos_i,pos_k);
    j=0;

    st_vt = 1;
    setup_vett =&st_vt;
    assignJobToPermWithSetup(set_matrix, pos_i,pos_k);
    VerificaMacchina(schedule_locali[pos_k],indisp[pos_k],disponibilita,setup_vett,0,perm,0);
    Schedula::AggiungiSchedula(schedule_locali[pos_k],perm[0],disponibilita[0],setup_vett[0]);
    kk = set_matrix[pos_i][pos_k];
    delete(indisp);
    delete(deadline_vett);
    delete(duedate_vett);
    delete(else_vett);
    delete(set);
    delete(tipo_job);
    delete(tipo_macchine);
    delete(campagna_macchine);
    for(i=0;i<dim_job;i++)
    {
        delete(setup_matrix[i]);
        delete(set_matrix[i]);
    }
    delete(setup_matrix);
    delete(set_matrix);
    return kk;

}
int Heuristics::AggiungiJobPermDelta (TJob1 *pArray_jobs,int pDimJob,TSchedula **pArraySchedule,int pDelta)
{
    TJob *perm          = NULL;
    TElem **indisp      = NULL;
    TTipo_New* tempo = getStartingTimes(pArraySchedule);
    int *disponibilita  = NULL;
    int disp = 0;
    int *deadline_vett  = NULL;
    int i,k,minimum;
    int *duedate_vett   = NULL;
    int *else_vett      = NULL;
    int deadline        = -1;
    int duedate         = -1;
    int nothing         = -1;
    int minimum_alternativo2;
    int *set_duedate    = NULL;//insieme dei job possibilmente schedulabili (minimi nella duedate o deadline)
    int *set_deadline   = NULL;
    int *set_nothing    = NULL;
    int *tipo_job_duedate   = NULL;
    int *tipo_job_deadline  = NULL;
    int *tipo_job_nothing   = NULL;
    int *tipo_macchine      = NULL;
    int *campagna_macchine  = NULL;
    int **setup_matrix      = NULL;
    int **set_matrix        = NULL;
    int minimum_alternativo = 100000;
    int tempi_ind[3];
    int fine=0;
    int min_deadline;
    int min_duedate;
    int cambio;
    int jj;
    int *setup_vett;
    int st_vt;
    int kk;
    int j1;
    int pos_i = 0;
    int pos_k = 0;

    indisp              = new TElem*[GNum_Macchine];
    disponibilita       = &disp;
    tempo               = new TTipo_New[GNum_Macchine];
    set_duedate         = new int[pDimJob];
    set_deadline        = new int[pDimJob];
    set_nothing         = new int[pDimJob];
    tipo_job_duedate    = new int[pDimJob];
    tipo_job_deadline   = new int[pDimJob];
    tipo_job_nothing    = new int[pDimJob];
    tipo_macchine       = new int[GNum_Macchine];
    campagna_macchine   = new int[GNum_Macchine];
    deadline_vett       = new int[pDimJob];
    duedate_vett        = new int[pDimJob];
    else_vett           = new int[pDimJob];
    setup_matrix        = new int*[pDimJob];
    set_matrix          = new int*[pDimJob];
    for(i = 0;i < pDimJob; i++)
    {
        setup_matrix[i] = new int[GNum_Macchine];
        set_matrix[i]   = new int[GNum_Macchine];
    }

    indisp[0] = GMacch1;
    if(GNum_Macchine >= 2) indisp[1] = GMacch2;
    if(GNum_Macchine == 3) indisp[2] = GMacch3;

    for(i = 0; i < pDimJob; i++)
    {
        for(k = 0; k < GNum_Macchine; k++)
        {
            setup_matrix[i][k]=-1;
            set_matrix[i][k]=-1;
            if(i == 0)
            {
                tipo_macchine[k] = -1;
                campagna_macchine[k] = -1;
            }
        }
        deadline_vett[i]    = -1;
        duedate_vett[i]     = -1;
        else_vett[i]        = -1;
        set_duedate[i]      = -1;
        set_deadline[i]     = -1;
        set_nothing[i]      = -1;
        tipo_job_duedate[i] = -1;
        tipo_job_deadline[i]= -1;
        tipo_job_nothing[i] = -1;
    }

    // 	ora conosco i tempi minimi di schedulazione sulle singole macchine
    // 	a questo punto devo solo cercare il minimo di questi tempi e dato questo stabilire il set di job con release dete
    // 	anteriore a tale data.
    minimum = getMinimumScheduleTime(tempo);

    //ora conosco il minimo
    // 	devo costruire il set dei job released
    minimum_alternativo = divideJobs(pArray_jobs,pDimJob,minimum,deadline_vett,duedate_vett,else_vett);
    // ora ho segnato i job che posso schedulare xche' rispettano la release date.
     // cerco ora un elemento con deadline != 0;

    k = getElemPosinVett(deadline_vett,pDimJob);

    min_deadline = 0;
    if(k >= 0)
    {
        min_deadline =  getMinInVett(k, deadline_vett,pDimJob);
        fine=1;
        deadline = 1;// segno che ho trovato una deadline
    }
    min_duedate=0;
    k = getElemPosinVett(duedate_vett,pDimJob);
    if(k>=0)
    {
        min_duedate = getMinInVett(k, duedate_vett,pDimJob);
        fine=1;
        duedate = 1;
    }
    else //cerco un job privo di duedate e deadline
    {
        k=getElemPosinVett(else_vett,pDimJob);
        if(k>=0)
        {
            fine = 1;
            nothing = 1;
        }

    }
    if(fine == 0)
    {
       minimum_alternativo=updateMachineUnavalVer2(tempo,indisp,minimum_alternativo,minimum);
       divideJobsVer2(pArray_jobs,pDimJob,minimum,deadline_vett,duedate_vett,else_vett);
       
        k=-getElemPosinVett(duedate_vett,pDimJob);
        min_deadline = 0;
        if(k>=0)
        {
            min_deadline = getMinInVett(k, deadline_vett,pDimJob);
            fine=1;
            deadline = 1;// segno che ho trovato una deadline
        }
        min_duedate=0;
        k= getElemPosinVett(duedate_vett,pDimJob);
        if(k>=0)
        {
            min_duedate = getMinInVett(k, duedate_vett,pDimJob);
            fine=1;
            duedate = 1;
        }
        //cerco un job privo di duedate e deadline

        k=getElemPosinVett(else_vett,pDimJob);
        if(k>=0)
        {
            fine = 1;
            nothing = 1;
        }

    }
    //calcolo ora il minimo tra duedate e deadline se min resta uguale a zero significa che devo considerare gli elementi del vettore else
    int minimus =0;
    if(deadline == -1 && duedate==1)
        minimus = min_duedate;
    else if(deadline == 1 && duedate== -1)
        minimus = min_deadline;
    else if (deadline == 1 && duedate==1)
        minimus = min(min_deadline,min_duedate);

    if (deadline == 1)
    {//minimus indica il valore + basso della deadline

        cambio = filterJobsDelta(deadline_vett, minimus,pArray_jobs, pDelta,pDimJob,set_deadline,tipo_job_deadline);
        if(cambio == 0)
        {//se nn ho trovato una deadline in delta
            deadline = -1;
        }
        else
        {
            duedate = -1;//da questo punto in poi do precedenza alle deadline
            nothing = -1;
        }

    }
    if (duedate == 1)
    {
        //minimus indica il valore + basso della deadline
        cambio = filterJobsDelta(duedate_vett, minimus,pArray_jobs, pDelta,pDimJob,set_duedate,tipo_job_duedate);

        if(cambio == 0)
        {//se nn ho trovato una duedate in delta
            duedate = -1;
        }
        else
        {
            deadline = -1;
            nothing = -1;
        }
    }
    if (nothing == 1)
    {
        //minimus indica il valore + basso della deadline
        filterJobs(else_vett,pArray_jobs,pDimJob,set_nothing,tipo_job_nothing);

    }

    updateTipoCampagna(minimum,tipo_macchine,campagna_macchine,tempo);


    if(deadline ==1) //privilegio sempre le deadline se ci sono
    {
        for(i=0;i<pDimJob;i++)
        {
            for(k=0;k<GNum_Macchine;k++)
            {
                if
                        (
                         (set_deadline[i]!=-1)
                         &&
                         (tipo_macchine[k]!=-1)
                         )
                {
                    if(tipo_job_deadline[i] == tipo_macchine[k])
                    {
                        int j = 0;
                        for(j=0;j<GNum_Tipi;j++)
                        {
                            if(GArray_Tipi[j].ID==tipo_macchine[k])
                            {
                                if(campagna_macchine[k]<GArray_Tipi[j].MaxOpCamp)
                                {
                                    // 					eureka posso shedulare questo job immediatamente
                                    // 					la macchina k-esima verra' assegnata al job
                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    perm = assignJobToPerm(set_deadline,i);
                                    VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    Schedula::AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
                                    kk = set_deadline[i];
                                    delete(deadline_vett);
                                    delete(duedate_vett);
                                    delete(else_vett);
                                    for(i=0;i<pDimJob;i++)
                                    {
                                        delete(setup_matrix[i]);
                                        delete(set_matrix[i]);
                                    }
                                    delete(setup_matrix);
                                    delete(set_matrix);
                                    delete(tempo);
                                    delete(indisp);
                                    delete(set_duedate);
                                    delete(set_deadline);
                                    delete(set_nothing);
                                    delete(tipo_job_duedate);
                                    delete(tipo_job_deadline);
                                    delete(tipo_job_nothing);
                                    delete(tipo_macchine);
                                    delete(campagna_macchine);
                                    delete(perm);
                                    return kk;// siccome aggiungo un job alla volta nn serve che io cancelli l'elemento corrispondente nel vettore delle duedate
                                }
                                else
                                {
                                    setup_matrix[i][k]= 	GCmaj_Matrix[campagna_macchine[k]-1][tipo_job_deadline[i]-1];
                                    set_matrix[i][k] = set_deadline[i];
                                }
                            }
                        }
                    }
                    else
                    {
                        if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gia' schedulati
                        {
                            // 						nn devo pagare setup

                            st_vt = 0;
                            setup_vett =&st_vt;
                            perm = assignJobToPerm(set_deadline,i);

                            VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            Schedula::AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
                            kk = set_deadline[i];
                            delete(deadline_vett);
                            delete(duedate_vett);
                            delete(else_vett);
                            for(i=0;i<pDimJob;i++)
                            {
                                delete(setup_matrix[i]);
                                delete(set_matrix[i]);
                            }
                            delete(setup_matrix);
                            delete(set_matrix);
                            delete(tempo);
                            delete(indisp);
                            delete(set_duedate);
                            delete(set_deadline);
                            delete(set_nothing);
                            delete(tipo_job_duedate);
                            delete(tipo_job_deadline);
                            delete(tipo_job_nothing);
                            delete(tipo_macchine);
                            delete(campagna_macchine);
                            delete(perm);
                            return kk;
                        }
                        setup_matrix[i][k]= GCmaj_Matrix[campagna_macchine[k]-1][tipo_job_deadline[i]-1];
                        set_matrix[i][k] = set_deadline[i];
                    }
                }
            }
        }
    }
    else if(duedate == 1) //non ci sono deadline e uso le duedate
    {
        for(i=0;i<pDimJob;i++)
        {
            for(k=0;k<GNum_Macchine;k++)
            {
                if
                        (
                         (set_duedate[i]!=-1)
                         &&
                         (tipo_macchine[k]!=-1)
                         )
                {
                    if(tipo_job_duedate[i] == tipo_macchine[k])
                    {
                        jj = 0;
                        for(jj=0;jj<GNum_Tipi;jj++)
                        {
                            if(GArray_Tipi[jj].ID==tipo_macchine[k])
                            {
                                if(campagna_macchine[k]<GArray_Tipi[jj].MaxOpCamp)
                                {
                                    // 					eureka posso shedulare questo job immediatamente
                                    // 					la macchina k-esima verr��?assegnata al job

                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    perm = assignJobToPerm(set_duedate,i);

                                    VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    Schedula::AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
                                    kk = set_duedate[i];
                                    delete(deadline_vett);
                                    delete(duedate_vett);
                                    delete(else_vett);
                                    for(i=0;i<pDimJob;i++)
                                    {
                                        delete(setup_matrix[i]);
                                        delete(set_matrix[i]);
                                    }
                                    delete(setup_matrix);
                                    delete(set_matrix);
                                    delete(tempo);
                                    delete(indisp);
                                    delete(set_duedate);
                                    delete(set_deadline);
                                    delete(set_nothing);
                                    delete(tipo_job_duedate);
                                    delete(tipo_job_deadline);
                                    delete(tipo_job_nothing);
                                    delete(tipo_macchine);
                                    delete(campagna_macchine);
                                    delete(perm);
                                    return kk;
                                    // siccome aggiungo un job alla volta nn serve che io cancelli l'elemento corrispondente nel vettore delle duedate
                                }
                                else
                                {
                                    setup_matrix[i][k]= 	GCmaj_Matrix[campagna_macchine[k]-1][tipo_job_duedate[i]-1];
                                    set_matrix[i][k] = set_duedate[i];
                                }
                            }
                        }
                    }
                    else
                    {
                        if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gi��?schedulati
                        {
                            // 						nn devo pagare setup
                            //MIA

                            st_vt = 0;
                            setup_vett =&st_vt;
                            perm = assignJobToPerm(set_duedate,i);
                            VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            Schedula::AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
                            kk = set_duedate[i];
                            delete(deadline_vett);
                            delete(duedate_vett);
                            delete(else_vett);
                            for(i=0;i<pDimJob;i++)
                            {
                                delete(setup_matrix[i]);
                                delete(set_matrix[i]);
                            }
                            delete(setup_matrix);
                            delete(set_matrix);
                            delete(tempo);
                            delete(indisp);
                            delete(set_duedate);
                            delete(set_deadline);
                            delete(set_nothing);
                            delete(tipo_job_duedate);
                            delete(tipo_job_deadline);
                            delete(tipo_job_nothing);
                            delete(tipo_macchine);
                            delete(campagna_macchine);
                            delete(perm);
                            return kk;
                        }
                        setup_matrix[i][k]= GCmaj_Matrix[campagna_macchine[k]-1][tipo_job_duedate[i]-1];
                        set_matrix[i][k] = set_duedate[i];
                    }
                }
            }
        }
    }
    else if (nothing == 1)
    {
        for(i=0;i<pDimJob;i++)
        {
            for(k=0;k<GNum_Macchine;k++)
            {
                if
                        (
                         (set_nothing[i]!=-1)
                         &&
                         (tipo_macchine[k]!=-1)
                         )
                {
                    if(tipo_job_nothing[i] == tipo_macchine[k])
                    {
                        jj = 0;
                        for(jj=0;jj<GNum_Tipi;jj++)
                        {
                            if(GArray_Tipi[jj].ID==tipo_macchine[k])
                            {
                                if(campagna_macchine[k]<GArray_Tipi[jj].MaxOpCamp)
                                {
                                    // 								eureka posso shedulare questo job immediatamente
                                    // 								la macchina k-esima verr��?assegnata al job

                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    perm = assignJobToPerm(set_nothing,i);
                                    VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    Schedula::AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
                                    kk = set_nothing[i];
                                    delete(deadline_vett);
                                    delete(duedate_vett);
                                    delete(else_vett);
                                    for(i=0;i<pDimJob;i++)
                                    {
                                        delete(setup_matrix[i]);
                                        delete(set_matrix[i]);
                                    }
                                    delete(setup_matrix);
                                    delete(set_matrix);
                                    delete(tempo);
                                    delete(indisp);
                                    delete(set_duedate);
                                    delete(set_deadline);
                                    delete(set_nothing);
                                    delete(tipo_job_duedate);
                                    delete(tipo_job_deadline);
                                    delete(tipo_job_nothing);
                                    delete(tipo_macchine);
                                    delete(campagna_macchine);
                                    delete(perm);
                                    return kk;
                                    // siccome aggiungo un job alla volta nn serve che io cancelli l'elemento corrispondente nel vettore delle nothing
                                }
                                else
                                {
                                    setup_matrix[i][k]= 	GCmaj_Matrix[campagna_macchine[k]-1][tipo_job_nothing[i]-1];
                                    set_matrix[i][k] = set_nothing[i];
                                }
                            }
                        }
                    }
                    else
                    {
                        if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gi��?schedulati
                        {
                            // 						nn devo pagare setup

                            st_vt = 0;
                            setup_vett =&st_vt;
                            perm = assignJobToPerm(set_nothing,i);
                            VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            Schedula::AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
                            kk = set_nothing[i];
                            delete(deadline_vett);
                            delete(duedate_vett);
                            delete(else_vett);
                            for(i=0;i<pDimJob;i++)
                            {
                                delete(setup_matrix[i]);
                                delete(set_matrix[i]);
                            }
                            delete(setup_matrix);
                            delete(set_matrix);
                            delete(tempo);
                            delete(indisp);
                            delete(set_duedate);
                            delete(set_deadline);
                            delete(set_nothing);
                            delete(tipo_job_duedate);
                            delete(tipo_job_deadline);
                            delete(tipo_job_nothing);
                            delete(tipo_macchine);
                            delete(campagna_macchine);
                            delete(perm);
                            return kk;
                        }
                        setup_matrix[i][k]= GCmaj_Matrix[campagna_macchine[k]-1][tipo_job_nothing[i]-1];
                        set_matrix[i][k] = set_nothing[i];
                    }
                }
            }
        }
    }

    // 	 a questo punto ho la matrice dei setup e devo scegliere quello a costo minimo.
    minMatrix(pDimJob,setup_matrix,pos_i,pos_k);

    st_vt = 1;
    setup_vett =&st_vt;
    assignJobToPermWithSetup(set_matrix, pos_i,pos_k);
    VerificaMacchina(pArraySchedule[pos_k],indisp[pos_k],disponibilita,setup_vett,0,perm,0);
    Schedula::AggiungiSchedula(pArraySchedule[pos_k],perm[0],disponibilita[0],setup_vett[0]);
    kk = set_matrix[pos_i][pos_k];
    delete(deadline_vett);
    delete(duedate_vett);
    delete(else_vett);
    for(i=0;i<pDimJob;i++)
    {
        delete(setup_matrix[i]);
        delete(set_matrix[i]);
    }
    delete(setup_matrix);
    delete(set_matrix);
    delete(tempo);
    delete(indisp);
    delete(set_duedate);
    delete(set_deadline);
    delete(set_nothing);
    delete(tipo_job_duedate);
    delete(tipo_job_deadline);
    delete(tipo_job_nothing);
    delete(tipo_macchine);
    delete(campagna_macchine);
    delete(perm);
    return kk;
}

//********************************************************************************************
//********************************************************************************************
TJob *Heuristics::PermutazioneEdd_1Tipo ( TJob *pArray_job_attuale,int pDim_job )
{
    //questa funzione costruira' una nuova perm di job utilizzando la regola EDD in particolare
    // verranno schedulati prima i job con deadline + vicina e poi quelli che hanno sono duedate
    int k = 0;

    int *vett_deadline  = new int[pDim_job];
    int *vett_duedate   = new int[pDim_job];
    int *vett_nothing   = new int[pDim_job];

    int i = 0;
    int pos = 0;
    int value = 0;
    int fine;

    TJob *permutation = new TJob[pDim_job];

    for ( k = 0; k < pDim_job; k++ )
    {//inizializzo
        vett_deadline[k] = 0 ;
        vett_duedate[k] = 0;
        vett_nothing[k] = 0;
    }
    for ( k = 0; k < pDim_job; k++ )
    {
        if ( pArray_job_attuale[k].deadline > 0 )
            vett_deadline[k] = pArray_job_attuale[k].deadline;

        if
                (
                 ( pArray_job_attuale[k].deadline == 0 )
                 &&
                 ( pArray_job_attuale[k].duedate > 0 )
                 )
        {
            vett_duedate[k] = pArray_job_attuale[k].duedate;
        }
        if
                (
                 ( pArray_job_attuale[k].deadline == 0 )
                 &&
                 ( pArray_job_attuale[k].duedate == 0 )
                 )
        {
            vett_nothing[k] = 1;
        }
    }

    fine = 0;
    while ( !fine )
    {
        for ( k = 0; k < pDim_job; k++ )
        {
            if ( vett_deadline[k] != 0 )
                break;//	trovo la prima deadline

            if ( k == pDim_job-1 )
            {//vuol dire che nn ci sono + job da schedulare di questo tipo
                fine = 1;
                break;
            }
        }
        if ( fine )
            break;

        pos = k;
        value = vett_deadline[k];
        for ( k = 0; k < pDim_job; k++ )
        {
            if
                    (
                     ( vett_deadline[k] != 0 )
                     &&
                     ( vett_deadline[k] < value )
                     )
            {
                value = vett_deadline[k];
                pos = k;
            }
        }
        vett_deadline[pos] = 0;
        permutation[i] = pArray_job_attuale[pos];
        i++;
    }

    fine = 0;
    while ( !fine )
    {
        for ( k = 0; k < pDim_job; k++ )
        {
            if ( vett_duedate[k] != 0 )
                break;//	trovo la prima duedatee

            if ( k == pDim_job-1 )
            {
                //vuol dire che nn ci sono + job da schedulare di questo tipo
                fine = 1;
                break;
            }
        }
        if ( fine )
            break;

        pos = k;
        value = vett_duedate[k];
        for ( k = 0; k < pDim_job; k++ )
        {
            if
                    (
                     ( vett_duedate[k] != 0 )
                     &&
                     ( vett_duedate[k] < value )
                     )
            {
                value = vett_duedate[k];
                pos = k;
            }
        }
        vett_duedate[pos] = 0;
        permutation[i] = pArray_job_attuale[pos];
        i++;
    }
    for ( k = 0; k < pDim_job; k++ )
    {
        if ( vett_nothing[k] == 1 )
        {
            permutation[i] = pArray_job_attuale[k];
            i++;
        }
    }

    /*Elimino i vettori utilizzati*/
    delete[] vett_deadline;
    delete[] vett_duedate;
    delete[] vett_nothing;

    return permutation;
}

TJob *Heuristics::PermutazioneEdd_2Tipo ( TJob *pArray_Job_Attuale,int pDim_Job )
{
    //questa funzione costruira' una nuova perm di job utilizzando la regola EDD in particolare
    // verranno schedulati prima i job con deadline o duedate + vicina
    int k = 0;
    int *vett_max_dd_dl = new int[pDim_Job];
    int i = 0;
    int fine = 0;
    TJob *perm1 = new TJob[pDim_Job];
    for ( k = 0;k < pDim_Job; k++ )//inizializzo
        vett_max_dd_dl[k] = 0 ;

    for ( k = 0;k < pDim_Job; k++ )
    {
        if ( pArray_Job_Attuale[k].duedate!=0 || pArray_Job_Attuale[k].deadline !=0 )
        {
            if (
                    (( pArray_Job_Attuale[k].deadline !=0 )
                        &&
                    ( pArray_Job_Attuale[k].duedate !=0 ))
                    ||
                    ( pArray_Job_Attuale[k].deadline == 0 )

                    )
            {
                vett_max_dd_dl[k] = pArray_Job_Attuale[k].duedate;
            }
            else
                vett_max_dd_dl[k] = pArray_Job_Attuale[k].deadline;
        }
    }
    i = 0;
    while ( !fine )
    {
        int cambio = 0 ;
        for ( k=0;k<pDim_Job;k++ )
        {
            if ( ( vett_max_dd_dl[k]!=0 ) && ( vett_max_dd_dl[k]!=-1 ) )
                break;

            if ( k == ( pDim_Job-1 ) )
                fine = 1;
        }
        if ( fine )
            break;

        cambio = k;
        for ( k=1;k<pDim_Job;k++ ) //trovo il minimo
        {
            if
                    (
                     (
                         ( vett_max_dd_dl[cambio]
                          >
                          vett_max_dd_dl[k] )
                         )
                     &&
                     ( vett_max_dd_dl[k]!=-1 )
                     &&
                     ( vett_max_dd_dl[k]!=0 )
                     )
            {
                cambio = k;
            }
        }
        perm1[i] = pArray_Job_Attuale[cambio];
        i++;
        vett_max_dd_dl[cambio] = -1;
    }
    fine = 0;
    while ( !fine )
    {
        for ( k=0;k<pDim_Job;k++ )
        {

            if ( vett_max_dd_dl[k]==0 )
                break;

            if ( k == pDim_Job-1 )
                fine=1;
        }

        if ( fine ) break;

        perm1[i] = pArray_Job_Attuale[k];
        i++;
        vett_max_dd_dl[k] = -1;
    }
    delete[] vett_max_dd_dl;
    return perm1;
}
TJob *Heuristics::PermutazioneBase(TJob *pArray_Job_Attuale, int pDimJob)
{//questa e' l'euristica di base presentata nella tesi di Ritota
    int i= 0;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    initializeLocalSchedules(M1_sch_locale,M2_sch_locale,M3_sch_locale);
    // 	schedule_locali a' un vettore di puntatori alle schedule locali
    TSchedula **schedule_locali = initializeArrayLocalSchedules(M1_sch_locale,M2_sch_locale,M3_sch_locale);
    TJob1 *array_job_locale = copyArrayJob(pArray_Job_Attuale, pDimJob);
    TJob * perm1 =  new TJob[pDimJob];
    int k = 0;

    //__________________________________________________________________________________
    // 	Qui comincia la parte principale dell'algoritmo
    i=0;
    while (i<pDimJob)// fin quando non ho schedulato tutti i job
    {
        temp = AggiungiJobPerm(array_job_locale,pDimJob,schedule_locali);

        k = 0;
        while(k < pDimJob)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID         =	array_job_locale[k].ID;
                perm1[i].tipo       =	array_job_locale[k].tipo;
                perm1[i].proc_time  =   array_job_locale[k].proc_time;
                perm1[i].duedate    =   array_job_locale[k].duedate;
                perm1[i].deadline   =   array_job_locale[k].deadline;
                perm1[i].priority   =   array_job_locale[k].priority;
                perm1[i].rel_time   =   array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
                break;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;
    }
    // ____________________________________________________________________________________

    //devo ora liberare lo spazio delle schedule locali
    eliminaSchedule(M1_sch_locale,M2_sch_locale, M3_sch_locale);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;
}
TJob *Heuristics::PermutazioneDelta_10(TJob *pArray_Job_Attuale, int pDimJob)
{//questa e' un'euristica basata sul concetto di Delta
    int i= 0;
    int Delta = 10;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    initializeLocalSchedules(M1_sch_locale,M2_sch_locale,M3_sch_locale);

    TSchedula **schedule_locali = initializeArrayLocalSchedules(M1_sch_locale,M2_sch_locale,M3_sch_locale);
    TJob1 *array_job_locale = copyArrayJob(pArray_Job_Attuale, pDimJob);
    TJob * perm1 =  new TJob[pDimJob];
    int k = 0;

    i = 0;
    while (i<pDimJob)
    {
        temp = AggiungiJobPermDelta (array_job_locale,pDimJob,schedule_locali,Delta);
        k = 0;
        while(k<pDimJob)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
	//devo ora liberare lo spazio delle schedule locali
    eliminaSchedule(M1_sch_locale,M2_sch_locale, M3_sch_locale);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_15(TJob *pArray_Job_Attuale, int pDimJob)
{//questa e' un'euristica basata sul concetto di Delta
    int i= 0;
    int Delta = 15;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    initializeLocalSchedules(M1_sch_locale,M2_sch_locale,M3_sch_locale);

    TSchedula **schedule_locali = initializeArrayLocalSchedules(M1_sch_locale,M2_sch_locale,M3_sch_locale);
    TJob1 *array_job_locale = copyArrayJob(pArray_Job_Attuale, pDimJob);
    TJob * perm1 =  new TJob[pDimJob];
    int k = 0;

    i=0;
    while (i<pDimJob)
    {
        temp = AggiungiJobPermDelta (array_job_locale,pDimJob,schedule_locali,Delta);
        k = 0;
        while(k<pDimJob)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    eliminaSchedule(M1_sch_locale,M2_sch_locale, M3_sch_locale);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_20(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 20;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)

        schedule_locali=new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=  0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,
                                     dim_job,
                                     schedule_locali,
                                     Delta);
        k = 0;
        while(k < dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);

    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_25(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 25;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale = new TJob1[dim_job];

    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali=new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::Permutazione_delta_30(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 30;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali=new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_35(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 35;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale = new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_40(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 40;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_50(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 50;
    int temp = 0;

    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::Permutazione_delta_14ore(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale;
    int i= 0;
    int Delta = 840;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_7ore(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 420;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale = new TJob1[dim_job];

    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali=new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
 	k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneFittizia(TJob *array_job_attuale, int dim_job)
{//questa euristica costruisce la permutazione in base al proc_time del job
    TJob *perm1;
    perm1 = new TJob[dim_job];
    int i;
    if(dim_job == GNum_Job)
    {
        for(i = 0; i < dim_job; i++)
        {
            perm1[i] = array_job_attuale[i];
            GBest_Perm[i] = array_job_attuale[i];
        }

    }
    else
    {//copio una parte della migliore perturbazione trovata
        for(i = 0; i < dim_job;i++)
            perm1[i] = GBest_Perm[GNum_Job-dim_job+i];
    }
    return perm1;
}
TJob *Heuristics::PermutazioneDelta_3(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i = 0;
    int Delta = 3;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale = new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_5(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 5;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale = new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_2(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 2;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale = new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali=new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =	array_job_locale[k].ID;
                perm1[i].tipo =	array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
	i++;	
    }
    //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}

/*Euristiche di tipo LLF con o senza delta*/
TJob *Heuristics::PermutazioneLLFDeltaBasica(TJob *pArray_job_attuale, int pDim_job,int pDelta)
{

    TElem **vett_indisp = new TElem*[GNum_Macchine];
    int i,j,done_tot = 0;

    TJob *perm = new TJob;

    int *vett_max_Lat   = new int[pDim_job];
    int **disponibilita = new int*[pDim_job];
    int **vett_Lat      = new int*[pDim_job];//contiene il valore della lateness per ogni macchina
    int **vett_End      = new int*[pDim_job];//contiene il valore dell'ending time per ogni macchina
    int **vett_setup    = new int*[pDim_job];
    int **scelti        = new int*[pDim_job];

    for(i=0;i<pDim_job;i++)
    {
        disponibilita[i]    = new int[GNum_Macchine];
        vett_Lat[i]         = new int[GNum_Macchine];
        vett_End[i]         = new int[GNum_Macchine];
        vett_setup[i]       = new int[GNum_Macchine];
        scelti[i]           = new int[3]; // questa matrice e' diversa ha sempre 3 valori
    }


    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;

    TSchedula **schedule_locali = new TSchedula*[GNum_Macchine];

    TJob1 *array_job_locale = NULL;

    TJob * perm1    = new TJob[pDim_job];
    array_job_locale= new TJob1[pDim_job];

    //inizializzo le schedule locali
    {
        M1_sch_locale = new TSchedula;
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine>=2)
        {
            M2_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(GNum_Macchine == 1)
    {
        vett_indisp[0] = GMacch1;
    }
    else if(GNum_Macchine == 2)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
    }
    else if(GNum_Macchine == 3)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
        vett_indisp[2] = GMacch3;
    }

    for(i = 0; i < pDim_job; i++)
    {
        array_job_locale[i].ID          = pArray_job_attuale[i].ID;
        array_job_locale[i].tipo        = pArray_job_attuale[i].tipo;
        array_job_locale[i].proc_time   = pArray_job_attuale[i].proc_time;
        array_job_locale[i].duedate     = pArray_job_attuale[i].duedate;
        array_job_locale[i].deadline    = pArray_job_attuale[i].deadline;
        array_job_locale[i].priority    = pArray_job_attuale[i].priority;
        array_job_locale[i].rel_time    = pArray_job_attuale[i].rel_time;
        array_job_locale[i].adatto      = 0;
        array_job_locale[i].schedulato  = 0;
    }

    int kk = 0;
    while(done_tot == 0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < pDim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn e' schedulato gia'
            {
                perm[0].ID          = pArray_job_attuale[i].ID;
                perm[0].tipo        = pArray_job_attuale[i].tipo;
                perm[0].proc_time   = pArray_job_attuale[i].proc_time;
                perm[0].duedate     = pArray_job_attuale[i].deadline;
                perm[0].priority    = pArray_job_attuale[i].priority;
                perm[0].rel_time    = pArray_job_attuale[i].rel_time;

                for(j = 0; j < GNum_Macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    VerificaMacchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + pArray_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];

                    if
                            (
                             (
                                 (array_job_locale[i].duedate != 0)
                                 &&
                                 (array_job_locale[i].rel_time == 0)
                                 )
                             ||
                             (
                                 (array_job_locale[i].duedate != 0)
                                 &&
                                 (disponibilita[i][j] >= array_job_locale[i].rel_time)
                                 )
                             )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if
                            (
                             (array_job_locale[i].duedate == 0)
                             &&
                             (array_job_locale[i].rel_time <= disponibilita[i][j])
                             )
                    {
                        vett_Lat[i][j] = -63000;
                        //indica che nn c'e' duedate e quindi vanno schedulati per ultimi
                    }
                    else
                        vett_Lat[i][j] = -64000;
                        //la release date nn e' soddisfatta
                }
            }
            else
            {
                for(j = 0; j < GNum_Macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
                    //segno i job che nn sono disponibili
                }
            }
        }
        // 	devo scegliere il job da schedulare
        // 	per ogni job scelgo la macchina con Ending time + basso.

        for(i = 0; i < pDim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < GNum_Macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < pDim_job; i++)
        {
            if(max < scelti[i][0])
                max = scelti[i][0];
        }

        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
            done_tot = 1;
        else
        {

            int totale = 0;
            for(i = 0; i < pDim_job; i++)//inizializzo
                vett_max_Lat[i] = 0;

            for(i = 0; i < pDim_job; i++)
            {
                if(max - pDelta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }

            // la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < pDim_job; i++)
            {

                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }

            if(totale > 1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < pDim_job; i++)
                {

                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }
            }
            /*  la coppia i j mi dice quale job schedulare e su quale macchina
                    in realta' costruiamo solo la sequenza di job e nn possiamo essere sicuri che
                    che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe
                    essere cosi'*/
            array_job_locale[coppia[0]].schedulato = 1;

            perm1[kk]           = pArray_job_attuale[coppia[0]];
            perm[0].ID          = pArray_job_attuale[coppia[0]].ID;
            perm[0].tipo        = pArray_job_attuale[coppia[0]].tipo;
            perm[0].proc_time   = pArray_job_attuale[coppia[0]].proc_time;
            perm[0].duedate     = pArray_job_attuale[coppia[0]].deadline;
            perm[0].priority    = pArray_job_attuale[coppia[0]].priority;
            perm[0].rel_time    = pArray_job_attuale[coppia[0]].rel_time;
            Schedula::AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }

    /*Elimino le schedule locali*/
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    for(i = 0;i < pDim_job;i++)
    {
        delete[] disponibilita[i];
        delete[] vett_Lat[i];
        delete[] vett_End[i];
        delete[] vett_setup[i];
        delete[] scelti[i];
    }
    delete[] vett_max_Lat;
    delete[] scelti;
    delete[] disponibilita;
    delete[] perm;
    delete[] vett_indisp;
    delete[] vett_Lat;
    delete[] vett_End;
    delete[] vett_setup;
    delete[] schedule_locali;
    delete[] array_job_locale;
    return perm1;
}
TJob *Heuristics::PermutazioneLLF(TJob *pArray_job_attuale, int pDim_job)
{
    // euristica LLF largest Lateness First
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso

    TElem **vett_indisp = new TElem*[GNum_Macchine];

    int *vett_max_Lat = NULL;

    int i,j,done_tot = 0;

    TJob *perm = new TJob;

    vett_max_Lat = new int[pDim_job];

    // i prossimi sono array di array.
    int **disponibilita   = new int*[pDim_job];
    int **vett_Lat        = new int*[pDim_job];/*contiene il valore della lateness per ogni macchina*/
    int **vett_End        = new int*[pDim_job];//contiene il valore dell'ending time per ogni macchina
    int **vett_setup      = new int*[pDim_job];
    int **scelti          = new int*[pDim_job];

    for(i = 0; i < pDim_job; i++)
    {
        disponibilita[i]    = new int[GNum_Macchine];
        vett_Lat[i]         = new int[GNum_Macchine];
        vett_End[i]         = new int[GNum_Macchine];
        vett_setup[i]       = new int[GNum_Macchine];
        scelti[i]           = new int[3]; /*sempre immagazina 3 valori non ?come gli altri array*/
    }

    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;

    TSchedula **schedule_locali = new TSchedula*[GNum_Macchine];

    TJob *perm1 = new TJob[pDim_job];
    TJob1 *array_job_locale = new TJob1[pDim_job];

    //inizializzo le schedule locali a partire dalle schedule globali

    {
        M1_sch_locale = new TSchedula;
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            M2_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }

    if(GNum_Macchine == 1)
    {
        vett_indisp[0] = GMacch1;
    }
    else if(GNum_Macchine == 2)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
    }
    else if(GNum_Macchine == 3)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
        vett_indisp[2] = GMacch3;
    }

    for(i = 0; i < pDim_job; i++)
    {
        array_job_locale[i].ID          = pArray_job_attuale[i].ID;
        array_job_locale[i].tipo        = pArray_job_attuale[i].tipo;
        array_job_locale[i].proc_time   = pArray_job_attuale[i].proc_time;
        array_job_locale[i].duedate     = pArray_job_attuale[i].duedate;
        array_job_locale[i].deadline    = pArray_job_attuale[i].deadline;
        array_job_locale[i].priority    = pArray_job_attuale[i].priority;
        array_job_locale[i].rel_time    = pArray_job_attuale[i].rel_time;
        array_job_locale[i].adatto      = 0;
        array_job_locale[i].schedulato  = 0;
    }

    int kk = 0;

    while(done_tot == 0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < pDim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn e' schedulato gia'?
            {
                perm[0].ID= pArray_job_attuale[i].ID;
                perm[0].tipo= pArray_job_attuale[i].tipo;
                perm[0].proc_time= pArray_job_attuale[i].proc_time;
                perm[0].duedate= pArray_job_attuale[i].deadline;
                perm[0].priority= pArray_job_attuale[i].priority;
                perm[0].rel_time= pArray_job_attuale[i].rel_time;

                for(j = 0; j < GNum_Macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    VerificaMacchina(schedule_locali[j],
                                      vett_indisp[j],
                                      &disponibilita[i][j],
                                      setup_vett,0,perm,0);

                    vett_End[i][j] = disponibilita[i][j] + pArray_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                            (
                             (
                                 (array_job_locale[i].duedate != 0)
                                 &&
                                 (array_job_locale[i].rel_time ==0)
                                 )
                             ||
                             (
                                 (array_job_locale[i].duedate != 0)
                                 &&
                                 (disponibilita[i][j] >= array_job_locale[i].rel_time)
                                 )
                             )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if
                            (
                             (array_job_locale[i].duedate == 0)
                             &&
                             (array_job_locale[i].rel_time <= disponibilita[i][j])
                             )
                    {
                        vett_Lat[i][j] = -63000;
                        //indica che nn c'e' duedate e quindi vanno schedulati per ultimi
                    }
                    else vett_Lat[i][j] = -64000;
                        //la release date nn e' soddisfatta

                }
            }
            else
            {
                for(j = 0; j < GNum_Macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
                    //segno i job che nn sono disponibili
                }
            }
        }
        /* 	devo scegliere il job da schedulare
                        per ogni job scelgo la macchina con Ending time + basso.*/

        for(i = 0; i < pDim_job; i++)
        {
            scelti[i][0] = vett_Lat[i][0];

            int min_C = vett_End[i][0];

            scelti[i][2] = min_C;
            scelti[i][1] = 0;

            for(j = 0; j < GNum_Macchine; j++)
            {
                if(min_C > vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];

                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;

        for(i = 0; i < pDim_job; i++)
            if(max < scelti[i][0])
                max = scelti[i][0];


        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
            done_tot = 1;
        else
        {
            int totale = 0;
            for(i = 0; i < pDim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            }

            for(i = 0; i < pDim_job; i++)
            {
                if(max == scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }

            // la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < pDim_job; i++)
            {

                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }

            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < pDim_job; i++)
                {

                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }
            }
            /*la coppia i j mi dice quale job schedulare e su quale macchina
             *in realta' costruiamo solo la sequenza di job e nn possiamo essere sicuri che
             *che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe
             *essere cosi?*/
            array_job_locale[coppia[0]].schedulato = 1;

            perm1[kk] = pArray_job_attuale[coppia[0]];
            perm[0].ID = pArray_job_attuale[coppia[0]].ID;
            perm[0].tipo = pArray_job_attuale[coppia[0]].tipo;
            perm[0].proc_time = pArray_job_attuale[coppia[0]].proc_time;
            perm[0].duedate = pArray_job_attuale[coppia[0]].deadline;
            perm[0].priority = pArray_job_attuale[coppia[0]].priority;
            perm[0].rel_time = pArray_job_attuale[coppia[0]].rel_time;
            Schedula::AggiungiSchedula(schedule_locali[coppia[1]],
                              perm[0],
                              disponibilita[coppia[0]][coppia[1]],
                              vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }


    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);


    for(i=0;i< pDim_job;i++)
    {
        delete[] disponibilita[i];
        delete[] vett_Lat[i];
        delete[] vett_End[i];
        delete[] vett_setup[i];
        delete[] scelti[i];
    }

    delete[] vett_max_Lat;
    delete[] scelti;
    delete[] disponibilita;
    delete perm;
    delete[] vett_indisp;
    delete[] vett_Lat;
    delete[] vett_End;
    delete[] vett_setup;
    delete[] schedule_locali;
    delete[] array_job_locale;
    return perm1;
}
TJob *Heuristics::PermutazioneLLFDelta_2(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 2;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *Heuristics::PermutazioneLLFDelta_5(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 5;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *Heuristics::PermutazioneLLFDelta_10(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 10;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *Heuristics::PermutazioneLLFDelta_7ore(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 420;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *Heuristics::PermutazioneLLFdelta_14ore(TJob *pArray_Job_Attuale,int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 840;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);

}
TJob *Heuristics::PermutazioneLLFDelta_3ProcMedio(TJob *array_job_attuale, int dim_job)
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si scegliera'il job con ending time + basso
    int delta = 0;
    TElem **vett_indisp = NULL;
    vett_indisp = new TElem*[GNum_Macchine];
    int **vett_Lat = NULL;//contiene il valore della lateness per ogni macchina
    int **vett_End = NULL;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup = NULL;

    int i,j,done_tot = 0;

    TJob *perm = NULL;
    perm = new TJob;
    int **disponibilita = NULL;
    int **scelti        = NULL;
    int *vett_max_Lat =  NULL;
    delta = 3*Heuristics::CalcolaProcTimeMedioArrayJob();
    vett_max_Lat    = new int[dim_job];
    disponibilita   = new int*[dim_job];
    vett_Lat        = new int*[dim_job];
    vett_End        = new int*[dim_job];
    vett_setup      = new int*[dim_job];
    scelti          = new int*[dim_job];

    for(i = 0;i < dim_job; i++)
    {
        disponibilita[i]    = new int[GNum_Macchine];
        vett_Lat[i]         = new int[GNum_Macchine];
        vett_End[i]         = new int[GNum_Macchine];
        vett_setup[i]       = new int[GNum_Macchine];
        scelti[i]           = new int[GNum_Macchine];
    }
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    schedule_locali = new TSchedula*[GNum_Macchine];
    TJob * perm1 = NULL;
    TJob1 *array_job_locale = NULL;

    perm1 = new TJob[dim_job];
    array_job_locale=new TJob1[dim_job];
    {//inizializzo le schedule locali
        M1_sch_locale = new TSchedula;
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine>=2)
        {
            M2_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(GNum_Macchine   == 1)
        vett_indisp[0] = GMacch1;
    if(GNum_Macchine == 2)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
    }
    if(GNum_Macchine == 3)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
        vett_indisp[2] = GMacch3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?
            {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < GNum_Macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    VerificaMacchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (

                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi
                    }
                    else
                        vett_Lat[i][j] = -64000;
               //la release date nn e' soddisfatta
                }
            }
            else
            {
                for(j = 0; j < GNum_Macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < GNum_Macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
                max = scelti[i][0];

        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
            done_tot = 1;
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            }
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }

//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {

                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }

            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {

                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina
//          in realta' costruiamo solo la sequenza di job e nn possiamo essere sicuri che
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe
//          essere cosi'
            array_job_locale[coppia[0]].schedulato = 1;

            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            Schedula::AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    for(i = 0;i < dim_job;i++)
    {
        delete(disponibilita[i]);
        delete(vett_Lat[i]);
        delete(vett_End[i]);
        delete(vett_setup[i]);
        delete(scelti[i]);
    }
    delete(vett_max_Lat);
    delete(scelti);
    delete(disponibilita);
    delete(perm);
    delete(vett_indisp);
    delete(vett_Lat);
    delete(vett_End);
    delete(vett_setup);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneLLFDeltaProcMedio(TJob *array_job_attuale, int dim_job)
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 0;
    TElem **vett_indisp = NULL;
    vett_indisp = new TElem*[GNum_Macchine];
    int **vett_Lat = NULL;//contiene il valore della lateness per ogni macchina
    int **vett_End = NULL;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup = NULL;

    int i,j,done_tot = 0;
    TJob *perm = NULL;
    perm = new TJob;
    int **disponibilita = NULL;
    int **scelti = NULL;
    int *vett_max_Lat = NULL;
    delta = Heuristics::CalcolaProcTimeMedioArrayJob();
    vett_max_Lat    = new int[dim_job];
    disponibilita   = new int*[dim_job];
    vett_Lat        = new int*[dim_job];
    vett_End        = new int*[dim_job];
    vett_setup      = new int*[dim_job];
    scelti          = new int*[dim_job];

    for(i = 0; i < dim_job; i++)
    {
        disponibilita[i]    = new int[GNum_Macchine];
        vett_Lat[i]         = new int[GNum_Macchine];
        vett_End[i]         = new int[GNum_Macchine];
        vett_setup[i]       = new int[GNum_Macchine];
        scelti[i]           = new int[GNum_Macchine];
    }
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    schedule_locali = new TSchedula*[GNum_Macchine];
    TJob * perm1 = NULL;
    TJob1 *array_job_locale = NULL;

    perm1 = new TJob[dim_job];
    array_job_locale = new TJob1[dim_job];
    {//inizializzo le schedule locali
        M1_sch_locale = new TSchedula;
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            M2_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(GNum_Macchine   == 1)
        vett_indisp[0] = GMacch1;
    if(GNum_Macchine == 2)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
    }
    if(GNum_Macchine == 3)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
        vett_indisp[2] = GMacch3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk = 0;
    while(done_tot == 0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?
            {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < GNum_Macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    VerificaMacchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (

                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'e' duedate e quindi vanno schedulati per ultimi
                    }
                    else
                    {
                        vett_Lat[i][j] = -64000;
               //la release date nn e' ?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < GNum_Macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < GNum_Macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
                max = scelti[i][0];
        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
            done_tot = 1;
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
                vett_max_Lat[i] = 0;

            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }

//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {

                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }

            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {

                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina
//          in realta' costruiamo solo la sequenza di job e nn possiamo essere sicuri che
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe
//          essere cosi

            array_job_locale[coppia[0]].schedulato = 1;
            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            Schedula::AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    for(i = 0;i < dim_job;i++)
    {
        delete(disponibilita[i]);
        delete(vett_Lat[i]);
        delete(vett_End[i]);
        delete(vett_setup[i]);
        delete(scelti[i]);
    }
    delete(vett_max_Lat);
    delete(scelti);
    delete(disponibilita);
    delete(perm);
    delete(vett_indisp);
    delete(vett_Lat);
    delete(vett_End);
    delete(vett_setup);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneLLFDeltaMezzoProcMedio(TJob *array_job_attuale, int dim_job)
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 0;
    TElem **vett_indisp = NULL;
    vett_indisp = new TElem*[GNum_Macchine];
    int **vett_Lat = NULL;//contiene il valore della lateness per ogni macchina
    int **vett_End = NULL;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup = NULL;

    int i,j,done_tot = 0;
    TJob *perm = NULL;
    perm = new TJob;
    int **disponibilita = NULL;
    int **scelti        = NULL;
    int *vett_max_Lat   = NULL;
    delta = ceil((float)Heuristics::CalcolaProcTimeMedioArrayJob()/2);
    vett_max_Lat    = new int[dim_job];
    disponibilita   = new int*[dim_job];
    vett_Lat        = new int*[dim_job];
    vett_End        = new int*[dim_job];
    vett_setup      = new int*[dim_job];
    scelti          = new int*[dim_job];

    for(i = 0;i < dim_job; i++)
    {
        disponibilita[i]    = new int[GNum_Macchine];
        vett_Lat[i]         = new int[GNum_Macchine];
        vett_End[i]         = new int[GNum_Macchine];
        vett_setup[i]       = new int[GNum_Macchine];
        scelti[i]           = new int[GNum_Macchine];
    }
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    schedule_locali = new TSchedula*[GNum_Macchine];
    TJob * perm1 = NULL;
    TJob1 *array_job_locale = NULL;

    perm1 = new TJob[dim_job];
    array_job_locale=new TJob1[dim_job];
    {//inizializzo le schedule locali
        M1_sch_locale = new TSchedula;
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine>=2)
        {
            M2_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(GNum_Macchine   == 1)
        vett_indisp[0] = GMacch1;
    if(GNum_Macchine == 2)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
    }
    if(GNum_Macchine == 3)
    {
        vett_indisp[0] = GMacch1;
        vett_indisp[1] = GMacch2;
        vett_indisp[2] = GMacch3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett = NULL;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?
            {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < GNum_Macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    VerificaMacchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (

                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi
                    }
                    else
                    {
                        vett_Lat[i][j] = -64000;
               //la release date nn ��?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < GNum_Macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < GNum_Macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
                max = scelti[i][0];

        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
            done_tot = 1;
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
                vett_max_Lat[i] = 0;
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }

//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {

                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }

            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {

                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina
//          in realta' costruiamo solo la sequenza di job e nn possiamo essere sicuri che
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe
//          essere cosi

             array_job_locale[coppia[0]].schedulato = 1;

            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            Schedula::AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
   Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    for(i = 0;i < dim_job;i++)
    {
        delete(disponibilita[i]);
        delete(vett_Lat[i]);
        delete(vett_End[i]);
        delete(vett_setup[i]);
        delete(scelti[i]);
    }
    delete(vett_max_Lat);
    delete(scelti);
    delete(disponibilita);
    delete(perm);
    delete(vett_indisp);
    delete(vett_Lat);
    delete(vett_End);
    delete(vett_setup);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}

TJob *Heuristics::PermutazioneSPTSemplice(TJob *array_job_attuale, int dim_job)
{//questa euristica costruisce la permutazione in base al proc_time del job
    int *proc_time_vett = NULL;
    proc_time_vett = new int[dim_job];
    int *ID_job_vett = NULL;
    ID_job_vett = new int[dim_job];
    int i,k;
    TJob *perm1 = NULL;
    perm1 = new TJob[dim_job];
    for(i = 0;i < dim_job; i++)
    {//carico il valore del proc_time
        proc_time_vett[i]=array_job_attuale[i].proc_time;
        ID_job_vett[i]=array_job_attuale[i].ID;
    }
    for(k=0;k<dim_job;k++)
    {
        int pos = 0;
        i=0;
        while(i<dim_job)
        {
            if(ID_job_vett[i]!=-1)//devo verificare che il job nn sia gi��?stato schedulato
            {
                pos = i;
                break;
            }
            i++;
        }
        // 	int min_proc_time = proc_time_vett[pos];
        for(i =1;i<dim_job;i++)
        {
            if
                    (
                     (ID_job_vett[i]!= -1)
                     &&
                     (proc_time_vett[i]<proc_time_vett[pos])
                     )
            {//sostituisco il valore trovato con quello migliore
                pos = i;// e' anche la posizione del job in array_job_attuale
            }

        }
        ID_job_vett[pos]=-1;//segno come schedulato il job prescelto
        perm1[k] = array_job_attuale[pos];
    }
    delete(ID_job_vett);
    delete(proc_time_vett);
    return perm1;
}
TJob *Heuristics::PermutazioneDeltaMezzoProcMedio(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i = 0;
    int Delta = 0;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[dim_job];
    Delta = ceil((float)Heuristics::CalcolaProcTimeMedioArrayJob()/2);

    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;
    }
   //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDeltaProcMedio(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale;

    int i = 0;
    int Delta = 0;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;
    array_job_locale=new TJob1[dim_job];
    Delta = Heuristics::CalcolaProcTimeMedioArrayJob();
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;
    }
   //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Heuristics::PermutazioneDelta_3ProcMedio(TJob *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale;
    int i = 0;
    int Delta = 0;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    Delta = 3*Heuristics::CalcolaProcTimeMedioArrayJob();
    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        Schedula::CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            Schedula::CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            Schedula::CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = AggiungiJobPermDelta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;
    }
   //devo ora liberare lo spazio delle schedule locali
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        Schedula::EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
