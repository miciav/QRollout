#include "qRolloutThread.h"
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define Max_tipi 40
//#include "rollout_time.cpp"
#include "Euristiche.cpp"
#include "ricerche_locali.cpp"
#include <QtCore>
#include <qstring.h>


void QRolloutThread::Carica_File_Configurazione ( char *path_file_conf )
{
    FILE *file_conf;
    if ( ( file_conf = fopen ( path_file_conf,"r" ) ) == NULL )
        emit ScriviASchermo( "\n errore nel file di configurazione \n" );
    else
    {
        //0 -> rollout   -   1->euristica
        fscanf ( file_conf,"%d \n",&FRollout_Or_Heuristic );
        //scelta euristica da lanciare singolarmente
        fscanf ( file_conf,"%d \n",&FEuristica );
        //scelta del tipo di rollout da lanciare
        fscanf ( file_conf,"%d \n",&Ftipo_Rollout );
        //scelta dell'algoritmo euristico da utilizzare nel rollout
        fscanf ( file_conf,"%d \n",&Ftipo_Eur );
        //scelta del tempo
        fscanf ( file_conf,"%d \n",&FTempo );
        //percorso+nome del file di output della computazione
        fscanf ( file_conf,"%s \n",FOutput_File );
        fscanf ( file_conf,"%d \n",&Fforce );
        //scelta della politica di pruning, utilizzata solo in alcuni tipi di rollout
        fscanf ( file_conf,"%d \n",&FPolitica_Pruning );
        //scelta del tipo di ricerca locale da applicare successivamente al rollout (0 se non la si vuole applicare)
        fscanf ( file_conf,"%d",&FLocal_Search_Mode );
        fclose ( file_conf );
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
    /*Lo faccio qui perché sempre in questa funzione faccio il delete*/
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
        Azzera_Schedule();
        Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                      GMacch2_Sched,
                                      GMacch3_Sched,
                                      Fprossimo,
                                      FPermutazione_Finale,
                                      GNum_Job );

        Stampa_Risultati_Su_File ( FfileOut,FInstance_File,0,Fforce );

        int c;

        //RICERCA LOCALE (PURA, CODA, CON VND) O VNS
        FTempo_Inizio2 = clock() ;
        switch(FLocal_Search_Mode)
        {
        case 0:{
            //nor local search neither VNS
            break;
        }
        case 1:{
            c = VNS(GMacch1_Sched,GMacch2_Sched,GMacch3_Sched);
            break;
        }
        case 2:{
            c = Ricerca_Locale_Tra_Macchine(GMacch1_Sched, GMacch2_Sched, GMacch3_Sched);
            break;
        }
        case 3:{
            c = Ricerca_Locale_Tra_Macchine_VND(GMacch1_Sched, GMacch2_Sched, GMacch3_Sched);
            break;
        }
        case 4:{
            c = Ricerca_Locale_Tra_Macchine_Coda(GMacch1_Sched, GMacch2_Sched, GMacch3_Sched);
            break;
        }
        }

        FTempo_Fine2 = clock();

        Valuta_Schedula (GMacch1_Sched,
                         GMacch2_Sched,
                         GMacch3_Sched,
                         Fprossimo );

        if (FLocal_Search_Mode==0)
            Stampa_Risultati_A_Video ( 0 );
        else
            Stampa_Risultati_A_Video ( 1 );

        Stampa_Risultati_Su_File ( FfileOut,FInstance_File,1,Fforce );

        //BILANCIAMENTO
        FTempo_Inizio3 = clock() ;

        //bilancio
        Bilanciamento_Schedule (GMacch1_Sched,
                                GMacch2_Sched,
                                GMacch3_Sched );

        FTempo_Fine3 = clock();

        Valuta_Schedula (GMacch1_Sched,
                         GMacch2_Sched,
                         GMacch3_Sched,
                         Fprossimo );

        Stampa_Risultati_A_Video ( 2 );
        Stampa_Risultati_Su_File ( FfileOut,FInstance_File,2,Fforce );

        Distruggi_Indisponibilita ( GNum_Macchine );
        Distruggi_Schedule ( GNum_Macchine );
        delete Fprossimo;
        delete Ffunzioni;
        delete GBest_Perm;
        delete GArray_Job; //disalloco la memoria occupata dall'array dei job.
        delete GArray_Tipi;//disalloco la memoria occupata dall'array dei tipi.

        fclose ( Fistanza );
        fclose ( FfileOut );
}
//**************************************************************************************************
//la funzione seguente carica i tempi di indisponibilita' delle macchine dal file,
//e restituisce il numero di macchine caricate o 0 se qualche errore e' stato
//individuato nel formato del file.
//**************************************************************************************************
int QRolloutThread::Carica_Indisponibilita ( FILE *istanza )
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
            QString testo = "ERRORE No M1\n";
            emit ScriviASchermo( testo );
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
                if ( i>numM1 )
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
                fscanf ( istanza,"%s",str ); //carico il numero di indisponibilitï¿½ï¿½?della M2
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
                        if ( i>numM2 )
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
                        fscanf ( istanza,"%s",str ); //carico il numero di indisponibilitï¿½ï¿½?della M3
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
        return totale_macchine; //il caricamento ï¿½ï¿½?andato a buon fine
        }
       catch(...) { // catch all exceptions
           return 0;
       }


}

//***********************************************************************************************
//la funzione seguente elimina la memoria occupata dalle lista dei tempi di indisponibilita'
//***********************************************************************************************
void QRolloutThread::Distruggi_Indisponibilita ( int pNum_macchine )
{
    Elimina_Lista ( GMacch1 );
    if ( pNum_macchine >= 2 ) //mi trovo nel caso di dispencing
        Elimina_Lista ( GMacch2 );
    if ( pNum_macchine == 3 )//mi trovo nel caso di counting
        Elimina_Lista ( GMacch3 );

}

//*****************************************************************************************************
//la funzione seguente elimina le liste di indisponibilita' ed e' asservita a distruggi_indisponibilita
//*****************************************************************************************************
void QRolloutThread::Elimina_Lista ( TElem * punt_lista )
{
    TElem *temp1;
    temp1 = punt_lista;
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

int QRolloutThread::Carica_Lista_Job ( FILE * istanza )
{
    char str[10];
    fscanf ( istanza,"%s",str );
    // a questo punto il file dovrebbe essere nella posizione che mi aspetto
    //cioe' dove si trovano le info sui job.

    if ( !strcmp ( str,"//" ) )
    {
        fscanf ( istanza,"%s",str );
        GNum_Job  = atoi ( str );
        fscanf ( istanza,"%s",str );
        if ( !strcmp ( str,"%%" ) )
        {
            GArray_Job = new TJob[GNum_Job];
            int i = 0;
            fscanf ( istanza,"%s",str );
            while ( strcmp ( str,"//" ) )
            {
                GArray_Job[i].ID = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Job[i].tipo = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Job[i].proc_time = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Job[i].duedate = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Job[i].deadline = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Job[i].priority = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Job[i].rel_time = atoi ( str );
                fscanf ( istanza,"%s",str );
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
                    fscanf ( istanza,"%s",str );
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
        fscanf ( istanza,"%s",str );
        GNum_Tipi = atoi ( str );
        GArray_Tipi = new TTipo[GNum_Tipi];
        fscanf ( istanza,"%s",str );
        if ( !strcmp ( str,"%%" ) )
        {
            int i = 0;
            fscanf ( istanza,"%s",str );
            while ( strcmp ( str,"//" ) )
            {
                GArray_Tipi[i].ID = atoi ( str );
                fscanf ( istanza,"%s",str );
                GArray_Tipi[i].MaxOpCamp = atoi ( str );
                fscanf ( istanza,"%s",str );
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
                    fscanf ( istanza,"%s",str );
                }
            }
        }
        else
        {
            emit ScriviASchermo(  "\n error sul numero dei tipi" );
            return 0;
        }
        int j = 0;
        fscanf ( istanza,"%s",str );
        while ( strcmp ( str,"//" ) )
        {
            int i = 0;
            for ( i=0;i<GNum_Tipi;i++ )
            {
                GCmaj_Matrix[j][i] = atoi ( str );
                fscanf ( istanza,"%s",str );
            }
            fscanf ( istanza,"%s",str );

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

int QRolloutThread::Costruisci_E_Valuta_Schedula (  TSchedula *pM1_sch_locale,
                                                    TSchedula *pM2_sch_locale,
                                                    TSchedula *pM3_sch_locale,
                                                    TNext_Elem *pProssimo,
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
    int *set;
    int cambio;
    int k;
    int schedulato;
    TSchedula *temp;
    TElem *temp1;
    //fine dichiarazione delle variabili
    // ____________________________________________________________________________________________________________________

    int *disponibilita  = new int[GNum_Macchine]; //ci dice quando la macchina e' disponibile
    int *setup_vett     = new int[GNum_Macchine]; //ci dice se la macchina deve attendere un setup
    temp_prox = pProssimo;

    set = new int[GNum_Macchine];
    while ( temp_prox->next != NULL ) // TODO questo si può mettere in una funzione
        temp_prox = temp_prox->next;

    //trovo l'elemento giusto

    for ( i = 0; i < pDim_job; i++ ) // considero singolarmente i job della pemutazione e costruisco
    {                                // la schedula

        // FIXME questo è sbagliato no? Pensiamolo bene
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
        temp = GMacch1_Sched;
        temp1 = GMacch1;
        Verifica_Macchina ( temp,
                           temp1,
                           disponibilita,
                           setup_vett,0,pPerm,i );
        if ( GNum_Macchine >= 2 )
        {
            temp = GMacch2_Sched;
            temp1 = GMacch2;
            Verifica_Macchina ( temp,
                               temp1,
                               disponibilita,
                               setup_vett,1,pPerm,i );
        }
        if ( GNum_Macchine == 3 )
        {
            temp = GMacch3_Sched;
            temp1 = GMacch3;
            Verifica_Macchina ( temp,
                               temp1,
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
            set[k]=-1;//inizializzo il set delle macchine

        set[cambio] = 1;
        for ( k = 1; k < GNum_Macchine; k++ )
        {
            if ( disponibilita[cambio] > disponibilita[k] )
            {
                int j = 0;
                cambio = k;
                for ( j = 0; j < GNum_Macchine; j++ )
                    set[j]=-1;

                set[k]=1;
            }
            else if ( disponibilita[cambio] == disponibilita[k] )
                set[k]=1;

        }

        //____________________________________________________________________________________________
        //a questo punto so quali macchine hanno tempo minimo uguale
        for ( k = 0; k < GNum_Macchine; k++ )
        {
            if ( set[k] == 1 && setup_vett[k] == 0 )
            {//allora schedulo il job sulla macchina k-esima
                schedulato = 2; //caso del primo job, quando non si deve pagare nessun setup
                if ( k == 0 ) // prima macchina
                {
                    Aggiungi_Schedula (GMacch1_Sched,
                                       pPerm[i],
                                       disponibilita[k],
                                       setup_vett[k] );
                    if ( i == 0 ) //mi serve l'info sul 1 job
                        Aggiorna_Temp_Prox ( k,pPerm[i],GMacch1_Sched,temp_prox );

                    break;
                }
                else if ( k == 1 ) // seconda macchina
                {
                    Aggiungi_Schedula (GMacch2_Sched,
                                       pPerm[i],
                                       disponibilita[k],
                                       setup_vett[k] );
                    if ( i == 0 ) //mi serve l'info sul 1 job
                        Aggiorna_Temp_Prox ( k,pPerm[i],GMacch2_Sched,temp_prox );

                    break;
                }
                else	// terza macchina
                {
                    Aggiungi_Schedula (GMacch3_Sched,
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
                        Aggiungi_Schedula (GMacch1_Sched,
                                           pPerm[i],
                                           disponibilita[k],
                                           setup_vett[k] );
                        if ( i == 0 ) //mi serve l'info sul 1 job
                            Aggiorna_Temp_Prox ( k,pPerm[i],GMacch1_Sched,temp_prox );

                        break;
                    }
                    else if ( k==1 )	//schedulo sulla seconda macchina
                    {
                        Aggiungi_Schedula (GMacch2_Sched,
                                           pPerm[i],
                                           disponibilita[k],
                                           setup_vett[k] );
                        if ( i == 0 ) //mi serve l'info sul 1 job
                            Aggiorna_Temp_Prox ( k,pPerm[i],GMacch2_Sched,temp_prox );

                        break;
                    }
                    else	//schedulo sulla terza macchina
                    {
                        Aggiungi_Schedula (GMacch3_Sched,
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
                            Aggiungi_Schedula (GMacch1_Sched,
                                               pPerm[i],
                                               disponibilita[k],
                                               setup_vett[k] );
                            if ( i == 0 ) //mi serve l'info sul 1 job
                                Aggiorna_Temp_Prox ( k,pPerm[i],GMacch1_Sched,temp_prox );

                            break;
                        }
                        else if ( k == 1 )	// schedulo sulla seconda macchina
                        {
                            Aggiungi_Schedula (GMacch2_Sched,
                                               pPerm[i],
                                               disponibilita[k],
                                               setup_vett[k] );

                            if ( i == 0 ) //mi serve l'info sul 1 job
                                Aggiorna_Temp_Prox ( k,pPerm[i],GMacch2_Sched,temp_prox );

                            break;
                        }
                        else	//schedulo sulla terza macchina
                        {
                            Aggiungi_Schedula (GMacch3_Sched,
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
    Valuta_Schedula (pM1_sch_locale,
                     pM2_sch_locale,
                     pM3_sch_locale,
                     temp_prox );
    delete set;
    delete[] disponibilita;
    delete[] setup_vett;
    return 1;
}

//*************************************************************************************************
//la funzione seguente aggiorna il valore di Temp_prox variabile utilizzata da Costruisci e valuta schedula
//***********************************************************************************************
void QRolloutThread::Aggiorna_Temp_Prox ( int k,TJob perm,TSchedula *M_sch,TNext_Elem *temp_prox )
{
        TSchedula *temp_macch;

        temp_prox->macchina = k+1;
        temp_macch = M_sch;
        while ( temp_macch->next != NULL )
                temp_macch = temp_macch->next;

        temp_prox->tipo         = temp_macch->tipo;
        temp_prox->inizio       = temp_macch->inizio;
        temp_prox->fine         = temp_macch->fine;
        temp_prox->Lmax_pers    = temp_macch->Lmax;
        temp_prox->Tardy_pers   = temp_macch->Tardy;
        temp_prox->index_camp   = temp_macch->index_camp;
        temp_prox->proc_time    = perm.proc_time;
        temp_prox->rel_time     = perm.rel_time;
        temp_prox->duedate      = perm.duedate;
        temp_prox->deadline     = perm.deadline;
        temp_prox->priority     = perm.priority;

}

//*************************************************************************************************
// la funzione seguente calcola il tempo minimo di schedulazione su ciascuna macchina per ogni job
// tiene conto dei setup e dei tempi di indisponibilita' delle macchine
//***********************************************************************************************
void Verifica_Macchina(TSchedula *pSchedula, TElem  *pElem, int *pDisponibilita, int *pSetup_vett,int p,TJob *perm,int i)
{
    int time_max = 0;
    int time_min = 0;
    int k;
    //int a=0;
    int slack1 = 0; //slack time, puo' essere utilizzato per eseguire setup prima della release date.

    while(pSchedula->next != NULL)
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
    //bisogna verificare se il job e' compatibile con le indisponibilitï¿½
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
                    {//in questo caso devo anche pagare un setup che puï¿½considerarsi come un ritardo dell'istante di inizio del processamento di un job.
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
            //il momento di disponibilitï¿½ ï¿½ pari al massimo tra time_mim + setup (eventuale) e time_max
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
                    {//in questo caso devo anche pagare un setup che puï¿½considerarsi come un ritardo dell'istante di inizio del processamento di un job.
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
                {// il primo job della schedula ï¿½ fittizio quindi non devo pagare nÃ¨ setup major nÃ¨ minor
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
                            )//controllo se posso inserire il job mentre giï¿½ï¿½ so di poter inserire il setup
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
void  QRolloutThread::Valuta_Schedula ( TSchedula *M1_sch_locale,TSchedula *M2_sch_locale,TSchedula *M3_sch_locale,TNext_Elem *prossimo )
{
    TSchedula *temp, *temp1;

    int *Cmax_temp = new int[GNum_Macchine];
    int lat =  -65000;
    int tard = 0;
    int feasible = 1;
    int k = 0;
    temp = M1_sch_locale;
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
        temp1=temp;
        temp=temp->next;
    }

    Cmax_temp[0]=temp1->fine;
    if ( GNum_Macchine >= 2 )
    {
        temp=M2_sch_locale;
        if ( temp->ID_job!=-3 )
        {
            while ( temp!=NULL )
            {

                for ( k=0;k<GNum_Job;k++ )
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
                temp=temp->next;
            }
            Cmax_temp[1]=temp1->fine;
        }
        else
        {
            Cmax_temp[1]=0;
        }
    }
    if ( GNum_Macchine == 3 )
    {
        temp=M3_sch_locale;
        if ( temp->ID_job!=-3 )
        {
            while ( temp!=NULL )
            {
                for ( k=0;k<GNum_Job;k++ )
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
                temp1=temp;
                temp=temp->next;
            }
            Cmax_temp[2]=temp1->fine;
        }
        else
        {
            Cmax_temp[2]=0;
        }
    }
    int cambio = 0;
    for ( k=1;k<GNum_Macchine;k++ )
    {
        if ( Cmax_temp[cambio] < Cmax_temp[k] )
        {
            cambio = k;
        }
    }
    FCmax =Cmax_temp[cambio];
    //_____________________________________________________________________ fine calcolo Cmax
    if ( Fswap_Lat_Tard==0 )
        FLmax = lat; //latenza massima
    else
        FLmax=Max ( lat,0 );//in questo caso la Lmax indica la Tardiness massima

    FTardy = tard;
    Ffeasible = feasible;
    //____________________________________________________ salvo le informazioni in prossimo

    prossimo->Lmax=FLmax;
    prossimo->Cmax=FCmax;
    prossimo->Tardy=FTardy;
    if ( feasible == 1 )
        prossimo->feasible = 1;
    else
        prossimo->feasible = 0;

    delete[] Cmax_temp;
}

//*************************************************************************************************
//			ROLLOUT Main function
//***********************************************************************************************
TJob *QRolloutThread::Rollout ( int force )
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
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    int fine = 0;
    TSchedula *M1_sch_attuale;
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    TSchedula *M3_sch_attuale;
    TJob * perm_di_passaggio;
    TNext_Elem *prossimo2;
    TNext_Elem *prossimo = NULL;
    TNext_Elem *prossimo1;
    TNext_Elem *temp;
    TNext_Elem *temp_prox;
    int ID_job ;
    int macchina;
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

    // FINE DICHIARAZIONE DELLE VARIABILI
    //___________________________________________________________________________________________________________

    permutazioni    = new TJob*[Fnum_Heur];
    Fpermutazione_Buffer = new TJob[GNum_Job];

    M1_sch_attuale  = GMacch1_Sched;
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    // in array job attuale metto una copia di array job
    array_job_attuale = new TJob[GNum_Job];
    for ( i = 0;i<GNum_Job;i++ )
    {
        array_job_attuale[i] = GArray_Job[i];
    }

    job_fisso = new TNext_Elem;

    while ( cont_livelli>0 )
    {
        TNext_Elem lista_prossimi_vincitori[500]; // questa la dovrei spostare
        //____________________________________________________________________!!!!!!!!!!

        index=0;
        index1=0;
        /*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/
        for ( iter_for = 0; iter_for < GNum_Job; iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
                        devo considerare il caso di tutti i job scedulati per primi  */
            if ( array_job_attuale[iter_for].ID!=-1 ) //se e' selezionabile
            {
                job_fisso->ID_job = array_job_attuale[iter_for].ID; //job base da cui faccio partire le euristiche
                array_job_attuale[iter_for].ID =-1;// non + selezionabile
                array_job_attuale_temp = new TJob[cont_livelli];
                iter=0;
                for ( kk=0;kk<cont_livelli;kk++ )
                {
                    while ( array_job_attuale[iter].ID==-1 )
                        iter++;

                    array_job_attuale_temp[kk]=array_job_attuale[iter];
                    iter++;
                }

                iter=0;
                prossimo = NULL;
                for ( i = 0;i<Fnum_Heur;i++ ) //num_heur dovra' essere cambiato con il numero di heuristiche effettivamente usato
                {
                    Azzera_Schedule();// azzero le schedule
                    //___________________________________________________________________________________
                    // calcolo i riempimenti parziali delle schedule
                    perm_di_passaggio = new TJob[GNum_Job];

                    Inizializza_Permutazione_Migliore ( perm_di_passaggio );

                    for ( pp=0;pp<GNum_Job-cont_livelli-1;pp++ )
                        perm_di_passaggio[pp] = GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];
                    prossimo2 = new TNext_Elem;
                    prossimo2->next=NULL;
                    Costruisci_E_Valuta_Schedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo2,perm_di_passaggio,GNum_Job-cont_livelli );
                    delete prossimo2;
                    //___________________________________________________________________________________

                    permutazioni[i] = NULL;
                    permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                    if ( prossimo == NULL )
                    {
                        prossimo         = new TNext_Elem;
                        prossimo->ID_job = permutazioni[i][0].ID;
                        prossimo->ID_heur= Ffunzioni[i].ID_heur;
                        prossimo->next   = NULL;
                    }
                    else
                    {
                        // 						next_elem *temp;
                        // 						next_elem *temp_prox;
                        temp = prossimo;
                        while ( temp->next!=NULL )
                        {
                            temp = temp->next;
                        }
                        temp_prox    = new TNext_Elem;
                        temp->next   = temp_prox;
                        temp = temp->next;
                        temp->ID_job = permutazioni[i][0].ID;
                        temp->ID_heur= Ffunzioni[i].ID_heur;
                        temp->next   = NULL;

                    }
                    // calcolo il valore delle schedule con la permutazione data dall'euristica i-esima
                    for ( jj = 0; pp < GNum_Job; pp++,jj++ ) // completo la permutazione di passaggio
                    {
                        perm_di_passaggio[pp] = permutazioni[i][jj];
                    }
                    Azzera_Schedule();
                    Costruisci_E_Valuta_Schedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,perm_di_passaggio,GNum_Job );

                    QString TestoASchermo = QString(" %i) %i %i %i %i num iter %i\n").arg(i).arg(FLmax).arg(FCmax).arg(FTardy).arg(Ffeasible).arg(GNum_Job-cont_livelli);
                    emit ScriviASchermo(  TestoASchermo );
                    Salva_Se_Meglio ( perm_di_passaggio );
                    delete[] perm_di_passaggio;
                    //devo riportare la macchina nelle condizioni orginarie

                    if ( force == 1 )
                    {
                        prossimo1       = new TNext_Elem;
                        M1_sch_buffer   = new TSchedula;
                        M2_sch_buffer   = new TSchedula;
                        M3_sch_buffer   = new TSchedula;

                        Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                        if ( GNum_Macchine >= 2 )
                            Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );
                        if ( GNum_Macchine == 3 )
                            Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                        VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                        Bilanciamento_Schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                        Valuta_Schedula (M1_sch_buffer,
                                         M2_sch_buffer,
                                         M3_sch_buffer,
                                         prossimo1 );

                        Elimina_Schedula ( M1_sch_buffer );
                        if ( GNum_Macchine>=2 )
                            Elimina_Schedula ( M2_sch_buffer );
                        if ( GNum_Macchine==3 )
                            Elimina_Schedula ( M3_sch_buffer );

                        TNext_Elem *tmp_prox;
                        tmp_prox=prossimo;
                        while ( tmp_prox->next!=NULL )
                        {
                            tmp_prox = tmp_prox->next;
                        }//trovo l'ultimo elemento.
                        if
                                (
                                 ( prossimo1->feasible>tmp_prox->feasible )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax<tmp_prox->Lmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax==tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax<tmp_prox->Cmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax==tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax==tmp_prox->Cmax )
                                     &&
                                     ( prossimo1->Tardy<tmp_prox->Cmax )

                                     )
                                 )
                        {
                            tmp_prox->Lmax      = prossimo1->Lmax;
                            tmp_prox->Cmax      = prossimo1->Cmax;
                            tmp_prox->Tardy     = prossimo1->Tardy;
                            tmp_prox->feasible  = prossimo1->feasible;
                        }

                        delete prossimo1;
                    }
                }

                /* se la schedula non e' feasible deve essere penalizzata rispetto alle altre.  *
                 *devo ridurre il numero di job che rimangono da schedulare                     *
                 *devo trovare il job con la Lateness + alta                                    *
                 *in condizioni di parita' quello con la Cmax +bassa                            *
                 *infine con il numero + basso di Tardy job                                     */

                array_job_attuale[iter_for].ID =iter_for;
                delete[] array_job_attuale_temp;

                ID_job      = prossimo->ID_job;
                macchina    = prossimo->macchina;
                fine        = prossimo->fine;
                inizio      = prossimo->inizio;
                tipo        = prossimo->tipo;
                index_camp  = prossimo->index_camp;
                LMAX        = prossimo->Lmax_pers;
                TARDY       = prossimo->Tardy_pers;
                L_max       = prossimo->Lmax;
                C_max       = prossimo->Cmax;
                tardy       = prossimo->Tardy;
                ID_heur     = prossimo->ID_heur;
                rel_time    = prossimo->rel_time;
                proc_time   = prossimo->proc_time;
                duedate     = prossimo->duedate;
                deadline    = prossimo->deadline;
                priority    = prossimo->priority;
                Ffeasible   = prossimo->feasible;

                temp = prossimo ;

                while ( temp->next!=NULL )
                {
                    if ( Ffeasible <= temp->next->feasible )
                    {
                        if
                                (
                                 ( Ffeasible ==0 )
                                 &&
                                 ( temp->next->feasible==1 )
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
                            ID_job      = temp->next->ID_job;
                            macchina    = temp->next->macchina;
                            tipo        = temp->next->tipo;
                            fine        = temp->next->fine;
                            inizio      = temp->next->inizio;
                            index_camp  = temp->next->index_camp;
                            LMAX        = temp->next->Lmax_pers;
                            TARDY       = temp->next->Tardy_pers;
                            L_max       = temp->next->Lmax;
                            C_max       = temp->next->Cmax;
                            tardy       = temp->next->Tardy;
                            deadline    = temp->next->deadline;
                            duedate     = temp->next->duedate;
                            proc_time   = temp->next->proc_time;
                            rel_time    = temp->next->rel_time;
                            priority    = temp->next->priority;
                            ID_heur     = temp->next->ID_heur;
                            Ffeasible   = temp->next->feasible;
                        }
                    }
                    temp=temp->next;
                }
                //ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                // e su quale macchina e' stato schedulato

                emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").arg(cont_livelli).arg(ID_heur).arg( L_max).arg(C_max).arg(tardy ) );

                // salvataggio nella lista delle info sul job prescelto in questo step
                lista_prossimi_vincitori[index].ID_job      = ID_job;
                lista_prossimi_vincitori[index].macchina    = macchina;
                lista_prossimi_vincitori[index].tipo        = tipo;
                lista_prossimi_vincitori[index].fine        = fine;
                lista_prossimi_vincitori[index].inizio      = inizio;
                lista_prossimi_vincitori[index].index_camp  = index_camp;
                lista_prossimi_vincitori[index].Lmax_pers   = LMAX;
                lista_prossimi_vincitori[index].Tardy_pers  = TARDY;
                lista_prossimi_vincitori[index].Lmax        = L_max;
                lista_prossimi_vincitori[index].Cmax        = C_max;
                lista_prossimi_vincitori[index].Tardy       = tardy;
                lista_prossimi_vincitori[index].deadline    = deadline;
                lista_prossimi_vincitori[index].duedate     = duedate;
                lista_prossimi_vincitori[index].proc_time   = proc_time;
                lista_prossimi_vincitori[index].rel_time    = rel_time;
                lista_prossimi_vincitori[index].priority    = priority;
                lista_prossimi_vincitori[index].ID_heur     = ID_heur;
                lista_prossimi_vincitori[index].feasible    = Ffeasible;
                index++;
                // fine salvataggio ___________________________________________________________


                TNext_Elem *temp_pr;
                for ( i=0;i<Fnum_Heur;i++ )
                {
                    temp_pr = prossimo->next;
                    delete prossimo;
                    prossimo = temp_pr;
                }


                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                for ( i=0;i<Fnum_Heur;i++ )
                    delete[] permutazioni[i];

            }

        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );

        //_____________________________________________________________________________________________________________________
        Pos_vincitore=Seleziona_Prossimo_Job ( lista_prossimi_vincitori,cont_livelli );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta = Trova_Posizione_Assoluta ( array_job_attuale, Pos_vincitore );

        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible > Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax < Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax < Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax == Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy < Ftardy_Best )
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = array_job_attuale[Pos_assoluta].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = array_job_attuale[Pos_assoluta].rel_time;
            array_job_attuale[Pos_assoluta].ID  = -1;
        }
        else
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].rel_time;
            array_job_attuale[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;
       }
        cont_livelli--;
    }

    i = 0;
    while ( array_job_attuale[i].ID==-1 )
        i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale[i].rel_time;
    array_job_attuale[i].ID=-1;

    delete job_fisso;
    delete[] array_job_attuale;

    Azzera_Schedule();

    prossimo1 = new TNext_Elem;
    prossimo1->next=NULL;
    Costruisci_E_Valuta_Schedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo1,GBest_Perm,GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible<Ffeasible_Best )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax>Lmax_best )
                 )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax==Lmax_best )
                 &&
                 ( FCmax>Cmax_best )
                 )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax==Lmax_best )
                 &&
                 ( FCmax==Cmax_best )
                 &&
                 ( FTardy>Ftardy_Best )
                 )
             )
    {
        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return	GBest_Perm;
    }
}
void QRolloutThread::Stampa_Risultati_A_Video ( int pFlag )
{
    TSchedula *temp;
    TElem *temp1;
    temp1 = GMacch1;
    temp = GMacch1_Sched;
    int i;
    if ( pFlag !=2 )
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
        temp=GMacch1_Sched;
        emit ScriviASchermo(  "\n\tPos\tJob\tTipo\tStart\tEnd\tCamp\tLat.\tTardy" );
        i=0;
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
        if ( GNum_Macchine==3 )
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

    if ( pFlag ==0 )
    {
        emit ScriviASchermo(  QString("\tTempo impiegato Rollout : %1\n").arg( ( double ) ( FTempo_Fine1-FTempo_Inizio1 )) );
    }
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

void QRolloutThread::Stampa_Risultati_Su_File (FILE *pFile_out,
                                               char *pInstance_file,
                                               int pFlag,
                                               int pForce )
{

    if ( pFlag == 0 )
    {

        fprintf ( pFile_out,"%s\t",pInstance_file );
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
    {
        fprintf ( pFile_out,"%i\t\t\t|\n",Ffeasible );
    }
    if ( pFlag == 2 )
    {
        fprintf ( pFile_out,"\n" );
    }
}
int QRolloutThread::Min ( int a, int b )
{
        if ( a<b )
        {
                return a;
        }
        else
        {
                return b;
        }
}

int QRolloutThread::Max ( int a, int b )
{
        if ( a<b )
        {
                return b;
        }
        else
        {
                return a;
        }
}

//***************************************************************************************************************************
// lo scopo di questa funzione ï¿½ï¿½?quello di eseguire uno spostamento di un job della macchina X da una posizione ad un'altra.
//
// **************************************************************************************************************************
TSchedula *QRolloutThread::Mossa ( TSchedula *M_sch, TElem *M, int pos_iniziale, int pos_finale )
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
    // creo ora una nuova schedula come copia di una parte di quella passatami e su di essa effettuerï¿½ï¿½?lo scambio
    // ____________________________________________________________________________________________________________________
    TSchedula *schedula_di_lavoro;// ovviamente nn lavoro sulla schedula ma su una sua copia.
    TSchedula *temp1,*temp2;
    TJob *job_temp;

    job_temp = new TJob;

    if ( pos_finale<pos_iniziale )
    {
        if ( pos_finale>0 ) // copio nella schedula di lavoro gli elementi che restano inalterati
        {
            schedula_di_lavoro = new TSchedula;
            temp1               = M_sch;

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
            for ( i=1;i<pos_finale;i++ )
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
        temp1 = M_sch;
        for ( i = 0; i < pos_iniziale; i++ )
            temp1 = temp1->next;

        // temp1 ora dovrebbe puntare al job da spostare
        int tipo = temp1->tipo;
        int tipo_predecessore ;
        if ( temp2==NULL )
        {tipo_predecessore =0;}
        else
        {tipo_predecessore = temp2->tipo;}
        int *setup_vett;
        int st_vt = 0;
        setup_vett =&st_vt;
        // faccio cosï¿½ï¿½?perchï¿½ï¿½?la funzione aggiungi_schedula richiede un puntatore ad intero
        int *disponibilita;
        int disp = 0;
        disponibilita =&disp;
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
        Verifica_Macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
        Aggiungi_Schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
        // 	a questo punto ho spostato il job nella posizione finale devo rischedulare gli altri
        // 	mi segno l'ID del job che ho spostato cosï¿½ï¿½?da poterlo saltare se lo re-incontro
        int ID_vietato = temp1->ID_job;
        tipo_predecessore = tipo;// il predecessore ï¿½ï¿½?ora il job appena schedulato
        temp1 = M_sch;
        for ( i=0;i<pos_finale;i++ )
        {
            temp1 = temp1->next;
        }//dovrei ora puntare alla posizione successiva a quella del job appena schedulato
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
                Verifica_Macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
                Aggiungi_Schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
                tipo_predecessore = tipo;// il predecessore ï¿½ï¿½?ora il job appena schedulato
            }
            temp1 = temp1->next;
        }
    }
    else
    {
        if ( pos_iniziale>0 ) // copio nella schedula di lavoro gli elementi che restano inalterati
        {
            schedula_di_lavoro = new TSchedula;
            temp1 = M_sch;
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
            for ( i=1;i<pos_iniziale;i++ )
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
        temp1 = M_sch;
        for ( i=0;i<pos_iniziale-1;i++ )
        {
            temp1 = temp1->next;
        }//punto ora all'elemento precedente a quello da spostare
        //salvo il tipo
        int tipo ;
        int tipo_predecessore;
        int *setup_vett;// faccio cosï¿½ï¿½?perchï¿½ï¿½?la funzione aggiungi_schedula richiede un puntatore ad intero
        int st_vt =0;
        setup_vett=&st_vt;
        int *disponibilita;
        int disp = 0;
        disponibilita = &disp;

        if ( pos_iniziale == 0 )
            tipo_predecessore = 0;//nn ho predecessori
        else
            tipo_predecessore = temp1->tipo;


        if ( pos_iniziale!=0 )
        {
            temp1 = temp1->next;//punto ora al job da spostare
        }//altrimenti giï¿½ï¿½?sto puntando a quell'elemento
        int ID_job_da_spostare = temp1->ID_job;
        int tipo_job_da_spostare = temp1->tipo;
        temp1 = temp1->next;//punto al job successivo
        for ( i = pos_iniziale;i<pos_finale;i++ )
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
            Verifica_Macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
            Aggiungi_Schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
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
        Verifica_Macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
        Aggiungi_Schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
        tipo_predecessore = tipo_job_da_spostare;// il predecessore ï¿½ï¿½?ora il job appena schedulato
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
            Verifica_Macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
            Aggiungi_Schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
            tipo_predecessore = tipo;// il predecessore e' ora il job appena schedulato
            temp1 = temp1->next;
        }

    }
    delete job_temp;
    return schedula_di_lavoro;
}
//VNS euristica di esplorazione di vicinati multipli
// restituisce 1 se sono stati trovati miglioramenti 0 atrimenti.
int QRolloutThread::VNS ( TSchedula *M1_sch_buffer,TSchedula *M2_sch_buffer,TSchedula *M3_sch_buffer )
{
        int ris = 0;
        ris += VNS_Per_Macchina ( M1_sch_buffer,GMacch1 );
        if ( GNum_Macchine >= 2 )
        {
                ris += VNS_Per_Macchina ( M2_sch_buffer,GMacch2 );
        }
        if ( GNum_Macchine==3 )
        {
                ris += VNS_Per_Macchina ( M3_sch_buffer,GMacch3 );
        }
        return ris;
}
//VNS_per_macchina implementa lo schema VNS per la singola macchina considerata.
int QRolloutThread::VNS_Per_Macchina ( TSchedula *M_sch,TElem *M )
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
    for ( i=0;i<GNum_Job;i++ )
        job_vett[i] = 0;

    i=0;
    temp = M_sch;
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
    quaterna_migliore = Valuta_Singola_Schedula ( M_sch );
    for ( j=0;j<cont;j++ )
    {
        Lat_max = 0;
        pos = -1;
        for ( i=0;i<GNum_Job;i++ )
        {
            if ( job_vett[i]>0 )
            {
                Lat_max= job_vett[i];
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
                schedula_di_lavoro = Mossa ( M_sch,M,pos,pos-1 );
                quaterna_di_lavoro = Valuta_Singola_Schedula ( schedula_di_lavoro );
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
                        temp = M_sch;
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
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VNS_Per_Macchina ( M_sch,M );
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
                        temp = M_sch;
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
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VNS_Per_Macchina ( M_sch,M );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else
                    {
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_di_lavoro;
                    }

                }
                else
                {//devo distruggere la shedula di lavoro
                    Elimina_Schedula ( schedula_di_lavoro );
                    delete quaterna_di_lavoro;
                }
            }
        }

    }
    for ( i=0;i<GNum_Job;i++ )
    {
        job_vett[i]=0;
    }
    int k=0;
    cont = 0;
    temp = M_sch;
    while ( temp!= NULL )
    {
        for ( i=0;i<GNum_Job;i++ )
        {
            if ( temp->ID_job== GArray_Job[i].ID )
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

    for ( j=0;j<cont;j++ )
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
        if ( pos>=0 )
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
            temp = M_sch;
            for ( k=0;k<pos;k++ )
            {
                temp = temp->next;
            }
            int limite = 0;
            int pos_finale = -1;
            limite = ( temp->inizio-violazione_max );
            temp = M_sch;
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
                schedula_di_lavoro = Mossa ( M_sch,M,pos,pos_finale );
                quaterna_di_lavoro = Valuta_Singola_Schedula ( schedula_di_lavoro );
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
                        temp = M_sch;
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
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VNS_Per_Macchina ( M_sch,M );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else
                    {
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_di_lavoro;
                    }
                }
                else
                {//devo distruggere la shedula di lavoro
                    Elimina_Schedula ( schedula_di_lavoro );
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
    temp = M_sch;
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

    for ( j=0;j<cont;j++ )
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
        if ( pos>=0 )
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
            temp = M_sch;
            for ( k=0;k<pos;k++ )
            {
                temp = temp->next;
            }
            job_vett[pos]=0; //per nn considerarlo + volte
            if ( temp->next!=NULL ) //verifico che il successivo esista davvero
            {
                schedula_di_lavoro = Mossa ( M_sch,M,pos,pos+1 );

                quaterna_di_lavoro = Valuta_Singola_Schedula ( schedula_di_lavoro );
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
                        temp = M_sch;
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
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_migliore;
                        delete quaterna_di_lavoro;
                        delete[] job_vett;
                        VNS_Per_Macchina ( M_sch,M );
                        //applico ricorsivamente l'algoritmo
                        return 1;
                    }
                    else
                    {
                        Elimina_Schedula ( schedula_di_lavoro );
                        delete quaterna_di_lavoro;
                    }
                }
                else
                {//devo distruggere la shedula di lavoro
                    Elimina_Schedula ( schedula_di_lavoro );
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
int QRolloutThread::Bilanciamento_Schedule (TSchedula *pM1_Schedula,
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
                        perm[0]=GArray_Job[j];
                        break;
                    }
                }
                int *disponibilita;
                int disp = 0;
                disponibilita = &disp;
                Verifica_Macchina ( vett_sch[pos],vett_indisp[pos],disponibilita,setup_vett,0,perm,0 );
                if ( disponibilita[0] < vett_C_inizio[i] )
                {
                    Aggiungi_Schedula ( vett_sch[pos],perm[0],disponibilita[0],setup_vett[0] );
                    TQuaterna *ris = Valuta_Singola_Schedula ( vett_sch[pos] );
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
                            Valuta_Schedula ( pM1_Schedula,pM2_Schedula,pM3_Schedula,prossimo1 );//ricalcolo i valori delle variabili globali
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
    delete[] perm;
    return 1;
}
void QRolloutThread::Stampa_Percentuali_Utilizzo_A_Video ( void )
{
        int i;
        for ( i = 0;i<Fnum_Heur;i++ )
        {
                emit ScriviASchermo( QString( "\n %1 %2% :").arg( i).arg( ( ( float ) Ffunzioni[i].perc_utilizzo/GNum_Job ) *100) );
        }

}
int QRolloutThread::Seleziona_Prossimo_Job ( TNext_Elem *lista_prossimi_vincitori,int cont_livelli )
{

        int pos=0,best_pos = 0;
        int ID_job = lista_prossimi_vincitori[pos].ID_job;
        int macchina = lista_prossimi_vincitori[pos].macchina;
        int fine = lista_prossimi_vincitori[pos].fine;
        int inizio = lista_prossimi_vincitori[pos].inizio;
        int tipo = lista_prossimi_vincitori[pos].tipo;
        int index_camp = lista_prossimi_vincitori[pos].index_camp;
        int LMAX = lista_prossimi_vincitori[pos].Lmax_pers;
        int TARDY = lista_prossimi_vincitori[pos].Tardy_pers;
        int L_max = lista_prossimi_vincitori[pos].Lmax;
        int C_max = lista_prossimi_vincitori[pos].Cmax;
        int tardy = lista_prossimi_vincitori[pos].Tardy;
        int ID_heur = lista_prossimi_vincitori[pos].ID_heur;
        int rel_time = lista_prossimi_vincitori[pos].rel_time;
        int proc_time = lista_prossimi_vincitori[pos].proc_time;
        int duedate = lista_prossimi_vincitori[pos].duedate;
        int deadline = lista_prossimi_vincitori[pos].deadline;
        int priority = lista_prossimi_vincitori[pos].priority;
        best_pos = pos;
        Ffeasible = lista_prossimi_vincitori[pos].feasible;
        while ( pos< cont_livelli-1 )
        {
                if ( Ffeasible <= lista_prossimi_vincitori[pos+1].feasible )
                {
                        if
                        (
                            ( Ffeasible ==0 )
                            &&
                            ( lista_prossimi_vincitori[pos+1].feasible==1 )
                        )
                        {
                                ID_job = lista_prossimi_vincitori[pos+1].ID_job;
                                macchina = lista_prossimi_vincitori[pos+1].macchina;
                                tipo = lista_prossimi_vincitori[pos+1].tipo;
                                fine = lista_prossimi_vincitori[pos+1].fine;
                                inizio = lista_prossimi_vincitori[pos+1].inizio;
                                index_camp = lista_prossimi_vincitori[pos+1].index_camp;
                                LMAX = lista_prossimi_vincitori[pos+1].Lmax_pers;
                                TARDY = lista_prossimi_vincitori[pos+1].Tardy_pers;
                                L_max = lista_prossimi_vincitori[pos+1].Lmax;
                                C_max = lista_prossimi_vincitori[pos+1].Cmax;
                                tardy = lista_prossimi_vincitori[pos+1].Tardy;
                                deadline = lista_prossimi_vincitori[pos+1].deadline;
                                duedate = lista_prossimi_vincitori[pos+1].duedate;
                                proc_time = lista_prossimi_vincitori[pos+1].proc_time;
                                rel_time = lista_prossimi_vincitori[pos+1].rel_time;
                                priority = lista_prossimi_vincitori[pos+1].priority;
                                ID_heur = lista_prossimi_vincitori[pos+1].ID_heur;
                                Ffeasible = lista_prossimi_vincitori[pos+1].feasible;
                                best_pos = pos+1;
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
                                ID_job = lista_prossimi_vincitori[pos+1].ID_job;
                                macchina = lista_prossimi_vincitori[pos+1].macchina;
                                tipo = lista_prossimi_vincitori[pos+1].tipo;
                                fine = lista_prossimi_vincitori[pos+1].fine;
                                inizio = lista_prossimi_vincitori[pos+1].inizio;
                                index_camp = lista_prossimi_vincitori[pos+1].index_camp;
                                LMAX = lista_prossimi_vincitori[pos+1].Lmax_pers;
                                TARDY = lista_prossimi_vincitori[pos+1].Tardy_pers;
                                L_max = lista_prossimi_vincitori[pos+1].Lmax;
                                C_max = lista_prossimi_vincitori[pos+1].Cmax;
                                tardy = lista_prossimi_vincitori[pos+1].Tardy;
                                deadline = lista_prossimi_vincitori[pos+1].deadline;
                                duedate = lista_prossimi_vincitori[pos+1].duedate;
                                proc_time = lista_prossimi_vincitori[pos+1].proc_time;
                                rel_time = lista_prossimi_vincitori[pos+1].rel_time;
                                priority = lista_prossimi_vincitori[pos+1].priority;
                                ID_heur = lista_prossimi_vincitori[pos+1].ID_heur;
                                Ffeasible = lista_prossimi_vincitori[pos+1].feasible;
                                best_pos = pos+1;
                        }
                }
                pos++;
        }
        return best_pos;
}
void QRolloutThread::Stampa_Permutazioni ( TJob *permutazione,int dim )
{
        int i;
        for ( i=0;i<dim;i++ )
        {
                emit ScriviASchermo(  QString(" %1 ").arg(permutazione[i].ID) );
        }
}
int QRolloutThread::Trova_Posizione_Assoluta ( TJob *array,int pos_rel )
{
        int pos_assoluta=0;
        int i=0,j=0;
        while ( i<=pos_rel )
        {
                if ( array[j].ID!=-1 )
                {
                        i++;
                }
                j++;
        }
        pos_assoluta = j-1;
        return pos_assoluta;
}
void QRolloutThread::Verifica_Cambiamento_Macchina ( int *primo_passo_M1,int *primo_passo_M2,int *primo_passo_M3 )
{
        TSchedula *temp;
        if ( primo_passo_M1[0]==0 )
        {
                temp = GMacch1_Sched;
                if ( temp->ID_job!=-3 )
                {
                        primo_passo_M1[0]=1;
                }
        }
        if ( primo_passo_M2[0]==0 )
        {
                if ( GNum_Macchine>=2 )
                {
                        temp = GMacch2_Sched;
                        if ( temp->ID_job!=-3 )
                        {
                                primo_passo_M2[0]=1;
                        }
                }
        }
        if ( primo_passo_M3[0]==0 )
        {
                if ( GNum_Macchine>=3 )
                {
                        temp = GMacch3_Sched;
                        if ( temp->ID_job!=-3 )
                        {
                                primo_passo_M3[0]=1;
                        }
                }
        }
        if ( primo_passo_M1[0]==1 )
        {
                temp = GMacch1_Sched;
                if ( temp->ID_job==-3 )
                {
                        primo_passo_M1[0]=0;
                }
        }
        if ( primo_passo_M2[0]==1 )
        {
                if ( temp->ID_job==-3 )
                {
                        temp = GMacch2_Sched;
                        if ( temp->next==NULL )
                        {
                                primo_passo_M2[0]=0;
                        }
                }
        }
        if ( primo_passo_M3[0]==1 )
        {
                if ( GNum_Macchine>=3 )
                {
                        temp = GMacch3_Sched;
                        if ( temp->ID_job==-3 )
                        {
                                primo_passo_M3[0]=0;
                        }
                }
        }
}
/***********************************************************************************************/

/***********************************************************************************************/

void QRolloutThread::Salva_Se_Meglio ( TJob* permutazioni)
{

        int i=0;
        if
        (
            ( Ffeasible>Ffeasible_Best )
            ||
            (
                ( Ffeasible==Ffeasible_Best )
                &&
                ( FLmax<Lmax_best )
            )
            ||
            (
                ( Ffeasible==Ffeasible_Best )
                &&
                ( FLmax==Lmax_best )
                &&
                ( FCmax<Cmax_best )
            )
            ||
            (
                ( Ffeasible==Ffeasible_Best )
                &&
                ( FLmax==Lmax_best )
                &&
                ( FCmax==Cmax_best )
                &&
                ( FTardy<Ftardy_Best )
            )
        )
        {
                for ( i=0;i<GNum_Job;i++ )
                {
                        Fpermutazione_Buffer[i]=permutazioni[i];
                }
                Ffeasible_Best=Ffeasible;
                Lmax_best=FLmax;
                Cmax_best=FCmax;
                Ftardy_Best=FTardy;
        }


}
void QRolloutThread::Stampa_Sequenza_Macchina ( TSchedula *M )
{
        TSchedula *temp;
        emit ScriviASchermo(  "\n" );
        temp=M;
        while ( temp->next!=NULL )
        {
                emit ScriviASchermo( QString("%1 ").arg(temp->ID_job) );
                temp=temp->next;
        }
        emit ScriviASchermo(  "\n" );
}
void QRolloutThread::Verifica_Se_Elementi_Uguali ( TJob *perm_di_passaggio,int num_job )
{
        int j=0,i=0;
        for ( i=0;i<num_job;i++ )
        {
                for ( j=i+1;j<num_job;j++ )
                {
                        if ( perm_di_passaggio[i].ID==perm_di_passaggio[j].ID )
                        {
                                emit ScriviASchermo(  QString("\n ERRORE JOB UGUALI %1 pos %2 e pos %3 num %4 \n\a").
                                                    arg(perm_di_passaggio[i].ID).
                                                    arg(i+1).
                                                    arg(j+1).
                                                    arg(perm_di_passaggio[j].ID) );
                                exit ( 0 );
                        }
                }
        }
}
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
    TSchedula *M1_sch_attuale;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    TSchedula *temp1;
    TSchedula *temp2;
    int fine = 0;

    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2

    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
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
            permutazioni[i] = NULL;
            permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale,GNum_Job-iter );

            prossimo1 = new TNext_Elem;
            if ( prossimo == NULL )
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
                while ( temp->next!=NULL )
                    temp = temp->next;

                temp_prox       = new TNext_Elem;
                temp->next      = temp_prox;
                temp            = temp->next;
                temp->ID_job    = permutazioni[i][0].ID;
                temp->ID_heur   = Ffunzioni[i].ID_heur;
                temp->next      = NULL;

            }
            Costruisci_E_Valuta_Schedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,permutazioni[i],GNum_Job-iter );

            //devo riportare la macchina nelle condizioni orginarie
            if ( force == 1 )
            {
                M1_sch_buffer = new TSchedula;
                M2_sch_buffer = new TSchedula;
                M3_sch_buffer = new TSchedula;
                Copia_Schedule ( GMacch1_Sched, M1_sch_buffer );

                if ( GNum_Macchine >= 2 )
                    Copia_Schedule ( GMacch2_Sched, M2_sch_buffer );

                if ( GNum_Macchine == 3 )
                    Copia_Schedule ( GMacch3_Sched, M3_sch_buffer );

                VNS (M1_sch_buffer,
                     M2_sch_buffer,
                     M3_sch_buffer );

                Bilanciamento_Schedule (M1_sch_buffer,
                                        M2_sch_buffer,
                                        M3_sch_buffer );//bilancio

                Valuta_Schedula (M1_sch_buffer,
                                 M2_sch_buffer,
                                 M3_sch_buffer,
                                 prossimo1 );

                Elimina_Schedula ( M1_sch_buffer );
                if ( GNum_Macchine >= 2 )
                    Elimina_Schedula ( M2_sch_buffer );

                if ( GNum_Macchine == 3 )
                    Elimina_Schedula ( M3_sch_buffer );

                TNext_Elem *tmp_prox;
                tmp_prox = prossimo;
                while ( tmp_prox->next!=NULL )
                    tmp_prox = tmp_prox->next;
                //trovo l'ultimo elemento.

                if
                        (
                         ( prossimo1->feasible>tmp_prox->feasible )
                         ||
                         (
                             ( prossimo1->feasible==tmp_prox->feasible )
                             &&
                             ( prossimo1->Lmax<tmp_prox->Lmax )
                             )
                         ||
                         (
                             ( prossimo1->feasible==tmp_prox->feasible )
                             &&
                             ( prossimo1->Lmax==tmp_prox->Lmax )
                             &&
                             ( prossimo1->Cmax<tmp_prox->Cmax )
                             )
                         ||
                         (
                             ( prossimo1->feasible==tmp_prox->feasible )
                             &&
                             ( prossimo1->Lmax==tmp_prox->Lmax )
                             &&
                             ( prossimo1->Cmax==tmp_prox->Cmax )
                             &&
                             ( prossimo1->Tardy<tmp_prox->Cmax )

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
        while ( temp->next!=NULL )
        {
            if ( Ffeasible <= temp->next->feasible )
            {
                if
                        (
                         ( Ffeasible ==0 )
                         &&
                         ( temp->next->feasible==1 )
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
            temp=temp->next;
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
                while ( temp1->next!=NULL )
                {
                    temp1 = temp1->next;
                }
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
//*************************************************************************************************
//       ROLLOUT Main function modified 18-5-2006
//***********************************************************************************************
TJob *QRolloutThread::Rollout_Modificato1 ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
    int pp=0,jj=0;
    int iter_for = 0;
    int index,index1 =0;
    int cont_livelli= GNum_Job-1;
    int Pos_vincitore = 0;
    int Pos_assoluta=0;

    TJob **permutazioni = new TJob*[Fnum_Heur];
    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_attuale;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job=1;
    int indice_job=0;
    int scelto;
    int *punt_job_scelti;
    Fpermutazione_Buffer = new TJob[GNum_Job];
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0; i < GNum_Job; i++ )
        array_job_attuale[i] = GArray_Job[i];

    TJob * perm_di_passaggio;
    num_job_daschedulare = GNum_Job;
    while ( cont_livelli>0 )
    {
        punt_job_scelti=NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        index=0;
        index1=0;
        if ( num_next_job==1 ) //ho un ultimo job da schedulare e poi ricomincio con la totalitï¿½ï¿½?di quelli mancanti
        {
            num_next_job=cont_livelli+1;//adesso considero i job trascurati
            indice_job=1;
        }
        else
        {
            num_next_job = ceil ( ( float ) num_next_job/2 );
            indice_job=0;
        }
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo pero' devo valutare tutte le sotto schedule

        if ( indice_job == 0 ) //questo vuol dire che devo scegliere solo i job migliori
        {
            indice_job=0;
            TNext_Elem *prossimo2;
            prossimo2 = NULL;
            for ( scelto=0;scelto<GNum_Job;scelto++ )
            {
                if ( array_job_attuale[scelto].ID!=-1 )
                {
                    Azzera_Schedule();
                    perm_di_passaggio = new TJob[GNum_Job-cont_livelli ];
                    for ( pp=0;pp<GNum_Job-cont_livelli-1;pp++ )
                        perm_di_passaggio[pp] = GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ]=GArray_Job[scelto];// aggiungo in coda il job prescelto
                    if ( prossimo2==NULL )
                    {
                        prossimo2= new TNext_Elem;
                        prossimo2->next=NULL;
                        prossimo2->ID_job = GArray_Job[scelto].ID;
                    }
                    else
                    {
                        TNext_Elem *temp;
                        TNext_Elem *temp_prox_prova;
                        temp = prossimo2;
                        while ( temp->next!=NULL )
                        {
                            temp = temp->next;
                        }
                        temp_prox_prova=new TNext_Elem;
                        temp->next = temp_prox_prova;
                        temp = temp->next;
                        temp->next = NULL;
                        temp->ID_job = GArray_Job[scelto].ID;
                    }
                    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                  GMacch2_Sched,
                                                  GMacch3_Sched,
                                                  prossimo2,
                                                  perm_di_passaggio,
                                                  GNum_Job - cont_livelli );
                    delete[] perm_di_passaggio;
                }
            }
            // devo ora individuare quei job che determinano valori migliori della schedula.
            // tropvo il primo job non segnato
            punt_job_scelti = new int[num_next_job];
            for ( scelto=0;scelto<num_next_job;scelto++ )
            {
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
                TNext_Elem *temp = prossimo2 ;
                while ( temp->ID_job==-1 )
                    temp=temp->next;

                ID_job = temp->ID_job;
                macchina = temp->macchina;
                fine = temp->fine;
                inizio = temp->inizio;
                tipo = temp->tipo;
                index_camp = temp->index_camp;
                LMAX = temp->Lmax_pers;
                TARDY = temp->Tardy_pers;
                L_max = prossimo2->Lmax;
                C_max = temp->Cmax;
                tardy = temp->Tardy;
                ID_heur = temp->ID_heur;
                rel_time = temp->rel_time;
                proc_time = temp->proc_time;
                duedate = temp->duedate;
                deadline = temp->deadline;
                priority = temp->priority;
                Feasible = temp->feasible;
                while ( temp->next!=NULL )
                {
                    if (
                            ( Feasible <= temp->next->feasible )
                            &&
                            ( temp->next->ID_job!=-1 )
                            )
                    {
                        if
                                (
                                 ( Feasible ==0 )
                                 &&
                                 ( temp->next->feasible==1 )
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
                            Feasible = temp->next->feasible;
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
                            Feasible = temp->next->feasible;
                        }
                    }
                    temp=temp->next;
                }
                //devo ora segnare il job prescelto in prossimo2
                TNext_Elem *temporaneo = prossimo2 ;
                while ( temporaneo->ID_job != ID_job )
                    temporaneo=temporaneo->next;

                temporaneo->ID_job = -1;
                punt_job_scelti[scelto] = ID_job;
            }

            //_______________________________________________________________________________
            TNext_Elem *temp_pr;
            temp_pr = prossimo2;
            while ( temp_pr->next!=NULL )
            {
                temp_pr = temp_pr->next;
                delete prossimo2;
                prossimo2 = temp_pr;
            }
            //_______________________________________________________________________________


        }
        for ( iter_for = 0; iter_for < GNum_Job; iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */

            if ( array_job_attuale[iter_for].ID == -1 ) //se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                int bou;
                if ( indice_job == 1 )
                    num_job_daschedulare = 1;
                else
                    num_job_daschedulare = num_next_job;

                for ( bou = 0; bou < num_job_daschedulare; bou++ )
                {
                    if (
                            ( indice_job==1 )
                            ||
                            ( array_job_attuale[iter_for].ID==punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                            )
                    {
                        array_job_attuale[iter_for].ID =-1;
                        array_job_attuale_temp = new TJob[cont_livelli];
                        iter=0;
                        for ( kk=0;kk<cont_livelli;kk++ )
                        {
                            while ( array_job_attuale[iter].ID==-1 )
                                iter++;

                            array_job_attuale_temp[kk] = array_job_attuale[iter];
                            iter++;
                        }
                        iter = 0;
                        TNext_Elem *prossimo = NULL;
                        TNext_Elem *prossimo1;
                        for ( i = 0;i < Fnum_Heur; i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                        {
                            // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                            Azzera_Schedule();
                            perm_di_passaggio = new TJob[GNum_Job];
                            Inizializza_Permutazione_Migliore ( perm_di_passaggio );
                            for ( pp = 0; pp < GNum_Job-cont_livelli-1; pp++ )
                                perm_di_passaggio[pp]=GBest_Perm[pp];

                            pp++;
                            perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];// aggiungo in coda il job prescelto
                            TNext_Elem *prossimo2;
                            prossimo2 = new TNext_Elem;
                            prossimo2->next = NULL;
                            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                          GMacch2_Sched,
                                                          GMacch3_Sched,
                                                          prossimo2,
                                                          perm_di_passaggio,
                                                          GNum_Job );
                            delete prossimo2;
                            // _____________________________________________________________________________________________________________________________
                            Azzera_Schedule();
                            permutazioni[i] = NULL;//calcolo tramite una euristica la parte mancante della permutazione
                            permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                            if ( prossimo == NULL )
                            {
                                prossimo            = new TNext_Elem;
                                prossimo->ID_job    = permutazioni[i][0].ID;
                                prossimo->ID_heur   = Ffunzioni[i].ID_heur;
                                prossimo->next      = NULL;
                            }
                            else
                            {
                                TNext_Elem *temp;
                                TNext_Elem *temp_prox;
                                temp = prossimo;
                                while ( temp->next!=NULL )
                                    temp = temp->next;

                                temp_prox=new TNext_Elem;
                                temp->next = temp_prox;
                                temp = temp->next;
                                temp->ID_job = permutazioni[i][0].ID;
                                temp->ID_heur= Ffunzioni[i].ID_heur;
                                temp->next = NULL;
                            }
                            for ( jj = 0; pp < GNum_Job; pp++, jj++ )
                                perm_di_passaggio[pp] = permutazioni[i][jj];

                            //aggiungo i job proposti dall'euristica
                            Azzera_Schedule();
                            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                          GMacch2_Sched,
                                                          GMacch3_Sched,
                                                          prossimo,
                                                          perm_di_passaggio,
                                                          GNum_Job );
                            emit ScriviASchermo( QString( " %1) %2 %3 %4 %5 num iter %6\n").
                                                arg(i).
                                                arg(FLmax).
                                                arg(FCmax).
                                                arg(FTardy).
                                                arg(Ffeasible).
                                                arg(GNum_Job-cont_livelli) );

                            Salva_Se_Meglio ( perm_di_passaggio );
                            delete[] perm_di_passaggio;

                            //_____________________________________________________________________________________________________________________
                            //devo riportare la macchina nelle condizioni orginarie
                            if ( force == 1 )
                            {
                                prossimo1 = new TNext_Elem;
                                M1_sch_buffer = new TSchedula;
                                M2_sch_buffer = new TSchedula;
                                M3_sch_buffer = new TSchedula;

                                Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                                if ( GNum_Macchine >= 2 )
                                    Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );

                                if ( GNum_Macchine == 3 )
                                    Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                                VNS (M1_sch_buffer,
                                     M2_sch_buffer,
                                     M3_sch_buffer );

                                Bilanciamento_Schedule (M1_sch_buffer,
                                                        M2_sch_buffer,
                                                        M3_sch_buffer );//bilancio

                                Valuta_Schedula (M1_sch_buffer,
                                                 M2_sch_buffer,
                                                 M3_sch_buffer,
                                                 prossimo1 );

                                Elimina_Schedula ( M1_sch_buffer );
                                if ( GNum_Macchine >= 2 ) {Elimina_Schedula ( M2_sch_buffer );}
                                if ( GNum_Macchine == 3 ) {Elimina_Schedula ( M3_sch_buffer );}

                                TNext_Elem *tmp_prox;
                                tmp_prox = prossimo;
                                while ( tmp_prox->next!=NULL )
                                    tmp_prox = tmp_prox->next;

                                //trovo l'ultimo elemento.
                                if
                                        (
                                         ( prossimo1->feasible>tmp_prox->feasible )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax<tmp_prox->Lmax )
                                             )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax==tmp_prox->Lmax )
                                             &&
                                             ( prossimo1->Cmax<tmp_prox->Cmax )
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
                                    tmp_prox->Lmax      = prossimo1->Lmax;
                                    tmp_prox->Cmax      = prossimo1->Cmax;
                                    tmp_prox->Tardy     = prossimo1->Tardy;
                                    tmp_prox->feasible  = prossimo1->feasible;
                                }
                                delete prossimo1;
                            }
                        }
                        // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                        // devo ridurre il numero di job che rimangono da schedulare
                        // devo trovare il job con la Lateness + alta
                        // in condizioni di parita' quello con la Cmax +bassa
                        // infine con il numero + basso di Tardy job
                        array_job_attuale[iter_for].ID = iter_for;
                        delete[] array_job_attuale_temp;
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
                        while ( temp->next!=NULL )
                        {
                            if ( Ffeasible <= temp->next->feasible )
                            {
                                if
                                        (
                                         ( Ffeasible ==0 )
                                         &&
                                         ( temp->next->feasible==1 )
                                         )
                                {
                                    ID_job      = temp->next->ID_job;
                                    macchina    = temp->next->macchina;
                                    tipo        = temp->next->tipo;
                                    fine        = temp->next->fine;
                                    inizio      = temp->next->inizio;
                                    index_camp  = temp->next->index_camp;
                                    LMAX        = temp->next->Lmax_pers;
                                    TARDY       = temp->next->Tardy_pers;
                                    L_max       = temp->next->Lmax;
                                    C_max       = temp->next->Cmax;
                                    tardy       = temp->next->Tardy;
                                    deadline    = temp->next->deadline;
                                    duedate     = temp->next->duedate;
                                    proc_time   = temp->next->proc_time;
                                    rel_time    = temp->next->rel_time;
                                    priority    = temp->next->priority;
                                    ID_heur     = temp->next->ID_heur;
                                    Ffeasible   = temp->next->feasible;
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
                                    ID_job      = temp->next->ID_job;
                                    macchina    = temp->next->macchina;
                                    tipo        = temp->next->tipo;
                                    fine        = temp->next->fine;
                                    inizio      = temp->next->inizio;
                                    index_camp  = temp->next->index_camp;
                                    LMAX        = temp->next->Lmax_pers;
                                    TARDY       = temp->next->Tardy_pers;
                                    L_max       = temp->next->Lmax;
                                    C_max       = temp->next->Cmax;
                                    tardy       = temp->next->Tardy;
                                    deadline    = temp->next->deadline;
                                    duedate     = temp->next->duedate;
                                    proc_time   = temp->next->proc_time;
                                    rel_time    = temp->next->rel_time;
                                    priority    = temp->next->priority;
                                    ID_heur     = temp->next->ID_heur;
                                    Ffeasible   = temp->next->feasible;
                                }
                            }
                            temp=temp->next;
                        }
                        // ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                        // e su quale macchina e' stato schedulato

                        emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                            arg(cont_livelli).
                                            arg(ID_heur).
                                            arg(L_max).
                                            arg( C_max).
                                            arg(tardy) );
                        // salvataggio nella lista delle info sul job prescelto in questo step
                        lista_prossimi_vincitori[index].ID_job      = ID_job;
                        lista_prossimi_vincitori[index].macchina    = macchina;
                        lista_prossimi_vincitori[index].tipo        =  tipo;
                        lista_prossimi_vincitori[index].fine        = fine;
                        lista_prossimi_vincitori[index].inizio      = inizio;
                        lista_prossimi_vincitori[index].index_camp  = index_camp;
                        lista_prossimi_vincitori[index].Lmax_pers   = LMAX;
                        lista_prossimi_vincitori[index].Tardy_pers  = TARDY;
                        lista_prossimi_vincitori[index].Lmax        = L_max;
                        lista_prossimi_vincitori[index].Cmax        = C_max;
                        lista_prossimi_vincitori[index].Tardy       = tardy;
                        lista_prossimi_vincitori[index].deadline    = deadline;
                        lista_prossimi_vincitori[index].duedate     = duedate;
                        lista_prossimi_vincitori[index].proc_time   = proc_time;
                        lista_prossimi_vincitori[index].rel_time    = rel_time;
                        lista_prossimi_vincitori[index].priority    = priority;
                        lista_prossimi_vincitori[index].ID_heur     = ID_heur;
                        lista_prossimi_vincitori[index].feasible    = Ffeasible;
                        index++;
                        // fine salvataggio ___________________________________________________________


                        // elimino el elemento prossimo
                        TNext_Elem *temp_pr;
                        for ( i = 0; i < Fnum_Heur; i++ )
                        {
                            temp_pr = prossimo->next;
                            delete prossimo;
                            prossimo = temp_pr;
                        }


                        /*elimino i vari candidati di questo step e procedo allo step successivo*/
                        for ( i = 0; i < Fnum_Heur; i++ )
                            delete permutazioni[i];

                    }
                }
            }
        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );
        //_____________________________________________________________________________________________________________________
        Pos_vincitore=Seleziona_Prossimo_Job ( lista_prossimi_vincitori,num_next_job );//da fare
        // Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta=Trova_Posizione_Assoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible > Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax < Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax < Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax == Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy < Ftardy_Best )
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = array_job_attuale[Pos_assoluta].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = array_job_attuale[Pos_assoluta].rel_time;
            array_job_attuale[Pos_assoluta].ID = -1;
        }
        else
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].rel_time;
            array_job_attuale[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID = -1;
        }

        cont_livelli--;
        if ( punt_job_scelti!=NULL )
            delete punt_job_scelti;

    }


    i = 0;
    while ( array_job_attuale[i].ID==-1 )
        i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale[i].rel_time;
    array_job_attuale[i].ID = -1;

    delete[] array_job_attuale;
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1 = new TNext_Elem;
    prossimo1->next = NULL;

    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible < Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax > Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax > Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy > Ftardy_Best )
                 )
             )
    {

        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return GBest_Perm;
    }
}

//*************************************************************************************************
//       ROLLOUT Main function modified 19-5-2006
//***********************************************************************************************
TJob *QRolloutThread::Rollout_Modificato2 ( int force)
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
    int pp = 0,jj = 0;
    int iter_for = 0;
    int index,index1 = 0;
    int cont_livelli= GNum_Job - 1;
    int Pos_vincitore = 0;
    int Pos_assoluta = 0;
    TJob **permutazioni = new TJob*[Fnum_Heur];
    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_attuale;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job = 1;
    int indice_job = 0;
    int scelto;
    int *punt_job_scelti;
    int *lista_migliori_passo_precedente;
    int *lista_purificata;
    int *dimensione_lista_purificata = new int;
    Fpermutazione_Buffer = new TJob[GNum_Job];

    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0;i<GNum_Job;i++ )
        array_job_attuale[i] = GArray_Job[i];

    TJob * perm_di_passaggio;
    num_job_daschedulare = GNum_Job;
    while ( cont_livelli > 0 )
    {
        punt_job_scelti = NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        TNext_Elem lista_prossimi_vincitori_swap[500];
        index  = 0;
        index1 = 0;
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo pero' devo valutare tutte le sotto schedule
        if ( num_next_job == 1 ) //ho un ultimo job da schedulare e poi ricomincio con la totalita' di quelli mancanti
        {
            num_next_job = cont_livelli+1;//adesso considero i job trascurati
            indice_job = 1;
        }
        else
        {
            num_next_job = ceil ( ( float ) num_next_job/4 );
            indice_job=0;
        }

        if ( indice_job == 1 ) //questo equivale a dire che non ho ancora schedulato niente
        {
        }
        else //questo vuol dire che devo scegliere solo i job migliori
        {
            indice_job = 0;
            TNext_Elem *prossimo2;
            prossimo2 = NULL;
            for ( scelto = 0; scelto < GNum_Job; scelto++ )
            {
                if ( array_job_attuale[scelto].ID != -1 )
                {
                    Azzera_Schedule();
                    perm_di_passaggio = new TJob[GNum_Job-cont_livelli ];
                    for ( pp = 0; pp < GNum_Job - cont_livelli - 1; pp++ )
                        perm_di_passaggio[pp] = GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[scelto];// aggiungo in coda il job prescelto
                    if ( prossimo2 == NULL )
                    {
                        prossimo2 = new TNext_Elem;
                        prossimo2->next = NULL;
                        prossimo2->ID_job = GArray_Job[scelto].ID;
                    }
                    else
                    {
                        TNext_Elem *temp;
                        TNext_Elem *temp_prox_prova;
                        temp = prossimo2;
                        while ( temp->next!=NULL )
                            temp = temp->next;

                        temp_prox_prova = new TNext_Elem;
                        temp->next = temp_prox_prova;
                        temp = temp->next;
                        temp->next = NULL;
                        temp->ID_job = GArray_Job[scelto].ID;
                    }
                    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                  GMacch2_Sched,
                                                  GMacch3_Sched,
                                                  prossimo2,
                                                  perm_di_passaggio,
                                                  GNum_Job - cont_livelli );
                    delete[] perm_di_passaggio;
                }
            }
            // devo ora individuare quei job che determinano valori migliori della schedula.
            // tropvo il primo job non segnato
            punt_job_scelti = new int[2*num_next_job];
            for ( scelto = 0; scelto < dimensione_lista_purificata[0]; scelto++ )
                punt_job_scelti[scelto] = lista_purificata[scelto];

            for ( scelto = 0;scelto < 2*num_next_job - dimensione_lista_purificata[0]; scelto++ )
            {
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
                TNext_Elem *temp = prossimo2 ;
                while ( temp->ID_job==-1 )
                    temp=temp->next;

                ID_job = temp->ID_job;
                macchina = temp->macchina;
                fine = temp->fine;
                inizio = temp->inizio;
                tipo = temp->tipo;
                index_camp = temp->index_camp;
                LMAX = temp->Lmax_pers;
                TARDY = temp->Tardy_pers;
                L_max = prossimo2->Lmax;
                C_max = temp->Cmax;
                tardy = temp->Tardy;
                ID_heur = temp->ID_heur;
                rel_time = temp->rel_time;
                proc_time = temp->proc_time;
                duedate = temp->duedate;
                deadline = temp->deadline;
                priority = temp->priority;
                Feasible = temp->feasible;
                while ( temp->next!=NULL )
                {
                    if (
                            ( Feasible <= temp->next->feasible )
                            &&
                            ( temp->next->ID_job!=-1 )
                            )
                    {
                        if
                                (
                                 ( Feasible ==0 )
                                 &&
                                 ( temp->next->feasible==1 )
                                 )
                        {
                            ID_job      = temp->next->ID_job;
                            macchina    = temp->next->macchina;
                            tipo        = temp->next->tipo;
                            fine        = temp->next->fine;
                            inizio      = temp->next->inizio;
                            index_camp  = temp->next->index_camp;
                            LMAX        = temp->next->Lmax_pers;
                            TARDY       = temp->next->Tardy_pers;
                            L_max       = temp->next->Lmax;
                            C_max       = temp->next->Cmax;
                            tardy       = temp->next->Tardy;
                            deadline    = temp->next->deadline;
                            duedate     = temp->next->duedate;
                            proc_time   = temp->next->proc_time;
                            rel_time    = temp->next->rel_time;
                            priority    = temp->next->priority;
                            ID_heur     = temp->next->ID_heur;
                            Feasible    = temp->next->feasible;
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
                            ID_job      = temp->next->ID_job;
                            macchina    = temp->next->macchina;
                            tipo        = temp->next->tipo;
                            fine        = temp->next->fine;
                            inizio      = temp->next->inizio;
                            index_camp  = temp->next->index_camp;
                            LMAX        = temp->next->Lmax_pers;
                            TARDY       = temp->next->Tardy_pers;
                            L_max       = temp->next->Lmax;
                            C_max       = temp->next->Cmax;
                            tardy       = temp->next->Tardy;
                            deadline    = temp->next->deadline;
                            duedate     = temp->next->duedate;
                            proc_time   = temp->next->proc_time;
                            rel_time    = temp->next->rel_time;
                            priority    = temp->next->priority;
                            ID_heur     = temp->next->ID_heur;
                            Feasible    = temp->next->feasible;
                        }
                    }
                    temp=temp->next;
                }
                //devo ora segnare il job prescelto in prossimo2
                TNext_Elem *temporaneo = prossimo2 ;
                while ( temporaneo->ID_job != ID_job )
                    temporaneo = temporaneo->next;

                temporaneo->ID_job = -1;
                punt_job_scelti[dimensione_lista_purificata[0]+scelto] = ID_job;
            }

            delete[] lista_migliori_passo_precedente;
            delete[] lista_purificata;

            //_______________________________________________________________________________
            TNext_Elem *temp_pr;
            temp_pr = prossimo2;
            while ( temp_pr->next!=NULL )
            {
                temp_pr = temp_pr->next;
                delete prossimo2;
                prossimo2 = temp_pr;
            }
            //_______________________________________________________________________________

        }
        for ( iter_for=0;iter_for<GNum_Job;iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if ( array_job_attuale[iter_for].ID == -1 ) //se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                int bou;
                if ( indice_job == 1 )
                    num_job_daschedulare = 1;
                else
                    num_job_daschedulare = 2*num_next_job;

                for ( bou = 0;bou < num_job_daschedulare; bou++ )
                {
                    if (
                            ( indice_job == 1 )
                            ||
                            ( array_job_attuale[iter_for].ID == punt_job_scelti[bou] ) //se e' selezionabile
                            )
                    {
                        array_job_attuale[iter_for].ID = -1;

                        array_job_attuale_temp = new TJob[cont_livelli];
                        iter = 0;
                        for ( kk = 0; kk < cont_livelli; kk++ )
                        {
                            while ( array_job_attuale[iter].ID==-1 )
                                iter++;

                            array_job_attuale_temp[kk] = array_job_attuale[iter];
                            iter++;
                        }
                        iter = 0;
                        TNext_Elem *prossimo = NULL;
                        TNext_Elem *prossimo1;
                        for ( i = 0; i < Fnum_Heur; i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                        {
                            // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                            Azzera_Schedule();
                            perm_di_passaggio = new TJob[GNum_Job];
                            Inizializza_Permutazione_Migliore ( perm_di_passaggio );
                            for ( pp = 0; pp < GNum_Job - cont_livelli - 1; pp++ )
                                perm_di_passaggio[pp] = GBest_Perm[pp];

                            pp++;
                            perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];// aggiungo in coda il job prescelto
                            TNext_Elem *prossimo2;
                            prossimo2 = new TNext_Elem;
                            prossimo2->next = NULL;
                            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                          GMacch2_Sched,
                                                          GMacch3_Sched,
                                                          prossimo2,
                                                          perm_di_passaggio,
                                                          GNum_Job );
                            delete prossimo2;
                            // _____________________________________________________________________________________________________________________________
                            Azzera_Schedule();
                            permutazioni[i] = NULL;//calcolo tramite una euristica la parte mancante della permutazione
                            permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                            if ( prossimo == NULL )
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
                                while ( temp->next!=NULL )
                                    temp = temp->next;

                                temp_prox       = new TNext_Elem;
                                temp->next      = temp_prox;
                                temp            = temp->next;
                                temp->ID_job    = permutazioni[i][0].ID;
                                temp->ID_heur   = Ffunzioni[i].ID_heur;
                                temp->next      = NULL;
                            }
                            for ( jj = 0; pp < GNum_Job; pp++,jj++ )
                                perm_di_passaggio[pp] = permutazioni[i][jj];
                             //aggiungo i job proposti dall'euristica

                            Azzera_Schedule();
                            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                          GMacch2_Sched,
                                                          GMacch3_Sched,
                                                          prossimo,
                                                          perm_di_passaggio,
                                                          GNum_Job );
                            emit ScriviASchermo( QString( " %1) %2 %3 %4 %5 num iter %6\n").
                                                arg(i).
                                                arg(FLmax).
                                                arg(FCmax).
                                                arg(FTardy).
                                                arg(Ffeasible).
                                                arg(GNum_Job-cont_livelli) );
                            Salva_Se_Meglio ( perm_di_passaggio);
                            delete[] perm_di_passaggio;

                            //_____________________________________________________________________________________________________________________
                            //devo riportare la macchina nelle condizioni orginarie
                            if ( force == 1 )
                            {
                                prossimo1       = new TNext_Elem;
                                M1_sch_buffer   = new TSchedula;
                                M2_sch_buffer   = new TSchedula;
                                M3_sch_buffer   = new TSchedula;

                                Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                                if ( GNum_Macchine >= 2 )
                                    Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );
                                if ( GNum_Macchine == 3 )
                                    Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                                VNS (M1_sch_buffer,
                                     M2_sch_buffer,
                                     M3_sch_buffer );

                                Bilanciamento_Schedule (M1_sch_buffer,
                                                        M2_sch_buffer,
                                                        M3_sch_buffer );//bilancio
                                Valuta_Schedula (M1_sch_buffer,
                                                 M2_sch_buffer,
                                                 M3_sch_buffer,
                                                 prossimo1 );

                                Elimina_Schedula ( M1_sch_buffer );
                                if ( GNum_Macchine >= 2 ) {Elimina_Schedula ( M2_sch_buffer );}
                                if ( GNum_Macchine == 3 ) {Elimina_Schedula ( M3_sch_buffer );}
                                TNext_Elem *tmp_prox;
                                tmp_prox = prossimo;
                                while ( tmp_prox->next != NULL )
                                    tmp_prox = tmp_prox->next;
                                //trovo l'ultimo elemento.

                                if
                                        (
                                         ( prossimo1->feasible>tmp_prox->feasible )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax<tmp_prox->Lmax )
                                             )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax==tmp_prox->Lmax )
                                             &&
                                             ( prossimo1->Cmax<tmp_prox->Cmax )
                                             )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax==tmp_prox->Lmax )
                                             &&
                                             ( prossimo1->Cmax==tmp_prox->Cmax )
                                             &&
                                             ( prossimo1->Tardy<tmp_prox->Cmax )
                                             )
                                         )
                                {
                                    tmp_prox->Lmax      = prossimo1->Lmax;
                                    tmp_prox->Cmax      = prossimo1->Cmax;
                                    tmp_prox->Tardy     = prossimo1->Tardy;
                                    tmp_prox->feasible  = prossimo1->feasible;
                                }
                                delete prossimo1;
                            }
                        }
                        // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                        // devo ridurre il numero di job che rimangono da schedulare
                        // devo trovare il job con la Lateness + alta
                        // in condizioni di parita' quello con la Cmax +bassa
                        // infine con il numero + basso di Tardy job
                        array_job_attuale[iter_for].ID = iter_for;
                        delete[] array_job_attuale_temp;
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
                        while ( temp->next!=NULL )
                        {
                            if ( Ffeasible <= temp->next->feasible )
                            {
                                if
                                        (
                                         ( Ffeasible ==0 )
                                         &&
                                         ( temp->next->feasible==1 )
                                         )
                                {
                                    ID_job      = temp->next->ID_job;
                                    macchina    = temp->next->macchina;
                                    tipo        = temp->next->tipo;
                                    fine        = temp->next->fine;
                                    inizio      = temp->next->inizio;
                                    index_camp  = temp->next->index_camp;
                                    LMAX        = temp->next->Lmax_pers;
                                    TARDY       = temp->next->Tardy_pers;
                                    L_max       = temp->next->Lmax;
                                    C_max       = temp->next->Cmax;
                                    tardy       = temp->next->Tardy;
                                    deadline    = temp->next->deadline;
                                    duedate     = temp->next->duedate;
                                    proc_time   = temp->next->proc_time;
                                    rel_time    = temp->next->rel_time;
                                    priority    = temp->next->priority;
                                    ID_heur     = temp->next->ID_heur;
                                    Ffeasible   = temp->next->feasible;
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
                                    ID_job      = temp->next->ID_job;
                                    macchina    = temp->next->macchina;
                                    tipo        = temp->next->tipo;
                                    fine        = temp->next->fine;
                                    inizio      = temp->next->inizio;
                                    index_camp  = temp->next->index_camp;
                                    LMAX        = temp->next->Lmax_pers;
                                    TARDY       = temp->next->Tardy_pers;
                                    L_max       = temp->next->Lmax;
                                    C_max       = temp->next->Cmax;
                                    tardy       = temp->next->Tardy;
                                    deadline    = temp->next->deadline;
                                    duedate     = temp->next->duedate;
                                    proc_time   = temp->next->proc_time;
                                    rel_time    = temp->next->rel_time;
                                    priority    = temp->next->priority;
                                    ID_heur     = temp->next->ID_heur;
                                    Ffeasible   = temp->next->feasible;
                                }
                            }
                            temp = temp->next;
                        }//ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                        //       e su quale macchina e' stato schedulato
                        emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                            arg(cont_livelli).
                                            arg(ID_heur).
                                            arg( L_max).
                                            arg( C_max,tardy) );
                        // salvataggio nella lista delle info sul job prescelto in questo step
                        lista_prossimi_vincitori[index].ID_job      = ID_job;
                        lista_prossimi_vincitori[index].macchina    = macchina;
                        lista_prossimi_vincitori[index].tipo        = tipo;
                        lista_prossimi_vincitori[index].fine        = fine;
                        lista_prossimi_vincitori[index].inizio      = inizio;
                        lista_prossimi_vincitori[index].index_camp  = index_camp;
                        lista_prossimi_vincitori[index].Lmax_pers   = LMAX;
                        lista_prossimi_vincitori[index].Tardy_pers  = TARDY;
                        lista_prossimi_vincitori[index].Lmax        = L_max;
                        lista_prossimi_vincitori[index].Cmax        = C_max;
                        lista_prossimi_vincitori[index].Tardy       = tardy;
                        lista_prossimi_vincitori[index].deadline    = deadline;
                        lista_prossimi_vincitori[index].duedate     = duedate;
                        lista_prossimi_vincitori[index].proc_time   = proc_time;
                        lista_prossimi_vincitori[index].rel_time    = rel_time;
                        lista_prossimi_vincitori[index].priority    = priority;
                        lista_prossimi_vincitori[index].ID_heur     = ID_heur;
                        lista_prossimi_vincitori[index].feasible    = Ffeasible;
                        index++;
                        // fine salvataggio ___________________________________________________________

                        //qui salvo i job che si sono dimostrati i migliori.C_max

                        TNext_Elem *temp_pr;
                        for ( i = 0; i < Fnum_Heur; i++ )
                        {
                            temp_pr = prossimo->next;
                            delete prossimo;
                            prossimo = temp_pr;
                        }

                        /*elimino i vari candidati di questo step e procedo allo step successivo*/
                        for ( i = 0;i < Fnum_Heur; i++ )
                            delete permutazioni[i];

                    }
                }
            }
        }
        emit ScriviASchermo( QString( "\n %1____________________\n").arg(cont_livelli) );
        //_____________________________________________________________________________________________________________________
        //         questa funzione restituisce i migliori elementi della lista ad esclusione del migliore.
        for ( i = 0; i < 500; i++ )
            lista_prossimi_vincitori_swap[i] = lista_prossimi_vincitori[i];

        lista_migliori_passo_precedente = Trova_Migliori ( lista_prossimi_vincitori_swap,num_next_job );
        lista_purificata = Purifica_Lista_Job_Prescelti ( lista_migliori_passo_precedente,num_next_job,dimensione_lista_purificata );
        Pos_vincitore = Seleziona_Prossimo_Job ( lista_prossimi_vincitori,num_next_job );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta = Trova_Posizione_Assoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible>Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Ftardy_Best )
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = array_job_attuale[Pos_assoluta].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = array_job_attuale[Pos_assoluta].rel_time;
            array_job_attuale[Pos_assoluta].ID  = -1;
        }
        else
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].rel_time;
            array_job_attuale[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;
        }
        cont_livelli--;
        if ( punt_job_scelti != NULL )
            delete[] punt_job_scelti;

    }

    i = 0;
    while ( array_job_attuale[i].ID==-1 )
        i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale[i].rel_time;
    array_job_attuale[i].ID = -1;

    delete[] array_job_attuale;
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1 = new TNext_Elem;
    prossimo1->next = NULL;
    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible<Ffeasible_Best )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax>Lmax_best )
                 )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax==Lmax_best )
                 &&
                 ( FCmax>Cmax_best )
                 )
             ||
             (
                 ( Ffeasible==Ffeasible_Best )
                 &&
                 ( FLmax==Lmax_best )
                 &&
                 ( FCmax==Cmax_best )
                 &&
                 ( FTardy>Ftardy_Best )
                 )
             )
    {

        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return   GBest_Perm;
    }
}
int *QRolloutThread::Trova_Migliori ( TNext_Elem *lista_prossimi_vincitori,int num_next_job )
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
        ID_job = lista_prossimi_vincitori[i].ID_job;
        macchina = lista_prossimi_vincitori[i].macchina;
        fine = lista_prossimi_vincitori[i].fine;
        inizio = lista_prossimi_vincitori[i].inizio;
        tipo = lista_prossimi_vincitori[i].tipo;
        index_camp = lista_prossimi_vincitori[i].index_camp;
        LMAX = lista_prossimi_vincitori[i].Lmax_pers;
        TARDY = lista_prossimi_vincitori[i].Tardy_pers;
        L_max = lista_prossimi_vincitori[i].Lmax;
        C_max = lista_prossimi_vincitori[i].Cmax;
        tardy = lista_prossimi_vincitori[i].Tardy;
        ID_heur = lista_prossimi_vincitori[i].ID_heur;
        rel_time = lista_prossimi_vincitori[i].rel_time;
        proc_time = lista_prossimi_vincitori[i].proc_time;
        duedate = lista_prossimi_vincitori[i].duedate;
        deadline = lista_prossimi_vincitori[i].deadline;
        priority = lista_prossimi_vincitori[i].priority;
        Feasible = lista_prossimi_vincitori[i].feasible;
        pos=i;
        best_pos=pos;
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
                        ID_job = lista_prossimi_vincitori[pos+1].ID_job;
                        macchina = lista_prossimi_vincitori[pos+1].macchina;
                        tipo = lista_prossimi_vincitori[pos+1].tipo;
                        fine = lista_prossimi_vincitori[pos+1].fine;
                        inizio = lista_prossimi_vincitori[pos+1].inizio;
                        index_camp = lista_prossimi_vincitori[pos+1].index_camp;
                        LMAX = lista_prossimi_vincitori[pos+1].Lmax_pers;
                        TARDY = lista_prossimi_vincitori[pos+1].Tardy_pers;
                        L_max = lista_prossimi_vincitori[pos+1].Lmax;
                        C_max = lista_prossimi_vincitori[pos+1].Cmax;
                        tardy = lista_prossimi_vincitori[pos+1].Tardy;
                        deadline = lista_prossimi_vincitori[pos+1].deadline;
                        duedate = lista_prossimi_vincitori[pos+1].duedate;
                        proc_time = lista_prossimi_vincitori[pos+1].proc_time;
                        rel_time = lista_prossimi_vincitori[pos+1].rel_time;
                        priority = lista_prossimi_vincitori[pos+1].priority;
                        ID_heur = lista_prossimi_vincitori[pos+1].ID_heur;
                        Feasible = lista_prossimi_vincitori[pos+1].feasible;
                        best_pos = pos+1;
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
                        ID_job = lista_prossimi_vincitori[pos+1].ID_job;
                        macchina = lista_prossimi_vincitori[pos+1].macchina;
                        tipo = lista_prossimi_vincitori[pos+1].tipo;
                        fine = lista_prossimi_vincitori[pos+1].fine;
                        inizio = lista_prossimi_vincitori[pos+1].inizio;
                        index_camp = lista_prossimi_vincitori[pos+1].index_camp;
                        LMAX = lista_prossimi_vincitori[pos+1].Lmax_pers;
                        TARDY = lista_prossimi_vincitori[pos+1].Tardy_pers;
                        L_max = lista_prossimi_vincitori[pos+1].Lmax;
                        C_max = lista_prossimi_vincitori[pos+1].Cmax;
                        tardy = lista_prossimi_vincitori[pos+1].Tardy;
                        deadline = lista_prossimi_vincitori[pos+1].deadline;
                        duedate = lista_prossimi_vincitori[pos+1].duedate;
                        proc_time = lista_prossimi_vincitori[pos+1].proc_time;
                        rel_time = lista_prossimi_vincitori[pos+1].rel_time;
                        priority = lista_prossimi_vincitori[pos+1].priority;
                        ID_heur = lista_prossimi_vincitori[pos+1].ID_heur;
                        Feasible = lista_prossimi_vincitori[pos+1].feasible;
                        best_pos = pos+1;
                    }
                }
            }
            pos++;
        }
        lista_migliori_passo_precedente[j]=lista_prossimi_vincitori[best_pos].ID_job;
        lista_prossimi_vincitori[best_pos].ID_job=-1;
        j++;
    }
    return lista_migliori_passo_precedente;


}

TJob *QRolloutThread::Rollout_Modificato3 ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
    int pp = 0,jj = 0;
    int iter_for = 0;
    int index,index1 = 0;
    int cont_livelli= GNum_Job-1;
    int Pos_vincitore = 0;
    int Pos_assoluta = 0;

    TJob **permutazioni = new TJob*[Fnum_Heur];
    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TSchedula *M1_sch_attuale;
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job=1;
    int indice_job=0;
    int scelto;
    int *punt_job_scelti;
    int *lista_migliori_passo_precedente;
    int *dimensione_lista_purificata;
    int *lista_purificata;
    FILE *f_log;
    char nome_file[256];
    strcpy ( nome_file,FInstance_File );
    strcat ( nome_file,".roll_filtro3.txt" );
    f_log = fopen ( nome_file,"a+" );
    dimensione_lista_purificata = new int;
    Fpermutazione_Buffer = new TJob[GNum_Job];
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;
    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0; i < GNum_Job; i++ )
        array_job_attuale[i] = GArray_Job[i];

    TJob * perm_di_passaggio;
    num_job_daschedulare = GNum_Job;
    while ( cont_livelli > 0 )
    {
        punt_job_scelti = NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        TNext_Elem lista_prossimi_vincitori_swap[500];
        index = 0;
        index1= 0;
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo pero' devo valutare tutte le sotto schedule
        if ( num_next_job==1 ) //ho un ultimo job da schedulare e poi ricomincio con la totalita' di quelli mancanti
        {
            num_next_job=cont_livelli+1;//adesso considero i job trascurati
            indice_job=1;
        }
        else
        {
            num_next_job = ceil ( ( float ) num_next_job/2 );
            indice_job=0;
        }
        if ( indice_job == 1 ) //questo equivale a dire che non ho ancora schedulato niente
        {
        }
        else //questo vuol dire che devo scegliere solo i job migliori
        {
            indice_job = 0;
            punt_job_scelti = new int[dimensione_lista_purificata[0]];
            for ( scelto = 0; scelto < dimensione_lista_purificata[0]; scelto++ )
                punt_job_scelti[scelto]=lista_purificata[scelto];

            if ( dimensione_lista_purificata[0]==0 )
            {
                indice_job=1;
                num_next_job=cont_livelli+1;
            }
            else
                num_next_job=dimensione_lista_purificata[0];

            delete[] lista_migliori_passo_precedente;
            delete[] lista_purificata;
        }
        for ( iter_for = 0; iter_for < GNum_Job; iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if ( array_job_attuale[iter_for].ID == -1 ) //se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                int bou;
                if ( indice_job == 1 )
                    num_job_daschedulare=1;
                else
                    num_job_daschedulare=num_next_job;

                for ( bou=0;bou<num_job_daschedulare;bou++ )
                {
                    if (
                            ( indice_job==1 )
                            ||
                            ( array_job_attuale[iter_for].ID == punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                            )
                    {
                        array_job_attuale[iter_for].ID =-1;
                        array_job_attuale_temp = new TJob[cont_livelli];
                        iter = 0;
                        for ( kk=0;kk<cont_livelli;kk++ )
                        {
                            while ( array_job_attuale[iter].ID==-1 ) iter++;

                            array_job_attuale_temp[kk] = array_job_attuale[iter];
                            iter++;
                        }
                        iter=0;

                        TNext_Elem *prossimo = NULL;
                        TNext_Elem *prossimo1;
                        for ( i = 0;i<Fnum_Heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                        {
                            // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                            Azzera_Schedule();
                            perm_di_passaggio = new TJob[GNum_Job];
                            Inizializza_Permutazione_Migliore ( perm_di_passaggio );
                            for ( pp = 0;pp < GNum_Job - cont_livelli-1; pp++ )
                                perm_di_passaggio[pp] = GBest_Perm[pp];

                            pp++;
                            perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ]=GArray_Job[iter_for];// aggiungo in coda il job prescelto

                            // _____________________________________________________________________________________________________________________________
                            Azzera_Schedule();
                            permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                            permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                            if ( prossimo == NULL )
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
                                while ( temp->next!=NULL ) temp = temp->next;

                                temp_prox=new TNext_Elem;
                                temp->next = temp_prox;
                                temp = temp->next;
                                temp->ID_job = permutazioni[i][0].ID;
                                temp->ID_heur= Ffunzioni[i].ID_heur;
                                temp->next = NULL;
                            }
                            for ( jj=0;pp<GNum_Job;pp++,jj++ )
                            {
                                perm_di_passaggio[pp]=permutazioni[i][jj];
                            } //aggiungo i job proposti dall'euristica
                            Azzera_Schedule();
                            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                          GMacch2_Sched,
                                                          GMacch3_Sched,
                                                          prossimo,
                                                          perm_di_passaggio,
                                                          GNum_Job );
                            emit ScriviASchermo( QString( " %1) %2 %3 %4 %5 num iter %6\n").
                                                arg(i).
                                                arg(FLmax).
                                                arg(FCmax).
                                                arg(FTardy).
                                                arg(Ffeasible).
                                                arg(GNum_Job-cont_livelli) );
                            Salva_Se_Meglio ( perm_di_passaggio );
                            delete[] perm_di_passaggio;

                            //_____________________________________________________________________________________________________________________
                            //devo riportare la macchina nelle condizioni orginarie
                            if ( force == 1 )
                            {
                                prossimo1 = new TNext_Elem;
                                M1_sch_buffer = new TSchedula;
                                M2_sch_buffer = new TSchedula;
                                M3_sch_buffer = new TSchedula;

                                Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                                if ( GNum_Macchine >= 2 )
                                    Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );
                                if ( GNum_Macchine == 3 )
                                    Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                                VNS (M1_sch_buffer,
                                     M2_sch_buffer,
                                     M3_sch_buffer );
                                Bilanciamento_Schedule (M1_sch_buffer,
                                                        M2_sch_buffer,
                                                        M3_sch_buffer );//bilancio
                                Valuta_Schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                Elimina_Schedula ( M1_sch_buffer );
                                if ( GNum_Macchine>=2 ) {Elimina_Schedula ( M2_sch_buffer );}
                                if ( GNum_Macchine==3 ) {Elimina_Schedula ( M3_sch_buffer );}
                                TNext_Elem *tmp_prox;
                                tmp_prox = prossimo;
                                while ( tmp_prox->next!=NULL )
                                    tmp_prox = tmp_prox->next;
                                //trovo l'ultimo elemento.

                                if
                                        (
                                         ( prossimo1->feasible>tmp_prox->feasible )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax<tmp_prox->Lmax )
                                             )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax==tmp_prox->Lmax )
                                             &&
                                             ( prossimo1->Cmax<tmp_prox->Cmax )
                                             )
                                         ||
                                         (
                                             ( prossimo1->feasible==tmp_prox->feasible )
                                             &&
                                             ( prossimo1->Lmax==tmp_prox->Lmax )
                                             &&
                                             ( prossimo1->Cmax==tmp_prox->Cmax )
                                             &&
                                             ( prossimo1->Tardy<tmp_prox->Cmax )
                                             )
                                         )
                                {
                                    tmp_prox->Lmax=prossimo1->Lmax;
                                    tmp_prox->Cmax=prossimo1->Cmax;
                                    tmp_prox->Tardy=prossimo1->Tardy;
                                    tmp_prox->feasible=prossimo1->feasible;
                                }
                                delete prossimo1;
                            }
                        }
                        // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                        // devo ridurre il numero di job che rimangono da schedulare
                        // devo trovare il job con la Lateness + alta
                        // in condizioni di parita' quello con la Cmax +bassa
                        // infine con il numero + basso di Tardy job
                        array_job_attuale[iter_for].ID = iter_for;
                        delete[] array_job_attuale_temp;
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
                        while ( temp->next!=NULL )
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
                                    ID_job      = temp->next->ID_job;
                                    macchina    = temp->next->macchina;
                                    tipo        = temp->next->tipo;
                                    fine        = temp->next->fine;
                                    inizio      = temp->next->inizio;
                                    index_camp  = temp->next->index_camp;
                                    LMAX        = temp->next->Lmax_pers;
                                    TARDY       = temp->next->Tardy_pers;
                                    L_max       = temp->next->Lmax;
                                    C_max       = temp->next->Cmax;
                                    tardy       = temp->next->Tardy;
                                    deadline    = temp->next->deadline;
                                    duedate     = temp->next->duedate;
                                    proc_time   = temp->next->proc_time;
                                    rel_time    = temp->next->rel_time;
                                    priority    = temp->next->priority;
                                    ID_heur     = temp->next->ID_heur;
                                    Ffeasible   = temp->next->feasible;
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
                                    ID_job      = temp->next->ID_job;
                                    macchina    = temp->next->macchina;
                                    tipo        = temp->next->tipo;
                                    fine        = temp->next->fine;
                                    inizio      = temp->next->inizio;
                                    index_camp  = temp->next->index_camp;
                                    LMAX        = temp->next->Lmax_pers;
                                    TARDY       = temp->next->Tardy_pers;
                                    L_max       = temp->next->Lmax;
                                    C_max       = temp->next->Cmax;
                                    tardy       = temp->next->Tardy;
                                    deadline    = temp->next->deadline;
                                    duedate     = temp->next->duedate;
                                    proc_time   = temp->next->proc_time;
                                    rel_time    = temp->next->rel_time;
                                    priority    = temp->next->priority;
                                    ID_heur     = temp->next->ID_heur;
                                    Ffeasible   = temp->next->feasible;
                                }
                            }
                            temp = temp->next;
                        }//ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                        //       e su quale macchina  e' stato schedulato
                        emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                            arg( cont_livelli).
                                            arg( ID_heur).
                                            arg( L_max).
                                            arg( C_max).
                                            arg( tardy) );
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

                        //qui salvo i job che si sono dimostrati i migliori.C_max

                        TNext_Elem *temp_pr;
                        for ( i = 0; i < Fnum_Heur; i++ )
                        {
                            temp_pr = prossimo->next;
                            delete prossimo;
                            prossimo = temp_pr;
                        }


                        /*elimino i vari candidati di questo step e procedo allo step successivo*/
                        for ( i =0 ; i < Fnum_Heur; i++ )
                            delete[] permutazioni[i];

                    }
                }
            }
        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );
        //_____________________________________________________________________________________________________________________
        //         questa funzione restituisce i migliori elementi della lista ad esclusione del migliore.
        for ( i=0;i<500;i++ )
        {
            lista_prossimi_vincitori_swap[i]=lista_prossimi_vincitori[i];
        }
        lista_migliori_passo_precedente=Trova_Migliori ( lista_prossimi_vincitori_swap,num_next_job );
        lista_purificata=Purifica_Lista_Job_Prescelti ( lista_migliori_passo_precedente,num_next_job,dimensione_lista_purificata );
        Pos_vincitore=Seleziona_Prossimo_Job ( lista_prossimi_vincitori,num_next_job );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta = Trova_Posizione_Assoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible>Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Ftardy_Best )
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
            //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
            GBest_Perm[GNum_Job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
            array_job_attuale[Pos_assoluta].ID=-1;
            fprintf ( f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy );
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
            fprintf ( f_log,"%i %i %i %i \n",Ffeasible_Best,Lmax_best,Cmax_best,Ftardy_Best );
        }
        cont_livelli--;
        if ( punt_job_scelti != NULL )
        {
            delete[] punt_job_scelti;
        }
    }


    i = 0;
    while ( array_job_attuale[i].ID==-1 ) i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale[i].rel_time;
    array_job_attuale[i].ID = -1;
    delete[] array_job_attuale;
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1= new TNext_Elem;
    prossimo1->next=NULL;
    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible < Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax > Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax > Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy > Ftardy_Best )
                 )
             )
    {
        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return GBest_Perm;
    }
}
int *QRolloutThread:: Purifica_Lista_Job_Prescelti ( int *pLista_migliori_passo_precedente,int pDim_lista, int *pProssima_dimensione )
{
    int j,i;
    int *lista_purificata;
    int contatore = 0;
    int dim_lista_locale = ceil ( ( float ) pDim_lista/2 ) +1;
    for ( i = 0; i < dim_lista_locale - 1; i++ )
        for ( j = i + 1; j < dim_lista_locale; j++ )
        {
            if (
                    ( pLista_migliori_passo_precedente[i]!=-1 )
                    &&
                    ( pLista_migliori_passo_precedente[i]==pLista_migliori_passo_precedente[j] )
                    )
            {
                pLista_migliori_passo_precedente[j]=-1;
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

TJob *QRolloutThread::Rollout_Modificato4 ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
    int pp = 0,jj = 0;
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
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job=1;
    int indice_job=0;
    int scelto;
    int *punt_job_scelti;
    int *lista_migliori_passo_precedente;
    int *dimensione_lista_purificata;
    int *lista_migliori_passo_precedente_posizione_assoluta;
    dimensione_lista_purificata = new int;
    Fpermutazione_Buffer = new TJob[GNum_Job];
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0;i<GNum_Job;i++ )
        array_job_attuale[i] = GArray_Job[i];

    TJob * perm_di_passaggio;
    num_job_daschedulare=GNum_Job;
    while ( cont_livelli > 0 )
    {
        punt_job_scelti = NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        TNext_Elem lista_prossimi_vincitori_swap[500];
        index = 0;
        index1= 0;
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perï¿½ï¿½?devo valutare tutte le sotto schedule
        if ( num_next_job == 1 ) //ho un ultimo job da schedulare e poi ricomincio con la totalitï¿½ï¿½?di quelli mancanti
        {
            num_next_job = cont_livelli+1;//adesso considero i job trascurati
            indice_job = 1;
        }
        else
        {
            num_next_job = ceil ( ( float ) num_next_job/2 );
            indice_job = 0;
        }
        if ( indice_job == 1 ) //questo equivale a dire che non ho ancora schedulato niente
        {
        }
        else //questo vuol dire che devo scegliere solo i job migliori al passo precedente.
        {
            indice_job = 0;
            punt_job_scelti = new int[num_next_job];
            for ( scelto = 0; scelto < num_next_job; scelto++ )
                punt_job_scelti[scelto] = lista_migliori_passo_precedente_posizione_assoluta[scelto];

            delete[] lista_migliori_passo_precedente;
            delete[] lista_migliori_passo_precedente_posizione_assoluta;

        }
        for ( iter_for=0;iter_for<GNum_Job;iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if ( array_job_attuale[iter_for].ID==-1 ) //se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                int bou;
                if ( indice_job == 1 )
                    num_job_daschedulare = 1;
                else
                    num_job_daschedulare = num_next_job;

                for ( bou = 0;bou < num_job_daschedulare; bou++ )
                {
                    if (
                            ( indice_job==1 )
                            ||
                            ( array_job_attuale[iter_for].ID == punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                            )
                    {
                        array_job_attuale[iter_for].ID =-1;
                        array_job_attuale_temp = new TJob[cont_livelli];
                        iter = 0;
                        for ( kk = 0;kk < cont_livelli; kk++ )
                        {
                            while ( array_job_attuale[iter].ID==-1 ) iter++;

                            array_job_attuale_temp[kk]=array_job_attuale[iter];
                            iter++;
                        }
                        iter = 0;
                        TNext_Elem *prossimo = NULL;
                        TNext_Elem *prossimo1;
                        for ( i = 0; i < Fnum_Heur; i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                        {
                            // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                            Azzera_Schedule();
                            perm_di_passaggio = new TJob[GNum_Job];
                            Inizializza_Permutazione_Migliore ( perm_di_passaggio );
                            for ( pp = 0; pp < GNum_Job - cont_livelli - 1; pp++ )
                                perm_di_passaggio[pp]=GBest_Perm[pp];

                            pp++;
                            perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];// aggiungo in coda il job prescelto

                            // _____________________________________________________________________________________________________________________________
                            Azzera_Schedule();
                            permutazioni[i] = NULL;//calcolo tramite una euristica la parte mancante della permutazione
                            permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                            if ( prossimo == NULL )
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
                                while ( temp->next!=NULL )
                                    temp = temp->next;

                                temp_prox=new TNext_Elem;
                                temp->next = temp_prox;
                                temp = temp->next;
                                temp->ID_job = permutazioni[i][0].ID;
                                temp->ID_heur= Ffunzioni[i].ID_heur;
                                temp->next = NULL;
                            }
                            for ( jj=0;pp<GNum_Job;pp++,jj++ )
                                perm_di_passaggio[pp]=permutazioni[i][jj];

                            //aggiungo i job proposti dall'euristica
                            Azzera_Schedule();
                            Costruisci_E_Valuta_Schedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,perm_di_passaggio,GNum_Job );
                            emit ScriviASchermo(  QString(" %1) %2 %3 %4 %5 num iter %6\n").
                                                arg(i).
                                                arg(FLmax).
                                                arg(FCmax).
                                                arg(FTardy).
                                                arg(Ffeasible).
                                                arg(GNum_Job-cont_livelli) );
                            Salva_Se_Meglio ( perm_di_passaggio );
                            delete[] perm_di_passaggio;

                            //_____________________________________________________________________________________________________________________
                            //devo riportare la macchina nelle condizioni orginarie
                            if ( force == 1 )
                            {
                                prossimo1 = new TNext_Elem;
                                M1_sch_buffer = new TSchedula;
                                M2_sch_buffer = new TSchedula;
                                M3_sch_buffer = new TSchedula;

                                Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                                if ( GNum_Macchine >= 2 )
                                    Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );
                                if ( GNum_Macchine == 3 )
                                    Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                                VNS (M1_sch_buffer,
                                     M2_sch_buffer,
                                     M3_sch_buffer );
                                Bilanciamento_Schedule (M1_sch_buffer,
                                                        M2_sch_buffer,
                                                        M3_sch_buffer );//bilancio
                                Valuta_Schedula (M1_sch_buffer,
                                                 M2_sch_buffer,
                                                 M3_sch_buffer,
                                                 prossimo1 );

                                Elimina_Schedula ( M1_sch_buffer );
                                if ( GNum_Macchine>=2 ) {Elimina_Schedula ( M2_sch_buffer );}
                                if ( GNum_Macchine==3 ) {Elimina_Schedula ( M3_sch_buffer );}
                                TNext_Elem *tmp_prox;
                                tmp_prox = prossimo;
                                while ( tmp_prox->next!=NULL )
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
                                    tmp_prox->Lmax      = prossimo1->Lmax;
                                    tmp_prox->Cmax      = prossimo1->Cmax;
                                    tmp_prox->Tardy     = prossimo1->Tardy;
                                    tmp_prox->feasible  = prossimo1->feasible;
                                }
                                delete prossimo1;
                            }
                        }
                        // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                        // devo ridurre il numero di job che rimangono da schedulare
                        // devo trovare il job con la Lateness + alta
                        // in condizioni di parita' quello con la Cmax +bassa
                        // infine con il numero + basso di Tardy job
                        array_job_attuale[iter_for].ID =iter_for;
                        delete[] array_job_attuale_temp;
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
                        while ( temp->next!=NULL )
                        {
                            if ( Ffeasible <= temp->next->feasible )
                            {
                                if
                                        (
                                         ( Ffeasible ==0 )
                                         &&
                                         ( temp->next->feasible==1 )
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
                            temp=temp->next;
                        }
                        // ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                        // e su quale macchina e' stato schedulato
                        emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                            arg( cont_livelli).
                                            arg( ID_heur).
                                            arg( L_max).
                                            arg( C_max).
                                            arg( tardy ) );
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

                        //qui salvo i job che si sono dimostrati i migliori.C_max

                        TNext_Elem *temp_pr;
                        for ( i=0;i<Fnum_Heur;i++ )
                        {
                            temp_pr = prossimo->next;
                            delete prossimo;
                            prossimo = temp_pr;
                        }

                        /*elimino i vari candidati di questo step e procedo allo step successivo*/
                        for ( i = 0; i < Fnum_Heur; i++ )
                            delete[] permutazioni[i];

                    }
                }
            }
        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );
        //_____________________________________________________________________________________________________________________
        //         questa funzione restituisce i migliori elementi della lista ad esclusione del migliore.
        for ( i = 0; i < 500; i++ )
            lista_prossimi_vincitori_swap[i] = lista_prossimi_vincitori[i];

        lista_migliori_passo_precedente = Trova_Prossimi_Migliori ( lista_prossimi_vincitori_swap,num_next_job );

        lista_migliori_passo_precedente_posizione_assoluta = new int[(int)ceil(( float )num_next_job/2.0)];

        for ( i = 0; i < ceil ( ( float ) num_next_job/2 ); i++ )
            lista_migliori_passo_precedente_posizione_assoluta[i]=Trova_Posizione_Assoluta ( array_job_attuale,lista_migliori_passo_precedente[i+1] );

        emit ScriviASchermo(  "\n" );

        for ( i = 0; i < ceil ( ( float ) num_next_job/2 ); i++ )
            emit ScriviASchermo( QString( "%1 ").arg(lista_migliori_passo_precedente_posizione_assoluta[i]) );

        emit ScriviASchermo(  "\n" );
        Pos_vincitore = Seleziona_Prossimo_Job ( lista_prossimi_vincitori,num_next_job );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta = Trova_Posizione_Assoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible > Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax < Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax < Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible == Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax == Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax == Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy < Ftardy_Best )
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

        if ( punt_job_scelti != NULL )
            delete[] punt_job_scelti;

    }

    i = 0;
    while ( array_job_attuale[i].ID == -1 ) i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale[i].rel_time;
    array_job_attuale[i].ID = -1;
    delete[] array_job_attuale;
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1 = new TNext_Elem;
    prossimo1->next = NULL;
    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible < Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax > Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax > Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy > Ftardy_Best )
                 )
             )
    {
        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return   GBest_Perm;
    }
}
int *QRolloutThread::Trova_Prossimi_Migliori ( TNext_Elem *lista_prossimi_vincitori,int num_next_job )
{
        int num_next_job_locale;
        int *lista_migliori_passo_precedente;
        num_next_job_locale=ceil ( ( float ) num_next_job/2 );
        lista_migliori_passo_precedente=new int[num_next_job_locale+1];

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
        j=0;
        while ( j<num_next_job_locale+1 )
        {
                best_pos=0;
                i=0;
                while ( lista_prossimi_vincitori[i].ID_job==-1 )
                {
                        i++;
                }
                ID_job = lista_prossimi_vincitori[i].ID_job;
                macchina = lista_prossimi_vincitori[i].macchina;
                fine = lista_prossimi_vincitori[i].fine;
                inizio = lista_prossimi_vincitori[i].inizio;
                tipo = lista_prossimi_vincitori[i].tipo;
                index_camp = lista_prossimi_vincitori[i].index_camp;
                LMAX = lista_prossimi_vincitori[i].Lmax_pers;
                TARDY = lista_prossimi_vincitori[i].Tardy_pers;
                L_max = lista_prossimi_vincitori[i].Lmax;
                C_max = lista_prossimi_vincitori[i].Cmax;
                tardy = lista_prossimi_vincitori[i].Tardy;
                ID_heur = lista_prossimi_vincitori[i].ID_heur;
                rel_time = lista_prossimi_vincitori[i].rel_time;
                proc_time = lista_prossimi_vincitori[i].proc_time;
                duedate = lista_prossimi_vincitori[i].duedate;
                deadline = lista_prossimi_vincitori[i].deadline;
                priority = lista_prossimi_vincitori[i].priority;
                Feasible = lista_prossimi_vincitori[i].feasible;
                pos=i;
                best_pos=pos;
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
                                                ID_job = lista_prossimi_vincitori[pos+1].ID_job;
                                                macchina = lista_prossimi_vincitori[pos+1].macchina;
                                                tipo = lista_prossimi_vincitori[pos+1].tipo;
                                                fine = lista_prossimi_vincitori[pos+1].fine;
                                                inizio = lista_prossimi_vincitori[pos+1].inizio;
                                                index_camp = lista_prossimi_vincitori[pos+1].index_camp;
                                                LMAX = lista_prossimi_vincitori[pos+1].Lmax_pers;
                                                TARDY = lista_prossimi_vincitori[pos+1].Tardy_pers;
                                                L_max = lista_prossimi_vincitori[pos+1].Lmax;
                                                C_max = lista_prossimi_vincitori[pos+1].Cmax;
                                                tardy = lista_prossimi_vincitori[pos+1].Tardy;
                                                deadline = lista_prossimi_vincitori[pos+1].deadline;
                                                duedate = lista_prossimi_vincitori[pos+1].duedate;
                                                proc_time = lista_prossimi_vincitori[pos+1].proc_time;
                                                rel_time = lista_prossimi_vincitori[pos+1].rel_time;
                                                priority = lista_prossimi_vincitori[pos+1].priority;
                                                ID_heur = lista_prossimi_vincitori[pos+1].ID_heur;
                                                Feasible = lista_prossimi_vincitori[pos+1].feasible;
                                                best_pos = pos+1;
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
                                                ID_job = lista_prossimi_vincitori[pos+1].ID_job;
                                                macchina = lista_prossimi_vincitori[pos+1].macchina;
                                                tipo = lista_prossimi_vincitori[pos+1].tipo;
                                                fine = lista_prossimi_vincitori[pos+1].fine;
                                                inizio = lista_prossimi_vincitori[pos+1].inizio;
                                                index_camp = lista_prossimi_vincitori[pos+1].index_camp;
                                                LMAX = lista_prossimi_vincitori[pos+1].Lmax_pers;
                                                TARDY = lista_prossimi_vincitori[pos+1].Tardy_pers;
                                                L_max = lista_prossimi_vincitori[pos+1].Lmax;
                                                C_max = lista_prossimi_vincitori[pos+1].Cmax;
                                                tardy = lista_prossimi_vincitori[pos+1].Tardy;
                                                deadline = lista_prossimi_vincitori[pos+1].deadline;
                                                duedate = lista_prossimi_vincitori[pos+1].duedate;
                                                proc_time = lista_prossimi_vincitori[pos+1].proc_time;
                                                rel_time = lista_prossimi_vincitori[pos+1].rel_time;
                                                priority = lista_prossimi_vincitori[pos+1].priority;
                                                ID_heur = lista_prossimi_vincitori[pos+1].ID_heur;
                                                Feasible = lista_prossimi_vincitori[pos+1].feasible;
                                                best_pos = pos+1;
                                        }
                                }
                        }
                        pos++;
                }
                lista_migliori_passo_precedente[j]=best_pos;
                lista_prossimi_vincitori[best_pos].ID_job=-1;
                j++;
        }
        return lista_migliori_passo_precedente;


}
//*************************************************************************************************
//       ROLLOUT Main function modified 9-8-2006
// 	inserira' nella lista dei job papabili solo quelli che hanno release date inferiore al massimo
// 	tempo di completamento delle schedule attuali.
//***********************************************************************************************
TJob *QRolloutThread::Rollout_Modificato5 ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
    int pp=0,jj=0;
    int iter_for = 0;
    int index,index1 =0;
    int cont_livelli= GNum_Job-1;
    int Pos_vincitore = 0;
    int Pos_assoluta=0;
    TJob **permutazioni;
    permutazioni = new TJob*[Fnum_Heur];
    //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
    TJob *array_job_attuale;
    TJob *array_job_attuale_temp;
    TJob *array_job_attuale_swap;//questo array conterra'  una copia del array_job_attuale al passo precedente per poterlo ripristinare
    TSchedula *M1_sch_attuale;
    TSchedula **vett_schedule_parziali; //questo vettore conterrï¿½  i puntatori alle schedule parziali via via costruite dal rollout.
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job=1;
    int *punt_job_scelti;
    int *dimensione_lista_purificata;
    dimensione_lista_purificata = new int;
    Fpermutazione_Buffer = new TJob[GNum_Job];
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
    if ( GNum_Macchine >= 2 )
        M2_sch_attuale = GMacch2_Sched;

    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
        M3_sch_attuale = GMacch3_Sched;

    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0; i < GNum_Job;i++ )
        array_job_attuale[i] = GArray_Job[i];

    TJob * perm_di_passaggio;
    num_job_daschedulare = GNum_Job;
    vett_schedule_parziali = new TSchedula*[GNum_Macchine];
    array_job_attuale_swap = new TJob[GNum_Job];
    while ( cont_livelli>0 )
    {
        punt_job_scelti=NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        index = 0;
        index1 = 0;

        // Per prima cosa devo calcolare il max tempo di completamento delle schedule parziali.
        int tempo_massimo_di_completamento = 0; //in questo filtro non tengo conto delle indisponibilita'  delle macchine.
        int tempo_minimo_di_release = 10000000;//e' un numero arbitrariamente grande
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo pero' devo verificare che il tempo di rilascio sia inferiore al max dei tempi di completamento delle schedule parziali.

        // devo quindi salvare in una struttura tutti i job validi e in un'altra tutti quegli elementi che pur non essendo ancora stati selezionati non verificano il criterio del filtro.
        if ( cont_livelli == GNum_Job-1 ) //
        {
            vett_schedule_parziali[0] = GMacch1_Sched;
            if ( GNum_Macchine == 2 )
                vett_schedule_parziali[1] = GMacch2_Sched;
            if ( GNum_Macchine==3 )
            {
                vett_schedule_parziali[1] = GMacch2_Sched;
                vett_schedule_parziali[2] = GMacch3_Sched;
            }
            tempo_massimo_di_completamento = Max_Cmax_Schedula ( vett_schedule_parziali,GNum_Macchine );

            for ( i = 0; i < GNum_Job; i++ )
                array_job_attuale_swap[i] = array_job_attuale[i];

            for (  i = 0; i < GNum_Job; i++ )
                if ( array_job_attuale[i].ID !=-1 )
                    if ( array_job_attuale[i].rel_time < tempo_minimo_di_release )
                        tempo_minimo_di_release = array_job_attuale[i].rel_time; //calcolo il tempo minimo di release


            tempo_massimo_di_completamento=Max ( tempo_massimo_di_completamento,tempo_minimo_di_release );
            for ( i=0;i<GNum_Job;i++ )
            {
                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time > tempo_massimo_di_completamento )
                {
                    array_job_attuale[i].ID=-1;//non schedulabile
                }
            }

        }
        else
        {
            Azzera_Schedule();
            perm_di_passaggio = new TJob[GNum_Job-cont_livelli-1];

            for ( pp=0;pp<GNum_Job-cont_livelli-1;pp++ )
            {
                perm_di_passaggio[pp]=GBest_Perm[pp];
                emit ScriviASchermo( QString( " %1").arg(perm_di_passaggio[pp].ID) );

            }
            Stampa_Permutazioni ( Fpermutazione_Buffer,GNum_Job );
            pp++;
            TNext_Elem *prossimo2;
            prossimo2 = new TNext_Elem;
            prossimo2->next = NULL;
            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                          GMacch2_Sched,
                                          GMacch3_Sched,
                                          prossimo2,
                                          perm_di_passaggio,
                                          GNum_Job-cont_livelli-1 );
            delete prossimo2;
            delete[] perm_di_passaggio;
            vett_schedule_parziali[0] = GMacch1_Sched;
            if ( GNum_Macchine == 2 )
                vett_schedule_parziali[1]=GMacch2_Sched;
            if ( GNum_Macchine == 3 )
            {
                vett_schedule_parziali[1] = GMacch2_Sched;
                vett_schedule_parziali[2] = GMacch3_Sched;
            }
            tempo_massimo_di_completamento = Max_Cmax_Schedula ( vett_schedule_parziali,GNum_Macchine );
            for ( i = 0; i < GNum_Job; i++ )
                array_job_attuale[i] = array_job_attuale_swap[i];
            // ho modificato l'array_job_attuale_swap in modo che indichi come non schedulabile il job scelto al passo precedente.

            tempo_minimo_di_release = 10000000;//e' un numero arbitrariamente grande
            for ( i = 0; i < GNum_Job;i++ )
                if ( array_job_attuale[i].ID !=-1 )
                    if ( array_job_attuale[i].rel_time < tempo_minimo_di_release )
                        tempo_minimo_di_release = array_job_attuale[i].rel_time; //calcolo il tempo minimo di release


            tempo_massimo_di_completamento=Max ( tempo_massimo_di_completamento,tempo_minimo_di_release );
            for ( i=0;i<GNum_Job;i++ )
            {
                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_massimo_di_completamento )
                {
                    array_job_attuale[i].ID=-1;//non schedulabile
                }
            }
        }
        for ( iter_for=0;iter_for<GNum_Job;iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
                           devo considerare il caso di tutti i job scedulati per primi
                           devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
                           tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if ( array_job_attuale[iter_for].ID==-1 ) //se ï¿½ï¿½?non selezionabile
            {
            }
            else //se ï¿½ï¿½?selezionabile
            {
                array_job_attuale_swap[iter_for].ID =-1;

                array_job_attuale_temp=new TJob[cont_livelli];
                iter=0;
                for ( kk=0;kk<cont_livelli;kk++ )
                {
                    while ( array_job_attuale_swap[iter].ID==-1 )
                    {
                        iter++;
                    }
                    array_job_attuale_temp[kk]=array_job_attuale_swap[iter];
                    iter++;
                }
                iter=0;

                TNext_Elem *prossimo = NULL;
                TNext_Elem *prossimo1;
                for ( i = 0;i<Fnum_Heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                {
                    // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                    Azzera_Schedule();
                    perm_di_passaggio=new TJob[GNum_Job];
                    Inizializza_Permutazione_Migliore ( perm_di_passaggio );
                    for ( pp=0;pp<GNum_Job-cont_livelli-1;pp++ )
                    {
                        perm_di_passaggio[pp]=GBest_Perm[pp];
                    }
                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ]=GArray_Job[iter_for];// aggiungo in coda il job prescelto

                    // _____________________________________________________________________________________________________________________________
                    Azzera_Schedule();
                    permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                    permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                    if ( prossimo == NULL )
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
                        while ( temp->next!=NULL )
                        {
                            temp = temp->next;
                        }
                        temp_prox=new TNext_Elem;
                        temp->next = temp_prox;
                        temp = temp->next;
                        temp->ID_job = permutazioni[i][0].ID;
                        temp->ID_heur= Ffunzioni[i].ID_heur;
                        temp->next = NULL;
                    }
                    for ( jj=0;pp<GNum_Job;pp++,jj++ )
                    {
                        perm_di_passaggio[pp]=permutazioni[i][jj];
                    } //aggiungo i job proposti dall'euristica
                    Azzera_Schedule();
                    Costruisci_E_Valuta_Schedula ( GMacch1_Sched,GMacch2_Sched,GMacch3_Sched,prossimo,perm_di_passaggio,GNum_Job );
                    emit ScriviASchermo( QString( " %1) %2 %3 %4 %5 num iter %6\n").
                                        arg(i).
                                        arg(FLmax).
                                        arg(FCmax).
                                        arg(FTardy).
                                        arg(Ffeasible).
                                        arg(GNum_Job-cont_livelli) );
                    Salva_Se_Meglio ( perm_di_passaggio );//qui modifica permutazione_buffer
                    delete[] perm_di_passaggio;

                    //_____________________________________________________________________________________________________________________
                    //devo riportare la macchina nelle condizioni orginarie
                    if ( force == 1 )
                    {
                        prossimo1       = new TNext_Elem;
                        M1_sch_buffer   = new TSchedula;
                        M2_sch_buffer   = new TSchedula;
                        M3_sch_buffer   = new TSchedula;

                        Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                        if ( GNum_Macchine >= 2 )
                            Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );
                        if ( GNum_Macchine == 3 )
                            Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                        VNS (M1_sch_buffer,
                             M2_sch_buffer,
                             M3_sch_buffer );
                        Bilanciamento_Schedule (M1_sch_buffer,
                                                M2_sch_buffer,
                                                M3_sch_buffer );//bilancio
                        Valuta_Schedula (M1_sch_buffer,
                                         M2_sch_buffer,
                                         M3_sch_buffer,
                                         prossimo1 );

                        Elimina_Schedula ( M1_sch_buffer );
                        if ( GNum_Macchine >= 2 ) {Elimina_Schedula ( M2_sch_buffer );}
                        if ( GNum_Macchine == 3 ) {Elimina_Schedula ( M3_sch_buffer );}
                        TNext_Elem *tmp_prox;
                        tmp_prox = prossimo;
                        while ( tmp_prox->next!=NULL )
                            tmp_prox = tmp_prox->next;

                        //trovo l'ultimo elemento.
                        if
                                (
                                 ( prossimo1->feasible>tmp_prox->feasible )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax<tmp_prox->Lmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax==tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax<tmp_prox->Cmax )
                                     )
                                 ||
                                 (
                                     ( prossimo1->feasible==tmp_prox->feasible )
                                     &&
                                     ( prossimo1->Lmax==tmp_prox->Lmax )
                                     &&
                                     ( prossimo1->Cmax==tmp_prox->Cmax )
                                     &&
                                     ( prossimo1->Tardy<tmp_prox->Cmax )
                                     )
                                 )
                        {
                            tmp_prox->Lmax=prossimo1->Lmax;
                            tmp_prox->Cmax=prossimo1->Cmax;
                            tmp_prox->Tardy=prossimo1->Tardy;
                            tmp_prox->feasible=prossimo1->feasible;
                        }
                        delete prossimo1;
                    }
                }
                // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                // devo ridurre il numero di job che rimangono da schedulare
                // devo trovare il job con la Lateness + alta
                // in condizioni di parita' quello con la Cmax +bassa
                // infine con il numero + basso di Tardy job
                array_job_attuale_swap[iter_for].ID = iter_for;
                delete[] array_job_attuale_temp;
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
                while ( temp->next!=NULL )
                {
                    if ( Ffeasible <= temp->next->feasible )
                    {
                        if
                                (
                                 ( Ffeasible == 0 )
                                 &&
                                 ( temp->next->feasible==1 )
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
                    temp=temp->next;
                }
                //ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                // e su quale macchina e' stato schedulato
                emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                    arg(cont_livelli).
                                    arg( ID_heur).
                                    arg( L_max).
                                    arg( C_max).
                                    arg(tardy) );
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

                //qui salvo i job che si sono dimostrati i migliori.C_max

                TNext_Elem *temp_pr;
                for ( i = 0; i < Fnum_Heur; i++ )
                {
                    temp_pr = prossimo->next;
                    delete prossimo;
                    prossimo = temp_pr;
                }


                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                for ( i = 0; i < Fnum_Heur; i++ )
                    delete[] permutazioni[i];




            }
        }
        emit ScriviASchermo( QString("\n %1____________________\n").arg(cont_livelli) );

        Pos_vincitore = Seleziona_Prossimo_Job ( lista_prossimi_vincitori,num_next_job );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta = Trova_Posizione_Assoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible>Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Ftardy_Best )
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
            array_job_attuale_swap[Pos_assoluta].ID=-1;

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
            array_job_attuale_swap[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;

        }
        cont_livelli--;
        if ( punt_job_scelti!=NULL )
            delete[] punt_job_scelti;

    }

    i = 0;
    while ( array_job_attuale_swap[i].ID == -1 ) i++;

    GBest_Perm[GNum_Job-1].ID=array_job_attuale_swap[i].ID;
    GBest_Perm[GNum_Job-1].tipo=array_job_attuale_swap[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time=array_job_attuale_swap[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate=array_job_attuale_swap[i].duedate;
    GBest_Perm[GNum_Job-1].deadline=array_job_attuale_swap[i].deadline;
    GBest_Perm[GNum_Job-1].priority=array_job_attuale_swap[i].priority;
    GBest_Perm[GNum_Job-1].rel_time=array_job_attuale_swap[i].rel_time;
    array_job_attuale[i].ID = -1;
    delete[] array_job_attuale;
    delete[] array_job_attuale_swap;
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1= new TNext_Elem;
    prossimo1->next=NULL;
    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;
    if
            (
             ( Ffeasible < Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax > Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax > Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy > Ftardy_Best )
                 )
             )
    {
        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return GBest_Perm;
    }
}


//*************************************************************************************************
//       ROLLOUT Main function modified 9-8-2006
// 	inserira' nella lista dei job papabili solo quelli che hanno release date inferiore al minimo
// 	tempo di completamento delle schedule attuali.
//***********************************************************************************************
TJob *QRolloutThread::Rollout_Modificato6 ( int force )
{

    // M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    int i,iter,kk  = 0;
    int num_job_daschedulare;
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
    TJob *array_job_attuale_swap;//questo array conterrï¿½  una copia del array_job_attuale al passo precedente per poterlo ripristinare
    TSchedula *M1_sch_attuale;
    TSchedula **vett_schedule_parziali; //questo vettore conterrï¿½  i puntatori alle schedule parziali via via costruite dal rollout.
    TSchedula *M1_sch_buffer;
    TSchedula *M2_sch_buffer;
    TSchedula *M3_sch_buffer;
    M1_sch_attuale = GMacch1_Sched;
    int num_next_job=1;
    int *punt_job_scelti;
    int *dimensione_lista_purificata;
    dimensione_lista_purificata=new int;
    Fpermutazione_Buffer=new TJob[GNum_Job];
    TSchedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2

    if ( GNum_Macchine >= 2 )
    {
        M2_sch_attuale = GMacch2_Sched;
    }
    TSchedula *M3_sch_attuale;
    if ( GNum_Macchine == 3 )
    {
        M3_sch_attuale = GMacch3_Sched;
    }
    array_job_attuale = new TJob[GNum_Job];

    for ( i = 0;i<GNum_Job;i++ )
    {
        array_job_attuale[i] = GArray_Job[i];
    }
    //     job_fisso = (next_elem *)malloc(sizeof(next_elem));
    TJob * perm_di_passaggio;
    num_job_daschedulare=GNum_Job;
    vett_schedule_parziali=new TSchedula*[GNum_Macchine];
    array_job_attuale_swap = new TJob[GNum_Job];
    while ( cont_livelli>0 )
    {
        punt_job_scelti=NULL;
        TNext_Elem lista_prossimi_vincitori[500];
        //         next_elem lista_prossimi_vincitori_swap[500];
        index=0;
        index1=0;

        // Per prima cosa devo calcolare il max tempo di completamento delle schedule parziali.
        int tempo_minimo_di_completamento=0; //in questo filtro non tengo conto delle indisponibilitï¿½  delle macchine.
        int tempo_minimo_di_release=10000000;//ï¿½? un numero arbitrariamente grande
        //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perÃ² devo verificare che il tempo di rilascio sia inferiore al max dei tempi di completamento delle schedule parziali.

        // devo quindi salvare in una struttura tutti i job validi e in un'altra tutti quegli elementi che pur non essendo ancora stati selezionati non verificano il criterio del filtro.
        if ( cont_livelli==GNum_Job-1 ) //
        {
            vett_schedule_parziali[0]=GMacch1_Sched;
            if ( GNum_Macchine==2 )
            {
                vett_schedule_parziali[1]=GMacch2_Sched;
            }
            if ( GNum_Macchine==3 )
            {
                vett_schedule_parziali[1]=GMacch2_Sched;
                vett_schedule_parziali[2]=GMacch3_Sched;
            }
            tempo_minimo_di_completamento=Min_Cmax_Schedula ( vett_schedule_parziali,GNum_Macchine );

            for ( i = 0;i<GNum_Job;i++ )
            {
                array_job_attuale_swap[i] = array_job_attuale[i];
            }
            for ( i=0;i<GNum_Job;i++ )
            {
                if ( array_job_attuale[i].ID !=-1 )
                {
                    if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                    {
                        tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                    }
                }
            }
            tempo_minimo_di_completamento=Max ( tempo_minimo_di_completamento,tempo_minimo_di_release );
            for ( i = 0; i < GNum_Job; i++ )
                if ( array_job_attuale[i].ID != -1 &&
                     array_job_attuale[i].rel_time > tempo_minimo_di_completamento )
                    array_job_attuale[i].ID = -1;//non schedulabile


        }
        else
        {
            Azzera_Schedule();
            perm_di_passaggio = new TJob[GNum_Job-cont_livelli-1];
            for ( pp = 0; pp < GNum_Job-cont_livelli-1; pp++ )
                perm_di_passaggio[pp] = GBest_Perm[pp];

            pp++;
            TNext_Elem *prossimo2;
            prossimo2 = new TNext_Elem;
            prossimo2->next = NULL;
            Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                          GMacch2_Sched,
                                          GMacch3_Sched,
                                          prossimo2,
                                          perm_di_passaggio,
                                          GNum_Job-cont_livelli-1 );
            delete prossimo2;
            delete[] perm_di_passaggio;
            vett_schedule_parziali[0]=GMacch1_Sched;
            if ( GNum_Macchine == 2 )
                vett_schedule_parziali[1] = GMacch2_Sched;
            if ( GNum_Macchine == 3 )
            {
                vett_schedule_parziali[1] = GMacch2_Sched;
                vett_schedule_parziali[2] = GMacch3_Sched;
            }
            tempo_minimo_di_completamento = Min_Cmax_Schedula ( vett_schedule_parziali,GNum_Macchine );
            for ( i = 0; i < GNum_Job; i++ )
                array_job_attuale[i] = array_job_attuale_swap[i];// ho modificato l'array_job_attuale_swap in modo che indichi come non schedulabile il job scelto al passo precedente.

            tempo_minimo_di_release = 10000000;//e' un numero arbitrariamente grande
            for ( i = 0; i < GNum_Job; i++ )
            {
                if ( array_job_attuale[i].ID !=-1 )
                {
                    if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                    {
                        tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                    }
                }
            }
            tempo_minimo_di_completamento=Max ( tempo_minimo_di_completamento,tempo_minimo_di_release );
            for ( i = 0; i < GNum_Job; i++ )
                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_minimo_di_completamento )
                    array_job_attuale[i].ID=-1;//non schedulabile


        }
        for ( iter_for = 0; iter_for < GNum_Job; iter_for++ )
        {
            /* di volta in volta ridurro il numero di num_job_relativo
             * devo considerare il caso di tutti i job scedulati per primi
             * devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
             * tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
            if ( array_job_attuale[iter_for].ID == -1 ) //se e' non selezionabile
            {
            }
            else //se e' selezionabile
            {
                array_job_attuale_swap[iter_for].ID =-1;

                array_job_attuale_temp = new TJob[cont_livelli];
                iter = 0;
                for ( kk=0;kk<cont_livelli;kk++ )
                {
                    while ( array_job_attuale_swap[iter].ID == -1 ) iter++;

                    array_job_attuale_temp[kk] = array_job_attuale_swap[iter];
                    iter++;
                }
                iter = 0;
                TNext_Elem *prossimo = NULL;
                TNext_Elem *prossimo1;
                for ( i = 0; i < Fnum_Heur; i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                {
                    // Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                    Azzera_Schedule();
                    perm_di_passaggio = new TJob[GNum_Job];
                    Inizializza_Permutazione_Migliore ( perm_di_passaggio );
                    for ( pp = 0; pp < GNum_Job-cont_livelli-1; pp++ )
                        perm_di_passaggio[pp]=GBest_Perm[pp];

                    pp++;
                    perm_di_passaggio[ ( GNum_Job-cont_livelli-1 ) ] = GArray_Job[iter_for];// aggiungo in coda il job prescelto

                    // _____________________________________________________________________________________________________________________________
                    Azzera_Schedule();
                    permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                    permutazioni[i] = Ffunzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                    if ( prossimo == NULL )
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
                        while ( temp->next!=NULL )
                            temp = temp->next;

                        temp_prox=new TNext_Elem;
                        temp->next = temp_prox;
                        temp = temp->next;
                        temp->ID_job = permutazioni[i][0].ID;
                        temp->ID_heur= Ffunzioni[i].ID_heur;
                        temp->next = NULL;
                    }
                    for ( jj = 0;pp < GNum_Job; pp++, jj++ )
                        perm_di_passaggio[pp] = permutazioni[i][jj];

                    //aggiungo i job proposti dall'euristica
                    Azzera_Schedule();
                    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                                  GMacch2_Sched,
                                                  GMacch3_Sched,
                                                  prossimo,
                                                  perm_di_passaggio,
                                                  GNum_Job );
                    emit ScriviASchermo(  QString(" %1) %2 %3 %4 %5 num iter %6\n").
                                        arg(i).
                                        arg(FLmax).
                                        arg(FCmax).
                                        arg(FTardy).
                                        arg(Ffeasible).
                                        arg(GNum_Job-cont_livelli) );
                    Salva_Se_Meglio ( perm_di_passaggio);
                    delete[] perm_di_passaggio;

                    //_____________________________________________________________________________________________________________________
                    //devo riportare la macchina nelle condizioni orginarie
                    if ( force == 1 )
                    {
                        prossimo1       = new TNext_Elem;
                        M1_sch_buffer   = new TSchedula;
                        M2_sch_buffer   = new TSchedula;
                        M3_sch_buffer   = new TSchedula;
                        Copia_Schedule ( GMacch1_Sched,M1_sch_buffer );
                        if ( GNum_Macchine >= 2 )
                            Copia_Schedule ( GMacch2_Sched,M2_sch_buffer );
                        if ( GNum_Macchine==3 )
                            Copia_Schedule ( GMacch3_Sched,M3_sch_buffer );

                        VNS (M1_sch_buffer,
                             M2_sch_buffer,
                             M3_sch_buffer );
                        Bilanciamento_Schedule (M1_sch_buffer,
                                                M2_sch_buffer,
                                                M3_sch_buffer );//bilancio
                        Valuta_Schedula (M1_sch_buffer,
                                         M2_sch_buffer,
                                         M3_sch_buffer,
                                         prossimo1 );

                        Elimina_Schedula ( M1_sch_buffer );
                        if ( GNum_Macchine >= 2 ) {Elimina_Schedula ( M2_sch_buffer );}
                        if ( GNum_Macchine == 3 ) {Elimina_Schedula ( M3_sch_buffer );}
                        TNext_Elem *tmp_prox;
                        tmp_prox = prossimo;
                        while ( tmp_prox->next!=NULL )
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
                            tmp_prox->Lmax=prossimo1->Lmax;
                            tmp_prox->Cmax=prossimo1->Cmax;
                            tmp_prox->Tardy=prossimo1->Tardy;
                            tmp_prox->feasible=prossimo1->feasible;
                        }
                        delete prossimo1;
                    }
                }
                // se la schedula non e' feasible deve essere penalizzata rispetto alle altre.
                // devo ridurre il numero di job che rimangono da schedulare
                // devo trovare il job con la Lateness + alta
                // in condizioni di parita' quello con la Cmax +bassa
                // infine con il numero + basso di Tardy job
                array_job_attuale_swap[iter_for].ID = iter_for;
                delete[] array_job_attuale_temp;
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
                while ( temp->next!=NULL )
                {
                    if ( Ffeasible <= temp->next->feasible )
                    {
                        if
                                (
                                 ( Ffeasible ==0 )
                                 &&
                                 ( temp->next->feasible==1 )
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
                    temp=temp->next;
                }//ora sappiamo qual e' L'ID del Job da eliminare dalla lista dei job da schedulare
                //       e su quale macchina e' stato schedulato
                emit ScriviASchermo(  QString("\n (%1) %2 %3 %4 %5\n").
                                    arg(cont_livelli).
                                    arg( ID_heur).
                                    arg( L_max).
                                    arg( C_max).
                                    arg(tardy ));
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

                //qui salvo i job che si sono dimostrati i migliori.C_max

                TNext_Elem *temp_pr;
                for ( i = 0; i < Fnum_Heur; i++ )
                {
                    temp_pr = prossimo->next;
                    delete prossimo;
                    prossimo = temp_pr;
                }


                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                for ( i = 0; i < Fnum_Heur; i++ )
                    delete[] permutazioni[i];

            }
        }
        emit ScriviASchermo(  QString("\n %1____________________\n").arg(cont_livelli) );

        Pos_vincitore=Seleziona_Prossimo_Job ( lista_prossimi_vincitori,num_next_job );//da fare
        //Pos_vincitore indica la posizione relativa del job da schedulare
        Pos_assoluta=Trova_Posizione_Assoluta ( array_job_attuale,Pos_vincitore );
        // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
        if
                (
                 ( lista_prossimi_vincitori[Pos_vincitore].feasible>Ffeasible_Best )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                     )
                 ||
                 (
                     ( lista_prossimi_vincitori[Pos_vincitore].feasible==Ffeasible_Best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                     &&
                     ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Ftardy_Best )
                     )
                 )
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = array_job_attuale[Pos_assoluta].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = array_job_attuale[Pos_assoluta].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = array_job_attuale[Pos_assoluta].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = array_job_attuale[Pos_assoluta].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = array_job_attuale[Pos_assoluta].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = array_job_attuale[Pos_assoluta].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = array_job_attuale[Pos_assoluta].rel_time;
            array_job_attuale_swap[Pos_assoluta].ID = -1;
        }
        else
        {
            GBest_Perm[GNum_Job-cont_livelli-1].ID          = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID;
            GBest_Perm[GNum_Job-cont_livelli-1].tipo        = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].tipo;
            GBest_Perm[GNum_Job-cont_livelli-1].proc_time   = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].proc_time;
            GBest_Perm[GNum_Job-cont_livelli-1].duedate     = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].duedate;
            GBest_Perm[GNum_Job-cont_livelli-1].deadline    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].deadline;
            GBest_Perm[GNum_Job-cont_livelli-1].priority    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].priority;
            GBest_Perm[GNum_Job-cont_livelli-1].rel_time    = Fpermutazione_Buffer[GNum_Job-cont_livelli-1].rel_time;
            array_job_attuale_swap[Fpermutazione_Buffer[GNum_Job-cont_livelli-1].ID].ID=-1;
        }
        cont_livelli--;
        if ( punt_job_scelti != NULL )
            delete[] punt_job_scelti;
    }


    i = 0;
    while ( array_job_attuale_swap[i].ID==-1 ) i++;

    GBest_Perm[GNum_Job-1].ID       = array_job_attuale_swap[i].ID;
    GBest_Perm[GNum_Job-1].tipo     = array_job_attuale_swap[i].tipo;
    GBest_Perm[GNum_Job-1].proc_time= array_job_attuale_swap[i].proc_time;
    GBest_Perm[GNum_Job-1].duedate  = array_job_attuale_swap[i].duedate;
    GBest_Perm[GNum_Job-1].deadline = array_job_attuale_swap[i].deadline;
    GBest_Perm[GNum_Job-1].priority = array_job_attuale_swap[i].priority;
    GBest_Perm[GNum_Job-1].rel_time = array_job_attuale_swap[i].rel_time;
    array_job_attuale[i].ID = -1;

    delete[] array_job_attuale;
    delete[] array_job_attuale_swap;
    Azzera_Schedule();
    TNext_Elem *prossimo1;
    prossimo1= new TNext_Elem;
    prossimo1->next = NULL;
    Costruisci_E_Valuta_Schedula (GMacch1_Sched,
                                  GMacch2_Sched,
                                  GMacch3_Sched,
                                  prossimo1,
                                  GBest_Perm,
                                  GNum_Job );
    delete prossimo1;


    //  TODO questo si dovrebbe riscrivere. Molte cose sono fatte male.
    if
            (
             ( Ffeasible < Ffeasible_Best )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax > Lmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax > Cmax_best )
                 )
             ||
             (
                 ( Ffeasible == Ffeasible_Best )
                 &&
                 ( FLmax == Lmax_best )
                 &&
                 ( FCmax == Cmax_best )
                 &&
                 ( FTardy > Ftardy_Best )
                 )
             )
    {
        delete[] permutazioni;
        return Fpermutazione_Buffer;
    }
    else
    {
        delete[] permutazioni;
        delete[] Fpermutazione_Buffer;
        return   GBest_Perm;
    }
}


int QRolloutThread::QRolloutThreadInicialize(QString pFileConfig, QString pFileSolution)
{
    GNum_Job = 0;
    GNum_Tipi = 0;
    GNum_Macchine = 0;

    FLmax = 0;// massima lateness
    FCmax = 0;//Makespan
    FTardy = 0;//numero di tardy jobs per schedula
    Ffeasible = 1; // mi dice se la schedula costruita ï¿½feasible
    Lmax_best = -65000;
    Cmax_best = -65000;
    Ftardy_Best = -65000;
    Ffeasible_Best = -2;
    Fswap_Lat_Tard = 0;
    FTempo_Sec_Inizio1 = 0 ;
    FTempo_Inizio1 = 0;
    FTempo_Inizio2 = 0;
    FTempo_Inizio3 = 0;
    FTempo_Inizio4=0;    
    FTempo_Sec_Fine1 = 0;
    FTempo_Fine1 = 0;
    FTempo_Fine2 = 0;
    FTempo_Fine3 = 0;

    Carica_File_Configurazione (pFileConfig.toAscii().data());

    QString local_search_mode_text;
    local_search_mode_text.setNum(FLocal_Search_Mode );
    emit ScriviASchermo( local_search_mode_text);

    strcpy ( FInstance_File, pFileSolution.toAscii().data() );
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
    Fistanza = fopen ( FInstance_File,"r" );
    if ( Fistanza == NULL )
    {
            emit ScriviASchermo(  "\n Error file inesistente \a\n" );
            return 0;
    }

    //codice inutile in questa versione del codice dove gli output sono distribuiti su piú files.
    FfileOut = fopen ( FOutput_File,"a+" );
    if ( FfileOut == NULL )
    {
            emit ScriviASchermo(  "Errore di apertura del file "+ QString::fromAscii(FOutput_File) );
            fclose(Fistanza);
            return 0;
    }


    GNum_Macchine = Carica_Indisponibilita ( Fistanza ); //info nella funzione

    if ( GNum_Macchine == 0 ) //dev'esserci almeno una macchina per il problema
    {
            emit ScriviASchermo(  "\n Error! formato file non riconosciuto\n" );
            fclose(FfileOut);
            return 0;
    }

    //a questo punto devo caricare le matrici con le caratteristiche dei singoli job da mandare
    //in esecuzione sulle macchine parallele.
    Fprossimo = new TNext_Elem;
    Fprossimo->next = NULL;

    Carica_Lista_Job ( Fistanza );  //info nella funz stessa

    Fnum_Heur = Calcola_Numero_Euristiche ( Ftipo_Eur ); //ritorna il numero di heuristiche del tipo

    FNum_Heur_Used = Fnum_Heur;

    Ffunzioni = new TStruttura_Funz[Fnum_Heur];//alloca lo spazio per l'array delle heuristiche

    Inizializza_Struttura_Euristiche ( Ftipo_Eur ); //info nella funzione stessa

    //inizializza le schedule
    GMacch1_Sched = NULL;
    GMacch2_Sched = NULL;
    GMacch3_Sched = NULL;

    Azzera_Schedule(); //info nella funzione stessa



    fclose(FfileOut);
    fclose(Fistanza);
    return 1;
}

int QRolloutThread::getNumJob(){
    return GNum_Job;
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
            Ffunzioni[0].funz = Permutazione_EDD_1_tipo;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 1:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_EDD_2_tipo;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 2:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_base;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 3:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_SPT_semplice;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 4:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_LLF;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 5:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_delta_7ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 6:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_delta_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 7:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_delta_mezzo_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 8:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_LLF_delta_7ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 9:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_LLF_delta_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 10:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_delta_24ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 11:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_delta_3_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 12:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_LLF_delta_14ore;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 13:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_LLF_delta_3_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 14:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_LLF_delta_mezzo_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            break;
        }
        case 15:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_delta_proc_medio;
            Ffunzioni[0].ID_heur = 0;
            Ffunzioni[1].perc_utilizzo = 0;
            Ffunzioni[1].funz = Permutazione_base;
            Ffunzioni[1].ID_heur = 1;
            break;
        }
        case 16:
        {
                Ffunzioni[0].perc_utilizzo = 0;
                Ffunzioni[0].funz = Permutazione_EDD_1_tipo;
                Ffunzioni[0].ID_heur = 0;
                Ffunzioni[1].perc_utilizzo = 0;
                Ffunzioni[1].funz = Permutazione_EDD_2_tipo;
                Ffunzioni[1].ID_heur = 0;
                Ffunzioni[2].perc_utilizzo = 0;
                Ffunzioni[2].funz = Permutazione_base;
                Ffunzioni[2].ID_heur = 0;
                Ffunzioni[3].perc_utilizzo = 0;
                Ffunzioni[3].funz = Permutazione_SPT_semplice;
                Ffunzioni[3].ID_heur = 0;
                Ffunzioni[4].perc_utilizzo = 0;
                Ffunzioni[4].funz = Permutazione_LLF;
                Ffunzioni[4].ID_heur = 0;
                Ffunzioni[5].perc_utilizzo = 0;
                Ffunzioni[5].funz = Permutazione_delta_7ore;
                Ffunzioni[5].ID_heur = 0;
                Ffunzioni[6].perc_utilizzo = 0;
                Ffunzioni[6].funz = Permutazione_delta_proc_medio;
                Ffunzioni[6].ID_heur = 0;
                Ffunzioni[7].perc_utilizzo = 0;
                Ffunzioni[7].funz = Permutazione_delta_mezzo_proc_medio;
                Ffunzioni[7].ID_heur = 0;
                Ffunzioni[8].perc_utilizzo = 0;
                Ffunzioni[8].funz = Permutazione_LLF_delta_7ore;
                Ffunzioni[8].ID_heur = 0;
                Ffunzioni[9].perc_utilizzo = 0;
                Ffunzioni[9].funz = Permutazione_LLF_delta_proc_medio;
                Ffunzioni[9].ID_heur = 0;
                break;
        }
        case 17:
        {
            Ffunzioni[0].perc_utilizzo = 0;
            Ffunzioni[0].funz = Permutazione_EDD_1_tipo;
            Ffunzioni[0].ID_heur = 0;

            Ffunzioni[1].perc_utilizzo = 0;
            Ffunzioni[1].funz = Permutazione_EDD_2_tipo;
            Ffunzioni[1].ID_heur = 1;

            Ffunzioni[2].perc_utilizzo = 0;
            Ffunzioni[2].funz = Permutazione_base;
            Ffunzioni[2].ID_heur = 2;

            Ffunzioni[3].perc_utilizzo = 0;
            Ffunzioni[3].funz = Permutazione_SPT_semplice;
            Ffunzioni[3].ID_heur = 3;

            Ffunzioni[4].perc_utilizzo = 0;
            Ffunzioni[4].funz = Permutazione_LLF;
            Ffunzioni[4].ID_heur = 4;

            Ffunzioni[5].perc_utilizzo = 0;
            Ffunzioni[5].funz = Permutazione_delta_7ore;
            Ffunzioni[5].ID_heur = 5;

            Ffunzioni[6].perc_utilizzo = 0;
            Ffunzioni[6].funz = Permutazione_delta_proc_medio;
            Ffunzioni[6].ID_heur = 6;

            Ffunzioni[7].perc_utilizzo = 0;
            Ffunzioni[7].funz = Permutazione_delta_mezzo_proc_medio;
            Ffunzioni[7].ID_heur = 7;

            Ffunzioni[8].perc_utilizzo = 0;
            Ffunzioni[8].funz = Permutazione_LLF_delta_7ore;
            Ffunzioni[8].ID_heur = 8;

            Ffunzioni[9].perc_utilizzo = 0;
            Ffunzioni[9].funz = Permutazione_LLF_delta_proc_medio;
            Ffunzioni[9].ID_heur = 9;

            Ffunzioni[10].perc_utilizzo = 0;
            Ffunzioni[10].funz = Permutazione_delta_24ore;
            Ffunzioni[10].ID_heur = 10;

            Ffunzioni[11].perc_utilizzo = 0;
            Ffunzioni[11].funz = Permutazione_delta_3_proc_medio;
            Ffunzioni[11].ID_heur = 11;

            Ffunzioni[12].perc_utilizzo = 0;
            Ffunzioni[12].funz = Permutazione_LLF_delta_14ore;
            Ffunzioni[12].ID_heur = 12;

            Ffunzioni[13].perc_utilizzo = 0;
            Ffunzioni[13].funz = Permutazione_LLF_delta_3_proc_medio;
            Ffunzioni[13].ID_heur = 13;

            Ffunzioni[14].perc_utilizzo = 0;
            Ffunzioni[14].funz = Permutazione_LLF_delta_mezzo_proc_medio;
            Ffunzioni[14].ID_heur = 14;
            //funzioni[15].perc_utilizzo = 0;
            //funzioni[15].funz = permutazione_fittizia;
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



