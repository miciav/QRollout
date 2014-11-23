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


int CalcolaProcTimeMedio(void)
{
//     questa funzione calcola il proc. time medio

    int totale = 0;
    int temp;
    for(int i = 0; i < GNum_Job; i++)
        totale += GArray_Job[i].proc_time;

    temp = ceil((float)totale/GNum_Job);
    return temp;
}

void InizializzaPermutazioneMigliore(TJob *pPerm)
{
    for(int i = 0; i < GNum_Job; i++)
        pPerm[i] = GArray_Job[i];
}

TTipo_New CaricaTempo(TSchedula *pM_sch, TElem *pM)
{
    TTipo_New temp;
    int fine = 0;
    while(pM_sch->next != NULL)
        pM_sch = pM_sch->next;

    fine = pM_sch->fine;
    while
            (
             (pM != NULL)
             &&
             (fine > pM->inizio)
             )
    {
        pM = pM->next;
    }
    if
            (
             (pM == NULL)
             ||
             (fine < pM->inizio)
             )
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
    }//assumo che nn possano esserci due interruzioni consecutive senza un periodo di attivitï¿½ï¿½?nel mezzo

}
/****************************************************************/
/* questa funzione verifica che minimum non si trovi 		*/
/* all'interno di un periodo di indisponibilita' ed in tal caso */
/* restituisce l'estremo superiore di tale periodo		*/
/****************************************************************/
int TrovaEdgeIndisp(TElem *M,int minimum)
{
    TElem *temp = NULL;
    temp = M;
    while(temp->next != NULL)
    {
        if(temp->inizio > minimum)      return minimum;
        else if(temp->fine >= minimum)  return temp->fine;
        else                            temp = temp->next;
    }
    return minimum;
    // nel caso in cui non ci siano indisponibilita'
    // successive a minimum

}

int AggiungiJobPerm (TJob1 *array_job_locale,int dim_job,TSchedula **schedule_locali)
{

    TTipo_New tempo[3];
    int tempi_ind[3];
    int *disponibilita;
    int disp = 0;
    int minimum_alternativo2;
    TJob *perm = NULL;
    TJob pp;
    int i,k,minimum;
    int minimum_alternativo = 100000;
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
    int jj = 0;
    int st_vt = 0;
    int kk;
    int pos_i = 0;
    int pos_k = 0;
    disponibilita = &disp;
    perm = &pp;
    indisp              = new TElem*[GNum_Macchine];
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
    if(GNum_Macchine == 3)
        indisp[2] = GMacch3;

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
    // __________________________________________________________________________________


    tempo[0] = CaricaTempo(schedule_locali[0],GMacch1);//carica gli starting time disponibili senza tenere conto dei possibili setup o della durata dei processing time
    if(GNum_Macchine >= 2)
        tempo[1] = CaricaTempo(schedule_locali[1],GMacch2);

    if (GNum_Macchine == 3)
        tempo[2] = CaricaTempo(schedule_locali[2],GMacch3);

    // ___________________________________________________________________________________
    // 	ora conosco i tempi minimi di schedulazione sulle singole macchine
    // 	a questo punto devo solo cercare il minimo di questi tempi e dato questo stabilire il set di job con release date
    // 	anteriore a tale data.

    minimum = tempo[0].fine;
    for (i = 1; i < GNum_Macchine; i++)
        minimum = min(minimum,tempo[i].fine);

    //ora conosco il minimo
    //_____________________________________________________________________________________

    // 	devo costruire il set dei job released
    for(i = 0; i < dim_job; i++)// verifico tutti i job uno per uno.
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
    }//ora ho segnato i job che posso schedulare xche' rispettano la release date.

    // _______________________________________________________________________________________

    // 	cerco ora un elemento con deadline != 0;
    k = -1;
    for(i = 0;i < dim_job; i++)
    {
        if(deadline_vett[i] > 0)
        {
            k=i; //cerco il primo elemento con deadline maggiore di 0
            break;
        }
    }

    //devo cercare il job con il valore + piccolo di deadline
    minimus = 0;
    if(k >= 0)// se ho trovato almeno un elemento con valore di deadline maggiore di 0
    {
        minimus = deadline_vett[k];
        for(i = k+1;i < dim_job; i++)
        {
            if
                    (
                     (deadline_vett[i] != -1)
                     &&
                     (deadline_vett[i] < deadline_vett[k])
                     )
            {
                k = i;// salvo la posizione
                minimus = deadline_vett[k];
            }
        }
        fine = 1;// flag che mi avverte che posso terminare
        deadline = 1;// segno che ho trovato una deadline
    }
    else// se nn ho trovato nessuna deadline cerco le duedate
    {
        k = -1;
        for(i = 0;i < dim_job; i++)
        {
            if(duedate_vett[i] > 0)
            {
                k=i;// ho trovato almeno una duedate
                break;
            }
        }

        minimus = 0;
        if(k>=0)// se ho trovato almeno una duedate
        {
            minimus = duedate_vett[k];
            for(i = k + 1;i < dim_job;i++)
            {
                if
                        (
                         (duedate_vett[i] != -1)
                         &&
                         (duedate_vett[i] < duedate_vett[k])
                         )
                {
                    k=i;// salvo la posizione
                    minimus = duedate_vett[k];// aggiorno il valore del minimo
                }
            }
            fine = 1;//flag che mi dice che posso terminare la ricerca
            duedate = 1;// ho trovato una duedate
        }
        else //cerco un job privo di duedate e deadline
        {
            k=-1;
            for(i=0;i<dim_job;i++)
            {
                if(else_vett[i] > 0)
                {
                    k=i;// ne ho trovato almeno uno
                    break;
                }
            }
            if(k>=0)//un job qualsiasi va bene
            {
                fine = 1;
                nothing = 1;
            }

        }
    }

    if(fine == 0)//non posso aggiungere job perche' nessun job ha release date inferiore o uguale al minimo tempo disponibile sulle macchine.
    {
        //  	per ogni macchina verifico che minimum non si trovi in un periodo di
        // 	indisponibilita', nel caso fosse inserisco in tempi_ind il valore
        // 	dell'estremo superiore di tale periodo
        for(i = 0;i < GNum_Macchine;i++)
            tempi_ind[i] = TrovaEdgeIndisp(indisp[i],minimum_alternativo);
        //_____________________________________________________________________________________
        // 	calcolo adesso il minimo di questi nuovi tempi
        minimum_alternativo2 = tempi_ind[0];
        for (i = 1; i < GNum_Macchine; i++)
        {
            if(tempi_ind[i]<minimum_alternativo2)
                minimum_alternativo2 = tempi_ind[i];

        }
        // _____________________________________________________________________________________

        i = 0;
        for(i = 0;i < GNum_Macchine;i++)
            tempo[i].fine = tempi_ind[i];

        minimum = minimum_alternativo2;

        //                 minimum_alternativo=max(minimum_alternativo,minimum_alternativo2);
        for(i=0; i<dim_job; i++)
        {
            if
                    (
                     (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                     &&
                     (array_job_locale[i].rel_time <= minimum)
                     )
            {
                array_job_locale[i].adatto = 1;
                // 			_______________________________________________________________
                if(array_job_locale[i].deadline > 0)
                    deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline

                // 			_______________________________________________________________
                if(array_job_locale[i].duedate > 0 && array_job_locale[i].deadline == 0)
                    duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate

                // 			_________________________________________________________________
                if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
                    else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline
            }
        }

        // come sopra devo individuare il job che deadline o duedate minima
        k = -1;
        for(i = 0;i < dim_job; i++)
        {
            if(deadline_vett[i] > 0)
            {
                k=i;
                break;
            }
        }
        minimus = 0;
        if(k >= 0)
        {
            minimus = deadline_vett[k];
            // 	ho trovato almeno un elemento che ha deadline > 0
            for(i=k+1;i<dim_job;i++)
            {
                if
                        (
                         (deadline_vett[i] != -1)
                         &&
                         (deadline_vett[i] < deadline_vett[k])
                         )
                {
                    k=i;// salvo la posizione
                    minimus = deadline_vett[k];
                }
            }
            fine=1;
            deadline = 1;// segno che ho trovato una deadline
        }
        else// se nn ho trovato nessuna deadline cerco le duedate
        {
            k=-1;
            for(i=0;i<dim_job;i++)
            {
                if(duedate_vett[i] > 0)
                {
                    k=i;
                    break;
                }
            }
            minimus = 0;
            if(k>=0)
            {
                minimus = duedate_vett[k];
                // 	ho trovato almeno un elemento che ha duedate > 0 && deadline == 0
                for(i=k+1;i<dim_job;i++)
                {
                    if
                            (
                             (duedate_vett[i] != -1)
                             &&
                             (duedate_vett[i] < duedate_vett[k])
                             )
                    {
                        k=i;// salvo la posizione
                        minimus = duedate_vett[k];
                    }
                }
                fine=1;
                duedate = 1;
            }
            else //cerco un job privo di duedate e deadline
            {
                k=-1;
                for(i=0;i<dim_job;i++)
                {
                    if(else_vett[i] > 0)
                    {
                        k=i;
                        break;
                    }
                }
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
            i=0;
            // 			salvo tutti i job che hanno deadline pari a minimus
            while(i<dim_job)
            {
                if(deadline_vett[i]==minimus)
                {
                    set[i] = array_job_locale[i].ID;
                    tipo_job[i] = array_job_locale[i].tipo;

                }
                i++;
            }

        }
        else if (duedate == 1)
        {
            //minimus indica il valore + basso della duedate
            // 			salvo i job con i valori minimi di duedate
            i=0;
            while(i<dim_job)
            {
                if(duedate_vett[i]==minimus)
                {
                    set[i] = array_job_locale[i].ID;
                    tipo_job[i] = array_job_locale[i].tipo;

                }
                i++;
            }
        }
        else if (nothing == 1)
        {
            // 			salvo tutti i job che non hanno ne' duedate ne' deadline
            i=0;
            while(i<dim_job)
            {
                if(else_vett[i]!=-1)
                {
                    set[i] = array_job_locale[i].ID;
                    tipo_job[i] = array_job_locale[i].tipo;

                }
                i++;
            }
        }
        // _________________________________________________________________________________
        // 		per ogni macchina salvo le informazioni sul tipo di job e sullo
        // 		stado della campagna
        i=0;
        while(i<GNum_Macchine)
        {
            if(tempo[i].fine == minimum)
            {
                tipo_macchine[i] = tempo[i].tipo;
                campagna_macchine[i] = tempo[i].Camp;
            }
            i++;
        }
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
                                if(campagna_macchine[k]<GArray_Tipi[j].MaxOpCamp)
                                {
                                    // 								eureka! posso shedulare questo job immediatamente
                                    // 								la macchina k-esima verra'assegnata al job
                                    jj=0;
                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    while(jj<GNum_Job)
                                    {
                                        if(GArray_Job[jj].ID == set[i])
                                        {
                                            perm[0]=GArray_Job[jj];
                                            break;
                                        }
                                        jj++;
                                    }
                                    VerificaMacchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    AggiungiSchedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
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
                            // 					nn devo pagare setup
                            jj=0;
                            st_vt = 0;
                            setup_vett =&st_vt;
                            while(jj<GNum_Job)
                            {
                                if(GArray_Job[jj].ID == set[i])
                                {
                                    perm[0]=GArray_Job[jj];
                                    break;
                                }
                                jj++;
                            }
                            VerificaMacchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            AggiungiSchedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
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
    pos_i = 0;
    pos_k = 0;
    for(i=0;i<dim_job;i++)
    {
        for(k=0;k<GNum_Macchine;k++)
        {
            // 			devo trovare un elemento nn nullo
            if
                    (
                     (setup_matrix[pos_i][pos_k] == -1)
                     &&
                     (setup_matrix[i][k]!=-1)
                     )
            {
                pos_i = i;
                pos_k = k;
            }
            if
                    (
                     (setup_matrix[i][k]>=0)
                     &&
                     (setup_matrix[i][k] < setup_matrix[pos_i][pos_k])
                     )
            {
                pos_i = i;
                pos_k = k;
            }
        }
    }
    j=0;

    st_vt = 1;
    setup_vett =&st_vt;
    while(j<GNum_Job)
    {
        if(GArray_Job[j].ID == set_matrix[pos_i][pos_k])
        {
            perm[0]=GArray_Job[j];
            break;
        }
        j++;
    }
    VerificaMacchina(schedule_locali[pos_k],indisp[pos_k],disponibilita,setup_vett,0,perm,0);
    AggiungiSchedula(schedule_locali[pos_k],perm[0],disponibilita[0],setup_vett[0]);
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
int AggiungiJobPermDelta (TJob1 *pArray_jobs,
                          int pDimJob,
                          TSchedula **pArraySchedule,
                          int pDelta)
{
    TJob *perm          = NULL;
    TElem **indisp      = NULL;
    TTipo_New *tempo    = NULL;
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
    int fine;
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

    perm                = new TJob;
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
    if(GNum_Macchine >= 2)
        indisp[1] = GMacch2;
    if(GNum_Macchine == 3)
        indisp[2] = GMacch3;

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

    tempo[0] = CaricaTempo(pArraySchedule[0],GMacch1);//carica gli starting time disponibili
    if(GNum_Macchine >= 2)
        tempo[1] = CaricaTempo(pArraySchedule[1],GMacch2);

    if (GNum_Macchine == 3)
        tempo[2] = CaricaTempo(pArraySchedule[2],GMacch3);

    // 	ora conosco i tempi minimi di schedulazione sulle singole macchine
    // 	a questo punto devo solo cercare il minimo di questi tempi e dato questo stabilire il set di job con release dete
    // 	anteriore a tale data.
    minimum = tempo[0].fine;
    for (i = 1; i < GNum_Macchine; i++)
        minimum = min(minimum,tempo[i].fine);

    //ora conosco il minimo
    // 	devo costruire il set dei job released
    for(i = 0; i < pDimJob; i++)
    {
        if
                (
                 (pArray_jobs[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                 &&
                 (pArray_jobs[i].rel_time <= minimum)
                 )
        {
            pArray_jobs[i].adatto = 1;
            if(pArray_jobs[i].deadline > 0)
                deadline_vett[i] = pArray_jobs[i].deadline;//salvo le deadline

            if(pArray_jobs[i].duedate > 0)//qui ho modificato puo' esserci lo stesso job su + vettori
                duedate_vett[i] = pArray_jobs[i].duedate;//salvo le duedate

            if(pArray_jobs[i].duedate == 0 && pArray_jobs[i].deadline == 0)
                else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline

        }
        if
                (
                 (pArray_jobs[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                 &&
                 (pArray_jobs[i].rel_time <= minimum_alternativo)
                 )
        {
            minimum_alternativo=pArray_jobs[i].rel_time;
        }
    }// ora ho segnato i job che posso schedulare xchï¿½ï¿½?rispettano la release date.
     // cerco ora un elemento con deadline != 0;
    fine = 0;
    k    = -1;
    for(i = 0;i < pDimJob;i++)
    {
        if(deadline_vett[i] > 0)
        {
            k=i;
            break;
        }
    }
    min_deadline = 0;
    if(k >= 0)
    {
        min_deadline = deadline_vett[k];
        // 	ho trovato almeno un elemento che ha deadline > 0
        for(i=k+1;i<pDimJob;i++)
        {
            if
                    (
                     (deadline_vett[i] != -1)
                     &&
                     (deadline_vett[i] < deadline_vett[k])
                     )
            {
                k=i;// salvo la posizione
                min_deadline = deadline_vett[k];
            }
        }
        fine=1;
        deadline = 1;// segno che ho trovato una deadline
    }
    min_duedate=0;
    k = -1;
    for(i=0;i<pDimJob;i++)
    {
        if(duedate_vett[i] > 0)
        {
            k=i;
            break;
        }
    }
    if(k>=0)
    {
        min_duedate = duedate_vett[k];
        // 	ho trovato almeno un elemento che ha duedate > 0
        for(i=k+1;i<pDimJob;i++)
        {
            if
                    (
                     (duedate_vett[i] != -1)
                     &&
                     (duedate_vett[i] < duedate_vett[k])
                     )
            {
                k=i;// salvo la posizione
                min_duedate = duedate_vett[k];
            }
        }
        fine=1;
        duedate = 1;
    }
    else //cerco un job privo di duedate e deadline
    {
        k=-1;
        for(i=0;i<pDimJob;i++)
        {
            if(else_vett[i] > 0)
            {
                k=i;
                break;
            }
        }
        if(k>=0)
        {
            fine = 1;
            nothing = 1;
        }

    }
    if(fine == 0)
    {
        for(i = 0; i < GNum_Macchine; i++)
            tempi_ind[i] = TrovaEdgeIndisp(indisp[i],minimum_alternativo);


        minimum_alternativo2=tempi_ind[0];
        for (i = 1; i < GNum_Macchine; i++)
        {
            if(tempi_ind[i] < minimum_alternativo2)
                minimum_alternativo2=tempi_ind[i];

        }
        if (minimum_alternativo>minimum_alternativo2)
        {
            i=0;
            for(i=0;i<GNum_Macchine;i++)
            {
                if(tempi_ind[i] < minimum_alternativo)
                    tempi_ind[i] = minimum_alternativo;

                tempo[i].fine = tempi_ind[i];
            }
            minimum = minimum_alternativo;
        }
        else
        {
            i=0;
            for(i=0;i<GNum_Macchine;i++)
            {
                tempo[i].fine=tempi_ind[i];
            }
            minimum=minimum_alternativo2;
        }
        minimum_alternativo=max(minimum_alternativo,minimum_alternativo2);
        for(i=0; i<pDimJob; i++)
        {
            if
                    (
                     (pArray_jobs[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                     &&
                     (pArray_jobs[i].rel_time <= minimum)
                     )
            {
                pArray_jobs[i].adatto = 1;
                if(pArray_jobs[i].deadline > 0)
                {
                    deadline_vett[i] = pArray_jobs[i].deadline;//salvo le deadline
                }
                if(pArray_jobs[i].duedate > 0)//qui ho modificato puï¿½ï¿½?esserci lo stesso job su + vettori
                {
                    duedate_vett[i] = pArray_jobs[i].duedate;//salvo le duedate
                }
                if(pArray_jobs[i].duedate == 0 && pArray_jobs[i].deadline == 0)
                {
                    else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline
                }
            }
        }
        k=-1;
        for(i=0;i<pDimJob;i++)
        {
            if(deadline_vett[i] > 0)
            {
                k=i;
                break;
            }
        }
        min_deadline = 0;
        if(k>=0)
        {
            min_deadline = deadline_vett[k];
            // 	ho trovato almeno un elemento che ha deadline > 0
            for(i=k+1;i<pDimJob;i++)
            {
                if
                        (
                         (deadline_vett[i] != -1)
                         &&
                         (deadline_vett[i] < deadline_vett[k])
                         )
                {
                    k=i;// salvo la posizione
                    min_deadline = deadline_vett[k];
                }
            }
            fine=1;
            deadline = 1;// segno che ho trovato una deadline
        }
        min_duedate=0;
        k=-1;
        for(i=0;i<pDimJob;i++)
        {
            if(duedate_vett[i] > 0)
            {
                k=i;
                break;
            }
        }
        if(k>=0)
        {
            min_duedate = duedate_vett[k];
            // 	ho trovato almeno un elemento che ha duedate > 0
            for(i=k+1;i<pDimJob;i++)
            {
                if
                        (
                         (duedate_vett[i] != -1)
                         &&
                         (duedate_vett[i] < duedate_vett[k])
                         )
                {
                    k=i;// salvo la posizione
                    min_duedate = duedate_vett[k];
                }
            }
            fine=1;
            duedate = 1;
        }
        //cerco un job privo di duedate e deadline

        k=-1;
        for(i=0;i<pDimJob;i++)
        {
            if(else_vett[i] > 0)
            {
                k=i;
                break;
            }
        }
        if(k>=0)
        {
            fine = 1;
            nothing = 1;
        }

        /*if(duedate != 1 && deadline != 1) //cerco un job privo di duedate e deadline
           {
              k=-1;
              for(i=0;i<dim_job;i++)
              {
                 if(else_vett[i] > 0)
                 {
                    k=i;
                    break;
                 }
              }
              if(k>=0)
              {
                 fine = 1;
                 nothing = 1;
              }
           }  */
    }
    //calcolo ora il minimo tra duedate e deadline se min resta uguale a zero significa che devo considerare gli elementi del vettore else
    int minimus =0;
    if(deadline == -1 && duedate==1)
    {
        minimus = min_duedate;
    }
    else if(deadline == 1 && duedate== -1)
    {
        minimus = min_deadline;
    }
    else if (deadline == 1 && duedate==1)
    {
        minimus = min(min_deadline,min_duedate);
    }

    if (deadline == 1)
    {//minimus indica il valore + basso della deadline
        i=0;
        cambio = 0;
        while(i<pDimJob)
        {
            if
                    (
                     (deadline_vett[i]<=minimus+pDelta)//scelgo i job a distanza delta dal minimo
                     &&
                     (deadline_vett[i]>0)
                     )
            {
                set_deadline[i] = pArray_jobs[i].ID;
                tipo_job_deadline[i] = pArray_jobs[i].tipo;
                cambio = 1;
            }
            i++;
        }
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
        i=0;
        cambio = 0;
        while(i<pDimJob)
        {
            if
                    (
                     (duedate_vett[i]<=minimus+pDelta)
                     &&
                     (duedate_vett[i]>0)
                     )
            {
                set_duedate[i] = pArray_jobs[i].ID;
                tipo_job_duedate[i] = pArray_jobs[i].tipo;
                cambio = 1;
            }
            i++;
        }
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
        i=0;
        while(i<pDimJob)
        {
            if(else_vett[i]!=-1)
            {
                set_nothing[i] = pArray_jobs[i].ID;
                tipo_job_nothing[i] = pArray_jobs[i].tipo;

            }
            i++;
        }
    }

    i=0;
    while(i<GNum_Macchine)
    {
        if(tempo[i].fine == minimum)
        {
            tipo_macchine[i] = tempo[i].tipo;
            campagna_macchine[i] = tempo[i].Camp;
        }
        i++;
    }

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
                                    jj=0;
                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    while(jj<GNum_Job)
                                    {
                                        if(GArray_Job[jj].ID == set_deadline[i])
                                        {
                                            perm[0]=GArray_Job[jj];
                                            break;
                                        }
                                        jj++;
                                    }
                                    VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
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
                            jj=0;
                            st_vt = 0;
                            setup_vett =&st_vt;
                            while(jj<GNum_Job)
                            {
                                if(GArray_Job[jj].ID == set_deadline[i])
                                {
                                    perm[0]=GArray_Job[jj];
                                    break;
                                }
                                jj++;
                            }
                            VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
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
                                    // 					la macchina k-esima verrï¿½ï¿½?assegnata al job
                                    j1=0;
                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    while(j1<GNum_Job)
                                    {
                                        if(GArray_Job[j1].ID == set_duedate[i])
                                        {
                                            perm[0]=GArray_Job[j1];
                                            break;
                                        }
                                        j1++;
                                    }
                                    VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
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
                        if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job giï¿½ï¿½?schedulati
                        {
                            // 						nn devo pagare setup
                            //MIA
                            jj=0;
                            st_vt = 0;
                            setup_vett =&st_vt;
                            while(jj<GNum_Job)
                            {
                                if(GArray_Job[jj].ID == set_duedate[i])
                                {
                                    perm[0]=GArray_Job[jj];
                                    break;
                                }
                                jj++;
                            }
                            VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
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
                                    // 								la macchina k-esima verrï¿½ï¿½?assegnata al job
                                    j1=0;
                                    st_vt = 0;
                                    setup_vett =&st_vt;
                                    while(j1<GNum_Job)
                                    {
                                        if(GArray_Job[j1].ID == set_nothing[i])
                                        {
                                            perm[0]=GArray_Job[j1];
                                            break;
                                        }
                                        j1++;
                                    }
                                    VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                                    AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
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
                        if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job giï¿½ï¿½?schedulati
                        {
                            // 						nn devo pagare setup
                            jj=0;
                            st_vt = 0;
                            setup_vett =&st_vt;
                            while(jj<GNum_Job)
                            {
                                if(GArray_Job[jj].ID == set_nothing[i])
                                {
                                    perm[0]=GArray_Job[jj];
                                    break;
                                }
                                jj++;
                            }
                            VerificaMacchina(pArraySchedule[k],indisp[k],disponibilita,setup_vett,0,perm,0);
                            AggiungiSchedula(pArraySchedule[k],perm[0],disponibilita[0],setup_vett[0]);
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
    pos_i = 0;
    pos_k = 0;
    for(i=0;i<pDimJob;i++)
    {
        for(k=0;k<GNum_Macchine;k++)
        {
            // 			devo trovare un elemento nn nullo
            if
                    (
                     (setup_matrix[pos_i][pos_k] == -1)
                     &&
                     (setup_matrix[i][k]!=-1)
                     )
            {
                pos_i = i;
                pos_k = k;
            }
            if
                    (
                     (setup_matrix[i][k]>=0)
                     &&
                     (setup_matrix[i][k] < setup_matrix[pos_i][pos_k])
                     )
            {
                pos_i = i;
                pos_k = k;
            }
        }
    }
    jj=0;
    st_vt = 1;
    setup_vett =&st_vt;
    while(jj<GNum_Job)
    {
        if(GArray_Job[jj].ID == set_matrix[pos_i][pos_k])
        {
            perm[0]=GArray_Job[jj];
            break;
        }
        jj++;
    }
    VerificaMacchina(pArraySchedule[pos_k],indisp[pos_k],disponibilita,setup_vett,0,perm,0);
    AggiungiSchedula(pArraySchedule[pos_k],perm[0],disponibilita[0],setup_vett[0]);
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
TJob *PermutazioneEdd_1Tipo ( TJob *pArray_job_attuale,int pDim_job )
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

TJob *PermutazioneEdd_2Tipo ( TJob *pArray_Job_Attuale,int pDim_Job )
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
TJob *PermutazioneBase(TJob *pArray_Job_Attuale, int pDimJob)
{//questa e' l'euristica di base presentata nella tesi di Ritota
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;


    array_job_locale = new TJob1[pDimJob];

    //	inizializzo le schedule locali che serviranno per la individuazione della sequenza di job
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    // 	schedule_locali a' un vettore di puntatori alle schedule locali
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[pDimJob];
    //inizializzo le schedule locali
    CopiaSchedule(GMacch1_Sched,M1_sch_locale);// fa una copia di M1_sch in M1_sch_locale
    schedule_locali[0] = M1_sch_locale;
    if(GNum_Macchine >= 2)
    {
        CopiaSchedule(GMacch2_Sched,M2_sch_locale);
        schedule_locali[1] = M2_sch_locale;
    }
    if(GNum_Macchine == 3)
    {
        CopiaSchedule(GMacch3_Sched,M3_sch_locale);
        schedule_locali[2] = M3_sch_locale;
    }
// ________________________________________________________________________________
// copio le informazioni contenute in array_job_attuale 	
    for(i = 0;i<pDimJob;i++)
    {
        array_job_locale[i].ID          = pArray_Job_Attuale[i].ID;
        array_job_locale[i].tipo        = pArray_Job_Attuale[i].tipo;
        array_job_locale[i].proc_time   = pArray_Job_Attuale[i].proc_time;
        array_job_locale[i].duedate     = pArray_Job_Attuale[i].duedate;
        array_job_locale[i].deadline    = pArray_Job_Attuale[i].deadline;
        array_job_locale[i].priority    = pArray_Job_Attuale[i].priority;
        array_job_locale[i].rel_time    = pArray_Job_Attuale[i].rel_time;
        array_job_locale[i].adatto      = 0;
        array_job_locale[i].schedulato  = 0;
    }
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
        for(int jj = 0;jj < pDimJob; jj++)
            array_job_locale[jj].adatto = 0;// devo verificare se e' cambiato

	i++;	
    }
// ____________________________________________________________________________________
	
//devo ora liberare lo spazio delle schedule locali
        EliminaSchedula(M1_sch_locale);
        if(GNum_Macchine >= 2)
            EliminaSchedula(M2_sch_locale);
        if(GNum_Macchine == 3)
            EliminaSchedula(M3_sch_locale);

        delete(schedule_locali);
        delete(array_job_locale);
	return perm1;

}

TJob *PermutazioneDelta_10(TJob *pArray_Job_Attuale, int pDimJob)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 10;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;
    array_job_locale = new TJob1[pDimJob];

    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali=new TSchedula*[GNum_Macchine];
    perm1 = new TJob[pDimJob];
    {//inizializzo le schedule locali
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i < pDimJob; i++)
    {
        array_job_locale[i].ID = pArray_Job_Attuale[i].ID;
        array_job_locale[i].tipo = pArray_Job_Attuale[i].tipo;
        array_job_locale[i].proc_time = pArray_Job_Attuale[i].proc_time;
        array_job_locale[i].duedate = pArray_Job_Attuale[i].duedate;
        array_job_locale[i].deadline = pArray_Job_Attuale[i].deadline;
        array_job_locale[i].priority = pArray_Job_Attuale[i].priority;
        array_job_locale[i].rel_time = pArray_Job_Attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_15(TJob *pArray_Job_Attuale, int pDimJob)
{//questa e' un'euristica basata sul concetto di Delta
    TJob1 *array_job_locale = NULL;
    int i= 0;
    int Delta = 15;
    int temp = 0;
    TSchedula *M1_sch_locale = NULL;
    TSchedula *M2_sch_locale = NULL;
    TSchedula *M3_sch_locale = NULL;
    TSchedula **schedule_locali = NULL;
    TJob * perm1 = NULL;
    int k = 0;

    array_job_locale=new TJob1[pDimJob];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[pDimJob];
    {//inizializzo le schedule locali
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<pDimJob;i++)
    {
        array_job_locale[i].ID = pArray_Job_Attuale[i].ID;
        array_job_locale[i].tipo = pArray_Job_Attuale[i].tipo;
        array_job_locale[i].proc_time = pArray_Job_Attuale[i].proc_time;
        array_job_locale[i].duedate = pArray_Job_Attuale[i].duedate;
        array_job_locale[i].deadline = pArray_Job_Attuale[i].deadline;
        array_job_locale[i].priority = pArray_Job_Attuale[i].priority;
        array_job_locale[i].rel_time = pArray_Job_Attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_20(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);

    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);
    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_25(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Permutazione_delta_30(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_35(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_40(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_50(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *Permutazione_delta_24ore(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_7ore(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneFittizia(TJob *array_job_attuale, int dim_job)
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
TJob *PermutazioneDelta_3(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_5(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_2(TJob *array_job_attuale, int dim_job)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}

/*Euristiche di tipo LLF con o senza delta*/
TJob *PermutazioneLLFDeltaBasica(TJob *pArray_job_attuale, int pDim_job,int pDelta)
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine>=2)
        {
            M2_sch_locale = new TSchedula;
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
            AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }

    /*Elimino le schedule locali*/
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

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
TJob *PermutazioneLLF(TJob *pArray_job_attuale, int pDim_job)
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
        scelti[i]           = new int[3]; /*sempre immagazina 3 valori non è come gli altri array*/
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            M2_sch_locale = new TSchedula;
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
            AggiungiSchedula(schedule_locali[coppia[1]],
                              perm[0],
                              disponibilita[coppia[0]][coppia[1]],
                              vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }


    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);


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
TJob *PermutazioneLLFDelta_2(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 2;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *PermutazioneLLFDelta_5(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 5;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *PermutazioneLLFDelta_10(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 10;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *PermutazioneLLFDelta_7ore(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 420;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);
}
TJob *PermutazioneLLFDelta_14ore(TJob *pArray_Job_Attuale, int pDim_Job)
{
    // per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
    // tra quelli equivalenti si scegliera' il job con ending time + basso
    int delta = 840;
    return PermutazioneLLFDeltaBasica(pArray_Job_Attuale, pDim_Job,delta);

}
TJob *PermutazioneLLFDelta_3ProcMedio(TJob *array_job_attuale, int dim_job)
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
    delta = 3*CalcolaProcTimeMedio();
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine>=2)
        {
            M2_sch_locale = new TSchedula;
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
            if(array_job_locale[i].schedulato == 0)//se nn ï¿½ï¿½?schedulato giï¿½ï¿½?
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
                  //indica che nn c'ï¿½ï¿½?duedate e quindi vanno schedulati per ultimi
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
            AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

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
TJob *PermutazioneLLFDeltaProcMedio(TJob *array_job_attuale, int dim_job)
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglierï¿½ï¿½?il job con ending time + basso
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
    delta = CalcolaProcTimeMedio();
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            M2_sch_locale = new TSchedula;
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
            if(array_job_locale[i].schedulato == 0)//se nn ï¿½ï¿½?schedulato giï¿½ï¿½?
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
            AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

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
TJob *PermutazioneLLFDeltaMezzoProcMedio(TJob *array_job_attuale, int dim_job)
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglierï¿½ï¿½?il job con ending time + basso
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
    delta = ceil((float)CalcolaProcTimeMedio()/2);
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
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine>=2)
        {
            M2_sch_locale = new TSchedula;
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            M3_sch_locale = new TSchedula;
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
            if(array_job_locale[i].schedulato == 0)//se nn ï¿½ï¿½?schedulato giï¿½ï¿½?
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
                  //indica che nn c'ï¿½ï¿½?duedate e quindi vanno schedulati per ultimi
                    }
                    else
                    {
                        vett_Lat[i][j] = -64000;
               //la release date nn ï¿½ï¿½?soddisfatta
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
            AggiungiSchedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine>=2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

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

TJob *PermutazioneSPTSemplice(TJob *array_job_attuale, int dim_job)
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
            if(ID_job_vett[i]!=-1)//devo verificare che il job nn sia giï¿½ï¿½?stato schedulato
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
TJob *PermutazioneDeltaMezzoProcMedio(TJob *array_job_attuale, int dim_job)
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
    Delta = ceil((float)CalcolaProcTimeMedio()/2);

    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;

    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDeltaProcMedio(TJob *array_job_attuale, int dim_job)
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
    Delta = CalcolaProcTimeMedio();
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}
TJob *PermutazioneDelta_3ProcMedio(TJob *array_job_attuale, int dim_job)
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

    Delta = 3*CalcolaProcTimeMedio();
    array_job_locale=new TJob1[dim_job];
    M1_sch_locale = new TSchedula;// tali schedule contengono almeno un elemento nullo
    if(GNum_Macchine >= 2)
        M2_sch_locale = new TSchedula;
    if(GNum_Macchine == 3)
        M3_sch_locale = new TSchedula;
    schedule_locali = new TSchedula*[GNum_Macchine];
    perm1 = new TJob[dim_job];
    {//inizializzo le schedule locali
        CopiaSchedule(GMacch1_Sched,M1_sch_locale);
        schedule_locali[0] = M1_sch_locale;
        if(GNum_Macchine >= 2)
        {
            CopiaSchedule(GMacch2_Sched,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(GNum_Macchine == 3)
        {
            CopiaSchedule(GMacch3_Sched,M3_sch_locale);
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
    EliminaSchedula(M1_sch_locale);
    if(GNum_Macchine >= 2)
        EliminaSchedula(M2_sch_locale);
    if(GNum_Macchine == 3)
        EliminaSchedula(M3_sch_locale);

    delete(schedule_locali);
    delete(array_job_locale);
    return perm1;

}















