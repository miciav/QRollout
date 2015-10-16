
/***************************************************************************
 *   Copyright (C) 2005 by Michele                                         *
 *   Ciavotta@dia.uniroma3.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*#ifdef HAVE_CONFIG_H
#include <config.h>
#endif*/
#include "qRolloutThread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define Max_tipi 40
//#include "rollout_time.cpp"
#include "Euristiche.cpp"
#include "Ricerche_Locali.cpp"
#include <QtCore>
#include <qstring.h>
#include <sys/time.h>
#include <stdint.h>

#if defined(WIN32)
    #include <windows.h>
#else
    #include <sys/resource.h>
#endif


void QRolloutThread::CaricaFileConfigurazione ( char *pPathFileConf )
{
    FILE *fileConf;
    if ( ( fileConf = fopen ( pPathFileConf,"r" ) ) == NULL )
        emit ScriviASchermo( "\n errore nel file di configurazione \n" );
    else
    {
        //0 -> rollout   -   1->euristica
        fscanf ( fileConf,"%d \n",&FRollout_Or_Heuristic );
        //scelta euristica da lanciare singolarmente
        fscanf ( fileConf,"%d \n",&FEuristica );
        //scelta del tipo di rollout da lanciare
        fscanf ( fileConf,"%d \n",&Ftipo_Rollout );
        //scelta dell'algoritmo euristico da utilizzare nel rollout
        fscanf ( fileConf,"%d \n",&Ftipo_Eur );
        //scelta del tempo
        fscanf ( fileConf,"%d \n",&FTempo );
        //percorso+nome del file di output della computazione
        fscanf ( fileConf,"%s \n",FOutput_File );
        fscanf ( fileConf,"%d \n",&Fforce );
        //scelta della politica di pruning, utilizzata solo in alcuni tipi di rollout
        fscanf ( fileConf,"%d \n",&FPolitica_Pruning );
        //scelta del tipo di ricerca locale da applicare successivamente al rollout (0 se non la si vuole applicare)
        fscanf ( fileConf,"%d",&FLocal_Search_Mode );

        fclose ( fileConf );
        if ( FRollout_Or_Heuristic == 1 )
            Ftipo_Eur = 17; //perche' si deve poter scegliere, tramite la variabile "euristica", tra tutte le euristiche disponibili.
    }
}

//*************************************************************************************************
// MAIN: versione di antonio maccioni adattata al testing su rollout dinamici e local search.
//***********************************************************************************************
void QRolloutThread::run ()
{
    // array/lista della migliore permutazione che trovera' il rollout che si andra' ad eseguire
    /*Lo faccio qui perch sempre in questa funzione faccio il delete*/
    GBest_Perm = new TJob[GNum_Job];

    FTempo_Sec_Inizio1 = time ( NULL ) ;

    FTempo_Inizio1 = clock(); //misurazione del rollout

    //ROLLOUT O LANCIO DELL'EURISTICA: TROVA LA PERMUTAZIONE BASE
    switch ( FRollout_Or_Heuristic )
    {
    case 0:
    {
        switch ( Ftipo_Rollout ) // stabilisco che tipo di rollout utilizzare.
        {
        case 0:
        {
            FPermutazione_Finale = Rollout_Old ( Fforce );
            break;
        }
        case 1:
        {
            FPermutazione_Finale = Rollout ( Fforce );
            break;
        }
        case 2:
        {
            Fswap_Lat_Tard = 1;
            FPermutazione_Finale = Rollout_Old ( Fforce );
            break;
        }
        case 3:
        {
            Fswap_Lat_Tard = 1;
            FPermutazione_Finale = Rollout ( Fforce);
            break;
        }
        case 4:
        {
            FPermutazione_Finale = Rollout_Modificato1 ( Fforce );
            break;
        }
        case 5:
        {
            FPermutazione_Finale = Rollout_Modificato2 ( Fforce);
            break;
        }
        case 6:
        {
            FPermutazione_Finale = Rollout_Modificato3 ( Fforce );
            break;
        }
        case 7:
        {
            FPermutazione_Finale = Rollout_Modificato4 ( Fforce);
            break;
        }
        case 8:
        {
            FPermutazione_Finale = Rollout_Modificato5 ( Fforce);
            break;
        }
        case 9:
        {
            FPermutazione_Finale = Rollout_Modificato6 ( Fforce );
            break;
        }
        case 10:
        {
            FPermutazione_Finale = Rollout_Heuristic_Pruning ( Fforce );
            break;
        }
            /*
                                case 11:
                                {
                                        permutazione_finale = rollout_time ( force,instance_file,Tempo );
                                        break;
                                }
                                */
        case 12:
        {
            FPermutazione_Finale = Rollout_Dynamic_Job_Choosing (Fforce);
            break;
        }
        case 13:
        {
            FPermutazione_Finale = Rollout_Dynamic (Fforce);
            break;
        }
        }
        break;

    }
        // nel case 1 lancia l'euristica scelta
    case 1:
    {
        FPermutazione_Finale = Ffunzioni[FEuristica].funz ( GArray_Job,GNum_Job );
        break;
    }
    }

        FTempo_Sec_Fine1 = time ( NULL );
        FTempo_Fine1 = clock();
        Schedula::AzzeraSchedule();
        CostruisciEValutaSchedula (GMacch1_Sched,
                                   GMacch2_Sched,
                                   GMacch3_Sched,
                                   Fprossimo,
                                   FPermutazione_Finale,
                                   GNum_Job );

        StampaRisultatiSuFile ( FfileOut,FInstance_File_Name,0,Fforce );

        int c;

        //RICERCA LOCALE (PURA, CODA, CON VND) O VNS
        FTempo_Inizio2 = clock() ;
        switch(FLocal_Search_Mode)
        {
            case 0:
            {
                //nor local search neither VNS
                break;
            }
            case 1:
            {
                c = VNS(GMacch1_Sched,GMacch2_Sched,GMacch3_Sched);
                break;
            }
            case 2:
            {
                c = RicercaLocaleTraMacchine(GMacch1_Sched, GMacch2_Sched, GMacch3_Sched);
                break;
            }
            case 3:
            {
                c = RicercaLocaleTraMacchineVND(GMacch1_Sched, GMacch2_Sched, GMacch3_Sched);
                break;
            }
            case 4:
            {
                c = RicercaLocaleTraMacchineCoda(GMacch1_Sched, GMacch2_Sched, GMacch3_Sched);
                break;
            }
        }

        FTempo_Fine2 = clock();

        ValutaSchedula (GMacch1_Sched,
                         GMacch2_Sched,
                         GMacch3_Sched,
                         Fprossimo );

        if (FLocal_Search_Mode == 0)
            StampaRisultatiAVideo ( 0 );
        else
            StampaRisultatiAVideo ( 1 );

        StampaRisultatiSuFile (FfileOut,
                                  FInstance_File_Name,
                                  1,Fforce );

        //BILANCIAMENTO
        FTempo_Inizio3 = clock() ;

        //bilancio
        BilanciamentoSchedule (GMacch1_Sched,
                                GMacch2_Sched,
                                GMacch3_Sched );

        FTempo_Fine3 = clock();

        ValutaSchedula (GMacch1_Sched,
                         GMacch2_Sched,
                         GMacch3_Sched,
                         Fprossimo );

        StampaRisultatiAVideo ( 2 );
        StampaRisultatiSuFile (FfileOut,
                                  FInstance_File_Name,
                                  2,
                                  Fforce );

        DistruggiIndisponibilita ( GNum_Macchine );
        Schedula::DistruggiSchedule ( GNum_Macchine );
        delete Fprossimo;
        delete Ffunzioni;
        delete GArray_Job; //disalloco la memoria occupata dall'array dei job.
        delete GArray_Tipi;//disalloco la memoria occupata dall'array dei tipi.
        delete[] FPermutazione_Finale;
        fclose ( Fistanza );
        fclose ( FfileOut );
}
//**************************************************************************************************
//la funzione seguente carica i tempi di indisponibilita' delle macchine dal file,
//e restituisce il numero di macchine caricate o 0 se qualche errore e' stato
//individuato nel formato del file.
//**************************************************************************************************
int QRolloutThread::CaricaIndisponibilita ( FILE *istanza )
{
    char str[10];
    char macchina[2];
    int totale_macchine = 0;
    int numM1 = 0;
    int numM2 = 0;
    int numM3 = 0,i;
    TElem *attuale;

    try {

        for ( i = 0; i < 10; i++ )
            str[i]='\n';


        fscanf ( istanza,"%s",macchina ); //carico il nome della macchina che normalmente e' M1
        if ( strcmp ( macchina , "M1" ) )
        {
            //emit ScriviASchermo( "ERRORE No M1\n" );
            return 0; //zero significa che il caricamento non e' andato a buon fine
        }
        totale_macchine++;
        GMacch1 = new TElem;
        fscanf ( istanza,"%s ",str ); //carico il numero di di inisponibilita' per la macchina M1
        numM1 = atoi ( str );
        fscanf ( istanza,"%s",str );

        GMacch1->inizio = atoi ( str );
        fscanf ( istanza,"%s",str );
        GMacch1->fine = GMacch1->inizio + atoi ( str );
        attuale = GMacch1;
        i=1;
        fscanf ( istanza,"%s",str );
        while (
               strcmp ( str , "M2" )

               &&

               strcmp ( str , "%%" )
               )
        {
            if ( i > numM1 )
            {
                emit ScriviASchermo(  "ERROR no M2" );
                return 0; //zero significa che il caricamento non e' andato a buon fine
            }
            attuale->next =new TElem;
            attuale = attuale->next;
            attuale->inizio = atoi ( str );
            fscanf ( istanza,"%s",str );
            attuale->fine = attuale->inizio + atoi ( str );
            fscanf ( istanza,"%s",str );
            i++;
        }
        attuale->next = NULL;
        if ( !strcmp ( str , "M2" ) )
        {
            totale_macchine++;
            i=1;
            fscanf ( istanza,"%s",str ); //carico il numero di indisponibilit��?della M2
            numM2=atoi ( str );
            fscanf ( istanza,"%s",str );
            GMacch2 =new TElem;
            GMacch2->inizio =atoi ( str );
            fscanf ( istanza,"%s",str );
            GMacch2->fine = GMacch2->inizio+atoi ( str );
            attuale = GMacch2;
            fscanf ( istanza,"%s",str );
            while (
                   strcmp ( str , "M3" )

                   &&

                   strcmp ( str , "%%" )
                   )
            {
                if ( i > numM2 )
                {
                    emit ScriviASchermo(  "ERROR no M3" );
                    return 0; //zero significa che il caricamento non e' andato a buon fine
                }
                attuale->next =new TElem;
                attuale = attuale->next;
                attuale->inizio = atoi ( str );
                fscanf ( istanza,"%s",str );
                attuale->fine = attuale->inizio + atoi ( str );
                fscanf ( istanza,"%s",str );
                i++;
            }
            attuale->next = NULL;
            if ( !strcmp ( str , "M3" ) )
            {
                i=1;
                totale_macchine++;
                fscanf ( istanza,"%s",str ); //carico il numero di indisponibilit��?della M3
                numM3 = atoi ( str );
                fscanf ( istanza,"%s",str );
                GMacch3 =new TElem;
                GMacch3->inizio =atoi ( str );
                fscanf ( istanza,"%s",str );
                GMacch3->fine = GMacch3->inizio+atoi ( str );
                attuale = GMacch3;
                fscanf ( istanza,"%s",str );
                while ( strcmp ( str , "%%" ) )
                {
                    if ( i>numM3 )
                    {
                        emit ScriviASchermo(  "ERROR in M3" );
                        return 0; //zero significa che il caricamento non e' andato a buon fine
                    }
                    attuale->next =new TElem;
                    attuale = attuale->next;
                    attuale->inizio = atoi ( str );
                    fscanf ( istanza,"%s",str );
                    attuale->fine = attuale->inizio + atoi ( str );
                    fscanf ( istanza,"%s",str );
                    i++;
                }
                attuale->next = NULL;
            }
        }
        return totale_macchine; //il caricamento ��?andato a buon fine
    }
    catch(...) { // catch all exceptions
        return 0;
    }


}

//***********************************************************************************************
//la funzione seguente elimina la memoria occupata dalle lista dei tempi di indisponibilita'
//***********************************************************************************************
void QRolloutThread::DistruggiIndisponibilita ( int pNum_macchine )
{
    EliminaLista ( GMacch1 );
    if ( pNum_macchine >= 2 ) //mi trovo nel caso di dispencing
        EliminaLista ( GMacch2 );
    if ( pNum_macchine == 3 )//mi trovo nel caso di counting
        EliminaLista ( GMacch3 );

}

//*****************************************************************************************************
//la funzione seguente elimina le liste di indisponibilita' ed e' asservita a distruggi_indisponibilita
//*****************************************************************************************************
void QRolloutThread::EliminaLista ( TElem * pPunt_lista )
{
    TElem *temp1;
    temp1 = pPunt_lista;
    while ( temp1->next != NULL )
    {
        TElem *temp;
        temp = temp1;
        temp1 = temp1->next;
        delete temp;
    }
}
//*************************************************************************************************
//la funzione seguente carica tutte le matrici necessarie a far funzionare il programma
//*************************************************************************************************

int QRolloutThread::CaricaListaJob ( FILE * pIstanza )
{
    char str[10];
    fscanf ( pIstanza,"%s",str );
    // a questo punto il file dovrebbe essere nella posizione che mi aspetto
    //cioe' dove si trovano le info sui job.

    if ( !strcmp ( str,"//" ) )
    {
        fscanf ( pIstanza,"%s",str );
        GNum_Job  = atoi ( str );
        fscanf ( pIstanza,"%s",str );
        if ( !strcmp ( str,"%%" ) )
        {
            GArray_Job = new TJob[GNum_Job];
            int i = 0;
            fscanf ( pIstanza,"%s",str );
            while ( strcmp ( str,"//" ) )
            {
                GArray_Job[i].ID = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Job[i].tipo = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Job[i].proc_time = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Job[i].duedate = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Job[i].deadline = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Job[i].priority = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Job[i].rel_time = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                if ( strcmp ( str,"%%" ) )
                {
                    if
                            (
                             strcmp ( str,"//" )
                             ||
                             ( i>=GNum_Job )
                             ) //verifico che non ci siano errori nella strutture
                    {		    //dei campi
                        emit ScriviASchermo(  "\n error sulla struttura dei job" );
                        return 0;
                    }
                }
                else
                {
                    i++;
                    fscanf ( pIstanza,"%s",str );
                }
            }
        }
        else
        {
            emit ScriviASchermo(  "\n error sul numero job" );
            return 0;
        }
        //a questo punto se i job ed i loro attributi sono stati caricati correttamente
        //il programma carica dal file i tipi e i MaxOpCamp.
        fscanf ( pIstanza,"%s",str );
        GNum_Tipi = atoi ( str );
        GArray_Tipi = new TTipo[GNum_Tipi];
        fscanf ( pIstanza,"%s",str );
        if ( !strcmp ( str,"%%" ) )
        {
            int i = 0;
            fscanf ( pIstanza,"%s",str );
            while ( strcmp ( str,"//" ) )
            {
                GArray_Tipi[i].ID = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                GArray_Tipi[i].MaxOpCamp = atoi ( str );
                fscanf ( pIstanza,"%s",str );
                if ( strcmp ( str,"%%" ) )
                {
                    if
                            (
                             strcmp ( str,"//" )
                             ||
                             ( i>=GNum_Job )
                             ) //verifico che non ci siano errori nella strutture
                    {		    //dei campi
                        emit ScriviASchermo(  "\n error sulla struttura dei tipi" );
                        return 0;
                    }
                }
                else
                {
                    i++;
                    fscanf ( pIstanza,"%s",str );
                }
            }
        }
        else
        {
            emit ScriviASchermo(  "\n error sul numero dei tipi" );
            return 0;
        }
        int j = 0;
        fscanf ( pIstanza,"%s",str );
        while ( strcmp ( str,"//" ) )
        {
            int i = 0;
            for ( i=0;i<GNum_Tipi;i++ )
            {
                GCmaj_Matrix[j][i] = atoi ( str );
                fscanf ( pIstanza,"%s",str );
            }
            fscanf ( pIstanza,"%s",str );

            if ( j >= GNum_Tipi )
            {
                emit ScriviASchermo(  "\n error nella matrice dei tipi" );
                return 0;
            }
            j++;
        }
        return 1;
    }
    else return 0;

}
//*************************************************************************************************
//la funzione seguente costruisce le schedule a partire da una permutazione dei job e la valuta
//*************************************************************************************************

int QRolloutThread::CostruisciEValutaSchedula (  TSchedula *pM1_Sch,
                                                 TSchedula *pM2_Sch,
                                                 TSchedula *pM3_Sch,
                                                 TNext_Elem *pProx_Elem,
                                                 TJob *pPerm,
                                                 int pDim_job )
{
    //per ogni macchina in cui il tempo disponibile nn e' utilizzabile verra' considerato un job fittizio.
    //regole
    //1)il job viene assegnato alla macchina disponibile che lo possa accogliere
    //2)tra le macchine disponibili si sceglie quella che ha gia' schedulati job dello stesso tipo
    //3)altrimenti si sceglie la macchina con costi di setup + bassi questo vale anche quando il job
    // e' il primo della campagna.

    int i;
    TNext_Elem *temp_prox;
    int cambio;
    int k;
    int schedulato;
    TSchedula *temp_Sch;
    TElem *temp_Elem;
    //fine dichiarazione delle variabili
    // ____________________________________________________________________________________________________________________

    int *disponibilita  = new int[GNum_Macchine]; //ci dice quando la macchina e' disponibile
    int *setup_vett     = new int[GNum_Macchine]; //ci dice se la macchina deve attendere un setup
    int *set            = new int[GNum_Macchine];

    temp_prox = pProx_Elem;
    // busco el ultimo elemento
    while ( temp_prox->next != NULL ) // TODO questo si pu mettere in una funzione
        temp_prox = temp_prox->next;

    //trovo l'elemento giusto

    for ( i = 0; i < pDim_job; i++ ) // considero singolarmente i job della pemutazione e costruisco
    {                                // la schedula

        // FIXME questo  sbagliato no? Pensiamolo bene
        //_______________________________________________________________________________
        //inizializzo l'array della disponibilita' e setup_vett
        disponibilita[0] = 0;
        setup_vett[0] = 0;
        if ( GNum_Macchine >= 2 )
        {
            disponibilita[1] = 0;
            setup_vett[1] = 0;
        }
        if ( GNum_Macchine == 3 )
        {
            disponibilita[2] = 0;
            setup_vett[2] = 0;
        }

        //_______________________________________________________________________________
        // calcolo le disponibilita' delle macchine
        temp_Sch = GMacch1_Sched;
        temp_Elem = GMacch1;
        VerificaMacchina (temp_Sch,
                           temp_Elem,
                           disponibilita,
                           setup_vett,0,pPerm,i );
        if ( GNum_Macchine >= 2 )
        {
            temp_Sch = GMacch2_Sched;
            temp_Elem = GMacch2;
            VerificaMacchina ( temp_Sch,
                               temp_Elem,
                               disponibilita,
                               setup_vett,1,pPerm,i );
        }
        if ( GNum_Macchine == 3 )
        {
            temp_Sch = GMacch3_Sched;
            temp_Elem = GMacch3;
            VerificaMacchina ( temp_Sch,
                               temp_Elem,
                               disponibilita,
                               setup_vett,2,pPerm,i );
        }

        //_______________________________________________________________________________________
        //a questo punto ho in disponibilita' i tempi in cui e' possibile schedulare un job scelgo
        //a parita' di tempo la macchina che mi riduce i setup
        cambio = 0;
        k = 0;
        schedulato = 0;
        for ( k = 0; k < GNum_Macchine; k++ )
            set[k] = -1;//inizializzo il set delle macchine

        set[cambio] = 1;
        for ( k = 1; k < GNum_Macchine; k++ )
        {
            if ( disponibilita[cambio] > disponibilita[k] )
            {
                int j = 0;
                cambio = k;
                for ( j = 0; j < GNum_Macchine; j++ )
                    set[j] = -1;

                set[k] = 1;
            }
            else if ( disponibilita[cambio] == disponibilita[k] )
                set[k] = 1;

        }

        //____________________________________________________________________________________________
        //a questo punto so quali macchine hanno tempo minimo uguale
        for ( k = 0; k < GNum_Macchine; k++ )
        {
            if ( set[k] == 1 && setup_vett[k] == 0 )
            {//allora schedulo il job sulla macchina k-esima
                schedulato = 2; //caso del primo job, quando non si deve pagare nessun setup
                // TODO forse un casa sarebbe meglio
                if ( k == 0 ) // prima macchina
                {
                    Schedula::AggiungiSchedula (GMacch1_Sched,
                                       pPerm[i],
                                       disponibilita[k],
                                       setup_vett[k] );
                    if ( i == 0 ) //mi serve l'info sul 1 job
                        Aggiorna_Temp_Prox ( k,pPerm[i],GMacch1_Sched,temp_prox );

                    break;
                }
                else if ( k == 1 ) // seconda macchina
                {
                    Schedula::AggiungiSchedula (GMacch2_Sched,
                                       pPerm[i],
                                       disponibilita[k],
                                       setup_vett[k] );
                    if ( i == 0 ) //mi serve l'info sul 1 job
                        Aggiorna_Temp_Prox ( k,pPerm[i],GMacch2_Sched,temp_prox );

                    break;
                }
                else	// terza macchina
                {
                    Schedula::AggiungiSchedula (GMacch3_Sched,
                                       pPerm[i],
                                       disponibilita[k],
                                       setup_vett[k] );
                    if ( i == 0 ) //mi serve l'info sul 1 job
                        Aggiorna_Temp_Prox ( k,pPerm[i],GMacch3_Sched,temp_prox );

                    break;
                }
            }

        }
        if ( schedulato != 2 ) // se al passo precedente non ho schedulato il job corrente
        {
            for ( k = 0; k < GNum_Macchine; k++ )
            {
                if ( set[k] == 1 && setup_vett[k] == 1 )
                {
                    schedulato = 1; //setup di tipo minor
                    if ( k == 0 )	// schedulo sulla prima macchina
                    {
                        Schedula::AggiungiSchedula (GMacch1_Sched,
                                           pPerm[i],
                                           disponibilita[k],
                                           setup_vett[k] );
                        if ( i == 0 ) //mi serve l'info sul 1 job
                            Aggiorna_Temp_Prox ( k,pPerm[i],GMacch1_Sched,temp_prox );

                        break;
                    }
                    else if ( k == 1 )	//schedulo sulla seconda macchina
                    {
                        Schedula::AggiungiSchedula (GMacch2_Sched,
                                           pPerm[i],
                                           disponibilita[k],
                                           setup_vett[k] );
                        if ( i == 0 ) //mi serve l'info sul 1 job
                            Aggiorna_Temp_Prox ( k,pPerm[i],GMacch2_Sched,temp_prox );

                        break;
                    }
                    else	//schedulo sulla terza macchina
                    {
                        Schedula::AggiungiSchedula (GMacch3_Sched,
                                           pPerm[i],
                                           disponibilita[k],
                                           setup_vett[k] );
                        if ( i == 0 ) //mi serve l'info sul 1 job
                            Aggiorna_Temp_Prox ( k,pPerm[i],GMacch3_Sched,temp_prox );

                        break;
                    }
                }
            }
            if ( schedulato == 0 )	// se ho un setup di tipo major
                for ( k = 0; k < GNum_Macchine; k++ )
                {
                    if ( set[k] == 1 )
                    {
                        if ( k == 0 )	// schedulo sulla prima macchina
                        {
                            Schedula::AggiungiSchedula (GMacch1_Sched,
                                               pPerm[i],
                                               disponibilita[k],
                                               setup_vett[k] );
                            if ( i == 0 ) //mi serve l'info sul 1 job
                                Aggiorna_Temp_Prox ( k,pPerm[i],GMacch1_Sched,temp_prox );

                            break;
                        }
                        else if ( k == 1 )	// schedulo sulla seconda macchina
                        {
                            Schedula::AggiungiSchedula (GMacch2_Sched,
                                               pPerm[i],
                                               disponibilita[k],
                                               setup_vett[k] );

                            if ( i == 0 ) //mi serve l'info sul 1 job
                                Aggiorna_Temp_Prox ( k,pPerm[i],GMacch2_Sched,temp_prox );

                            break;
                        }
                        else	//schedulo sulla terza macchina
                        {
                            Schedula::AggiungiSchedula (GMacch3_Sched,
                                               pPerm[i],
                                               disponibilita[k],
                                               setup_vett[k] );
                            if ( i == 0 ) //mi serve l'info sul 1 job
                                Aggiorna_Temp_Prox ( k,pPerm[i],GMacch3_Sched,temp_prox );

                            break;
                        }
                    }
                }
        }
    }
    ValutaSchedula (pM1_Sch,
                     pM2_Sch,
                     pM3_Sch,
                     temp_prox );
    delete[] set;
    delete[] disponibilita;
    delete[] setup_vett;
    return 1;
}

//*************************************************************************************************
//la funzione seguente aggiorna il valore di Temp_prox variabile utilizzata da Costruisci e valuta schedula
//***********************************************************************************************
void QRolloutThread::Aggiorna_Temp_Prox ( int k,TJob pPerm,TSchedula *pM_sch,TNext_Elem *pTemp_Prox )
{
    TSchedula *temp_macch;

    pTemp_Prox->macchina = k + 1;
    temp_macch = pM_sch;
    while ( temp_macch->next != NULL )
        temp_macch = temp_macch->next;

    pTemp_Prox->tipo         = temp_macch->tipo;
    pTemp_Prox->inizio       = temp_macch->inizio;
    pTemp_Prox->fine         = temp_macch->fine;
    pTemp_Prox->Lmax_pers    = temp_macch->Lmax;
    pTemp_Prox->Tardy_pers   = temp_macch->Tardy;
    pTemp_Prox->index_camp   = temp_macch->index_camp;
    pTemp_Prox->proc_time    = pPerm.proc_time;
    pTemp_Prox->rel_time     = pPerm.rel_time;
    pTemp_Prox->duedate      = pPerm.duedate;
    pTemp_Prox->deadline     = pPerm.deadline;
    pTemp_Prox->priority     = pPerm.priority;

}

//*************************************************************************************************
// la funzione seguente calcola il tempo minimo di schedulazione su ciascuna macchina per ogni job
// tiene conto dei setup e dei tempi di indisponibilita' delle macchine
//***********************************************************************************************
void VerificaMacchina( TSchedula *pSchedula,
                       TElem  *pElem,
                       int *pDisponibilita,
                       int *pSetup_vett,
                       int p,
                       TJob *perm,
                       int i)
{
    int time_max = 0;
    int time_min = 0;
    int k;
    //int a=0;
    int slack1 = 0; //slack time, puo' essere utilizzato per eseguire setup prima della release date.

    while(pSchedula->next!=NULL)
        pSchedula = pSchedula->next;//cerco l'ultimo elemento schedulato

    //devo tener conto del massimo tra il momento in cui la macchina si libera ed il rel. time del job
    if (pSchedula->fine <= perm[i].rel_time)
    {
        time_max = perm[i].rel_time;
        time_min = pSchedula->fine;
    }
    else
    {
        time_max = pSchedula->fine;
        time_min = perm[i].rel_time;
    }
    //la macchina 1 potrebbe essere disponibile
    //bisogna verificare se il job e' compatibile con le indisponibilit�
    while	(
                 (pElem->inizio < pSchedula->fine)
                 &&//cerco la prima indisponibilita' successiva all'ultimo job schedulato
                 (pElem->next != NULL)//potrebbe nn esserci una indisp. successiva
                 )
    {
        pElem = pElem->next;
    }
    if (pElem->inizio < pSchedula->fine)
    {
        //non ho indisponibilita' e posso calcolare il momento di disp. semplicemente in base alla rel_date
        slack1 = perm[i].rel_time-pSchedula->fine-1;	//posso sfruttare lo slack time per eseguire il setup
        // lo slack time puo' essere sia positivo che negativo ed e' il tempo che intercorre tra la fine del job
        if (slack1 <= 0)//non posso sfruttare il tempo slack
        {
            for(k = 0; k < GNum_Tipi; k++)
            {
                if(GArray_Tipi[k].ID == perm[i].tipo)
                {
                    if(
                            (perm[i].tipo == pSchedula->tipo)
                            &&
                            (GArray_Tipi[k].MaxOpCamp < pSchedula->index_camp)
                            )
                    { //non devo pagare setup major quindi calcolo il tempo necessario ad eseguirlo
                        pSetup_vett[p] = 1;
                        pDisponibilita[p] = time_max + GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1;
                    }
                    else
                    {//in questo caso devo anche pagare un setup che pu�considerarsi come un ritardo dell'istante di inizio del processamento di un job.
                        // non puo' capitare che il setup possa essere eseguito nel tempo morto slack
                        pSetup_vett[p] = 2;
                        pDisponibilita[p] = time_max + GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1;
                    }
                    break;
                }
            }
        }
        else
        {//in questo caso ho un tempo di slack che mi permette di eseguire almeno parzialmente il setup
            //il momento di disponibilit� � pari al massimo tra time_mim + setup (eventuale) e time_max
            for(k=0;k<GNum_Tipi;k++)
            {
                if(GArray_Tipi[k].ID == perm[i].tipo)
                {
                    if
                            (
                             (perm[i].tipo == pSchedula->tipo)
                             &&
                             (GArray_Tipi[k].MaxOpCamp < pSchedula->index_camp)

                             )
                    { //non devo pagare setup quindi calcolo il tempo necessario ad eseguirlo
                        pSetup_vett[p] = 1;

                    }
                    else
                    {//in questo caso devo anche pagare un setup che pu�considerarsi come un ritardo dell'istante di inizio del processamento di un job.
                        // non puo' capitare che il setup possa essere eseguito nel tempo morto slack
                        pSetup_vett[p] = 2;

                    }
                    if	(
                            (time_min + GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1)
                            >
                            time_max
                            )
                    {
                        pDisponibilita[p] = time_min + GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1;
                    }
                    else pDisponibilita[p] = time_max;

                    break;
                }
            }
        }
    }
    else if (pElem->inizio >= pSchedula->fine)//nel tempo che avanza il job potrebbe entrarci
    {
        for(k = 0; k < GNum_Tipi; k++)
        {
            if(GArray_Tipi[k].ID == perm[i].tipo)
            {
                if (pSchedula->ID_job == -3)
                {// il primo job della schedula � fittizio quindi non devo pagare nè setup major nè minor
                    pSetup_vett[p] = 0;
                    if (
                            (pElem->inizio - time_max)
                            >=
                            perm[i].proc_time
                            )//posso inserire il job
                    {
                        pDisponibilita[p] = time_max ;//posso schedulare il job
                    }
                    else
                    {//in questo caso nn sono ancora sicuro della possibile data di inizio schedulazione
                        while (
                               (pElem->next != NULL)
                               &&
                               (
                                   (
                                       (pElem->next->inizio - pElem->fine)
                                       <
                                       perm[i].proc_time
                                       )
                                   ||
                                   (
                                       (pElem->next->inizio-time_max)
                                       <
                                       perm[i].proc_time
                                       )
                                   )
                               )
                        {
                            pElem = pElem->next;
                        }
                        if(pElem->fine >= time_max)
                            pDisponibilita[p] = pElem->fine;
                        else
                            pDisponibilita[p] = time_max;

                    }
                }
                else
                {
                    if	(
                            (perm[i].tipo == pSchedula->tipo)
                            &&
                            (GArray_Tipi[k].MaxOpCamp > pSchedula->index_camp)
                            )
                    { //non devo pagare setup major quindi calcolo il tempo necessario ad eseguirlo
                        pSetup_vett[p] = 1;
                    }
                    else pSetup_vett[p] = 2;


                    if (
                            (pElem->inizio - pSchedula->fine-1)
                            >=
                            (GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1])
                            )//controllo se posso inserire il job mentre gi�� so di poter inserire il setup
                    {
                        if(
                                (
                                    (
                                        (pSchedula->fine + GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1)
                                        >=
                                        perm[i].rel_time
                                        )
                                    &&
                                    (
                                        pElem->inizio - (pSchedula->fine + GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1)
                                        >=
                                        perm[i].proc_time
                                        )
                                    )

                                )
                        {//posso schedulare entrambi
                            pDisponibilita[p] = max(pSchedula->fine+GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]+1,perm[i].rel_time);
                        }
                        else//posso assegnare solo il setup mentre per il job devo attendere
                        {
                            while (
                                   (pElem->next!=NULL)
                                   &&
                                   (
                                       (
                                           (pElem->next->inizio - pElem->fine)
                                           <=
                                           perm[i].proc_time
                                           )
                                       ||
                                       (
                                           (pElem->next->inizio-time_max)
                                           <=
                                           perm[i].proc_time
                                           )
                                       )
                                   )
                            {
                                pElem=pElem->next;
                            }
                            if(pElem->fine>=time_max) pDisponibilita[p] = pElem->fine;
                            else pDisponibilita[p] = time_max;

                        }
                    }
                    else //non posso schedulare per intero neppure il setup
                    {//in questo caso nn sono ancora sicuro della possibile data di inizio Schedulazione
                        slack1 = pElem->inizio - pSchedula->fine-1;
                        slack1 = GCmaj_Matrix[pSchedula->tipo-1][perm[i].tipo-1]-slack1;
                        while (
                               (pElem->next!=NULL)
                               &&
                               (
                                   (
                                       (pElem->next->inizio - pElem->fine)
                                       <=
                                       perm[i].proc_time+slack1
                                       )
                                   ||
                                   (
                                       (pElem->next->inizio-time_max)
                                       <=
                                       perm[i].proc_time+slack1
                                       )
                                   )
                               )
                        {
                            if (slack1>0)
                                slack1 = slack1 - (pElem->next->inizio - pElem->fine);

                            if (slack1<0) slack1 = 0;// finisco di schedulare il setup

                            pElem=pElem->next;
                        }
                        if(pElem->fine >= time_max-slack1)
                            pDisponibilita[p] = pElem->fine + slack1;
                        else
                            pDisponibilita[p] = time_max;

                    }


                }
                break;
            }
        }
    }
}

//*************************************************************************************************
//la funzione seguente calcola il la lateness massima il makespan ed il numero di tardyjob
//***********************************************************************************************
void  QRolloutThread::ValutaSchedula (  TSchedula *pMacch1_sch,
                                        TSchedula *pMacch2_sch,
                                        TSchedula *pMacch3_sch,
                                        TNext_Elem *pProx_Elem )
{
    TSchedula *temp = NULL;
    TSchedula *temp1= NULL;

    int *Cmax_temp = new int[GNum_Macchine];
    int lat =  -65000;
    int tard = 0;
    int feasible = 1;
    int k = 0;
    temp = pMacch1_sch;
    while ( temp != NULL )
    {
        for ( k = 0; k < GNum_Job; k++ )
        {
            if ( temp->ID_job==GArray_Job[k].ID )
                break;
        }

        if
                (
                 ( GArray_Job[k].duedate!=0 )
                 &&
                 (
                     ( temp->fine - GArray_Job[k].duedate )
                     >
                     lat
                     )
                 )
        {
            lat = ( temp->fine - GArray_Job[k].duedate );
        }
        if
                (
                 ( GArray_Job[k].duedate!= 0 )
                 &&
                 ( ( temp->fine - GArray_Job[k].duedate ) > 0 )
                 )
        {
            tard++;
        }
        if
                (
                 ( GArray_Job[k].deadline != 0 )
                 &&
                 ( ( temp->fine - GArray_Job[k].deadline ) > 0 )
                 )
        {
            feasible = 0;
        }
        temp1 = temp;
        temp  = temp->next;
    }

    Cmax_temp[0] = temp1->fine;
    if ( GNum_Macchine >= 2 )
    {
        temp = pMacch2_sch;
        if ( temp->ID_job != -3 )
        {
            while ( temp != NULL )
            {

                for ( k = 0; k < GNum_Job; k++ )
                {
                    if ( temp->ID_job==GArray_Job[k].ID )
                    {
                        break;
                    }
                }
                if
                        (
                         ( GArray_Job[k].duedate!=0 )
                         &&
                         (
                             ( temp->fine - GArray_Job[k].duedate )
                             >
                             lat
                             )
                         )
                {
                    lat = ( temp->fine - GArray_Job[k].duedate );
                }
                if
                        (
                         ( GArray_Job[k].duedate!= 0 )
                         &&
                         ( ( temp->fine - GArray_Job[k].duedate ) > 0 )
                         )
                {
                    tard++;
                }
                if
                        (
                         ( GArray_Job[k].deadline != 0 )
                         &&
                         ( ( temp->fine - GArray_Job[k].deadline ) > 0 )
                         )
                {
                    feasible = 0;
                }
                temp1 = temp;
                temp  = temp->next;
            }
            Cmax_temp[1] = temp1->fine;
        }
        else
            Cmax_temp[1] = 0;

    }
    if ( GNum_Macchine == 3 )
    {
        temp = pMacch3_sch;
        if ( temp->ID_job != -3 )
        {
            while ( temp != NULL )
            {
                for ( k = 0; k < GNum_Job; k++ )
                {
                    if ( temp->ID_job == GArray_Job[k].ID )
                    {
                        break;
                    }
                }
                if
                        (
                         ( GArray_Job[k].duedate!=0 )
                         &&
                         (
                             ( temp->fine - GArray_Job[k].duedate )
                             >
                             lat
                             )
                         )
                {
                    lat = ( temp->fine - GArray_Job[k].duedate );
                }
                if
                        (
                         ( GArray_Job[k].duedate!= 0 )
                         &&
                         ( ( temp->fine - GArray_Job[k].duedate ) > 0 )
                         )
                {
                    tard++;
                }
                if
                        (
                         ( GArray_Job[k].deadline != 0 )
                         &&
                         ( ( temp->fine - GArray_Job[k].deadline ) > 0 )
                         )
                {
                    feasible = 0;
                }
                temp1= temp;
                temp = temp->next;
            }
            Cmax_temp[2]=temp1->fine;
        }
        else
            Cmax_temp[2] = 0;

    }
    int cambio = 0;
    for ( k = 1; k < GNum_Macchine; k++ )
    {
        if ( Cmax_temp[cambio] < Cmax_temp[k] )
            cambio = k;
    }
    FCmax = Cmax_temp[cambio];
    //_____________________________________________________________________ fine calcolo Cmax
    if ( Fswap_Lat_Tard==0 )
        FLmax = lat; //latenza massima
    else
        FLmax=Max ( lat,0 );//in questo caso la Lmax indica la Tardiness massima

    FTardy = tard;
    Ffeasible = feasible;
    //____________________________________________________ salvo le informazioni in prossimo

    pProx_Elem->Lmax = FLmax;
    pProx_Elem->Cmax = FCmax;
    pProx_Elem->Tardy= FTardy;
    if ( feasible == 1 )
        pProx_Elem->feasible = 1;
    else
        pProx_Elem->feasible = 0;

    delete[] Cmax_temp;
}

void QRolloutThread::StampaRisultatiAVideo ( int pFlag )
{
    TSchedula *temp;
    TElem *temp1;
    temp1 = GMacch1;
    temp = GMacch1_Sched;
    int i;
    if ( pFlag != 2 )
    {
        emit ScriviASchermo(  "\n\tMacchina 1:\n\t" );
        emit ScriviASchermo(  "Indisponibilita\n\t" );
        while ( temp1->next!=NULL )
        {
            emit ScriviASchermo(  QString("|%1-%2|__").arg(temp1->inizio).arg(temp1->fine) );
            temp1 = temp1->next;
        }
        emit ScriviASchermo(  QString("|%1-%2|\n\t").arg(temp1->inizio).arg(temp1->fine) );
        emit ScriviASchermo(  "------------------------------------------------------------------" );
        emit ScriviASchermo(  "\n\tSchedula:\n\t" );
        while ( temp->next!=NULL )
        {
            emit ScriviASchermo( QString( "|%1-%2|__").arg(temp->inizio).arg(temp->fine) );
            temp = temp->next;
        }
        emit ScriviASchermo(  QString( "|%1-%2|\n\t").arg(temp->inizio).arg(temp->fine) );
        temp = GMacch1_Sched;
        emit ScriviASchermo(  "\n\tPos\tJob\tTipo\tStart\tEnd\tCamp\tLat.\tTardy" );
        i = 0;
        while ( temp->next!=NULL )
        {
            emit ScriviASchermo(    QString("\n\t|%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8|").
                                arg(i++).
                                arg(temp->ID_job).
                                arg(temp->tipo).
                                arg(temp->inizio).
                                arg(temp->fine).
                                arg(temp->index_camp).
                                arg(temp->Lmax).
                                arg(temp->Tardy )
                                );
            temp = temp->next;
        }
        emit ScriviASchermo(    QString("\n\t|%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8|\n\t").
                            arg(i).
                            arg(temp->ID_job).
                            arg(temp->tipo).
                            arg(temp->inizio).
                            arg(temp->fine).
                            arg(temp->index_camp).
                            arg(temp->Lmax).
                            arg(temp->Tardy)
                            );
        emit ScriviASchermo(  "__________________________________________________________________\n\n" );
        if ( GNum_Macchine >= 2 )
        {
            temp = GMacch2_Sched;
            temp1 = GMacch2;
            emit ScriviASchermo(  "\n\tMacchina 2:\n\t" );
            emit ScriviASchermo(  "Indisponibilita\n\t" );
            while ( temp1->next!=NULL )
            {
                emit ScriviASchermo( QString( "|%1-%2|__").arg(temp1->inizio).arg(temp1->fine) );
                temp1 = temp1->next;
            }
            emit ScriviASchermo(  QString("|%1-%2|\n\t").arg(temp1->inizio).arg(temp1->fine) );
            emit ScriviASchermo(  "------------------------------------------------------------------" );
            emit ScriviASchermo(  "\n\tSchedula:\n\t" );

            while ( temp->next!=NULL )
            {
                emit ScriviASchermo(  QString("|%1-%2|__").arg(temp->inizio).arg(temp->fine) );
                temp = temp->next;
            }

            emit ScriviASchermo(  QString("|%1-%2|\n\t").arg(temp->inizio).arg(temp->fine) );
            temp=GMacch2_Sched;
            emit ScriviASchermo(  "\n\tPos\tJob\tipo\tStart\tEnd\tCamp\tLat.\tTardy" );
            i=0;
            while ( temp->next!=NULL )
            {
                emit ScriviASchermo( QString("\n\t|%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8|").
                                    arg(i++).
                                    arg(temp->ID_job).
                                    arg(temp->tipo).
                                    arg(temp->inizio).
                                    arg(temp->fine).
                                    arg(temp->index_camp).
                                    arg(temp->Lmax).
                                    arg(temp->Tardy) );
                temp = temp->next;
            }
            emit ScriviASchermo(  QString("\n\t|%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8|\n\t").
                                arg(i).
                                arg(temp->ID_job).
                                arg(temp->tipo).
                                arg(temp->inizio).
                                arg(temp->fine).
                                arg(temp->index_camp).
                                arg(temp->Lmax).
                                arg(temp->Tardy) );
            emit ScriviASchermo(  "__________________________________________________________________\n\n" );
        }
        if ( GNum_Macchine == 3 )
        {
            temp = GMacch3_Sched;
            temp1 = GMacch3;
            emit ScriviASchermo(  "\n\tMacchina 3:\n\t" );
            emit ScriviASchermo(  "Indisponibilita\n\t" );
            while ( temp1->next!=NULL )
            {
                emit ScriviASchermo(  QString("|%1-%2|__").arg(temp1->inizio).arg(temp1->fine) );
                temp1 = temp1->next;
            }
            emit ScriviASchermo(  QString("|%1-%2|\n\t").arg(temp1->inizio).arg(temp1->fine) );
            emit ScriviASchermo(  "------------------------------------------------------------------" );
            emit ScriviASchermo(  "\n\tSchedula:\n\t" );
            while ( temp->next!=NULL )
            {
                emit ScriviASchermo(  QString("|%1-%2|__").arg(temp->inizio).arg(temp->fine) );
                temp = temp->next;
            }
            emit ScriviASchermo(  QString("|%1-%2|\n\t").arg(temp->inizio).arg(temp->fine) );
            temp = GMacch3_Sched;
            emit ScriviASchermo(  "\n\tPos\tJob\tTipo\tStart\tEnd\tCamp\tLat.\tTardy" );
            i = 0;
            while ( temp->next!=NULL )
            {
                emit ScriviASchermo( QString("\n\t|%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8|").
                                    arg(i++).
                                    arg(temp->ID_job).
                                    arg(temp->tipo).
                                    arg(temp->inizio).
                                    arg(temp->fine).
                                    arg(temp->index_camp).
                                    arg(temp->Lmax).
                                    arg(temp->Tardy) );
                temp = temp->next;
            }


            emit ScriviASchermo(  QString("\n\t|%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8|\n\t").
                                arg(i).
                                arg(temp->ID_job).
                                arg(temp->tipo).
                                arg(temp->inizio).
                                arg(temp->fine).
                                arg(temp->index_camp).
                                arg(temp->Lmax).
                                arg(temp->Tardy) );
            emit ScriviASchermo(  "__________________________________________________________________\n\n" );
        }
    }
    emit ScriviASchermo(  QString("\tLmax: %1\tCmax: %2\tNum. Tardy Jobs: %3\tFeas.: %4\n").
                        arg(FLmax).
                        arg(FCmax).
                        arg(FTardy).
                        arg(Ffeasible) );

    if ( pFlag == 0 )
        emit ScriviASchermo(  QString("\tTempo impiegato Rollout : %1\n").arg( ( double ) ( FTempo_Fine1-FTempo_Inizio1 )) );
    else if ( pFlag == 1 )
    {
        emit ScriviASchermo(  QString("\tTempo impiegato VNS : %1\n").arg( ( double ) ( FTempo_Fine2-FTempo_Inizio2 ) /CLOCKS_PER_SEC) );
        emit ScriviASchermo(  QString("\tTempo Totale (Rollout + VNS): %1\n").arg( ( double ) ( FTempo_Fine1-FTempo_Inizio1 ) + ( double ) ( FTempo_Fine2-FTempo_Inizio2 ) /CLOCKS_PER_SEC) );
    }
    else
    {
        emit ScriviASchermo( QString( "\tTempo impiegato Bilanc. : %1\n").arg( ( double ) ( FTempo_Fine3-FTempo_Inizio3 )) );
        emit ScriviASchermo(  QString("\tTempo Totale (Rollout + VNS + Bilanc.): %1\n").arg( ( double ) ( FTempo_Fine1-FTempo_Inizio1 ) + ( double ) ( FTempo_Fine3-FTempo_Inizio2 ) /CLOCKS_PER_SEC ));
    }
}

void QRolloutThread::StampaRisultatiSuFile (FILE *pFile_out,
                                            char *pInstance_File_Nome,
                                            int pFlag,
                                            int pForce )
{

    if ( pFlag == 0 )
    {

        fprintf ( pFile_out,"%s\t",pInstance_File_Nome );
        fprintf ( pFile_out,"%i\n\t",Ffeasible );
        if (FRollout_Or_Heuristic == 1)//euristica
            fprintf ( pFile_out,"%.3f\t", ( double ) ( FTempo_Fine1-FTempo_Inizio1 )/CLOCKS_PER_SEC );
        else
            fprintf ( pFile_out,"%.3f\t", ( double ) ( FTempo_Sec_Fine1-FTempo_Sec_Inizio1 ) );

    }
    else if ( pFlag == 1 )
    {
        fprintf ( pFile_out,"%.3f\t", ( double ) ( FTempo_Fine2-FTempo_Inizio2 ) /CLOCKS_PER_SEC );

        if (FRollout_Or_Heuristic==1)//euristica
            fprintf ( pFile_out,"%.3f\t", ( double )( ( FTempo_Fine1-FTempo_Inizio1 ) + ( FTempo_Fine2-FTempo_Inizio2 )) /CLOCKS_PER_SEC );
        else
            fprintf ( pFile_out,"%.3f\t", ( double )( FTempo_Sec_Fine1-FTempo_Sec_Inizio1 ) + (double)(( FTempo_Fine2-FTempo_Inizio2 )) /CLOCKS_PER_SEC );
    }
    else
    {
        fprintf ( pFile_out,"%.3f\t", ( double ) ( FTempo_Fine3-FTempo_Inizio3 ) );
        if (FRollout_Or_Heuristic==1)//euristica
            fprintf ( pFile_out,"%.3f\t", ( double )( ( FTempo_Fine1-FTempo_Inizio1 ) + ( FTempo_Fine3-FTempo_Inizio2 )) /CLOCKS_PER_SEC );
        else
            fprintf ( pFile_out,"%.3f\t", ( double )( FTempo_Sec_Fine1-FTempo_Sec_Inizio1 ) + (double)(( FTempo_Fine3-FTempo_Inizio2 )) /CLOCKS_PER_SEC );
    }

    fprintf ( pFile_out,"%i\t",GNum_Macchine );
    fprintf ( pFile_out,"%i\t",FLmax );
    fprintf ( pFile_out,"%i\t",FCmax );
    fprintf ( pFile_out,"%i\t",FTardy );
    if ( pFlag==0 )
    {
        fprintf ( pFile_out,"%i\t",Ffeasible );
        fprintf ( pFile_out,"force = %i\t|\n",pForce );
    }
    else
        fprintf ( pFile_out,"%i\t\t\t|\n",Ffeasible );

    if ( pFlag == 2 )
        fprintf ( pFile_out,"\n" );

}
int QRolloutThread::Min ( int a, int b )
{
    if ( a < b )
        return a;
    else
        return b;

}

int QRolloutThread::Max ( int a, int b )
{
    if ( a < b )
        return b;

    else
        return a;

}

//***************************************************************************************************************************
// lo scopo di questa funzione e' quello di eseguire uno spostamento di un job della macchina X da una posizione ad un'altra.
// **************************************************************************************************************************
TSchedula *QRolloutThread::Mossa ( TSchedula *pM_sch, TElem *pM, int pPos_Iniziale, int pPos_Finale )
{
    // 	M_sch e' la schedula su cui deve essere effettuato lo spostamento.
    // 	M invece e' la lista delle indisponibilita' della macchina in questione.
    // 	una volta effettuato lo scabio sara' necessario rivedere l'inizio e la fine di tutti i successivi job
    // 	struct sch //questa struttura conterra' le schedulazioni per ogni macchina
    // 	{
    // 	int ID_job;
    // 	int tipo;
    // 	int inizio;
    // 	int fine;
    // 	int Lmax;
    // 	int Tardy;
    // 	int index_camp;
    // 	struct sch *next;
    // 	};
    // typedef struct sch schedula;
    int i ;
    // creo ora una nuova schedula come copia di una parte di quella passatami e su di essa effettuera' lo scambio
    // ____________________________________________________________________________________________________________________
    TSchedula *schedula_di_lavoro;// ovviamente nn lavoro sulla schedula ma su una sua copia.
    TSchedula *temp1;
    TSchedula *temp2;
    TJob *job_temp;

    job_temp = new TJob;

    if ( pPos_Finale < pPos_Iniziale )
    {
        if ( pPos_Finale > 0 ) // copio nella schedula di lavoro gli elementi che restano inalterati
        {
            schedula_di_lavoro = new TSchedula;
            temp1               = pM_sch;

            temp2               = schedula_di_lavoro;
            temp2->ID_job       = temp1->ID_job;
            temp2->tipo         = temp1->tipo;
            temp2->inizio       = temp1->inizio;
            temp2->fine         = temp1->fine;
            temp2->Lmax         = temp1->Lmax;
            temp2->Tardy        = temp1->Tardy;
            temp2->index_camp   = temp1->index_camp;
            temp2->next         = NULL;

            temp1 = temp1->next;
            for ( i = 1;i < pPos_Finale;i++ )
            {
                temp2->next         = new TSchedula;
                //creo una nuova schedula e mi devo ricordare di distruggerla
                temp2               = temp2->next;
                temp2->ID_job       = temp1->ID_job;
                temp2->tipo         = temp1->tipo;
                temp2->inizio       = temp1->inizio;
                temp2->fine         = temp1->fine;
                temp2->Lmax         = temp1->Lmax;
                temp2->Tardy        = temp1->Tardy;
                temp2->index_camp   = temp1->index_camp;
                temp2->next         = NULL;

                temp1 = temp1->next;
            }
        }
        else
        {
            schedula_di_lavoro  = new TSchedula;
            temp2               = schedula_di_lavoro;
            temp2->ID_job       = -3;
            temp2->tipo         = 0;
            temp2->inizio       = 0;
            temp2->fine         = 0;
            temp2->Lmax         = 0;
            temp2->Tardy        = 0;
            temp2->index_camp   = 0;
            temp2->next         = NULL;
        }
        //_____________________________________________________________________________________________________________________
        // ora devo considerare il job nella posizione iniziale e spostarlo in quella finale.
        // devo tener conto della possibilita' di incorrere in setup o inisponibilita'
        // uso la funzione aggiungi_schedula
        temp1 = pM_sch;
        for ( i = 0; i < pPos_Iniziale; i++ )
            temp1 = temp1->next;

        // temp1 ora dovrebbe puntare al job da spostare
        int tipo = temp1->tipo;
        int tipo_predecessore ;
        if ( temp2==NULL )
            tipo_predecessore = 0;
        else
            tipo_predecessore = temp2->tipo;

        int *setup_vett;
        int st_vt = 0;
        setup_vett =&st_vt;
        // faccio cosi' perche' la funzione aggiungi_schedula richiede un puntatore ad intero
        int *disponibilita;
        int disp = 0;
        disponibilita =&disp;
        if ( tipo != tipo_predecessore )
            setup_vett[0] = 0;
        else
            setup_vett[0] = 1;

        i = 0;
        while ( i < GNum_Job )
        {
            if ( temp1->ID_job == GArray_Job[i].ID )
            {
                job_temp[0] = GArray_Job[i];
                break;
            }
            i++;
        }
        VerificaMacchina ( schedula_di_lavoro,pM,disponibilita,setup_vett,0,job_temp,0 );
        Schedula::AggiungiSchedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
        // 	a questo punto ho spostato il job nella posizione finale devo rischedulare gli altri
        // 	mi segno l'ID del job che ho spostato cos��?da poterlo saltare se lo re-incontro
        int ID_vietato = temp1->ID_job;
        tipo_predecessore = tipo;// il predecessore ��?ora il job appena schedulato
        temp1 = pM_sch;
        for ( i = 0; i < pPos_Finale; i++ )
            temp1 = temp1->next;

        //dovrei ora puntare alla posizione successiva a quella del job appena schedulato
        while ( temp1!=NULL )
        {
            if ( temp1->ID_job!=ID_vietato )
            {
                tipo = temp1->tipo;
                setup_vett[0]=0;
                disponibilita[0] = 0;
                if ( tipo != tipo_predecessore )
                {
                    setup_vett[0] = 0;
                }
                else
                {
                    setup_vett[0] = 1;
                }
                i=0;
                while ( i<GNum_Job )
                {
                    if ( temp1->ID_job == GArray_Job[i].ID )
                    {
                        job_temp[0] = GArray_Job[i];
                        break;
                    }
                    i++;
                }
                VerificaMacchina ( schedula_di_lavoro,pM,disponibilita,setup_vett,0,job_temp,0 );
                Schedula::AggiungiSchedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
                tipo_predecessore = tipo;// il predecessore e' ora il job appena schedulato
            }
            temp1 = temp1->next;
        }
    }
    else
    {
        if ( pPos_Iniziale > 0 ) // copio nella schedula di lavoro gli elementi che restano inalterati
        {
            schedula_di_lavoro = new TSchedula;
            temp1 = pM_sch;
            temp2 = schedula_di_lavoro;
            temp2->ID_job = temp1->ID_job;
            temp2->tipo = temp1->tipo;
            temp2->inizio = temp1->inizio;
            temp2->fine = temp1->fine;
            temp2->Lmax = temp1->Lmax;
            temp2->Tardy = temp1->Tardy;
            temp2->index_camp = temp1->index_camp;
            temp2->next = NULL;
            temp1 = temp1->next;
            for ( i=1;i<pPos_Iniziale;i++ )
            {
                temp2->next = new TSchedula;
                //creo una nuova schedula e mi devo ricordare di distruggerla
                temp2 = temp2->next;
                temp2->ID_job = temp1->ID_job;
                temp2->tipo = temp1->tipo;
                temp2->inizio = temp1->inizio;
                temp2->fine = temp1->fine;
                temp2->Lmax = temp1->Lmax;
                temp2->Tardy = temp1->Tardy;
                temp2->index_camp = temp1->index_camp;
                temp2->next = NULL;
                temp1 = temp1->next;
            }
        }
        else
        {
            schedula_di_lavoro = new TSchedula;
            schedula_di_lavoro->ID_job = -3;
            schedula_di_lavoro->tipo = 0;
            schedula_di_lavoro->inizio = 0;
            schedula_di_lavoro->fine = 0;
            schedula_di_lavoro->Lmax = 0;
            schedula_di_lavoro->Tardy = 0;
            schedula_di_lavoro->index_camp = 0;
            schedula_di_lavoro->next = NULL;
        }
        //bisogna aggiungere i job intermedi
        temp1 = pM_sch;
        for ( i = 0; i < pPos_Iniziale - 1; i++ )
            temp1 = temp1->next;

        //punto ora all'elemento precedente a quello da spostare
        //salvo il tipo
        int tipo ;
        int tipo_predecessore;
        int *setup_vett;// faccio cosi perche' la funzione aggiungi_schedula richiede un puntatore ad intero
        int st_vt =0;
        setup_vett=&st_vt;
        int *disponibilita;
        int disp = 0;
        disponibilita = &disp;

        if ( pPos_Iniziale == 0 )
            tipo_predecessore = 0;//nn ho predecessori
        else
            tipo_predecessore = temp1->tipo;


        if ( pPos_Iniziale != 0 )
            temp1 = temp1->next;//punto ora al job da spostare

        //altrimenti gia' sto puntando a quell'elemento
        int ID_job_da_spostare = temp1->ID_job;
        int tipo_job_da_spostare = temp1->tipo;
        temp1 = temp1->next;//punto al job successivo
        for ( i = pPos_Iniziale;i<pPos_Finale;i++ )
        {
            tipo = temp1->tipo;
            setup_vett[0]=0;
            disponibilita[0] = 0;
            if ( tipo != tipo_predecessore )
                setup_vett[0] = 0;
            else
                setup_vett[0] = 1;

            int jj = 0;
            while ( jj < GNum_Job )
            {
                if ( temp1->ID_job == GArray_Job[jj].ID )
                {
                    job_temp[0] = GArray_Job[jj];
                    break;
                }
                jj++;
            }
            VerificaMacchina ( schedula_di_lavoro,pM,disponibilita,setup_vett,0,job_temp,0 );
            Schedula::AggiungiSchedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
            tipo_predecessore = tipo;// il predecessore e' ora il job appena schedulato
            temp1 = temp1->next;
        }
        i=0;
        while ( i<GNum_Job )
        {
            if ( ID_job_da_spostare == GArray_Job[i].ID )
            {
                job_temp[0] = GArray_Job[i];
                break;
            }
            i++;
        }
        VerificaMacchina ( schedula_di_lavoro,pM,disponibilita,setup_vett,0,job_temp,0 );
        Schedula::AggiungiSchedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
        tipo_predecessore = tipo_job_da_spostare;// il predecessore ��?ora il job appena schedulato
        while ( temp1!=NULL ) //aggiungo tutti gli altri
        {

            tipo = temp1->tipo;
            setup_vett[0]=0;
            disponibilita[0] = 0;
            if ( tipo != tipo_predecessore )
                setup_vett[0] = 0;
            else
                setup_vett[0] = 1;

            i = 0;
            while ( i<GNum_Job )
            {
                if ( temp1->ID_job == GArray_Job[i].ID )
                {
                    job_temp[0] = GArray_Job[i];
                    break;
                }
                i++;
            }
            VerificaMacchina ( schedula_di_lavoro,pM,disponibilita,setup_vett,0,job_temp,0 );
            Schedula::AggiungiSchedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
            tipo_predecessore = tipo;// il predecessore e' ora il job appena schedulato
            temp1 = temp1->next;
        }

    }
    delete job_temp;
    return schedula_di_lavoro;
}
//VNS euristica di esplorazione di vicinati multipli
// restituisce 1 se sono stati trovati miglioramenti 0 atrimenti.
int QRolloutThread::VNS (TSchedula *pM1_sch,
                         TSchedula *pM2_sch,
                         TSchedula *pM3_sch )
{
    int ris = 0;
    ris += VnsPerMacchina ( pM1_sch,GMacch1 );
    if ( GNum_Macchine >= 2 )
        ris += VnsPerMacchina ( pM2_sch,GMacch2 );

    if ( GNum_Macchine==3 )
        ris += VnsPerMacchina ( pM3_sch,GMacch3 );

    return ris;
}
//VNS_per_macchina implementa lo schema VNS per la singola macchina considerata.
int QRolloutThread::VnsPerMacchina ( TSchedula *pMacch_sch,TElem *pElem )
{
    // 	per prima cosa devo trovare i job con lateness positiva decrescente
    // 	ed ordinarli in modo decrescente (dal + ritardatario al meno ritardatario)
    //  struct sch //questa struttura conterra' le schedulazioni per ogni macchina
    // 	{
    // 	int ID_job;
    // 	int tipo;
    // 	int inizio;
    // 	int fine;
    // 	int Lmax;
    // 	int Tardy;
    // 	int index_camp;
    // 	struct sch *next;
    // 	};
    // typedef struct sch schedula;
    TSchedula *temp;
    TSchedula *temp1,*temp2;
    TSchedula *schedula_di_lavoro;
    TQuaterna *quaterna_di_lavoro;
    TQuaterna *quaterna_migliore;

    int *job_vett = new int[GNum_Job];
    int i,j;

    int Lat_max,pos,cont = 0;

    for ( i = 0; i < GNum_Job; i++ )
        job_vett[i] = 0;

    i=0;
    temp = pMacch_sch;
    while ( temp!= NULL )
    {
        if ( temp->Lmax > 0 )
        {// salvo la lateness nel vettore delle posizioni
            job_vett[i] = temp->Lmax;
            cont++;
        }
        temp = temp->next;
        i++;
    }
    // 	devo ora trovare il job con lateness maggiore
    quaterna_migliore = Schedula::ValutaSingolaSchedula ( pMacch_sch );
    for ( j = 0; j < cont; j++ )
    {
        Lat_max = 0;
        pos = -1;
        for ( i = 0; i < GNum_Job; i++ )
        {
            if ( job_vett[i] > 0 )
            {
                Lat_max = job_vett[i];
                pos = i;
                break;
            }
        }
        if ( pos>=0 )
        {
            for ( i=pos+1;i<GNum_Job;i++ )
            {
                if
                        (
                         ( job_vett[i]>0 )
                         &&
                         ( Lat_max<job_vett[i] )
                         )
                {
                    Lat_max = job_vett[i];
                    pos = i;
                }
            }
            job_vett[pos]=0; //per nn considerarlo + volte
            if ( pos!=0 )
            {
                schedula_di_lavoro = Mossa ( pMacch_sch,pElem,pos,pos-1 );
                quaterna_di_lavoro = Schedula::ValutaSingolaSchedula ( schedula_di_lavoro );
                //dopo devo distruggerla
                if ( quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible )
                {
                    if
                            (
                             ( quaterna_di_lavoro->Feasible == 1 )
                             &&
                             ( quaterna_migliore->Feasible == 0 )
                             )
                    {
                        temp = pMacch_sch;
                        temp1 = schedula_di_lavoro;
                        while ( temp!= NULL )
                        {
                            temp->ID_job = temp1->ID_job;
                            temp->tipo = temp1->tipo;
                            temp->inizio = temp1->inizio;
                            temp->fine = temp1->fine;
                            temp->Lmax = temp1->Lmax;
                            temp->Tardy = temp1->Tardy;
                            temp->index_camp = temp1->index_camp;
                            temp = temp->next;
                            temp2 = temp1->next;
                            temp1 = temp2;
                        }
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VnsPerMacchina ( pMacch_sch,pElem );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else if //sostituisco la schedula alla corrente se ho un miglioramento
                    (
                    ( quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax )
                            ||
                            (
                                ( quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax )
                                &&
                                ( quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax )
                                )
                            ||
                            (
                                ( quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax )
                                &&
                                ( quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax )
                                &&
                                ( quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy )
                                )
                            )
                    {
                        temp = pMacch_sch;
                        temp1 = schedula_di_lavoro;
                        while ( temp!= NULL )
                        {
                            temp->ID_job    = temp1->ID_job;
                            temp->tipo      = temp1->tipo;
                            temp->inizio    = temp1->inizio;
                            temp->fine      = temp1->fine;
                            temp->Lmax      = temp1->Lmax;
                            temp->Tardy     = temp1->Tardy;
                            temp->index_camp= temp1->index_camp;
                            temp            = temp->next;
                            temp2           = temp1->next;
                            temp1           = temp2;
                        }
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VnsPerMacchina ( pMacch_sch,pElem );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else
                    {
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_di_lavoro;
                    }

                }
                else
                {//devo distruggere la shedula di lavoro
                    Schedula::EliminaSchedula ( schedula_di_lavoro );
                    delete quaterna_di_lavoro;
                }
            }
        }

    }
    for ( i=0;i<GNum_Job;i++ )
        job_vett[i]=0;

    int k = 0;
    cont = 0;
    temp = pMacch_sch;
    while ( temp!= NULL )
    {
        for ( i = 0; i < GNum_Job; i++ )
        {
            if ( temp->ID_job == GArray_Job[i].ID )
            {
                if
                        (
                         ( GArray_Job[i].deadline!=0 )
                         &&
                         (
                             ( temp->fine-GArray_Job[i].deadline )
                             >0
                             )
                         )
                {
                    job_vett[k] = ( temp->fine-GArray_Job[i].deadline );
                    cont++;
                    break;
                }
            }
        }
        k++;
        temp = temp->next;
    }
    // 	devo ora trovare il job con lateness maggiore

    for ( j = 0;j < cont; j++ )
    {
        int violazione_max =0;
        pos = -1;
        for ( i=0;i<GNum_Job;i++ )
        {
            if ( job_vett[i]>0 )
            {
                violazione_max= job_vett[i];
                pos = i;
                break;
            }
        }
        if ( pos >= 0 )
        {
            for ( i=pos+1;i<GNum_Job;i++ )
            {
                if
                        (
                         ( job_vett[i]>0 )
                         &&
                         ( violazione_max<job_vett[i] )
                         )
                {
                    violazione_max = job_vett[i];
                    pos = i;
                }
            }
            // devo ora stabilire di quante posizioni in avanti devo spostare il job.
            // cerco il job incriminato e quindi la sua data di inizio e poi cerco un job precedente con data di inizio<=(inizio-violazione_max)j
            temp = pMacch_sch;
            for ( k = 0; k < pos; k++ )
                temp = temp->next;

            int limite = 0;
            int pos_finale = -1;
            limite = ( temp->inizio-violazione_max );
            temp = pMacch_sch;
            for ( k=0;k<pos;k++ )
            {
                if
                        (
                         ( temp->inizio<= limite )
                         &&
                         ( temp->fine>limite )
                         )
                {
                    pos_finale = k;
                    break;
                }
                temp = temp->next;
            }
            job_vett[pos]=0; //per nn considerarlo + volte
            if ( pos!=0 )
            {
                schedula_di_lavoro = Mossa ( pMacch_sch,pElem,pos,pos_finale );
                quaterna_di_lavoro = Schedula::ValutaSingolaSchedula ( schedula_di_lavoro );
                //dopo devo distruggerla
                if ( quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible )
                {
                    if //sostituisco la schedula alla corrente se ho un miglioramento
                    (
                    ( quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax )
                            ||
                            (
                                ( quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax )
                                &&
                                ( quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax )
                                )
                            ||
                            (
                                ( quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax )
                                &&
                                ( quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax )
                                &&
                                ( quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy )
                                )
                            )
                    {
                        temp = pMacch_sch;
                        temp1 = schedula_di_lavoro;
                        while ( temp!= NULL )
                        {
                            temp->ID_job    = temp1->ID_job;
                            temp->tipo      = temp1->tipo;
                            temp->inizio    = temp1->inizio;
                            temp->fine      = temp1->fine;
                            temp->Lmax      = temp1->Lmax;
                            temp->Tardy     = temp1->Tardy;
                            temp->index_camp= temp1->index_camp;
                            temp            = temp->next;
                            temp2           = temp1->next;
                            temp1           = temp2;
                        }
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VnsPerMacchina ( pMacch_sch,pElem );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else
                    {
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_di_lavoro;
                    }
                }
                else
                {//devo distruggere la shedula di lavoro
                    Schedula::EliminaSchedula ( schedula_di_lavoro );
                    delete quaterna_di_lavoro;
                }
            }
        }
    }
    //ultima parte
    for ( i=0;i<GNum_Job;i++ )

        job_vett[i]=0;

    k = 0;
    cont = 0;
    temp = pMacch_sch;
    while ( temp!= NULL )
    {
        for ( i = 0;i < GNum_Job; i++ )
        {
            if ( temp->ID_job == GArray_Job[i].ID )
            {
                if
                        (
                         ( GArray_Job[i].deadline!=0 )
                         &&
                         (
                             ( temp->fine-GArray_Job[i].deadline )
                             <0
                             )
                         )
                {
                    job_vett[k] = - ( temp->fine-GArray_Job[i].deadline );
                    cont++;
                    break;
                }
            }
        }
        k++;
        temp = temp->next;
    }

    for ( j = 0;j < cont; j++ )
    {
        int margine_max =0;
        pos = -1;
        for ( i=0;i<GNum_Job;i++ )
        {
            if ( job_vett[i]>0 )
            {
                margine_max= job_vett[i];
                pos = i;
                break;
            }
        }
        if ( pos >= 0 )
        {
            for ( i=pos+1;i<GNum_Job;i++ )
            {
                if
                        (
                         ( job_vett[i]>0 )
                         &&
                         ( margine_max<job_vett[i] )
                         )
                {
                    margine_max = job_vett[i];
                    pos = i;
                }
            }
            temp = pMacch_sch;
            for ( k = 0; k < pos;k++ )
                temp = temp->next;

            job_vett[pos]=0; //per nn considerarlo + volte
            if ( temp->next!=NULL ) //verifico che il successivo esista davvero
            {
                schedula_di_lavoro = Mossa ( pMacch_sch,pElem,pos,pos+1 );

                quaterna_di_lavoro = Schedula::ValutaSingolaSchedula ( schedula_di_lavoro );
                //dopo devo distruggerla
                if ( quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible )
                {
                    if //sostituisco la schedula alla corrente se ho un miglioramento
                    (
                    ( quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax )
                            ||
                            (
                                ( quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax )
                                &&
                                ( quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax )
                                )
                            ||
                            (
                                ( quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax )
                                &&
                                ( quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax )
                                &&
                                ( quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy )
                                )
                            )
                    {
                        temp = pMacch_sch;
                        temp1 = schedula_di_lavoro;
                        while ( temp!= NULL )
                        {
                            temp->ID_job = temp1->ID_job;
                            temp->tipo = temp1->tipo;
                            temp->inizio = temp1->inizio;
                            temp->fine = temp1->fine;
                            temp->Lmax = temp1->Lmax;
                            temp->Tardy = temp1->Tardy;
                            temp->index_camp = temp1->index_camp;
                            temp = temp->next;
                            temp2 = temp1->next;
                            temp1 = temp2;
                        }
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VnsPerMacchina ( pMacch_sch,pElem );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else
                    {
                        Schedula::EliminaSchedula ( schedula_di_lavoro );
                        delete quaterna_di_lavoro;
                    }
                }
                else
                {//devo distruggere la shedula di lavoro
                    Schedula::EliminaSchedula ( schedula_di_lavoro );
                    delete quaterna_di_lavoro;
                }
            }
        }
    }

    delete quaterna_migliore;
    delete[] job_vett;
    return 0;//se nn ho fatto modifiche alla sequenza
}

//lo scopo di questa funzione e' quello di valutare una singola schedula invece di fare una analisi globale cone
//valuta schedula che tra l'altro fissa anche il valore delle variabili globali Lmax, Cmax e Tardy
int QRolloutThread::BilanciamentoSchedule (TSchedula *pM1_Schedula,
                                            TSchedula *pM2_Schedula,
                                            TSchedula *pM3_Schedula )
{
    //le schedule pM1_Schedula, pM2_Schedula,pM3_Schedula sono state modificate ed adesso devono essere ribilanciate.

    TSchedula **vett_sch = new TSchedula*[GNum_Macchine];
    TElem **vett_indisp  = new TElem *[GNum_Macchine];
    int *vett_C_fine     = new int[GNum_Macchine];
    int *vett_C_inizio   = new int[GNum_Macchine];
    TSchedula **punt_fine_sch = new TSchedula*[GNum_Macchine];

    int i,done = 0;
    TJob *perm = new TJob;
    if ( GNum_Macchine == 1 )
    {
        vett_sch[0] = pM1_Schedula;
        vett_indisp[0] = GMacch1;
    }
    else if ( GNum_Macchine == 2 )
    {
        vett_sch[0] = pM1_Schedula;
        vett_indisp[0] = GMacch1;
        vett_sch[1] = pM2_Schedula;
        vett_indisp[1] = GMacch2;
    }
    else if ( GNum_Macchine == 3 )
    {
        vett_sch[0] = pM1_Schedula;
        vett_indisp[0] = GMacch1;
        vett_sch[1] = pM2_Schedula;
        vett_indisp[1] = GMacch2;
        vett_sch[2] = pM3_Schedula;
        vett_indisp[2] = GMacch3;
    }

    while ( !done )
    {
        for ( i = 0;i<GNum_Macchine;i++ )
        {

            punt_fine_sch[i] = vett_sch[i];
            while ( punt_fine_sch[i]->next!=NULL )
                punt_fine_sch[i] = punt_fine_sch[i]->next;

            //ora sto puntando all'ultimo elemento schedulato
            vett_C_fine[i] = punt_fine_sch[i]->fine;
            vett_C_inizio[i] = punt_fine_sch[i]->inizio;
        }
        int min = 65000;
        int pos = -1;
        int *setup_vett;
        int st_vt = 0;
        setup_vett = &st_vt;
        for ( i = 0; i < GNum_Macchine; i++ )
        {
            if ( min > vett_C_fine[i] )
            {
                min = vett_C_fine[i];
                pos = i;
            }
        }//trovo la schedula + corta
        for ( i = 0; i < GNum_Macchine; i++ )
        {
            if
                    (
                     ( i != pos )
                     &&
                     ( vett_C_inizio[i] > vett_C_fine[pos] )
                     )
            {
                // devo spostare il job schedulato alla macchina puntata da i in quella puntata da pos
                // non e' detto che spostarlo sia utile devo verificare.
                int ID = punt_fine_sch[i]->ID_job;
                int j;
                for ( j = 0; j < GNum_Job; j++ )
                {
                    if ( ID == GArray_Job[j].ID )
                    {
                        perm[0] = GArray_Job[j];
                        break;
                    }
                }
                int *disponibilita;
                int disp = 0;
                disponibilita = &disp;
                VerificaMacchina ( vett_sch[pos],vett_indisp[pos],disponibilita,setup_vett,0,perm,0 );
                if ( disponibilita[0] < vett_C_inizio[i] )
                {
                    Schedula::AggiungiSchedula ( vett_sch[pos],perm[0],disponibilita[0],setup_vett[0] );
                    TQuaterna *ris = Schedula::ValutaSingolaSchedula ( vett_sch[pos] );
                    if ( ris->Feasible >=Ffeasible )
                    {
                        if
                                (
                                 ( ris->Lmax < FLmax )
                                 ||
                                 (
                                     ( ris->Lmax == FLmax )
                                     &&
                                     ( ris->Cmax < FCmax )
                                     )
                                 ||
                                 (
                                     ( ris->Lmax == FLmax )
                                     &&
                                     ( ris->Cmax == FCmax )
                                     &&
                                     ( ris->Tardy < FTardy )
                                     )
                                 )
                        {
                            // devo eliminare il job dalla schedula i
                            punt_fine_sch[i] = vett_sch[i];
                            while ( punt_fine_sch[i]->next->next!=NULL )
                                punt_fine_sch[i] = punt_fine_sch[i]->next;

                            delete punt_fine_sch[i]->next;
                            punt_fine_sch[i]->next = NULL;
                            punt_fine_sch[pos] = vett_sch[pos];
                            while ( punt_fine_sch[pos]->next!=NULL ) //update
                                punt_fine_sch[pos] = punt_fine_sch[pos]->next;
                            TNext_Elem *prossimo1 = new TNext_Elem;
                            ValutaSchedula ( pM1_Schedula,pM2_Schedula,pM3_Schedula,prossimo1 );//ricalcolo i valori delle variabili globali
                            delete prossimo1;
                            done = 1;
                            break;
                        }
                    }
                    else
                    {
                        punt_fine_sch[pos] = vett_sch[pos];
                        while ( punt_fine_sch[pos]->next->next!=NULL )
                            punt_fine_sch[pos] = punt_fine_sch[pos]->next;

                        delete punt_fine_sch[pos]->next;
                        punt_fine_sch[pos]->next = NULL;
                    }
                }
            }
        }
        if ( done == 1 ) //ho effettuato almeno una modifica e quindi provo a riapplicare il bilanciamento
            done =0;
        else
            done = 1;

    }
    // TODO questo si deve sistemare.

    delete[] vett_sch;
    delete[] vett_indisp;
    delete[] vett_C_fine;
    delete[] vett_C_inizio;
    delete[] punt_fine_sch;
    delete perm;
    return 1;
}

void QRolloutThread::StampaPercentualiUtilizzoAVideo ( void )
{
    for (int i = 0; i < Fnum_Heur; i++ )
        emit ScriviASchermo( QString( "\n %1 %2% :").arg( i).arg( ( ( float ) Ffunzioni[i].perc_utilizzo/GNum_Job ) *100) );
}
int QRolloutThread::SelezionaProssimoJob ( TNext_Elem *pLista_prossimi_vincitori,int pCont_livelli )
{

        int pos = 0;
        int best_pos = 0;
        int ID_job      = pLista_prossimi_vincitori[pos].ID_job;
        int macchina    = pLista_prossimi_vincitori[pos].macchina;
        int fine        = pLista_prossimi_vincitori[pos].fine;
        int inizio      = pLista_prossimi_vincitori[pos].inizio;
        int tipo        = pLista_prossimi_vincitori[pos].tipo;
        int index_camp  = pLista_prossimi_vincitori[pos].index_camp;
        int LMAX        = pLista_prossimi_vincitori[pos].Lmax_pers;
        int TARDY       = pLista_prossimi_vincitori[pos].Tardy_pers;
        int L_max       = pLista_prossimi_vincitori[pos].Lmax;
        int C_max       = pLista_prossimi_vincitori[pos].Cmax;
        int tardy       = pLista_prossimi_vincitori[pos].Tardy;
        int ID_heur     = pLista_prossimi_vincitori[pos].ID_heur;
        int rel_time    = pLista_prossimi_vincitori[pos].rel_time;
        int proc_time   = pLista_prossimi_vincitori[pos].proc_time;
        int duedate     = pLista_prossimi_vincitori[pos].duedate;
        int deadline    = pLista_prossimi_vincitori[pos].deadline;
        int priority    = pLista_prossimi_vincitori[pos].priority;
        best_pos = pos;
        Ffeasible = pLista_prossimi_vincitori[pos].feasible;
        while ( pos < pCont_livelli - 1 )
        {
                if ( Ffeasible <= pLista_prossimi_vincitori[pos+1].feasible )
                {
                        if
                        (
                            ( Ffeasible ==0 )
                            &&
                            ( pLista_prossimi_vincitori[pos+1].feasible==1 )
                        )
                        {
                                ID_job      = pLista_prossimi_vincitori[pos+1].ID_job;
                                macchina    = pLista_prossimi_vincitori[pos+1].macchina;
                                tipo        = pLista_prossimi_vincitori[pos+1].tipo;
                                fine        = pLista_prossimi_vincitori[pos+1].fine;
                                inizio      = pLista_prossimi_vincitori[pos+1].inizio;
                                index_camp  = pLista_prossimi_vincitori[pos+1].index_camp;
                                LMAX        = pLista_prossimi_vincitori[pos+1].Lmax_pers;
                                TARDY       = pLista_prossimi_vincitori[pos+1].Tardy_pers;
                                L_max       = pLista_prossimi_vincitori[pos+1].Lmax;
                                C_max       = pLista_prossimi_vincitori[pos+1].Cmax;
                                tardy       = pLista_prossimi_vincitori[pos+1].Tardy;
                                deadline    = pLista_prossimi_vincitori[pos+1].deadline;
                                duedate     = pLista_prossimi_vincitori[pos+1].duedate;
                                proc_time   = pLista_prossimi_vincitori[pos+1].proc_time;
                                rel_time    = pLista_prossimi_vincitori[pos+1].rel_time;
                                priority    = pLista_prossimi_vincitori[pos+1].priority;
                                ID_heur     = pLista_prossimi_vincitori[pos+1].ID_heur;
                                Ffeasible   = pLista_prossimi_vincitori[pos+1].feasible;
                                best_pos    = pos + 1;
                        }
                        else if
                        (
                            ( L_max > pLista_prossimi_vincitori[pos+1].Lmax )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max > pLista_prossimi_vincitori[pos+1].Cmax )
                            )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max == pLista_prossimi_vincitori[pos+1].Cmax )
                                &&
                                ( tardy > pLista_prossimi_vincitori[pos+1].Tardy )
                            )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max == pLista_prossimi_vincitori[pos+1].Cmax )
                                &&
                                ( tardy == pLista_prossimi_vincitori[pos+1].Tardy )
                                &&
                                ( LMAX > pLista_prossimi_vincitori[pos+1].Lmax_pers )
                            )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max == pLista_prossimi_vincitori[pos+1].Cmax )
                                &&
                                ( tardy == pLista_prossimi_vincitori[pos+1].Tardy )
                                &&
                                ( LMAX > pLista_prossimi_vincitori[pos+1].Lmax_pers )
                                &&
                                (
                                    ( pLista_prossimi_vincitori[pos+1].duedate !=0 )
                                    &&
                                    ( duedate > pLista_prossimi_vincitori[pos+1].duedate )
                                )
                            )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max == pLista_prossimi_vincitori[pos+1].Cmax )
                                &&
                                ( tardy == pLista_prossimi_vincitori[pos+1].Tardy )
                                &&
                                ( LMAX > pLista_prossimi_vincitori[pos+1].Lmax_pers )
                                &&
                                ( duedate == pLista_prossimi_vincitori[pos+1].duedate )
                                &&
                                (
                                    ( pLista_prossimi_vincitori[pos+1].deadline !=0 )
                                    &&
                                    ( deadline > pLista_prossimi_vincitori[pos+1].deadline )
                                )
                            )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max == pLista_prossimi_vincitori[pos+1].Cmax )
                                &&
                                ( tardy == pLista_prossimi_vincitori[pos+1].Tardy )
                                &&
                                ( LMAX > pLista_prossimi_vincitori[pos+1].Lmax_pers )
                                &&
                                ( duedate == pLista_prossimi_vincitori[pos+1].duedate )
                                &&
                                ( deadline == pLista_prossimi_vincitori[pos+1].deadline )
                                &&
                                ( proc_time > pLista_prossimi_vincitori[pos+1].proc_time )
                            )
                            ||
                            (
                                ( L_max == pLista_prossimi_vincitori[pos+1].Lmax )
                                &&
                                ( C_max == pLista_prossimi_vincitori[pos+1].Cmax )
                                &&
                                ( tardy == pLista_prossimi_vincitori[pos+1].Tardy )
                                &&
                                ( LMAX > pLista_prossimi_vincitori[pos+1].Lmax_pers )
                                &&
                                ( duedate == pLista_prossimi_vincitori[pos+1].duedate )
                                &&
                                ( deadline == pLista_prossimi_vincitori[pos+1].deadline )
                                &&
                                ( proc_time == pLista_prossimi_vincitori[pos+1].proc_time )
                                &&
                                ( rel_time > pLista_prossimi_vincitori[pos+1].proc_time )
                            )
                        )
                        {
                                ID_job      = pLista_prossimi_vincitori[pos+1].ID_job;
                                macchina    = pLista_prossimi_vincitori[pos+1].macchina;
                                tipo        = pLista_prossimi_vincitori[pos+1].tipo;
                                fine        = pLista_prossimi_vincitori[pos+1].fine;
                                inizio      = pLista_prossimi_vincitori[pos+1].inizio;
                                index_camp  = pLista_prossimi_vincitori[pos+1].index_camp;
                                LMAX        = pLista_prossimi_vincitori[pos+1].Lmax_pers;
                                TARDY       = pLista_prossimi_vincitori[pos+1].Tardy_pers;
                                L_max       = pLista_prossimi_vincitori[pos+1].Lmax;
                                C_max       = pLista_prossimi_vincitori[pos+1].Cmax;
                                tardy       = pLista_prossimi_vincitori[pos+1].Tardy;
                                deadline    = pLista_prossimi_vincitori[pos+1].deadline;
                                duedate     = pLista_prossimi_vincitori[pos+1].duedate;
                                proc_time   = pLista_prossimi_vincitori[pos+1].proc_time;
                                rel_time    = pLista_prossimi_vincitori[pos+1].rel_time;
                                priority    = pLista_prossimi_vincitori[pos+1].priority;
                                ID_heur     = pLista_prossimi_vincitori[pos+1].ID_heur;
                                Ffeasible   = pLista_prossimi_vincitori[pos+1].feasible;
                                best_pos    = pos + 1;
                        }
                }
                pos++;
        }
        return best_pos;
}
void QRolloutThread::StampaPermutazioni ( TJob *pPermutazione,int pDim )
{
    for ( int i = 0; i < pDim; i++ )
        emit ScriviASchermo(  QString(" %1 ").arg(pPermutazione[i].ID) );

}
int QRolloutThread::TrovaPosizioneAssoluta ( TJob *pArray,int pPos_rel )
{
    int pos_assoluta = 0;
    int i = 0,j = 0;
    while ( i <= pPos_rel )
    {
        if ( pArray[j].ID != -1 ) i++;
        j++;
    }
    pos_assoluta = j-1;
    return pos_assoluta;
}
void QRolloutThread::VerificaCambiamentoMacchina (  int *pPrimo_Passo_M1,
                                                    int *pPrimo_Passo_M2,
                                                    int *pPrimo_Passo_M3 )
{
    TSchedula *temp = NULL;
    if ( pPrimo_Passo_M1[0] == 0 )
    {
        temp = GMacch1_Sched;
        if ( temp->ID_job != -3 )
            pPrimo_Passo_M1[0] = 1;

    }
    if ( pPrimo_Passo_M2[0] == 0 )
    {
        if ( GNum_Macchine >= 2 )
        {
            temp = GMacch2_Sched;
            if ( temp->ID_job != -3 )
                pPrimo_Passo_M2[0] = 1;

        }
    }
    if ( pPrimo_Passo_M3[0] == 0 )
    {
        if ( GNum_Macchine >= 3 )
        {
            temp = GMacch3_Sched;
            if ( temp->ID_job != -3 )
                pPrimo_Passo_M3[0] = 1;

        }
    }
    if ( pPrimo_Passo_M1[0] == 1 )
    {
        temp = GMacch1_Sched;
        if ( temp->ID_job == -3 )
            pPrimo_Passo_M1[0] = 0;

    }
    if ( pPrimo_Passo_M2[0] == 1 )
    {
        if ( temp->ID_job == -3 )
        {
            temp = GMacch2_Sched;
            if ( temp->next == NULL )
                pPrimo_Passo_M2[0] = 0;

        }
    }
    if ( pPrimo_Passo_M3[0]==1 )
    {
        if ( GNum_Macchine>=3 )
        {
            temp = GMacch3_Sched;
            if ( temp->ID_job==-3 )
                pPrimo_Passo_M3[0]=0;
        }
    }
}
/***********************************************************************************************/

/***********************************************************************************************/

void QRolloutThread::SalvaSeMeglio ( TJob* pPermutazioni)
{

    int i = 0;
    if
            (
             ( Ffeasible > Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax < Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax < Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy < Ftardy_Best )
                 )
             )
    {
        for ( i = 0;i < GNum_Job; i++ )
            Fpermutazione_Buffer[i] = pPermutazioni[i];

        Ffeasible_Best  = Ffeasible;
        Lmax_best       = FLmax;
        Cmax_best       = FCmax;
        Ftardy_Best     = FTardy;
    }


}
void QRolloutThread::StampaSequenzaMacchina ( TSchedula *M )
{
    TSchedula *temp;
    emit ScriviASchermo(  "\n" );
    temp = M;
    while ( temp->next != NULL )
    {
        emit ScriviASchermo( QString("%1 ").arg(temp->ID_job) );
        temp =temp->next;
    }
    emit ScriviASchermo(  "\n" );
}
void QRolloutThread::VerificaSeElementiUguali ( TJob *perm_di_passaggio,int num_job )
{
    int j = 0;
    int i = 0;
    for ( i = 0; i < num_job; i++ )
        for ( j = i + 1;j < num_job; j++ )
        {
            if ( perm_di_passaggio[i].ID == perm_di_passaggio[j].ID )
            {
                emit ScriviASchermo(QString("\n ERRORE JOB UGUALI %1 pos %2 e pos %3 num %4 \n\a").
                                    arg(perm_di_passaggio[i].ID).
                                    arg(i+1).
                                    arg(j+1).
                                    arg(perm_di_passaggio[j].ID) );
                exit ( 0 );
            }
        }

}
int *QRolloutThread::TrovaMigliori ( TNext_Elem *lista_prossimi_vincitori,int num_next_job )
{
    int num_next_job_locale;
    int *lista_migliori_passo_precedente;
    num_next_job_locale = ceil ( ( float ) num_next_job/2 );
    lista_migliori_passo_precedente = new int[num_next_job_locale+1];

    int pos=0,best_pos = 0;
    best_pos = pos;
    int ID_job;
    int macchina;
    int fine;
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
    int Feasible;
    int i,j;
    j = 0;
    while ( j < num_next_job_locale+1 )
    {
        best_pos=0;
        i=0;
        while ( lista_prossimi_vincitori[i].ID_job==-1 )
        {
            i++;
        }
        ID_job      = lista_prossimi_vincitori[i].ID_job;
        macchina    = lista_prossimi_vincitori[i].macchina;
        fine        = lista_prossimi_vincitori[i].fine;
        inizio      = lista_prossimi_vincitori[i].inizio;
        tipo        = lista_prossimi_vincitori[i].tipo;
        index_camp  = lista_prossimi_vincitori[i].index_camp;
        LMAX        = lista_prossimi_vincitori[i].Lmax_pers;
        TARDY       = lista_prossimi_vincitori[i].Tardy_pers;
        L_max       = lista_prossimi_vincitori[i].Lmax;
        C_max       = lista_prossimi_vincitori[i].Cmax;
        tardy       = lista_prossimi_vincitori[i].Tardy;
        ID_heur     = lista_prossimi_vincitori[i].ID_heur;
        rel_time    = lista_prossimi_vincitori[i].rel_time;
        proc_time   = lista_prossimi_vincitori[i].proc_time;
        duedate     = lista_prossimi_vincitori[i].duedate;
        deadline    = lista_prossimi_vincitori[i].deadline;
        priority    = lista_prossimi_vincitori[i].priority;
        Feasible    = lista_prossimi_vincitori[i].feasible;
        pos = i;
        best_pos = pos;
        while ( pos< num_next_job-1 )
        {
            if ( lista_prossimi_vincitori[pos+1].ID_job!=-1 )
            {
                if ( Feasible <= lista_prossimi_vincitori[pos+1].feasible )
                {
                    if
                      (
                        ( Feasible ==0 )
                        &&
                        ( lista_prossimi_vincitori[pos+1].feasible==1 )
                      )
                    {
                        ID_job      = lista_prossimi_vincitori[pos+1].ID_job;
                        macchina    = lista_prossimi_vincitori[pos+1].macchina;
                        tipo        = lista_prossimi_vincitori[pos+1].tipo;
                        fine        = lista_prossimi_vincitori[pos+1].fine;
                        inizio      = lista_prossimi_vincitori[pos+1].inizio;
                        index_camp  = lista_prossimi_vincitori[pos+1].index_camp;
                        LMAX        = lista_prossimi_vincitori[pos+1].Lmax_pers;
                        TARDY       = lista_prossimi_vincitori[pos+1].Tardy_pers;
                        L_max       = lista_prossimi_vincitori[pos+1].Lmax;
                        C_max       = lista_prossimi_vincitori[pos+1].Cmax;
                        tardy       = lista_prossimi_vincitori[pos+1].Tardy;
                        deadline    = lista_prossimi_vincitori[pos+1].deadline;
                        duedate     = lista_prossimi_vincitori[pos+1].duedate;
                        proc_time   = lista_prossimi_vincitori[pos+1].proc_time;
                        rel_time    = lista_prossimi_vincitori[pos+1].rel_time;
                        priority    = lista_prossimi_vincitori[pos+1].priority;
                        ID_heur     = lista_prossimi_vincitori[pos+1].ID_heur;
                        Feasible    = lista_prossimi_vincitori[pos+1].feasible;
                        best_pos    = pos + 1;
                    }
                    else if
                            (
                             ( L_max > lista_prossimi_vincitori[pos+1].Lmax )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max > lista_prossimi_vincitori[pos+1].Cmax )
                                 )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == lista_prossimi_vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy > lista_prossimi_vincitori[pos+1].Tardy )
                                 )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == lista_prossimi_vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == lista_prossimi_vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > lista_prossimi_vincitori[pos+1].Lmax_pers )
                                 )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == lista_prossimi_vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == lista_prossimi_vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > lista_prossimi_vincitori[pos+1].Lmax_pers )
                                 &&
                                 (
                                     ( lista_prossimi_vincitori[pos+1].duedate !=0 )
                                     &&
                                     ( duedate > lista_prossimi_vincitori[pos+1].duedate )
                                     )
                                 )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == lista_prossimi_vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == lista_prossimi_vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > lista_prossimi_vincitori[pos+1].Lmax_pers )
                                 &&
                                 ( duedate == lista_prossimi_vincitori[pos+1].duedate )
                                 &&
                                 (
                                     ( lista_prossimi_vincitori[pos+1].deadline !=0 )
                                     &&
                                     ( deadline > lista_prossimi_vincitori[pos+1].deadline )
                                     )
                                 )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == lista_prossimi_vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == lista_prossimi_vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > lista_prossimi_vincitori[pos+1].Lmax_pers )
                                 &&
                                 ( duedate == lista_prossimi_vincitori[pos+1].duedate )
                                 &&
                                 ( deadline == lista_prossimi_vincitori[pos+1].deadline )
                                 &&
                                 ( proc_time > lista_prossimi_vincitori[pos+1].proc_time )
                                 )
                             ||
                             (
                                 ( L_max == lista_prossimi_vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == lista_prossimi_vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == lista_prossimi_vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > lista_prossimi_vincitori[pos+1].Lmax_pers )
                                 &&
                                 ( duedate == lista_prossimi_vincitori[pos+1].duedate )
                                 &&
                                 ( deadline == lista_prossimi_vincitori[pos+1].deadline )
                                 &&
                                 ( proc_time == lista_prossimi_vincitori[pos+1].proc_time )
                                 &&
                                 ( rel_time > lista_prossimi_vincitori[pos+1].proc_time )
                                 )
                             )
                    {
                        ID_job      = lista_prossimi_vincitori[pos+1].ID_job;
                        macchina    = lista_prossimi_vincitori[pos+1].macchina;
                        tipo        = lista_prossimi_vincitori[pos+1].tipo;
                        fine        = lista_prossimi_vincitori[pos+1].fine;
                        inizio      = lista_prossimi_vincitori[pos+1].inizio;
                        index_camp  = lista_prossimi_vincitori[pos+1].index_camp;
                        LMAX        = lista_prossimi_vincitori[pos+1].Lmax_pers;
                        TARDY       = lista_prossimi_vincitori[pos+1].Tardy_pers;
                        L_max       = lista_prossimi_vincitori[pos+1].Lmax;
                        C_max       = lista_prossimi_vincitori[pos+1].Cmax;
                        tardy       = lista_prossimi_vincitori[pos+1].Tardy;
                        deadline    = lista_prossimi_vincitori[pos+1].deadline;
                        duedate     = lista_prossimi_vincitori[pos+1].duedate;
                        proc_time   = lista_prossimi_vincitori[pos+1].proc_time;
                        rel_time    = lista_prossimi_vincitori[pos+1].rel_time;
                        priority    = lista_prossimi_vincitori[pos+1].priority;
                        ID_heur     = lista_prossimi_vincitori[pos+1].ID_heur;
                        Feasible    = lista_prossimi_vincitori[pos+1].feasible;
                        best_pos    = pos + 1;
                    }
                }
            }
            pos++;
        }
        lista_migliori_passo_precedente[j] = lista_prossimi_vincitori[best_pos].ID_job;
        lista_prossimi_vincitori[best_pos].ID_job = -1;
        j++;
    }
    return lista_migliori_passo_precedente;


}

int *QRolloutThread:: PurificaListaJobPrescelti ( int *pLista_migliori_passo_precedente,int pDim_lista, int *pProssima_dimensione )
{
    int j,i;
    int *lista_purificata;
    int contatore = 0;
    int dim_lista_locale = ceil ( ( float ) pDim_lista/2 ) +1;
    for ( i = 0; i < dim_lista_locale - 1; i++ )
        for ( j = i + 1; j < dim_lista_locale; j++ )
        {
            if (
                    ( pLista_migliori_passo_precedente[i] != -1 )
                    &&
                    ( pLista_migliori_passo_precedente[i] == pLista_migliori_passo_precedente[j] )
                    )
            {
                pLista_migliori_passo_precedente[j] = -1;
                contatore++;
            }
        }

    lista_purificata = new int[dim_lista_locale-contatore-1];
    j = 0;
    for ( i=1;i<dim_lista_locale;i++ )
    {
        if ( pLista_migliori_passo_precedente[i]!=-1 )
        {
            lista_purificata[j]=pLista_migliori_passo_precedente[i];
            j++;
        }
    }
    pProssima_dimensione[0] = ( dim_lista_locale-contatore-1 );
    return lista_purificata;
}

int *QRolloutThread::TrovaProssimiMigliori ( TNext_Elem *pLista_Prox_Vincitori,int pNum_Next_Job )
{
    int num_next_job_locale;
    int *lista_migliori_passo_precedente;
    num_next_job_locale = ceil ( ( float ) pNum_Next_Job/2 );
    lista_migliori_passo_precedente = new int[num_next_job_locale+1];

    int pos = 0;
    int best_pos = 0;
    best_pos = pos;
    int ID_job;
    int macchina;
    int fine;
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
    int Feasible;
    int i,j;
    j = 0;
    while ( j < num_next_job_locale+1 )
    {
        best_pos = 0;
        i = 0;
        while ( pLista_Prox_Vincitori[i].ID_job==-1 )
            i++;

        ID_job      = pLista_Prox_Vincitori[i].ID_job;
        macchina    = pLista_Prox_Vincitori[i].macchina;
        fine        = pLista_Prox_Vincitori[i].fine;
        inizio      = pLista_Prox_Vincitori[i].inizio;
        tipo        = pLista_Prox_Vincitori[i].tipo;
        index_camp  = pLista_Prox_Vincitori[i].index_camp;
        LMAX        = pLista_Prox_Vincitori[i].Lmax_pers;
        TARDY       = pLista_Prox_Vincitori[i].Tardy_pers;
        L_max       = pLista_Prox_Vincitori[i].Lmax;
        C_max       = pLista_Prox_Vincitori[i].Cmax;
        tardy       = pLista_Prox_Vincitori[i].Tardy;
        ID_heur     = pLista_Prox_Vincitori[i].ID_heur;
        rel_time    = pLista_Prox_Vincitori[i].rel_time;
        proc_time   = pLista_Prox_Vincitori[i].proc_time;
        duedate     = pLista_Prox_Vincitori[i].duedate;
        deadline    = pLista_Prox_Vincitori[i].deadline;
        priority    = pLista_Prox_Vincitori[i].priority;
        Feasible    = pLista_Prox_Vincitori[i].feasible;
        pos = i;
        best_pos = pos;
        while ( pos < pNum_Next_Job-1 )
        {
            if ( pLista_Prox_Vincitori[pos+1].ID_job!=-1 )
            {
                if ( Feasible <= pLista_Prox_Vincitori[pos+1].feasible )
                {
                    if
                      (
                        ( Feasible ==0 )
                        &&
                        ( pLista_Prox_Vincitori[pos+1].feasible==1 )
                      )
                    {
                        ID_job      = pLista_Prox_Vincitori[pos+1].ID_job;
                        macchina    = pLista_Prox_Vincitori[pos+1].macchina;
                        tipo        = pLista_Prox_Vincitori[pos+1].tipo;
                        fine        = pLista_Prox_Vincitori[pos+1].fine;
                        inizio      = pLista_Prox_Vincitori[pos+1].inizio;
                        index_camp  = pLista_Prox_Vincitori[pos+1].index_camp;
                        LMAX        = pLista_Prox_Vincitori[pos+1].Lmax_pers;
                        TARDY       = pLista_Prox_Vincitori[pos+1].Tardy_pers;
                        L_max       = pLista_Prox_Vincitori[pos+1].Lmax;
                        C_max       = pLista_Prox_Vincitori[pos+1].Cmax;
                        tardy       = pLista_Prox_Vincitori[pos+1].Tardy;
                        deadline    = pLista_Prox_Vincitori[pos+1].deadline;
                        duedate     = pLista_Prox_Vincitori[pos+1].duedate;
                        proc_time   = pLista_Prox_Vincitori[pos+1].proc_time;
                        rel_time    = pLista_Prox_Vincitori[pos+1].rel_time;
                        priority    = pLista_Prox_Vincitori[pos+1].priority;
                        ID_heur     = pLista_Prox_Vincitori[pos+1].ID_heur;
                        Feasible    = pLista_Prox_Vincitori[pos+1].feasible;
                        best_pos    = pos + 1;
                    }
                    else if
                            (
                             ( L_max > pLista_Prox_Vincitori[pos+1].Lmax )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max > pLista_Prox_Vincitori[pos+1].Cmax )
                                 )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == pLista_Prox_Vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy > pLista_Prox_Vincitori[pos+1].Tardy )
                                 )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == pLista_Prox_Vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == pLista_Prox_Vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > pLista_Prox_Vincitori[pos+1].Lmax_pers )
                                 )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == pLista_Prox_Vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == pLista_Prox_Vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > pLista_Prox_Vincitori[pos+1].Lmax_pers )
                                 &&
                                 (
                                     ( pLista_Prox_Vincitori[pos+1].duedate !=0 )
                                     &&
                                     ( duedate > pLista_Prox_Vincitori[pos+1].duedate )
                                     )
                                 )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == pLista_Prox_Vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == pLista_Prox_Vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > pLista_Prox_Vincitori[pos+1].Lmax_pers )
                                 &&
                                 ( duedate == pLista_Prox_Vincitori[pos+1].duedate )
                                 &&
                                 (
                                     ( pLista_Prox_Vincitori[pos+1].deadline !=0 )
                                     &&
                                     ( deadline > pLista_Prox_Vincitori[pos+1].deadline )
                                     )
                                 )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == pLista_Prox_Vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == pLista_Prox_Vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > pLista_Prox_Vincitori[pos+1].Lmax_pers )
                                 &&
                                 ( duedate == pLista_Prox_Vincitori[pos+1].duedate )
                                 &&
                                 ( deadline == pLista_Prox_Vincitori[pos+1].deadline )
                                 &&
                                 ( proc_time > pLista_Prox_Vincitori[pos+1].proc_time )
                                 )
                             ||
                             (
                                 ( L_max == pLista_Prox_Vincitori[pos+1].Lmax )
                                 &&
                                 ( C_max == pLista_Prox_Vincitori[pos+1].Cmax )
                                 &&
                                 ( tardy == pLista_Prox_Vincitori[pos+1].Tardy )
                                 &&
                                 ( LMAX > pLista_Prox_Vincitori[pos+1].Lmax_pers )
                                 &&
                                 ( duedate == pLista_Prox_Vincitori[pos+1].duedate )
                                 &&
                                 ( deadline == pLista_Prox_Vincitori[pos+1].deadline )
                                 &&
                                 ( proc_time == pLista_Prox_Vincitori[pos+1].proc_time )
                                 &&
                                 ( rel_time > pLista_Prox_Vincitori[pos+1].proc_time )
                                 )
                             )
                    {
                        ID_job      = pLista_Prox_Vincitori[pos+1].ID_job;
                        macchina    = pLista_Prox_Vincitori[pos+1].macchina;
                        tipo        = pLista_Prox_Vincitori[pos+1].tipo;
                        fine        = pLista_Prox_Vincitori[pos+1].fine;
                        inizio      = pLista_Prox_Vincitori[pos+1].inizio;
                        index_camp  = pLista_Prox_Vincitori[pos+1].index_camp;
                        LMAX        = pLista_Prox_Vincitori[pos+1].Lmax_pers;
                        TARDY       = pLista_Prox_Vincitori[pos+1].Tardy_pers;
                        L_max       = pLista_Prox_Vincitori[pos+1].Lmax;
                        C_max       = pLista_Prox_Vincitori[pos+1].Cmax;
                        tardy       = pLista_Prox_Vincitori[pos+1].Tardy;
                        deadline    = pLista_Prox_Vincitori[pos+1].deadline;
                        duedate     = pLista_Prox_Vincitori[pos+1].duedate;
                        proc_time   = pLista_Prox_Vincitori[pos+1].proc_time;
                        rel_time    = pLista_Prox_Vincitori[pos+1].rel_time;
                        priority    = pLista_Prox_Vincitori[pos+1].priority;
                        ID_heur     = pLista_Prox_Vincitori[pos+1].ID_heur;
                        Feasible    = pLista_Prox_Vincitori[pos+1].feasible;
                        best_pos    = pos + 1;
                    }
                }
            }
            pos++;
        }
        lista_migliori_passo_precedente[j] = best_pos;
        pLista_Prox_Vincitori[best_pos].ID_job=-1;
        j++;
    }
    return lista_migliori_passo_precedente;


}

//*****************************************************************
// Funzione di inizializazione del Thread
//*****************************************************************
int QRolloutThread::QRolloutThreadInicialize(QString pFileConfig,
                                             QString pFileSolution)
{
    GNum_Job = 0;
    GNum_Tipi = 0;
    GNum_Macchine = 0;

    FLmax               = 0;// massima lateness
    FCmax               = 0;//Makespan
    FTardy              = 0;//numero di tardy jobs per schedula
    Ffeasible           = 1; // mi dice se la schedula costruita �feasible
    Lmax_best           = -65000;
    Cmax_best           = -65000;
    Ftardy_Best         = -65000;
    Ffeasible_Best      = -2;
    Fswap_Lat_Tard      = 0;
    FTempo_Sec_Inizio1  = 0 ;
    FTempo_Inizio1      = 0;
    FTempo_Inizio2      = 0;
    FTempo_Inizio3      = 0;
    FTempo_Inizio4      = 0;
    FTempo_Sec_Fine1    = 0;
    FTempo_Fine1        = 0;
    FTempo_Fine2        = 0;
    FTempo_Fine3        = 0;

    CaricaFileConfigurazione (pFileConfig.toLocal8Bit().data());

    QString local_search_mode_text;
    local_search_mode_text.setNum(FLocal_Search_Mode );
    emit ScriviASchermo( local_search_mode_text);

    strcpy ( FInstance_File_Name, pFileSolution.toLocal8Bit().data() );
    if ( !strcmp ( FOutput_File,"--force" ) )
    {
            Fforce =1;
            strcpy ( FOutput_File,"./output.txt" );
    }

    if (    ( Ftipo_Rollout != 0 ) &&
            ( Ftipo_Rollout != 1 ) &&
            ( Ftipo_Rollout != 2 ) &&
            ( Ftipo_Rollout != 3 ) &&
            ( Ftipo_Rollout != 4 ) &&
            ( Ftipo_Rollout != 5 ) &&
            ( Ftipo_Rollout != 6 ) &&
            ( Ftipo_Rollout != 7 ) &&
            ( Ftipo_Rollout != 8 ) &&
            ( Ftipo_Rollout != 9 ) &&
            ( Ftipo_Rollout != 10 ) &&
            ( Ftipo_Rollout != 11 ) &&
            ( Ftipo_Rollout != 12 ) &&
            ( Ftipo_Rollout != 13)
       )
    {
            emit ScriviASchermo( "\nERROR: il primo parametro puo' essere solo 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 o 0!\n\a" );
            return 0;
    }

    //apro il file dove e' contenuta l'istanza da considerare
    Fistanza = fopen ( FInstance_File_Name,"r" );
    if ( Fistanza == NULL )
    {
            emit ScriviASchermo(  "\n Error file inesistente \a\n" );
            return 0;
    }

    FfileOut = fopen ( FOutput_File,"a+" );
    if ( FfileOut == NULL )
    {
            emit ScriviASchermo(  "Errore di apertura del file "+ QString::fromLocal8Bit(FOutput_File) );
            fclose(Fistanza);
            return 0;
    }


    GNum_Macchine = CaricaIndisponibilita ( Fistanza ); //info nella funzione

    if ( GNum_Macchine == 0 ) //dev'esserci almeno una macchina per il problema
    {
            emit ScriviASchermo(  "\n Error! formato file non riconosciuto\n" );
            fclose(Fistanza);
            fclose(FfileOut);
            return 0;
    }

    //a questo punto devo caricare le matrici con le caratteristiche dei singoli job da mandare
    //in esecuzione sulle macchine parallele.
    Fprossimo = new TNext_Elem;
    Fprossimo->next = NULL;

    CaricaListaJob ( Fistanza );  //info nella funz stessa

    Fnum_Heur = Calcola_Numero_Euristiche ( Ftipo_Eur ); //ritorna il numero di heuristiche del tipo

    FNum_Heur_Used = Fnum_Heur;

    Ffunzioni = new TStruttura_Funz[Fnum_Heur];//alloca lo spazio per l'array delle heuristiche

    Inizializza_Struttura_Euristiche ( Ftipo_Eur ); //info nella funzione stessa

    //inizializza le schedule
    GMacch1_Sched = NULL;
    GMacch2_Sched = NULL;
    GMacch3_Sched = NULL;

    Schedula::AzzeraSchedule(); //info nella funzione stessa

    return 1;
}

int QRolloutThread::getNumJob(){
    return GNum_Job;
}

QStringList *QRolloutThread::GetListaPolitiche()
{
    QStringList *lListaNomiPolitiche = new QStringList;
    lListaNomiPolitiche->append("No Exclusion");
    lListaNomiPolitiche->append("Politica Never Win");
    lListaNomiPolitiche->append("Politica Less 3Perc Reset");
    lListaNomiPolitiche->append("Politica Less 5Perc Reset");
    lListaNomiPolitiche->append("Politica Less 7Perc Reset");
    lListaNomiPolitiche->append("Politica Less 3Perc");
    lListaNomiPolitiche->append("Politica Less 5Perc");
    lListaNomiPolitiche->append("Politica Less 7Perc");
    lListaNomiPolitiche->append("Politica Linear Spread");
    lListaNomiPolitiche->append("Politica Linear Spread Reset");
    lListaNomiPolitiche->append("Politica Never Win One at a time");
    return lListaNomiPolitiche;

}


QStringList *QRolloutThread::GetListaRicercheLocali()
{
    QStringList *lListaNomiVNS = new QStringList;
    lListaNomiVNS->append("Nessuna");
    lListaNomiVNS->append("VNS");
    lListaNomiVNS->append("LS tra macchine");
    lListaNomiVNS->append("LS tra macchine VND");
    lListaNomiVNS->append("LS tra macchine Coda");
    return lListaNomiVNS;
}

QStringList *QRolloutThread::GetListaEuristicas()
{
    /*devuelve una lista de string con los tipos de las euristicas  */
    QStringList *lListaNamesEuristicas = new QStringList;
    lListaNamesEuristicas->append("EDD 1 tipo");
    lListaNamesEuristicas->append("EDD 2 tipo");
    lListaNamesEuristicas->append("Base");
    lListaNamesEuristicas->append("SPT semplice");
    lListaNamesEuristicas->append("LLF");
    lListaNamesEuristicas->append("Delta 7 ore");
    lListaNamesEuristicas->append("Delta proc. medio");
    lListaNamesEuristicas->append("Delta mezzo proc. medio");
    lListaNamesEuristicas->append("LLF delta 7 ore");
    lListaNamesEuristicas->append("LLF delta proc. medio");
    lListaNamesEuristicas->append("Delta 24 ore");
    lListaNamesEuristicas->append("Delta 3 proc. medio");
    lListaNamesEuristicas->append("LLF delta 24 ore");
    lListaNamesEuristicas->append("LLF delta 3 proc. medio");
    lListaNamesEuristicas->append("LLF delta mezzo proc. medio");
    lListaNamesEuristicas->append("Delta proc. medio + Base");
    lListaNamesEuristicas->append("10 euristiche");
    lListaNamesEuristicas->append("15 euristiche");
    return lListaNamesEuristicas;
}

QStringList *QRolloutThread::GetListaAlgoritmos()
{
    /*devuelve una lista de string con los tipos de rollout  */
    QStringList *lListaNamesRollout = new QStringList;

    lListaNamesRollout->append("Euristica");
    lListaNamesRollout->append("Rollout");
    lListaNamesRollout->append("Rollout Antonio");
    lListaNamesRollout->append("Rollout mod. 1");
    lListaNamesRollout->append("Rollout mod. 2");
    lListaNamesRollout->append("Rollout mod. 3");
    lListaNamesRollout->append("Rollout mod. 4");
    lListaNamesRollout->append("Rollout mod. 5");
    lListaNamesRollout->append("Rollout mod. 6");
    lListaNamesRollout->append("Rollout Old");
    lListaNamesRollout->append("Rollout heuristic pruning");
    lListaNamesRollout->append("Rollout dynamic job choosing");
    lListaNamesRollout->append("Rollout dynamic");

    return lListaNamesRollout;
}

void QRolloutThread::Inizializza_Struttura_Euristiche(int tipo_eur)
{
    switch(tipo_eur)
    {
        case 0:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneEdd_1Tipo;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 1:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneEdd_2Tipo;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 2:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneBase;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 3:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneSPTSemplice;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 4:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneLLF;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 5:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneDelta_7ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 6:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneDeltaProcMedio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 7:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneDeltaMezzoProcMedio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 8:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneLLFDelta_7ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 9:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneLLFDeltaProcMedio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 10:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::Permutazione_delta_14ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 11:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneDelta_3ProcMedio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 12:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneLLFdelta_14ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 13:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneLLFDelta_3ProcMedio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 14:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneLLFDeltaMezzoProcMedio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 15:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneDeltaProcMedio;
            Ffunzioni[0].ID_heur = 0;
            Ffunzioni[1].perc_utilizzo = 0;
            Ffunzioni[1].funz = Heuristics::PermutazioneBase;
            Ffunzioni[1].ID_heur = 1;
            break;
        }
        case 16:
        {
                Ffunzioni[0].perc_utilizzo = 0;
                Ffunzioni[0].funz = Heuristics::PermutazioneEdd_1Tipo;
                Ffunzioni[0].ID_heur = 0;
                Ffunzioni[1].perc_utilizzo = 0;
                Ffunzioni[1].funz = Heuristics::PermutazioneEdd_2Tipo;
                Ffunzioni[1].ID_heur = 0;
                Ffunzioni[2].perc_utilizzo = 0;
                Ffunzioni[2].funz = Heuristics::PermutazioneBase;
                Ffunzioni[2].ID_heur = 0;
                Ffunzioni[3].perc_utilizzo = 0;
                Ffunzioni[3].funz = Heuristics::PermutazioneSPTSemplice;
                Ffunzioni[3].ID_heur = 0;
                Ffunzioni[4].perc_utilizzo = 0;
                Ffunzioni[4].funz = Heuristics::PermutazioneLLF;
                Ffunzioni[4].ID_heur = 0;
                Ffunzioni[5].perc_utilizzo = 0;
                Ffunzioni[5].funz = Heuristics::PermutazioneDelta_7ore;
                Ffunzioni[5].ID_heur = 0;
                Ffunzioni[6].perc_utilizzo = 0;
                Ffunzioni[6].funz = Heuristics::PermutazioneDeltaProcMedio;
                Ffunzioni[6].ID_heur = 0;
                Ffunzioni[7].perc_utilizzo = 0;
                Ffunzioni[7].funz = Heuristics::PermutazioneDeltaMezzoProcMedio;
                Ffunzioni[7].ID_heur = 0;
                Ffunzioni[8].perc_utilizzo = 0;
                Ffunzioni[8].funz = Heuristics::PermutazioneLLFDelta_7ore;
                Ffunzioni[8].ID_heur = 0;
                Ffunzioni[9].perc_utilizzo = 0;
                Ffunzioni[9].funz = Heuristics::PermutazioneLLFDeltaProcMedio;
                Ffunzioni[9].ID_heur = 0;
                break;
        }
        case 17:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Heuristics::PermutazioneEdd_1Tipo;
            Ffunzioni[0].ID_heur = 0;

            Ffunzioni[1].perc_utilizzo = 0;
            Ffunzioni[1].funz = Heuristics::PermutazioneEdd_2Tipo;
            Ffunzioni[1].ID_heur = 1;

            Ffunzioni[2].perc_utilizzo = 0;
            Ffunzioni[2].funz = Heuristics::PermutazioneBase;
            Ffunzioni[2].ID_heur = 2;

            Ffunzioni[3].perc_utilizzo = 0;
            Ffunzioni[3].funz = Heuristics::PermutazioneSPTSemplice;
            Ffunzioni[3].ID_heur = 3;

            Ffunzioni[4].perc_utilizzo = 0;
            Ffunzioni[4].funz = Heuristics::PermutazioneLLF;
            Ffunzioni[4].ID_heur = 4;

            Ffunzioni[5].perc_utilizzo = 0;
            Ffunzioni[5].funz = Heuristics::PermutazioneDelta_7ore;
            Ffunzioni[5].ID_heur = 5;

            Ffunzioni[6].perc_utilizzo = 0;
            Ffunzioni[6].funz = Heuristics::PermutazioneDeltaProcMedio;
            Ffunzioni[6].ID_heur = 6;

            Ffunzioni[7].perc_utilizzo = 0;
            Ffunzioni[7].funz = Heuristics::PermutazioneDeltaMezzoProcMedio;
            Ffunzioni[7].ID_heur = 7;

            Ffunzioni[8].perc_utilizzo = 0;
            Ffunzioni[8].funz = Heuristics::PermutazioneLLFDelta_7ore;
            Ffunzioni[8].ID_heur = 8;

            Ffunzioni[9].perc_utilizzo = 0;
            Ffunzioni[9].funz = Heuristics::PermutazioneLLFDeltaProcMedio;
            Ffunzioni[9].ID_heur = 9;

            Ffunzioni[10].perc_utilizzo = 0;
            Ffunzioni[10].funz = Heuristics::Permutazione_delta_14ore;
            Ffunzioni[10].ID_heur = 10;


            Ffunzioni[11].perc_utilizzo = 0;
            Ffunzioni[11].funz = Heuristics::PermutazioneDelta_3ProcMedio;
            Ffunzioni[11].ID_heur = 11;

            Ffunzioni[12].perc_utilizzo = 0;
            Ffunzioni[12].funz = Heuristics::PermutazioneLLFdelta_14ore;
            Ffunzioni[12].ID_heur = 12;

            Ffunzioni[13].perc_utilizzo = 0;
            Ffunzioni[13].funz = Heuristics::PermutazioneLLFDelta_3ProcMedio;
            Ffunzioni[13].ID_heur = 13;

            Ffunzioni[14].perc_utilizzo = 0;
            Ffunzioni[14].funz = Heuristics::PermutazioneLLFDeltaMezzoProcMedio;
            Ffunzioni[14].ID_heur = 14;
            //funzioni[15].perc_utilizzo = 0;
            //funzioni[15].funz = Heuristics::permutazione_fittizia;
            //funzioni[15].ID_heur = 15;
            break;
        }
    }
}

int QRolloutThread::Calcola_Numero_Euristiche(int pTipo_Eur)
{
    if(pTipo_Eur == 15)
        return 2;
    else if(pTipo_Eur == 16)
        return 10;
    else if(pTipo_Eur == 17)
        return 15;
    else
        return 1;

}

//aggiunge un job in coda ad un insieme dinamico se questo job non  gi  presente nell'insieme
TInsieme_Dinamico *QRolloutThread::AggiungiDinamicamente(TInsieme_Dinamico *pInsieme, TJob pJob_Insert)
{
    TInsieme_Dinamico *temp;
    if(pInsieme == NULL)
    {
        pInsieme            = new TInsieme_Dinamico;
        pInsieme->ID        = pJob_Insert.ID;
        pInsieme->proc_time = pJob_Insert.proc_time;
        pInsieme->duedate   = pJob_Insert.duedate;
        pInsieme->deadline  = pJob_Insert.deadline;
        pInsieme->priority  = pJob_Insert.priority;
        pInsieme->rel_time  = pJob_Insert.rel_time;
        pInsieme->next      = NULL;
    }
    else{
        temp = pInsieme;
        while (temp->next != NULL && (temp->ID != pJob_Insert.ID))
            temp = temp->next;

        if(temp->next == NULL && temp->ID != pJob_Insert.ID)
        {
            temp->next      = new TInsieme_Dinamico;
            temp            = temp->next;
            temp->ID        = pJob_Insert.ID;
            temp->proc_time = pJob_Insert.proc_time;
            temp->duedate   = pJob_Insert.duedate;
            temp->deadline  = pJob_Insert.deadline;
            temp->priority  = pJob_Insert.priority;
            temp->rel_time  = pJob_Insert.rel_time;
            temp->next      = NULL;
        }
        else
        {
            //se c'era gi quel job non fare nulla
        }
    }

    return pInsieme;
}

//Copia in insieme tutti i job di insieme_next meno job_insert che  quello che viene schedulato dopo il turno di rollout e che quindi non pu essere rischedulato
TInsieme_Dinamico *QRolloutThread::Copia_Dinamicamente(TInsieme_Dinamico *pInsieme,
                                                       TInsieme_Dinamico *pInsieme_Next,
                                                       TJob pJob_Insert){
    TInsieme_Dinamico *temp, *temp2;
    temp = pInsieme;
    temp2 = pInsieme;
    //libero insieme
    while (temp != NULL)
    {
        if (temp->next != NULL){
            temp2 = temp->next;
            delete(temp);
            temp = temp2;
        }
        else{
            delete(temp);
            temp = NULL;
        }
    }
    pInsieme=temp;
    while (pInsieme_Next != NULL) {
        if (pInsieme_Next->ID != pJob_Insert.ID){
            if(pInsieme==NULL){
                temp= new TInsieme_Dinamico();
                pInsieme=temp;
                temp->ID=pInsieme_Next->ID;
                temp->proc_time=pInsieme_Next->proc_time;
                temp->duedate=pInsieme_Next->duedate;
                temp->deadline=pInsieme_Next->deadline;
                temp->priority=pInsieme_Next->priority;
                temp->rel_time=pInsieme_Next->rel_time;
                temp->next=NULL;
                temp2=pInsieme_Next;
                pInsieme_Next=pInsieme_Next->next;
                delete(temp2);
            }
            else{
                temp->next=new TInsieme_Dinamico();
                temp=temp->next;
                temp->ID=pInsieme_Next->ID;
                temp->proc_time=pInsieme_Next->proc_time;
                temp->duedate=pInsieme_Next->duedate;
                temp->deadline=pInsieme_Next->deadline;
                temp->priority=pInsieme_Next->priority;
                temp->rel_time=pInsieme_Next->rel_time;
                temp->next=NULL;
                temp2=pInsieme_Next;
                pInsieme_Next=pInsieme_Next->next;
                delete(temp2);
            }
        }
        else{
            temp2=pInsieme_Next;
            pInsieme_Next=pInsieme_Next->next;
            delete(temp2);
        }
    }
    return pInsieme;
}


/*calcola le statistiche relative all'utilizzo delle funzioni
Restituisce un vettore con le percentuali d'utilizzo per ogni funzione euristica
*/
float *QRolloutThread::CalcolaStatistiche()
{
    int i, somma=0;
    float *stat;
    stat = new float[Fnum_Heur];

    for (i = 0; i < Fnum_Heur; i++)
        if (Ffunzioni[i].perc_utilizzo != -1)
            somma += Ffunzioni[i].perc_utilizzo;


    for (i = 0;i<Fnum_Heur;i++)
    {
        if(Ffunzioni[i].perc_utilizzo!=-1)
            stat[i] = ((float)Ffunzioni[i].perc_utilizzo/somma)*100.0;
        else

            stat[i] = 0.0;
    }
    return stat;
}

// questo metodo dovrebbe essere una funzione in una libreria a parte
int QRolloutThread::Round(double pValue)
//round up a double
{
    double t;
    double value;
    t = pValue - floor(pValue);
    if (t >= 0.5)
        value = ceil(pValue);
    else
        value = floor(pValue);

    return value;

}

/*Metodo che calcola la reale CPU time del thread*/
int64_t QRolloutThread::GetCpuTime(void)
{
    int64_t t;
#ifndef WIN32
    struct timeval time;
    struct rusage Rusage;
    getrusage(RUSAGE_SELF, &Rusage);
    time =   Rusage.ru_stime;
    // t e' in millisecondi
    t   =  Round( (double)time.tv_sec * 1000.0 + (double)time.tv_usec/1000.0) ;
#else
    // copiato dal codice di Ruben
    LPFILETIME createtime, exittime, kerneltime, usertime;
    GetThreadTimes(GetCurrentThread(), createtime, exittime, kerneltime,usertime);
    t = (int64_t)usertime/10;
#endif
    return t;
}

