#include "Schedula.h"
#include "Globals.h"
#include <stdlib.h>

//*************************************************************************************************
//la funzione seguente aggiunge un singolo job alla schedula
//*************************************************************************************************
void AggiungiSchedula ( TSchedula *pSched, TJob pTask,int pTime,int pSetup_Vett )
{
    //dichiarazione di variabili
    TSchedula *nuovo;
    TSchedula *temp;
    temp = pSched;
    //__________________________________________________________
    while ( temp->next != NULL ) //trovo l'ultimo elemento della lista
        temp = temp->next;

    if ( temp->ID_job != -3 )
    {
        nuovo = new TSchedula;
        temp->next = nuovo;
        if
                (
                 ( pSetup_Vett == 2 ) //eseguo un setup di tipo Cmaj
                 ||
                 ( temp->tipo != pTask.tipo )
                 )
        {
            nuovo->index_camp = 0;//azzero il campo
        }
        else
            nuovo->index_camp= temp->index_camp+1;
    }
    else
    {
        nuovo = temp;
        nuovo->index_camp = 1;
    }
    nuovo->ID_job = pTask.ID;
    nuovo->tipo = pTask.tipo;
    nuovo->next = NULL;
    nuovo->inizio = pTime;
    nuovo->fine = nuovo->inizio + pTask.proc_time;

    if ( pTask.duedate !=0 )
        nuovo->Lmax = nuovo->fine -pTask.duedate;
    else
        nuovo->Lmax = -65000;

    if
            (
             ( nuovo->fine > pTask.duedate )
             &&
             ( pTask.duedate != 0 )
             )
    {
        nuovo->Tardy =  1;
    }
    else
        nuovo->Tardy = 0;

    return;

}

void InizializzaSchedula  ( TSchedula *pSched )
{
    pSched->ID_job = -3;
    pSched->tipo = 0;
    pSched->inizio = 0;
    pSched->fine = 0;
    pSched->Lmax = 0;
    pSched->Tardy = 0;
    pSched->index_camp = 0;
    pSched->next = NULL;
}

void EliminaSchedula ( TSchedula *pSched )
{
    //devo ora liberare lo spazio delle schedule locali
    TSchedula *Sch_Attuale;
    TSchedula *Sch_Seguente;
    Sch_Attuale = pSched;
    while ( Sch_Attuale != NULL )
    {
        Sch_Seguente = Sch_Attuale->next; // save the following

        delete Sch_Attuale; // because it is not null

        Sch_Attuale = Sch_Seguente;
    }
}

void CopiaSchedule ( TSchedula *pSched1,TSchedula *pSched2 )
{
    /*int ID_job;
                int tipo;
                int inizio;
                int fine;
                int Lmax;
                int Tardy;
                int index_camp;
                struct sch *next;*/

    TSchedula *temp = pSched2;
    while ( pSched1->next != NULL )
    {
        temp->ID_job = pSched1->ID_job;
        temp->tipo = pSched1->tipo;
        temp->inizio = pSched1->inizio;
        temp->fine = pSched1->fine;
        temp->Lmax = pSched1->Lmax;
        temp->Tardy = pSched1->Tardy;
        temp->index_camp = pSched1->index_camp;

        pSched1 = pSched1->next;
        temp->next = new TSchedula;
        temp = temp->next;
    }
    temp->ID_job = pSched1->ID_job;
    temp->tipo = pSched1->tipo;
    temp->inizio = pSched1->inizio;
    temp->fine = pSched1->fine;
    temp->Lmax = pSched1->Lmax;
    temp->Tardy = pSched1->Tardy;
    temp->index_camp = pSched1->index_camp;//finisco di copiare l'ultimo elemento
    temp->next = NULL;

}

void  AzzeraSchedule ( void )
{
    if ( GMacch1_Sched != NULL )
        EliminaSchedula(GMacch1_Sched);

    // creo ed inizializzo la schedula
    GMacch1_Sched = new TSchedula;
    InizializzaSchedula(GMacch1_Sched);
    if ( GNum_Macchine >= 2 )
    {

        if ( GMacch2_Sched != NULL )
            EliminaSchedula(GMacch2_Sched);

        GMacch2_Sched = new TSchedula;
        InizializzaSchedula(GMacch2_Sched);
    }
    if ( GNum_Macchine == 3 )
    {
        if ( GMacch3_Sched != NULL )
            EliminaSchedula(GMacch3_Sched);

        GMacch3_Sched = new TSchedula;
        InizializzaSchedula(GMacch3_Sched);
    }
}


void DistruggiSchedule ( int pNum_Macchine )
{
    if ( pNum_Macchine == 2 ) //mi trovo nel caso di dispencing
    {
        EliminaSchedula ( GMacch1_Sched );
        EliminaSchedula ( GMacch2_Sched );
    }
    else//mi trovo nel caso di counting
    {
        EliminaSchedula ( GMacch1_Sched );
        EliminaSchedula ( GMacch2_Sched );
        EliminaSchedula ( GMacch3_Sched );
    }

}

int CmaxSchedula ( TSchedula *pSched )
{
    // 	lo scopo di questa funzione e' quello di individuare il valore del tempo di completamento di una schedula passatagli come parametro.
    if ( pSched == NULL ) return 0;

    while ( pSched->next != NULL )
        pSched = pSched->next;

    return pSched->fine;

}

int MaxCmaxSchedula ( TSchedula **pVett_Schedule,int pNum_Macchine )
{
    // questa funzione considera la schedule separatamente e poi calcola il massimo del loro tempo di completamento.
    int *vettore_dei_tempi; //conterra'  i tempi di completamento delle macchine.
    int massimo; //conterra'  il valore del massimo.
    vettore_dei_tempi = new int[pNum_Macchine];
    int i = 0;

    for ( i = 0; i < pNum_Macchine; i++ )
        vettore_dei_tempi[i] = CmaxSchedula ( pVett_Schedule[i] );

    // a questo punto devo calcolare il max di tali elementi.
    massimo = vettore_dei_tempi[0];

    for ( i = 1; i < pNum_Macchine; i++ )
        if ( vettore_dei_tempi[i]>massimo )
            massimo = vettore_dei_tempi[i];

    delete[] vettore_dei_tempi;
    return massimo;
}

int MinCmaxSchedula ( TSchedula **pVett_Schedule,int pNum_Macchine )
{
    // questa funzione considera la schedule separatamente e poi calcola il minimo del loro tempo di completamento.
    int *vettore_dei_tempi; //conterra'  i tempi di completamento delle macchine.
    int minimo; //conterra'  il valore del minimo.
    vettore_dei_tempi = new int[pNum_Macchine];
    int i = 0;
    for ( i = 0; i < pNum_Macchine; i++ )
        vettore_dei_tempi[i] = CmaxSchedula ( pVett_Schedule[i] );

    // a questo punto devo calcolare il max di tali elementi.
    minimo = vettore_dei_tempi[0];
    for ( i=1; i < pNum_Macchine; i++ )
        if ( vettore_dei_tempi[i]<minimo )
            minimo = vettore_dei_tempi[i];


    delete[] vettore_dei_tempi;
    return minimo;
}

TQuaterna *ValutaSingolaSchedula ( TSchedula *pSched )
{
    TQuaterna *quaterna_di_lavoro;
    quaterna_di_lavoro = new TQuaterna;
    quaterna_di_lavoro->Lmax = -65000;
    quaterna_di_lavoro->Tardy = 0;
    quaterna_di_lavoro->Feasible = 1;
    TSchedula *temp     = NULL;
    TSchedula *temp1    = NULL;
    temp = pSched;
    int k;
    while ( temp!=NULL )
    {
        for ( k=0;k<GNum_Job;k++ )
        {
            if ( temp->ID_job == GArray_Job[k].ID )
                break;

        }
        if
                (
                 ( GArray_Job[k].duedate!=0 )
                 &&
                 (
                     ( temp->fine - GArray_Job[k].duedate )
                     >
                     quaterna_di_lavoro->Lmax
                     )
                 )
        {
            quaterna_di_lavoro->Lmax = ( temp->fine - GArray_Job[k].duedate );
        }
        if
                (
                 ( GArray_Job[k].duedate!= 0 )
                 &&
                 ( ( temp->fine - GArray_Job[k].duedate ) > 0 )
                 )
        {
            quaterna_di_lavoro->Tardy++;
        }
        if
                (
                 ( GArray_Job[k].deadline != 0 )
                 &&
                 ( ( temp->fine - GArray_Job[k].deadline ) > 0 )
                 )
        {
            quaterna_di_lavoro->Feasible = 0;
        }
        temp1 = temp;
        temp  = temp->next;
    }

    quaterna_di_lavoro->Cmax = temp1->fine;
    return quaterna_di_lavoro;
}
