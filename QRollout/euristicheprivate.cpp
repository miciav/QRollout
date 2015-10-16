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
//using namespace std;


void Heuristics::initializeLocalSchedules(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale){
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2) M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3) M3_sch_locale = new TSchedula;
}

TSchedula** Heuristics::initializeArrayLocalSchedules(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale){
    TSchedula**schedule_locali = new TSchedula*[GNum_Macchine];
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
    return schedule_locali;
}

TJob1* Heuristics::copyArrayJob(TJob *from,int pDimJob){
    TJob1* to = new TJob1[pDimJob];
    for(int i = 0;i < pDimJob; i++)
    {
        to[i].ID = from[i].ID;
        to[i].tipo = from[i].tipo;
        to[i].proc_time = from[i].proc_time;
        to[i].duedate = from[i].duedate;
        to[i].deadline = from[i].deadline;
        to[i].priority = from[i].priority;
        to[i].rel_time = from[i].rel_time;
        to[i].adatto = 0;
        to[i].schedulato = 0;
    }
    return to;
}

void Heuristics::eliminaSchedule(TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale){
    Schedula::EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2) Schedula::EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3) Schedula::EliminaSchedula(M3_sch_locale);

}

TTipo_New* Heuristics::getStartingTimes(TSchedula **schedule){
    TTipo_New *tempo = new TTipo_New[GNum_Macchine];
    tempo[0] = CaricaTempo(schedule[0],GMacch1);//carica gli starting time disponibili senza tenere conto dei possibili setup o della durata dei processing time
    if(GNum_Macchine >= 2) tempo[1] = CaricaTempo(schedule[1],GMacch2);
    if (GNum_Macchine == 3) tempo[2] = CaricaTempo(schedule[2],GMacch3);
    return tempo;
}
int Heuristics::getMinimumScheduleTime(TTipo_New* tempo){
    int minimum = tempo[0].fine;
    for (int i = 1; i < GNum_Macchine; i++) minimum = fmin(minimum,tempo[i].fine);
    return minimum;
}

int Heuristics::divideJobs(TJob1 *array_job_locale, int dim_job, int minimum,int* deadline_vett,int* duedate_vett, int*else_vett){
    int minimum_alternativo= 100000;
    for(int i = 0; i < dim_job; i++)// verifico tutti i job uno per uno.
    {
        if
                (
                 (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                 &&
                 (array_job_locale[i].rel_time <= minimum)// se puo' essere schedulato perche' gia' rilasciato
                 )
        {
            array_job_locale[i].adatto = 1;// lo segno come adatto
            //________________________________________________________________
            if(array_job_locale[i].deadline > 0) // se ha una deadline
                deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline nel vettore delle deadline

            //________________________________________________________________

            if(array_job_locale[i].duedate > 0 && array_job_locale[i].deadline == 0)// se ha "solo" una duedate
                duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate nel vettore delle duedate

            //________________________________________________________________

            if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
            // se non ha ne' duedate ne' deadline
                else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline nel vettore else

        }
        if
                (
                 (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                 &&
                 (array_job_locale[i].rel_time <= minimum_alternativo)
                 )//calcolo un minimo alternativo nel caso non esiste nessun job con rel_date inferione al minimo tempo di schedulazione sulle macchine
        {
            minimum_alternativo=array_job_locale[i].rel_time;
        }
    }
    return minimum_alternativo;
}

int Heuristics::divideJobsVer2(TJob1 *array_job_locale, int dim_job, int minimum,int* deadline_vett,int* duedate_vett, int*else_vett){
    for(int i = 0; i < dim_job; i++)// verifico tutti i job uno per uno.
    {
        if
                (
                 (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                 &&
                 (array_job_locale[i].rel_time <= minimum)// se puo' essere schedulato perche' gia' rilasciato
                 )
        {
            array_job_locale[i].adatto = 1;// lo segno come adatto
            //________________________________________________________________
            if(array_job_locale[i].deadline > 0) // se ha una deadline
                deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline nel vettore delle deadline

            //________________________________________________________________

            if(array_job_locale[i].duedate > 0 )//qui ho modificato pu��?esserci lo stesso job su + vettori
                duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate nel vettore delle duedate

            //________________________________________________________________

            if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
            // se non ha ne' duedate ne' deadline
                else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline nel vettore else

        }
    }
}

int Heuristics::getElemPosinVett(int* vett,int dim_job){
    int k = -1;
    for(int i = 0;i < dim_job; i++)
        if(vett[i] > 0)
        {
            k=i; //cerco il primo elemento con deadline maggiore di 0
            break;
        }
    return k;
}

int Heuristics::getMinInVett(int startingPos, int* vett, int dim_job){
    int k = startingPos;
    int minimus = vett[k];
    for(int i = k+1;i < dim_job; i++)
    {
        if ( (vett[i] != -1) && (vett[i] < vett[k]) )
        {
            k = i;// salvo la posizione
            minimus = vett[k];
        }
    }
    return minimus;
}

int Heuristics::updateMachineUnaval(TTipo_New* tempo,TElem** indisp, int pMin){
    //  per ogni macchina verifico che minimum non si trovi in un periodo di
    // 	indisponibilita', nel caso fosse inserisco in tempi_ind il valore
    // 	dell'estremo superiore di tale periodo
    int tempi_ind[3];
    for(int i = 0;i < GNum_Macchine;i++) tempi_ind[i] = TrovaEdgeIndisp(indisp[i],pMin);
    //_____________________________________________________________________________________
    // 	calcolo adesso il minimo di questi nuovi tempi
    int minimum_alternativo2 = tempi_ind[0];
    for (int i = 1; i < GNum_Macchine; i++)
        if(tempi_ind[i]<minimum_alternativo2) minimum_alternativo2 = tempi_ind[i];

    // _____________________________________________________________________________________

    for(int i = 0;i < GNum_Macchine;i++) tempo[i].fine = tempi_ind[i];

    return minimum_alternativo2;

}

int Heuristics::updateMachineUnavalVer2(TTipo_New* tempo,TElem** indisp, int pMin,int minimum){
    //  per ogni macchina verifico che minimum non si trovi in un periodo di
    // 	indisponibilita', nel caso fosse inserisco in tempi_ind il valore
    // 	dell'estremo superiore di tale periodo
    int tempi_ind[3];
    for(int i = 0; i < GNum_Macchine; i++) tempi_ind[i] = Heuristics::TrovaEdgeIndisp(indisp[i],pMin);


    int minimum_alternativo2=tempi_ind[0];
    for (int i = 1; i < GNum_Macchine; i++) if(tempi_ind[i] < minimum_alternativo2) minimum_alternativo2=tempi_ind[i];


    if (pMin > minimum_alternativo2)
    {
        int i=0;
        for(i=0;i<GNum_Macchine;i++)
        {
            if(tempi_ind[i] < pMin)
                tempi_ind[i] = pMin;

            tempo[i].fine = tempi_ind[i];
        }
        minimum = pMin;
    }
    else
    {
        int i=0;
        for(i=0;i<GNum_Macchine;i++)
        {
            tempo[i].fine=tempi_ind[i];
        }
        minimum=minimum_alternativo2;
    }
    return fmax(pMin,minimum_alternativo2);

}

void Heuristics::filterJobs(int* vett, int minimus, TJob1 *array_job_locale,int dim_job, int* set, int* tipo_job){
    int i=0;
    // 			salvo tutti i job che hanno deadline pari a minimus
    while(i<dim_job)
    {
        if(vett[i]==minimus)
        {
            set[i] = array_job_locale[i].ID;
            tipo_job[i] = array_job_locale[i].tipo;
        }
        i++;
    }
}

void Heuristics::filterJobs(int* vett, TJob1 *array_job_locale,int dim_job, int* set, int* tipo_job){
    int i=0;
    // 			salvo tutti i job che hanno deadline pari a minimus
    while(i<dim_job)
    {
        if(vett[i]!=-1)
        {
            set[i] = array_job_locale[i].ID;
            tipo_job[i] = array_job_locale[i].tipo;
        }
        i++;
    }
}


int Heuristics::filterJobsDelta(int* vett, int minimus,TJob1 *pArray_jobs, int delta, int dim_job, int *set, int * tipo_job){
    int i=0;
    int cambio=0;
    while(i<dim_job)
    {
        if
                (
                 (vett[i] <= minimus + delta)//scelgo i job a distanza delta dal minimo
                 &&
                 (vett[i] > 0)
                 )
        {
            set[i] = pArray_jobs[i].ID;
            tipo_job[i] = pArray_jobs[i].tipo;
            cambio = 1;
        }
        i++;
    }
    return cambio;
}

void Heuristics::updateTipoCampagna(int minimum, int *tipo_macchine,int *campagna_macchine,TTipo_New* tempo){
    int i=0;
    while(i<GNum_Macchine)
    {
        if(tempo[i].fine == minimum)
        {
            tipo_macchine[i] = tempo[i].tipo;
            campagna_macchine[i] = tempo[i].Camp;
        }
        i++;
    }
}

TJob* Heuristics::assignJobToPerm(int *set, int i){
  TJob *perm = new TJob;
  int jj=0;
  while(jj<GNum_Job)
  {
      if(GArray_Job[jj].ID == set[i])
      {
          perm[0]=GArray_Job[jj];
          break;
      }
      jj++;
  }
  return perm;
}

void Heuristics::minMatrix(int dim_job, int**matrix, int pos_i, int pos_k){
    pos_i = 0;
    pos_k = 0;
    for(int i=0;i<dim_job;i++)
    {
        for(int k=0;k<GNum_Macchine;k++)
        {
            // 			devo trovare un elemento nn nullo
            if
                    (
                     (matrix[pos_i][pos_k] == -1)
                     &&
                     (matrix[i][k]!=-1)
                     )
            {
                pos_i = i;
                pos_k = k;
            }
            if
                    (
                     (matrix[i][k]>=0)
                     &&
                     (matrix[i][k] < matrix[pos_i][pos_k])
                     )
            {
                pos_i = i;
                pos_k = k;
            }
        }
    }
}

TJob* Heuristics::assignJobToPermWithSetup(int **set_matrix, int pos_i, int pos_k){
    TJob *perm = new TJob;
    int j=0;
    while(j<GNum_Job)
    {
        if(GArray_Job[j].ID == set_matrix[pos_i][pos_k])
        {
            perm[0]=GArray_Job[j];
            break;
        }
        j++;
    }
    return perm;
}
