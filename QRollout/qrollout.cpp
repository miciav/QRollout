#include "qrollout.h"
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

#include "antonio.cpp"
//GiÃ  commentata //#include "rollout_time.c"
#include "euristiche.cpp"
#include "ricerche_locali.cpp"



void QRollout::carica_file ( char *path_file_conf )
{
        FILE *file_conf;
        if ( ( file_conf=fopen ( path_file_conf,"r" ) ) ==NULL )
        {
                printf ( "\n errore nel file di configurazione \n" );
        }
        else
        {
                //0 -> rollout   -   1->euristica
                fscanf ( file_conf,"%d \n",&rollout_or_heuristic );
                //scelta euristica da lanciare singolarmente
                fscanf ( file_conf,"%d \n",&euristica );
                //scelta del tipo di rollout da lanciare
                fscanf ( file_conf,"%d \n",&tipo_rollout );
                //scelta dell'algoritmo euristico da utilizzare nel rollout
                fscanf ( file_conf,"%d \n",&tipo_eur );
                //scelta del tempo
                fscanf ( file_conf,"%d \n",&Tempo );
                //percorso+nome del file di output della computazione
                fscanf ( file_conf,"%s \n",output_file );
                fscanf ( file_conf,"%d \n",&force );
                //scelta della politica di pruning, utilizzata solo in alcuni tipi di rollout
                fscanf ( file_conf,"%d \n",&politica_pruning );
                //scelta del tipo di ricerca locale da applicare successivamente al rollout (0 se non la si vuole applicare)
                fscanf ( file_conf,"%d",&local_search_mode );
                fclose ( file_conf );
                if ( rollout_or_heuristic==1 )
                        tipo_eur=17; //perchÃ¨ si deve poter scegliere, tramite la variabile "euristica", tra tutte le euristiche disponibili.
        }
}
//*************************************************************************************************
// MAIN: versione di antonio maccioni adattata al testing su rollout dinamici e local search.
//***********************************************************************************************

int QRollout::mainFunction (char**argv )
{

        next_elem *prossimo1;
        FILE *istanza;// file di testo formattato contenente l'istanza da eseguire.
        FILE *f_out;// file di testo in cui verrï¿½ï¿½?salvato l'output

        carica_file ( argv[1] );
        printf ( "%d",local_search_mode );
        strcpy ( instance_file,argv[2] );
        if ( !strcmp ( output_file,"--force" ) )
        {
                force =1;
                strcpy ( output_file,"./output.txt" );
        }

        if ( ( tipo_rollout!=1 ) && ( tipo_rollout!=0 ) && ( tipo_rollout!=2 ) && ( tipo_rollout!=3 ) && ( tipo_rollout!=4 ) && ( tipo_rollout!=5 ) && ( tipo_rollout!=6 )
                && ( tipo_rollout!=7 ) && ( tipo_rollout!=8 ) && ( tipo_rollout!=9 ) && ( tipo_rollout!=10 ) && ( tipo_rollout!=11 ) && (tipo_rollout != 12) && (tipo_rollout !=13))
        {
                printf ( "\nERROR: il primo parametro puï¿½ï¿½?essere solo 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 o 0!\n\a" );
                exit ( 0 );
        }
        istanza = fopen ( instance_file,"r" );//apro il file deve ï¿½ï¿½?contenuta l'istanza da considerare
        if ( istanza == NULL )
        {
                printf ( "\n Error file inesistente \a\n" );
        }


// 	strcpy(str_temp, output_file);
// 	out_Lmax = strcat(str_temp, "_Lmax.txt");
// 	f_out_Lmax = fopen (out_Lmax,"a" );
// 	strcpy(str_temp, output_file);
//
// 	out_Cmax = strcat(str_temp, "_Cmax.txt");
// 	f_out_Cmax = fopen (out_Cmax,"a" );
// 	strcpy(str_temp, output_file);
//
// 	out_Tardy = strcat( str_temp, "_Tardy.txt");
// 	f_out_Tardy = fopen ( out_Tardy,"a" );
// 	strcpy(str_temp, output_file);
//
// 	out_Feas = strcat( str_temp, "_Feas.txt");
// 	f_out_Feas = fopen ( out_Feas,"a" );
// 	strcpy(str_temp, output_file);
//
// 	out_Tempo = strcat( str_temp, "_Tempo.txt");
// 	f_out_Tempo = fopen ( out_Tempo,"a" );
// 	strcpy(str_temp, output_file);
        /*
        fclose(f_out_Lmax);
        fclose(f_out_Cmax);
        fclose(f_out_Tardy);
        fclose(f_out_Feas);
        fclose(f_out_Tempo);
        */
// 	strcpy(str_temp, output_file);
// 	out_Lmax = strcat(str_temp, "_Lmax.txt");
// 	f_out_Lmax = fopen (out_Lmax,"r+" );
// 	strcpy(str_temp, output_file);
//
// 	out_Cmax = strcat(str_temp, "_Cmax.txt");
// 	f_out_Cmax = fopen (out_Cmax,"r+" );
// 	strcpy(str_temp, output_file);
//
// 	out_Tardy = strcat( str_temp, "_Tardy.txt");
// 	f_out_Tardy = fopen ( out_Tardy,"r+" );
// 	strcpy(str_temp, output_file);
//
// 	out_Feas = strcat( str_temp, "_Feas.txt");
// 	f_out_Feas = fopen ( out_Feas,"r+" );
// 	strcpy(str_temp, output_file);
//
// 	out_Tempo = strcat( str_temp, "_Tempo.txt");
// 	f_out_Tempo = fopen ( out_Tempo,"r+" );
// 	strcpy(str_temp, output_file);




        //codice inutile in questa versione del codice dove gli output sono distribuiti su piÃ¹ files.
        f_out = fopen ( output_file,"a+" );
        if ( f_out == NULL )
        {
                printf ( "Errore di apertura del file %s",output_file );
                exit ( 12 );
        }


        num_macchine = carica_indisponibilita ( istanza ); //info nella funzione

        if ( num_macchine == 0 ) //dev'esserci almeno una macchina per il problema
        {
                printf ( "\n Error! formato file non riconosciuto\n" );
                return 0;
        }

        //a questo punto devo caricare le matrici con le caratteristiche dei singoli job da mandare
        //in esecuzione sulle macchine parallele.
        prossimo1 = new next_elem;
        carica_lista_job ( istanza );  //info nella funz stessa
        num_heur= calcola_numero_eur ( tipo_eur ); //ritorna il numero di heuristiche del tipo
        num_heur_utilizz=num_heur;
        funzioni=new strutt_funz[num_heur];//alloca lo spazio per l'array delle heuristiche
        inizializza_struttura_euristiche ( tipo_eur ); //info nella funzione stessa
        //funzioni[15].perc_utilizzo=-1;
        //inizializza le schedule
        M1_sch=NULL;
        M2_sch=NULL;
        M3_sch=NULL;
        azzera_schedule(); //info nella funzione stessa
        best_perm = new job[num_job]; // array/lista della migliore permutazione che trovera' il rollout che si andra' ad eseguire
        tempo_secondi_inizio1 = time ( NULL ) ;
        tempo_inizio1=clock(); //misurazione del rollout
        //tempo_inizio2=clock(); //misurazione rollout+local_search

        //ROLLOUT O LANCIO DELL'EURISTICA: TROVA LA PERMUTAZIONE BASE
        switch ( rollout_or_heuristic )
        {
                case 0:
                {
                        switch ( tipo_rollout ) // stabilisco che tipo di rollout utilizzare.
                        {
                                case 0:
                                {
                                        permutazione_finale = rollout_old ( force,instance_file );
                                        break;
                                }
                                case 1:
                                {
                                        permutazione_finale = rollout ( force,instance_file );
                                        break;
                                }
                                case 2:
                                {
                                        swap_lat_tard=1;
                                        permutazione_finale = rollout_old ( force,instance_file );
                                        break;
                                }
                                case 3:
                                {
                                        swap_lat_tard=1;
                                        permutazione_finale = rollout ( force,instance_file );
                                        break;
                                }
                                case 4:
                                {
                                        permutazione_finale = rollout_modificato1 ( force,instance_file );
                                        break;
                                }
                                case 5:
                                {
                                        permutazione_finale = rollout_modificato2 ( force,instance_file );
                                        break;
                                }
                                case 6:
                                {
                                        permutazione_finale = rollout_modificato3 ( force,instance_file );
                                        break;
                                }
                                case 7:
                                {
                                        permutazione_finale = rollout_modificato4 ( force,instance_file );
                                        break;
                                }
                                case 8:
                                {
                                        permutazione_finale = rollout_modificato5 ( force,instance_file );
                                        break;
                                }
                                case 9:
                                {
                                        permutazione_finale = rollout_modificato6 ( force,instance_file );
                                        break;
                                }
                                case 10:
                                {
                                        permutazione_finale = rollout_heuristic_pruning ( force,instance_file );
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
                                        permutazione_finale = rollout_dynamic_job_choosing (force , instance_file);
                                        break;
                                }
                                case 13:
                                {
                                        permutazione_finale = rollout_dynamic (force, instance_file);
                                        break;
                                }
                        }
                        break;

                }
                // nel case 1 lancia l'euristica scelta
                case 1:
                {
                        permutazione_finale = funzioni[euristica].funz ( array_job,num_job );
                        break;
                }
        }
        //stampa testualmente l'ordine della permutazione risultante dal rollout
        //stampa_permutazioni ( permutazione_finale,num_job );
        tempo_secondi_fine1 = time ( NULL );
        tempo_fine1 = clock();
        azzera_schedule();
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,permutazione_finale,num_job );

        stampa_risultati_su_file ( f_out,instance_file,0,force );

        int c;

        //RICERCA LOCALE (PURA, CODA, CON VND) O VNS
        tempo_inizio2 = clock() ;
        switch(local_search_mode)
        {
                case 0:{
                //nor local search neither VNS
                        break;
                }
                case 1:{
                        c =VNS(M1_sch,M2_sch,M3_sch);
                        break;
                }
                case 2:{
                        c = ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
                        break;
                }
                case 3:{
                        c = ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
                        break;
                }
                case 4:{
                        c = ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
                        break;
                }
        }

        tempo_fine2 = clock();
        valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1 );

        /*if (local_search_mode==0)
                stampa_su_file_antonio (output_file, 0);
        else
                stampa_su_file_antonio(output_file,1);*/

        if (local_search_mode==0)
                stampa_risultati_a_video ( 0 );
        else
                stampa_risultati_a_video ( 1 );

        stampa_risultati_su_file ( f_out,instance_file,1,force );

        //BILANCIAMENTO
        tempo_inizio3 = clock() ;
        bilanciamento_schedule ( M1_sch,M2_sch,M3_sch );//bilancio
        tempo_fine3 = clock();
        valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1 );
        stampa_risultati_a_video ( 2 );
        stampa_risultati_su_file ( f_out,instance_file,2,force );
        //stampa_percentuali_utilizzo_a_video();
        distruggi_indisponibilita ( num_macchine );
        distruggi_schedule ( num_macchine );
        free ( prossimo1 );
        free ( funzioni );
        free ( best_perm );
        //free(permutazione_finale);//con questa free disalloco anche best_perm
        free ( array_job ); //disalloco la memoria occupata dall'array dei job.
        free ( array_tipi );//disalloco la memoria occupata dall'array dei tipi.
        fclose ( istanza );
        fclose ( f_out );
        return EXIT_SUCCESS;
}
//*************************************************************************************************
//la funzione seguente carica i tempi di indisponibilitï¿½ï¿½?delle macchine dal file, e restituisce il numero di macchine caricate o 0 se qualche errore ï¿½ï¿½?stato individuato nel formato del file.
//***********************************************************************************************
int QRollout::carica_indisponibilita ( FILE *istanza )
{
        char str[10];
        char macchina[2];
        int totale_macchine=0;
//        int time=0;
        int numM1=0,numM2=0,numM3=0,i;
        elem *attuale;
        for ( i=0;i<10;i++ )
        {
                str[i]='\n';
        }
        fscanf ( istanza,"%s",macchina ); //carico il nome della macchina che normalmente ï¿½ï¿½?M1
        if ( strcmp ( macchina , "M1" ) )
        {
                printf ( "ERRORE No M1 " );
                return 0; //zero significa che il caricamento non ï¿½ï¿½?andato a buon fine
        }
        totale_macchine++;
        M1 =new elem;
        fscanf ( istanza,"%s ",str ); //carico il numero di di inisponibilitï¿½ï¿½?per la macchina M1
        numM1 = atoi ( str );
        fscanf ( istanza,"%s",str );
        //printf("%s ",str);
        M1->inizio =atoi ( str );
        fscanf ( istanza,"%s",str );
        //fscanf(istanza,"%s",str);
        M1->fine = M1->inizio+atoi ( str );
        attuale = M1;
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
                        printf ( "ERROR no M2" );
                        return 0; //zero significa che il caricamento non ï¿½ï¿½?andato a buon fine
                }
                attuale->next =new elem;
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
                M2 =new elem;
                M2->inizio =atoi ( str );
                fscanf ( istanza,"%s",str );
                M2->fine = M2->inizio+atoi ( str );
                attuale = M2;
                fscanf ( istanza,"%s",str );
                while (
                    strcmp ( str , "M3" )

                    &&

                    strcmp ( str , "%%" )
                )
                {
                        if ( i>numM2 )
                        {
                                printf ( "ERROR no M3" );
                                return 0; //zero significa che il caricamento non ï¿½ï¿½?andato a buon fine
                        }
                        attuale->next =new elem;
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
                        numM3=atoi ( str );
                        fscanf ( istanza,"%s",str );
                        M3 =new elem;
                        M3->inizio =atoi ( str );
                        fscanf ( istanza,"%s",str );
                        M3->fine = M3->inizio+atoi ( str );
                        attuale = M3;
                        fscanf ( istanza,"%s",str );
                        while ( strcmp ( str , "%%" ) )
                        {
                                if ( i>numM3 )
                                {
                                        printf ( "ERROR in M3" );
                                        return 0; //zero significa che il caricamento non ï¿½ï¿½?andato a buon fine
                                }
                                attuale->next =new elem;
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
//*************************************************************************************************
//la funzione seguente elimina la memoria occupata dalle lista dei tempi di indisponibilitï¿½ï¿½?//***********************************************************************************************
void QRollout::distruggi_indisponibilita ( int num_macchine )
{
        if ( num_macchine == 1 )
        {
                elimina_lista ( M1 );
        }
        else if ( num_macchine == 2 ) //mi trovo nel caso di dispencing
        {
                elimina_lista ( M1 );
                elimina_lista ( M2 );
        }
        else//mi trovo nel caso di counting
        {
                elimina_lista ( M1 );
                elimina_lista ( M2 );
                elimina_lista ( M3 );
        }
}
void QRollout::distruggi_schedule ( int num_macchine )
{
        if ( num_macchine == 2 ) //mi trovo nel caso di dispencing
        {
                elimina_schedula ( M1_sch );
                elimina_schedula ( M2_sch );
        }
        else//mi trovo nel caso di counting
        {
                elimina_schedula ( M1_sch );
                elimina_schedula ( M2_sch );
                elimina_schedula ( M3_sch );
        }

}
void elimina_schedula ( schedula * punt_schedula )
{
        //devo ora liberare lo spazio delle schedule locali
        schedula *temp1;
        schedula *temp2;
        temp1 = punt_schedula;
        while ( temp1!=NULL )
        {
                temp2 = temp1->next;//questo serve nel caso di ultimo elemento
                while ( temp1->next!=NULL )
                {
                        temp2 = temp1;
                        temp1=temp1->next;
                }
                free ( temp1 );
                if ( temp2!=NULL )
                {
                        temp2->next = NULL;
                }
                else
                {
                        punt_schedula = NULL;
                        break;
                }

                temp1 = punt_schedula;
        }
}
//*************************************************************************************************
//la funzione seguente elimina le liste di indisponibilitï¿½ï¿½?ed ï¿½ï¿½?asservita a distruggi_indisponibilita
//***********************************************************************************************
void QRollout::elimina_lista ( elem * punt_lista )
{
        elem *temp1;
        temp1 = punt_lista;
        while ( temp1->next != NULL )
        {
                elem *temp;
                temp = temp1;
                temp1 = temp1->next;
                free ( temp );
        }
}
//*************************************************************************************************
//la funzione seguente carica tutte le matrici necessarie a far funzionare il programma
//***********************************************************************************************

int QRollout::carica_lista_job ( FILE * istanza )
{
        char str[10];
        fscanf ( istanza,"%s",str );// a questo punto il file dovrebbe essere nella posizione che mi aspetto
        //cioï¿½ï¿½?dove si trovano le info sui job.

        if ( !strcmp ( str,"//" ) )
        {
                fscanf ( istanza,"%s",str );
                num_job  = atoi ( str );
                fscanf ( istanza,"%s",str );
                if ( !strcmp ( str,"%%" ) )
                {
                        array_job = new job[num_job];
                        int i = 0;
                        fscanf ( istanza,"%s",str );
                        while ( strcmp ( str,"//" ) )
                        {
                                array_job[i].ID = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_job[i].tipo = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_job[i].proc_time = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_job[i].duedate = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_job[i].deadline = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_job[i].priority = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_job[i].rel_time = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                if ( strcmp ( str,"%%" ) )
                                {
                                        if
                                        (
                                            strcmp ( str,"//" )
                                            ||
                                            ( i>=num_job )
                                        ) //verifico che non ci siano errori nella strutture
                                        {		    //dei campi
                                                printf ( "\n error sulla struttura dei job" );
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
                        printf ( "\n error sul numero job" );
                        return 0;
                }
                //a questo punto se i job ed i loro attributi sono stati caricati correttamente
                //il programma carica dal file i tipi e i MaxOpCamp.
                fscanf ( istanza,"%s",str );
                num_tipi = atoi ( str );
                array_tipi = new tipo[num_tipi];
                fscanf ( istanza,"%s",str );
                if ( !strcmp ( str,"%%" ) )
                {
                        int i = 0;
                        fscanf ( istanza,"%s",str );
                        while ( strcmp ( str,"//" ) )
                        {
                                array_tipi[i].ID = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                array_tipi[i].MaxOpCamp = atoi ( str );
                                fscanf ( istanza,"%s",str );
                                if ( strcmp ( str,"%%" ) )
                                {
                                        if
                                        (
                                            strcmp ( str,"//" )
                                            ||
                                            ( i>=num_job )
                                        ) //verifico che non ci siano errori nella strutture
                                        {		    //dei campi
                                                printf ( "\n error sulla struttura dei tipi" );
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
                        printf ( "\n error sul numero dei tipi" );
                        return 0;
                }
                int j=0;
                fscanf ( istanza,"%s",str );
                while ( strcmp ( str,"//" ) )
                {
                        int i = 0;
                        for ( i=0;i<num_tipi;i++ )
                        {
                                Cmaj_matrix[j][i]=atoi ( str );
                                fscanf ( istanza,"%s",str );
                        }
                        fscanf ( istanza,"%s",str );

                        if ( j>=num_tipi )
                        {
                                printf ( "\n error nella matrice dei tipi" );
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
//***********************************************************************************************

int costruisci_e_valuta_schedula ( schedula *M1_sch_locale,schedula *M2_sch_locale,schedula *M3_sch_locale,next_elem *prossimo,job *perm,int dim_job )
{
//per ogni macchina in cui il tempo disponibile nn ï¿½ï¿½?utilizzabile verrï¿½ï¿½?considerato un job fittizio.
//regole
//1)il job viene assegnato alla macchina disponibile che lo possa accogliere
//2)tra le macchine disponibili si sceglie quella che ha giï¿½ï¿½?schedulati job dello stesso tipo
//3)altrimenti si sceglie la macchina con costi di setup + bassi questo vale anche quando il job
// e' il primo della campagna.

        int i;
        //int time_max = 0,time_min = 0;
        int *disponibilita; //ci dice quando la macchina e' disponibile
        int *setup_vett;//ci dice se la macchina deve attendere un setup
    //    int slack1 = 0; //slack time, puï¿½ï¿½?essere utilizzato per eseguire setup prima della release date.
        next_elem *temp_prox;
        int *set;
        int cambio ;
        int k ;
        int schedulato;
        schedula *temp;
        elem *temp1;
        //fine dichiarazione delle variabili
// ____________________________________________________________________________________________________________________

        disponibilita=new int[num_macchine];
        setup_vett=new int[num_macchine];
        temp_prox = prossimo;

        set=new int[num_macchine];
        while ( temp_prox->next!=NULL )
        {
                temp_prox = temp_prox->next;
        }//trovo l'elemento giusto



        for ( i = 0;i<dim_job;i++ ) //considero singolarmente i job della pemutazione e costruisco
        {			//la schedula
                //_______________________________________________________________________________
                //inizializzo l'array della disponibilita' e setup_vett
                disponibilita[0] = 0;
                setup_vett[0] = 0;
                if ( num_macchine >= 2 )
                {
                        disponibilita[1] = 0;
                        setup_vett[1] = 0;
                }
                if ( num_macchine == 3 )
                {
                        disponibilita[2] = 0;
                        setup_vett[2] = 0;
                }

                //_______________________________________________________________________________
                // calcolo le disponibilita' delle macchine
                temp = M1_sch;
                temp1 = M1;
                verifica_macchina ( temp,temp1,disponibilita,setup_vett,0,perm,i );
                if ( num_macchine >= 2 )
                {
                        temp = M2_sch;
                        temp1 = M2;
                        verifica_macchina ( temp,temp1,disponibilita,setup_vett,1,perm,i );
                }
                if ( num_macchine == 3 )
                {
                        temp = M3_sch;
                        temp1 = M3;
                        verifica_macchina ( temp,temp1,disponibilita,setup_vett,2,perm,i );
                }

                //_______________________________________________________________________________________
                //a questo punto ho in disponibilita' i tempi in cui e' possibile schedulare un job scelgo
                //a parita' di tempo la macchina che mi riduce i setup
                cambio = 0;
                k = 0;
                schedulato = 0;
                for ( k = 0; k < num_macchine; k++ )
                {
                        set[k]=-1;//inizializzo il set delle macchine
                }
                set[cambio]=1;
                for ( k = 1; k < num_macchine; k++ )
                {
                        if ( disponibilita[cambio] > disponibilita[k] )
                        {
                                int j = 0;
                                cambio = k;
                                for ( j=0;j<num_macchine;j++ )
                                {
                                        set[j]=-1;
                                }
                                set[k]=1;
                        }
                        else if ( disponibilita[cambio]==disponibilita[k] )
                        {
                                set[k]=1;
                        }
                }

                //____________________________________________________________________________________________
                //a questo punto so quali macchine hanno tempo minimo uguale
                for ( k = 0; k < num_macchine; k++ )
                {
                        if ( set[k]==1 && setup_vett[k]==0 )
                                {//allora schedulo il job sulla macchina k-esima
                                        schedulato =2; //caso del primo job, quando non si deve pagare nessun setup
                                        if ( k==0 ) // prima macchina
                                        {
                                                aggiungi_schedula ( M1_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                if ( i==0 ) //mi serve l'info sul 1 job
                                                {
                                                        AggiornaTempProx ( k,perm[i],M1_sch,temp_prox );
                                                }
                                                break;
                                        }
                                        else if ( k==1 ) // seconda macchina
                                        {
                                                aggiungi_schedula ( M2_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                if ( i==0 ) //mi serve l'info sul 1 job
                                                {
                                                        AggiornaTempProx ( k,perm[i],M2_sch,temp_prox );
                                                }
                                                break;
                                        }
                                        else	// terza macchina
                                        {
                                                aggiungi_schedula ( M3_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                if ( i==0 ) //mi serve l'info sul 1 job
                                                {
                                                        AggiornaTempProx ( k,perm[i],M3_sch,temp_prox );
                                                }
                                                break;
                                        }
                                }

                }
                if ( schedulato !=2 ) // se al passo precedente non ho schedulato il job corrente
                {
                        for ( k = 0; k < num_macchine; k++ )
                        {
                                if ( set[k]==1 && setup_vett[k]==1 )
                                        {
                                                schedulato=1; //setup di tipo minor
                                                if ( k==0 )	// schedulo sulla prima macchina
                                                {
                                                        aggiungi_schedula ( M1_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                        if ( i==0 ) //mi serve l'info sul 1 job
                                                        {
                                                                AggiornaTempProx ( k,perm[i],M1_sch,temp_prox );
                                                        }
                                                        break;
                                                }
                                                else if ( k==1 )	//schedulo sulla seconda macchina
                                                {
                                                        aggiungi_schedula ( M2_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                        if ( i==0 ) //mi serve l'info sul 1 job
                                                        {
                                                                AggiornaTempProx ( k,perm[i],M2_sch,temp_prox );
                                                        }
                                                        break;
                                                }
                                                else	//schedulo sulla terza macchina
                                                {
                                                        aggiungi_schedula ( M3_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                        if ( i==0 ) //mi serve l'info sul 1 job
                                                        {
                                                                AggiornaTempProx ( k,perm[i],M3_sch,temp_prox );
                                                        }
                                                        break;
                                                }
                                        }
                        }
                        if ( schedulato ==0 )	// se ho un setup di tipo major
                        {
                                for ( k = 0; k < num_macchine; k++ )
                                {
                                        if ( set[k]==1 )
                                                {
                                                        if ( k==0 )	// schedulo sulla prima macchina
                                                        {
                                                                aggiungi_schedula ( M1_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                                if ( i==0 ) //mi serve l'info sul 1 job
                                                                {
                                                                        AggiornaTempProx ( k,perm[i],M1_sch,temp_prox );
                                                                }
                                                                break;
                                                        }
                                                        else if ( k==1 )	// schedulo sulla seconda macchina
                                                        {
                                                                aggiungi_schedula ( M2_sch,perm[i],disponibilita[k],setup_vett[k] );

                                                                if ( i==0 ) //mi serve l'info sul 1 job
                                                                {
                                                                        AggiornaTempProx ( k,perm[i],M2_sch,temp_prox );
                                                                }
                                                                break;
                                                        }
                                                        else	//schedulo sulla terza macchina
                                                        {
                                                                aggiungi_schedula ( M3_sch,perm[i],disponibilita[k],setup_vett[k] );
                                                                if ( i==0 ) //mi serve l'info sul 1 job
                                                                {
                                                                        AggiornaTempProx ( k,perm[i],M3_sch,temp_prox );
                                                                }
                                                                break;
                                                        }
                                                }
                                }
                        }
                }

        }
        valuta_schedula ( M1_sch_locale,M2_sch_locale,M3_sch_locale,temp_prox );
        free ( set );
        free ( disponibilita );
        free ( setup_vett );
        return 1;
}

//*************************************************************************************************
//la funzione seguente aggiorna il valore di Temp_prox variabile utilizzata da Costruisci e valuta schedula
//***********************************************************************************************
void AggiornaTempProx ( int k,job perm,schedula *M_sch,next_elem *temp_prox )
{
        schedula *temp_macch;

        temp_prox->macchina = k+1;
        temp_macch = M_sch;
        while ( temp_macch->next!=NULL )
        {
                temp_macch=temp_macch->next;
        }
        temp_prox->tipo = temp_macch->tipo;
        temp_prox->inizio = temp_macch->inizio;
        temp_prox->fine = temp_macch->fine;
        temp_prox->Lmax_pers = temp_macch->Lmax;
        temp_prox->Tardy_pers = temp_macch->Tardy;
        temp_prox->index_camp = temp_macch->index_camp;
        temp_prox->proc_time = perm.proc_time;
        temp_prox->rel_time = perm.rel_time;
        temp_prox->duedate = perm.duedate;
        temp_prox->deadline = perm.deadline;
        temp_prox->priority = perm.priority;

}
//*************************************************************************************************
//la funzione seguente aggiunge un singolo job alla schedula
//***********************************************************************************************

void aggiungi_schedula ( schedula *M_sch, job task,int time,int setup_vett )
{
        //dichiarazione di variabili
        schedula *nuovo;
        schedula *temp;
        temp = M_sch;
        //__________________________________________________________
        while ( temp->next != NULL ) //trovo l'ultimo elemento della lista
        {
                temp = temp->next;
        }

        if ( temp->ID_job != -3 )
        {
                nuovo = new schedula;
                temp->next = nuovo;
                if
                (
                    ( setup_vett == 2 ) //eseguo un setup di tipo Cmaj
                    ||
                    ( temp->tipo != task.tipo )
                )
                {
                        nuovo->index_camp = 0;//azzero il campo
                }
                else
                {
                        nuovo->index_camp= temp->index_camp+1;
                }
        }
        else
        {
                nuovo = temp;
                nuovo->index_camp = 1;
        }
        nuovo->ID_job = task.ID;
        nuovo->tipo = task.tipo;
        nuovo->next = NULL;
        nuovo->inizio = time;
        nuovo->fine = nuovo->inizio + task.proc_time;
        if ( task.duedate !=0 )
        {
                nuovo->Lmax = nuovo->fine -task.duedate;
        }
        else
        {
                nuovo->Lmax = -65000;
        }
        if
        (
            ( nuovo->fine > task.duedate )
            &&
            ( task.duedate != 0 )
        )
        {
                nuovo->Tardy =  1;
        }
        else
        {
                nuovo->Tardy = 0;
        }
        return;

}
//*************************************************************************************************
//la funzione seguente calcola il tempo minimo di schedulazione su ciascuna macchina per ogni job
// tiene conto dei setup e dei tempi di indisponibilitï¿½ï¿½?delle macchine
//***********************************************************************************************
void verifica_macchina(schedula *temp, elem  *temp1, int *disponibilita, int *setup_vett,int p,job *perm,int i)
{
        int time_max = 0;
        int time_min = 0;
        int k;
        //int a=0;
        int slack1 = 0; //slack time, puï¿½ essere utilizzato per eseguire setup prima della release date.

        while(temp->next!=NULL)
        {
                temp=temp->next;//cerco l'ultimo elemento schedulato
        }
        //devo tener conto del massimo tra il momento in cui la macchina si libera ed il rel. time del job
        if (temp->fine <=perm[i].rel_time)
        {
                time_max = perm[i].rel_time;
                time_min = temp->fine;
        }
        else
        {
                time_max = temp->fine;
                time_min = perm[i].rel_time;
        }
        //la macchina 1 potrebbe essere disponibile
        //bisogna verificare se il job ï¿½ compatibile con le indisponibilitï¿½
        while	(
                        (temp1->inizio < temp->fine)
                        &&//cerco la prima indisponibilitï¿½successiva all'ultimo job schedulato
                        (temp1->next != NULL)//potrebbe nn esserci una indisp. successiva
                )
        {
                temp1=temp1->next;
        }
        if (temp1->inizio < temp->fine)
        {
                //non ho indisponibilitï¿½ e posso calcolare il momento di disp. semplicemente in base alla rel_date
                slack1 = perm[i].rel_time-temp->fine-1;	//posso sfruttare lo slack time per eseguire il setup
                // lo slack time puï¿½ essere sia positivo che negativo ed ï¿½ il tempo che intercorre tra la fine del job
                if (slack1 <=0)//non posso sfruttare il tempo slack
                {
                        for(k=0;k<num_tipi;k++)
                        {
                                if(array_tipi[k].ID==perm[i].tipo)
                                {
                                        if(
                                                (perm[i].tipo == temp->tipo)
                                                &&
                                                (array_tipi[k].MaxOpCamp < temp->index_camp)
                                          )
                                        { //non devo pagare setup major quindi calcolo il tempo necessario ad eseguirlo
                                                setup_vett[p] = 1;
                                                disponibilita[p] = time_max + Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1;
                                        }
                                        else
                                        {//in questo caso devo anche pagare un setup che puï¿½considerarsi come un ritardo dell'istante di inizio del processamento di un job.
                                        // non puï¿½capitare che il setup possa essere eseguito nel tempo morto slack
                                                setup_vett[p] = 2;
                                                disponibilita[p] = time_max + Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1;
                                        }
                                        break;
                                }
                        }
                }
                else
                {//in questo caso ho un tempo di slack che mi permette di eseguire almeno parzialmente il setup
                //il momento di disponibilitï¿½ ï¿½ pari al massimo tra time_mim + setup (eventuale) e time_max
                        for(k=0;k<num_tipi;k++)
                        {
                                if(array_tipi[k].ID==perm[i].tipo)
                                {
                                        if
                                        (
                                                (perm[i].tipo == temp->tipo)
                                                &&
                                                (array_tipi[k].MaxOpCamp < temp->index_camp)

                                        )
                                        { //non devo pagare setup quindi calcolo il tempo necessario ad eseguirlo
                                                setup_vett[p] = 1;

                                        }
                                        else
                                        {//in questo caso devo anche pagare un setup che puï¿½considerarsi come un ritardo dell'istante di inizio del processamento di un job.
                                                // non puï¿½capitare che il setup possa essere eseguito nel tempo morto slack
                                                setup_vett[p] = 2;

                                        }
                                        if	(
                                                                (time_min + Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1)
                                                                >
                                                                time_max
                                                        )
                                                {
                                                        disponibilita[p] = time_min + Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1;
                                                }
                                                else
                                                {
                                                        disponibilita[p] = time_max;
                                                }
                                        break;
                                }
                        }
                }
        }
        else if (temp1->inizio >= temp->fine)//nel tempo che avanza il job potrebbe entrarci
        {
                for(k=0;k<num_tipi;k++)
                {
                        if(array_tipi[k].ID==perm[i].tipo)
                        {
                                if (temp->ID_job==-3)
                                {// il primo job della schedula ï¿½ fittizio quindi non devo pagare nÃ¨ setup major nÃ¨ minor
                                        setup_vett[p] = 0;
                                        if (
                                                (temp1->inizio - time_max)
                                                >=
                                                perm[i].proc_time
                                           )//posso inserire il job
                                        {
                                                disponibilita[p] = time_max ;//posso schedulare il job
                                        }
                                        else
                                        {//in questo caso nn sono ancora sicuro della possibile data di inizio schedulazione
                                                while (
                                                        (temp1->next!=NULL)
                                                        &&
                                                        (
                                                                (
                                                                        (temp1->next->inizio - temp1->fine)
                                                                        <
                                                                        perm[i].proc_time
                                                                )
                                                                ||
                                                                (
                                                                        (temp1->next->inizio-time_max)
                                                                        <
                                                                        perm[i].proc_time
                                                                )
                                                        )
                                                      )
                                                {
                                                        temp1=temp1->next;
                                                }
                                                if(temp1->fine >= time_max)
                                                {
                                                        disponibilita[p] = temp1->fine;
                                                }
                                                else
                                                {
                                                        disponibilita[p] = time_max;
                                                }
                                        }
                                }
                                else
                                {
                                        if	(
                                                (perm[i].tipo == temp->tipo)
                                                &&
                                                (array_tipi[k].MaxOpCamp > temp->index_camp)
                                                )
                                        { //non devo pagare setup major quindi calcolo il tempo necessario ad eseguirlo
                                                setup_vett[p] = 1;
                                        }
                                        else
                                        {
                                                setup_vett[p] = 2;
                                        }

                                        if (
                                                (temp1->inizio - temp->fine-1)
                                                >=
                                                 (Cmaj_matrix[temp->tipo-1][perm[i].tipo-1])
                                           )//controllo se posso inserire il job mentre giï¿½ï¿½ so di poter inserire il setup
                                        {
                                                if(
                                                        (
                                                                (
                                                                        (temp->fine + Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1)
                                                                        >=
                                                                        perm[i].rel_time
                                                                )
                                                                &&
                                                                (
                                                                        temp1->inizio - (temp->fine + Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1)
                                                                        >=
                                                                        perm[i].proc_time
                                                                )
                                                        )

                                                  )
                                                {//posso schedulare entrambi
                                                        disponibilita[p] =max(temp->fine+Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]+1,perm[i].rel_time);
                                                }
                                                else//posso assegnare solo il setup mentre per il job devo attendere
                                                {
                                                        while (
                                                                (temp1->next!=NULL)
                                                                &&
                                                                (
                                                                        (
                                                                                (temp1->next->inizio - temp1->fine)
                                                                                <=
                                                                                perm[i].proc_time
                                                                        )
                                                                        ||
                                                                        (
                                                                                (temp1->next->inizio-time_max)
                                                                                <=
                                                                                perm[i].proc_time
                                                                        )
                                                                )
                                                              )
                                                        {
                                                                temp1=temp1->next;
                                                        }
                                                        if(temp1->fine>=time_max)
                                                        {
                                                                disponibilita[p] = temp1->fine;
                                                        }
                                                        else
                                                        {
                                                                disponibilita[p] = time_max;
                                                        }
                                                }
                                        }
                                        else //non posso schedulare per intero neppure il setup
                                        {//in questo caso nn sono ancora sicuro della possibile data di inizio Schedulazione
                                                slack1=temp1->inizio - temp->fine-1;
                                                slack1=Cmaj_matrix[temp->tipo-1][perm[i].tipo-1]-slack1;
                                                while (
                                                        (temp1->next!=NULL)
                                                        &&
                                                        (
                                                                (
                                                                        (temp1->next->inizio - temp1->fine)
                                                                        <=
                                                                        perm[i].proc_time+slack1
                                                                )
                                                                ||
                                                                (
                                                                        (temp1->next->inizio-time_max)
                                                                        <=
                                                                        perm[i].proc_time+slack1
                                                                )
                                                        )
                                                      )
                                                {
                                                        if (slack1>0)
                                                        {
                                                                slack1=slack1-(temp1->next->inizio - temp1->fine);
                                                        }
                                                        if (slack1<0) slack1=0;// finisco di schedulare il setup

                                                        temp1=temp1->next;
                                                }
                                                if(temp1->fine>=time_max-slack1)
                                                {
                                                        disponibilita[p] = temp1->fine+slack1;
                                                }
                                                else
                                                {
                                                        disponibilita[p] = time_max;
                                                }
                                        }


                                }
                                break;
                        }
                }
        }
}
//********************************************************************************************
//********************************************************************************************
job *permutazione_EDD_1_tipo ( job *array_job_attuale,int dim_job )
{//questa funzione costruirï¿½ï¿½?una nuova perm di job utilizzando la regola EDD in particolare
// verranno schedulati prima i job con deadline + vicina e poi quelli che hanno sono duedate
        int k = 0;
        int *vett_deadline;
        int *vett_duedate;
        int *vett_nothing;

        vett_deadline=new int[dim_job];
        vett_duedate=new int[dim_job];
        vett_nothing=new int[dim_job];

        int i = 0;
        int pos = 0;
        int value = 0;
        int fine;
        job *perm1;
        perm1 =new job[dim_job];
        for ( k = 0;k < dim_job; k++ )
        {//inizializzo
                vett_deadline[k] = 0 ;
                vett_duedate[k] = 0;
                vett_nothing[k] = 0;
        }
        for ( k = 0; k < dim_job; k++ )
        {
                if ( array_job_attuale[k].deadline > 0 )
                {
                        vett_deadline[k] = array_job_attuale[k].deadline;
                }
                if
                (
                    ( array_job_attuale[k].deadline == 0 )
                    &&
                    ( array_job_attuale[k].duedate > 0 )
                )
                {
                        vett_duedate[k] = array_job_attuale[k].duedate;
                }
                if
                (
                    ( array_job_attuale[k].deadline == 0 )
                    &&
                    ( array_job_attuale[k].duedate == 0 )
                )
                {
                        vett_nothing[k] = 1;
                }
        }
        fine = 0;
        while ( !fine )
        {
                for ( k = 0; k < dim_job; k++ )
                {
                        if ( vett_deadline[k] != 0 )
                        {
                                break;//	trovo la prima deadline
                        }
                        if ( k == dim_job-1 )
                        {//vuol dire che nn ci sono + job da schedulare di questo tipo
                                fine = 1;
                                break;
                        }
                }
                if ( fine )
                {
                        break;
                }
                pos = k;
                value = vett_deadline[k];
                for ( k = 0; k < dim_job; k++ )
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
                perm1[i] = array_job_attuale[pos];
                i++;
        }

        fine = 0;
        while ( !fine )
        {
                for ( k = 0; k < dim_job; k++ )
                {
                        if ( vett_duedate[k] != 0 )
                        {
                                break;//	trovo la prima duedatee
                        }
                        if ( k == dim_job-1 )
                        {//vuol dire che nn ci sono + job da schedulare di questo tipo
                                fine = 1;
                                break;
                        }
                }
                if ( fine )
                {
                        break;
                }
                pos = k;
                value = vett_duedate[k];
                for ( k = 0; k < dim_job; k++ )
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
                perm1[i] = array_job_attuale[pos];
                i++;
        }
        for ( k = 0; k < dim_job; k++ )
        {
                if ( vett_nothing[k] == 1 )
                {
                        perm1[i] = array_job_attuale[k];
                        i++;
                }
        }
        free ( vett_deadline );
        free ( vett_duedate );
        free ( vett_nothing );
        return perm1;
}
job *permutazione_EDD_2_tipo ( job *array_job_attuale,int dim_job )
{//questa funzione costruirï¿½ï¿½?una nuova perm di job utilizzando la regola EDD in particolare
// verranno schedulati prima i job con deadline o duedate + vicina
        int k = 0;
        int *vett_max_dd_dl;
        vett_max_dd_dl= new int[dim_job];
        int i = 0;
        int fine=0;
        job *perm1;
        perm1 =new job[dim_job];
        for ( k = 0;k < dim_job; k++ )
        {//inizializzo
                vett_max_dd_dl[k] = 0 ;
        }
        for ( k=0;k < dim_job; k++ )
        {
                if ( array_job_attuale[k].duedate!=0 || array_job_attuale[k].deadline !=0 )
                {
                        if (
                            (
                                ( array_job_attuale[k].deadline !=0 )
                                &&
                                ( array_job_attuale[k].duedate !=0 )
                            )
                            ||
                            ( array_job_attuale[k].deadline == 0 )

                        )
                        {
                                vett_max_dd_dl[k] = array_job_attuale[k].duedate;
                        }
                        else
                        {
                                vett_max_dd_dl[k] = array_job_attuale[k].deadline;
                        }
                }
        }
        i=0;
        while ( !fine )
        {
                int cambio = 0 ;
                for ( k=0;k<dim_job;k++ )
                {
                        if ( ( vett_max_dd_dl[k]!=0 ) && ( vett_max_dd_dl[k]!=-1 ) )
                        {
                                break;
                        }
                        if ( k == ( dim_job-1 ) )
                        {
                                fine=1;
                        }
                }
                if ( fine )
                {
                        break;
                }
                cambio = k;
                for ( k=1;k<dim_job;k++ ) //trovo il minimo
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
                perm1[i] = array_job_attuale[cambio];
                i++;
                vett_max_dd_dl[cambio] = -1;
        }
        fine = 0;
        while ( !fine )
        {
                for ( k=0;k<dim_job;k++ )
                {

                        if ( vett_max_dd_dl[k]==0 )
                        {
                                break;
                        }
                        if ( k==dim_job-1 )
                        {
                                fine=1;
                        }
                }
                if ( fine )
                {
                        break;
                }
                perm1[i] = array_job_attuale[k];
                i++;
                vett_max_dd_dl[k] = -1;
        }
        free ( vett_max_dd_dl );
        return perm1;
}
//*************************************************************************************************
//la funzione seguente calcola il la lateness massima il makespan ed il numero di tardyjob
//***********************************************************************************************
void  valuta_schedula ( schedula *M1_sch_locale,schedula *M2_sch_locale,schedula *M3_sch_locale,next_elem *prossimo )
{
        schedula *temp, *temp1;
        int *Cmax_temp;
        Cmax_temp=new int[num_macchine];
        int lat =  -65000;
        int tard = 0;
        int feasible = 1;
        int k=0;
        temp=M1_sch_locale;
        while ( temp!=NULL )
        {
                for ( k=0;k<num_job;k++ )
                {
                        if ( temp->ID_job==array_job[k].ID )
                        {
                                break;
                        }
                }
                if
                (
                    ( array_job[k].duedate!=0 )
                    &&
                    (
                        ( temp->fine - array_job[k].duedate )
                        >
                        lat
                    )
                )
                {
                        lat = ( temp->fine - array_job[k].duedate );
                }
                if
                (
                    ( array_job[k].duedate!= 0 )
                    &&
                    ( ( temp->fine - array_job[k].duedate ) > 0 )
                )
                {
                        tard++;
                }
                if
                (
                    ( array_job[k].deadline != 0 )
                    &&
                    ( ( temp->fine - array_job[k].deadline ) > 0 )
                )
                {
                        feasible = 0;
                }
                temp1=temp;
                temp=temp->next;
        }

        Cmax_temp[0]=temp1->fine;
        if ( num_macchine >= 2 )
        {
                temp=M2_sch_locale;
                if ( temp->ID_job!=-3 )
                {
                        while ( temp!=NULL )
                        {

                                for ( k=0;k<num_job;k++ )
                                {
                                        if ( temp->ID_job==array_job[k].ID )
                                        {
                                                break;
                                        }
                                }
                                if
                                (
                                    ( array_job[k].duedate!=0 )
                                    &&
                                    (
                                        ( temp->fine - array_job[k].duedate )
                                        >
                                        lat
                                    )
                                )
                                {
                                        lat = ( temp->fine - array_job[k].duedate );
                                }
                                if
                                (
                                    ( array_job[k].duedate!= 0 )
                                    &&
                                    ( ( temp->fine - array_job[k].duedate ) > 0 )
                                )
                                {
                                        tard++;
                                }
                                if
                                (
                                    ( array_job[k].deadline != 0 )
                                    &&
                                    ( ( temp->fine - array_job[k].deadline ) > 0 )
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
        if ( num_macchine == 3 )
        {
                temp=M3_sch_locale;
                if ( temp->ID_job!=-3 )
                {
                        while ( temp!=NULL )
                        {
                                for ( k=0;k<num_job;k++ )
                                {
                                        if ( temp->ID_job==array_job[k].ID )
                                        {
                                                break;
                                        }
                                }
                                if
                                (
                                    ( array_job[k].duedate!=0 )
                                    &&
                                    (
                                        ( temp->fine - array_job[k].duedate )
                                        >
                                        lat
                                    )
                                )
                                {
                                        lat = ( temp->fine - array_job[k].duedate );
                                }
                                if
                                (
                                    ( array_job[k].duedate!= 0 )
                                    &&
                                    ( ( temp->fine - array_job[k].duedate ) > 0 )
                                )
                                {
                                        tard++;
                                }
                                if
                                (
                                    ( array_job[k].deadline != 0 )
                                    &&
                                    ( ( temp->fine - array_job[k].deadline ) > 0 )
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
        for ( k=1;k<num_macchine;k++ )
        {
                if ( Cmax_temp[cambio] < Cmax_temp[k] )
                {
                        cambio = k;
                }
        }
        Cmax =Cmax_temp[cambio];
//_____________________________________________________________________ fine calcolo Cmax
        if ( swap_lat_tard==0 )
        {
                Lmax = lat; //latenza massima
        }
        else
        {
                Lmax=max ( lat,0 );//in questo caso la Lmax indica la Tardiness massima
        }
        Tardy = tard;
        Feasible = feasible;
        //____________________________________________________ salvo le informazioni in prossimo

        prossimo->Lmax=Lmax;
        prossimo->Cmax=Cmax;
        prossimo->Tardy=Tardy;
        if ( feasible == 1 )
        {
                prossimo->feasible = 1;
        }
        else
        {
                prossimo->feasible = 0;
        }
        free ( Cmax_temp );
}
void  azzera_schedule ( void )
{
// 	struct sch //questa struttura conterrï¿½ï¿½?le schedulazioni per ogni macchina
// 	{
// 	int ID_job;
// 	int tipo;
// 	int inizio;
// 	int fine;
// 	int index_camp;
// 	struct sch *next;
// 	};
// typedef struct sch schedula;
        schedula *temp;
        schedula *temp1;
        int fine = 0;
        if ( M1_sch == NULL )
        {
                M1_sch = new schedula;
                {
                        M1_sch->ID_job = -3;
                        M1_sch->tipo = 0;
                        M1_sch->inizio = 0;
                        M1_sch->fine = 0;
                        M1_sch->Lmax = 0;
                        M1_sch->Tardy = 0;
                        M1_sch->index_camp = 0;
                        M1_sch->next = NULL;
                }
        }
        else
        {
                while ( !fine )
                {
                        temp = M1_sch;
                        if ( temp==NULL )
                        {
                                fine = 1;
                                break;
                        }
                        if ( temp->next==NULL )
                        {
                                M1_sch = NULL	;
                        }
                        while ( temp->next!=NULL )
                        {
                                temp1=temp;
                                temp=temp->next;
                        }
                        free ( temp );
                        if ( M1_sch!=NULL )
                        {
                                temp1->next = NULL;
                        }
                }
                M1_sch = new schedula;
                {
                        M1_sch->ID_job = -3;
                        M1_sch->tipo = 0;
                        M1_sch->inizio = 0;
                        M1_sch->fine = 0;
                        M1_sch->Lmax = 0;
                        M1_sch->Tardy = 0;
                        M1_sch->index_camp = 0;
                        M1_sch->next = NULL;
                }
        }
        if ( num_macchine >= 2 )
        {
                if ( M2_sch == NULL )
                {
                        M2_sch = new schedula;
                        {
                                M2_sch->ID_job = -3;
                                M2_sch->tipo = 0;
                                M2_sch->inizio = 0;
                                M2_sch->fine = 0;
                                M2_sch->Lmax = 0;
                                M2_sch->Tardy = 0;
                                M2_sch->index_camp = 0;
                                M2_sch->next = NULL;
                        }
                }
                else
                {
                        fine = 0;
                        while ( !fine )
                        {
                                temp = M2_sch;
                                if ( temp==NULL )
                                {
                                        fine = 1;
                                        break;
                                }
                                if ( temp->next==NULL )
                                {
                                        M2_sch = NULL;
                                }
                                while ( temp->next!=NULL )
                                {
                                        temp1=temp;
                                        temp=temp->next;
                                }
                                free ( temp );
                                if ( M2_sch != NULL )
                                {
                                        temp1->next = NULL;
                                }
                        }
                        M2_sch = new schedula;
                        {
                                M2_sch->ID_job = -3;
                                M2_sch->tipo = 0;
                                M2_sch->inizio = 0;
                                M2_sch->fine = 0;
                                M2_sch->Lmax = 0;
                                M2_sch->Tardy = 0;
                                M2_sch->index_camp = 0;
                                M2_sch->next = NULL;
                        }
                }
        }
        if ( num_macchine == 3 )
        {
                if ( M3_sch == NULL )
                {
                        M3_sch = new schedula;
                        {
                                M3_sch->ID_job = -3;
                                M3_sch->tipo = 0;
                                M3_sch->inizio = 0;
                                M3_sch->fine = 0;
                                M3_sch->Lmax = 0;
                                M3_sch->Tardy = 0;
                                M3_sch->index_camp = 0;
                                M3_sch->next = NULL;
                        }
                }
                else
                {
                        fine = 0;
                        while ( !fine )
                        {
                                temp = M3_sch;
                                if ( temp==NULL )
                                {
                                        fine = 1;
                                        break;
                                }
                                if ( temp->next==NULL )
                                {
                                        M3_sch = NULL;
                                }
                                while ( temp->next!=NULL )
                                {
                                        temp1=temp;
                                        temp=temp->next;
                                }
                                free ( temp );
                                if ( M3_sch != NULL )
                                {
                                        temp1->next = NULL;
                                }
                        }
                        M3_sch = new schedula;
                        {
                                M3_sch->ID_job = -3;
                                M3_sch->tipo = 0;
                                M3_sch->inizio = 0;
                                M3_sch->fine = 0;
                                M3_sch->Lmax = 0;
                                M3_sch->Tardy = 0;
                                M3_sch->index_camp = 0;
                                M3_sch->next = NULL;
                        }
                }
        }
}


//*************************************************************************************************
//			ROLLOUT Main function
//***********************************************************************************************
job *QRollout::rollout ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

        next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
     //   int num_job_relativo = num_job;
     //   int j = 0;
        job **permutazioni;
     //   job *job_da_inserire;
     //   int primo_passo_M1 = 0;
     //   int primo_passo_M2 = 0;
     //   int primo_passo_M3 = 0;
     //   FILE *f_log;
     //   char nome_file[25];
        job *array_job_attuale;
        job *array_job_attuale_temp;
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
    //    schedula *M1_sch_buffer1;
    //    schedula *M2_sch_buffer1;
    //    schedula *M3_sch_buffer1;
    //    schedula **vettore_schedule_rollout;
    //    schedula *punt_vett_roll;

    //    next_elem *lista_prossimi_vincitori; // questo vettore conterra' la lista dei vincitori dei singoli turni di rollout

    //    schedula *temp1;
    //    schedula *temp2;
        int fine = 0;
        schedula *M1_sch_attuale;
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
        schedula *M3_sch_attuale;
        job * perm_di_passaggio;
        next_elem *prossimo2;
        next_elem *prossimo = NULL;
        next_elem *prossimo1;
        next_elem *temp;
        next_elem *temp_prox;
        int ID_job ;
        int macchina;
        //int fine;
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
        //next_elem *temp;
        // FINE DICHIARAZIONE DELLE VARIABILI
        //___________________________________________________________________________________________________________

        permutazioni= new job*[num_heur];
        M1_sch_attuale = M1_sch;
        permutazione_buffer=new job[num_job];
        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        if ( num_macchine == 3 )
        {

                M3_sch_attuale = M3_sch;
        }

        // in array job attuale metto una copia di array job
        array_job_attuale = new job[num_job];
        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }

        job_fisso = ( next_elem * ) new next_elem;

        while ( cont_livelli>0 )
        {
                next_elem lista_prossimi_vincitori[500]; // questa la dovrei spostare
                //____________________________________________________________________!!!!!!!!!!

                index=0;
                index1=0;
                /*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/
                for ( iter_for=0;iter_for<num_job;iter_for++ )
                {
                        /* di volta in volta ridurro il numero di num_job_relativo
                        devo considerare il caso di tutti i job scedulati per primi  */
                        if ( array_job_attuale[iter_for].ID!=-1 ) //se e' selezionabile
                        {
                                job_fisso->ID_job = array_job_attuale[iter_for].ID; //job base da cui faccio partire le euristiche
                                array_job_attuale[iter_for].ID =-1;// non + selezionabile
                                array_job_attuale_temp=new job[cont_livelli];
                                iter=0;
                                for ( kk=0;kk<cont_livelli;kk++ )
                                {
                                        while ( array_job_attuale[iter].ID==-1 )
                                        {
                                                iter++;
                                        }
                                        array_job_attuale_temp[kk]=array_job_attuale[iter];
                                        iter++;
                                }

                                iter=0;
                                prossimo = NULL;
// 				next_elem *prossimo1;
                                for ( i = 0;i<num_heur;i++ ) //num_heur dovra' essere cambiato con il numero di heuristiche effettivamente usato
                                {
                                        azzera_schedule();// azzero le schedule
                                        //___________________________________________________________________________________
                                        // calcolo i riempimenti parziali delle schedule
                                        perm_di_passaggio=new job[num_job];
                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                        {
                                                perm_di_passaggio[pp]=best_perm[pp];
                                        }
                                        pp++;
                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];
// 					next_elem *prossimo2;
                                        prossimo2= new next_elem;
                                        prossimo2->next=NULL;
                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job-cont_livelli );
                                        free ( prossimo2 );
                                        //___________________________________________________________________________________

                                        permutazioni[i]=NULL;
                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                        if ( prossimo == NULL )
                                        {
                                                prossimo = new next_elem;
                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                prossimo->next = NULL;
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
                                                temp_prox=new next_elem;
                                                temp->next = temp_prox;
                                                temp = temp->next;
                                                temp->ID_job = permutazioni[i][0].ID;
                                                temp->ID_heur= funzioni[i].ID_heur;
                                                temp->next = NULL;

                                        }
                                        // calcolo il valore delle schedule con la permutazione data dall'euristica i-esima
                                        for ( jj=0;pp<num_job;pp++,jj++ ) // completo la permutazione di passaggio
                                        {
                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                        }
                                        azzera_schedule();
                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );
                                        free ( perm_di_passaggio );
                                        //devo riportare la macchina nelle condizioni orginarie
// 					stampa_risultati_a_video(0);
                                        if ( force == 1 )
                                        {
                                                prossimo1 = new next_elem;
                                                M1_sch_buffer=new schedula;
                                                M2_sch_buffer=new schedula;
                                                M3_sch_buffer=new schedula;
                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                if ( num_macchine>=2 )
                                                {
                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                }
                                                if ( num_macchine==3 )
                                                {
                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                }
                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                elimina_schedula ( M1_sch_buffer );
                                                if ( num_macchine>=2 )
                                                {
                                                        elimina_schedula ( M2_sch_buffer );
                                                }
                                                if ( num_macchine==3 )
                                                {

                                                        elimina_schedula ( M3_sch_buffer );
                                                }
                                                next_elem *tmp_prox;
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
                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                        tmp_prox->feasible=prossimo1->feasible;
                                                }

                                                free ( prossimo1 );
                                        }
                                }
                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                //devo ridurre il numero di job che rimangono da schedulare
                                //devo trovare il job con la Lateness + alta
                                // 		in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                // 		infine con il numero + basso di Tardy job
                                array_job_attuale[iter_for].ID =iter_for;
                                free ( array_job_attuale_temp );
                                ID_job = prossimo->ID_job;
                                macchina = prossimo->macchina;
                                fine = prossimo->fine;
                                inizio = prossimo->inizio;
                                tipo = prossimo->tipo;
                                index_camp = prossimo->index_camp;
                                LMAX = prossimo->Lmax_pers;
                                TARDY = prossimo->Tardy_pers;
                                L_max = prossimo->Lmax;
                                C_max = prossimo->Cmax;
                                tardy = prossimo->Tardy;
                                ID_heur = prossimo->ID_heur;
                                rel_time = prossimo->rel_time;
                                proc_time = prossimo->proc_time;
                                duedate = prossimo->duedate;
                                deadline = prossimo->deadline;
                                priority = prossimo->priority;
                                Feasible = prossimo->feasible;
                                temp = prossimo ;

                                while ( temp->next!=NULL )
                                {
                                        if ( Feasible <= temp->next->feasible )
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
                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                // 		e su quale macchina ï¿½ï¿½?stato schedulato
                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                lista_prossimi_vincitori[index].feasible = Feasible;
                                index++;
                                // fine salvataggio ___________________________________________________________


                                next_elem *temp_pr;
                                for ( i=0;i<num_heur;i++ )
                                {
                                        temp_pr = prossimo->next;
                                        free ( prossimo );
                                        prossimo = temp_pr;
                                }


                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                for ( i=0;i<num_heur;i++ )
                                {
                                        free ( permutazioni[i] );
                                }
                        }
// 		verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );
//_____________________________________________________________________________________________________________________
                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,cont_livelli );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
// 		printf("\nla posizione_vincitore ï¿½ï¿½?: %i Pos_assoluta ï¿½ï¿½?: %i",Pos_vincitore,Pos_assoluta);
// 		printf("\n l'id del job successore a quello da inserire ï¿½ï¿½?:%i \n",lista_prossimi_vincitori[Pos_vincitore].ID_job);
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        // 		printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale[Pos_assoluta].ID=-1;
//          fprintf(f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy);
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
// 			printf("\a\nhelp!\n");

//         fprintf(f_log,"%i %i %i %i \n",Feasible_best,Lmax_best,Cmax_best,Tardy_best);
                }
// 		printf("\n P_vincitore %i job aggiunto= %i \n",Pos_vincitore,best_perm[num_job-cont_livelli-1].ID);
// 		free(lista_prossimi_vincitori);
// 		printf("\a\n___!\n");
// 		stampa_permutazioni( permutazione_buffer,num_job);
// 		printf("\a\n___!\n");
                cont_livelli--;
        }
//    fclose(f_log);
// 	stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale[i].ID;
// 	printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
        array_job_attuale[i].ID=-1;
        free ( job_fisso );
        free ( array_job_attuale );
        azzera_schedule();
// 	next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
// 		printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return	best_perm;
        }
}
void QRollout::stampa_risultati_a_video ( int flag )
{
        schedula *temp;
        elem *temp1;
        temp1 = M1;
        temp = M1_sch;
        int i;
        if ( flag !=2 )
        {
                printf ( "\n\tMacchina 1:\n\t" );
                printf ( "Indisponibilitï¿½ï¿½?\n\t" );
                while ( temp1->next!=NULL )
                {
                        printf ( "|%i-%i|__",temp1->inizio,temp1->fine );
                        temp1 = temp1->next;
                }
                printf ( "|%i-%i|\n\t",temp1->inizio,temp1->fine );
                printf ( "------------------------------------------------------------------" );
                printf ( "\n\tSchedula:\n\t" );
                while ( temp->next!=NULL )
                {
                        printf ( "|%i-%i|__",temp->inizio,temp->fine );
                        temp = temp->next;
                }
                printf ( "|%i-%i|\n\t",temp->inizio,temp->fine );
                temp=M1_sch;
                printf ( "\n\tPos\tJob\tTipo\tStart\tEnd\tCamp\tLat.\tTardy" );
                i=0;
                while ( temp->next!=NULL )
                {
                        printf ( "\n\t|%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i|",i++,temp->ID_job,temp->tipo,temp->inizio,
                                 temp->fine,temp->index_camp,temp->Lmax,temp->Tardy );
                        temp = temp->next;
                }
                printf ( "\n\t|%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i|\n\t",i,temp->ID_job,temp->tipo,temp->inizio,
                         temp->fine,temp->index_camp,temp->Lmax,temp->Tardy );
                printf ( "__________________________________________________________________\n\n" );
                if ( num_macchine>=2 )
                {
                        temp = M2_sch;
                        temp1 = M2;
                        printf ( "\n\tMacchina 2:\n\t" );
                        printf ( "Indisponibilitï¿½ï¿½?\n\t" );
                        while ( temp1->next!=NULL )
                        {
                                printf ( "|%i-%i|__",temp1->inizio,temp1->fine );
                                temp1 = temp1->next;
                        }
                        printf ( "|%i-%i|\n\t",temp1->inizio,temp1->fine );
                        printf ( "------------------------------------------------------------------" );
                        printf ( "\n\tSchedula:\n\t" );
                        while ( temp->next!=NULL )
                        {
                                printf ( "|%i-%i|__",temp->inizio,temp->fine );
                                temp = temp->next;
                        }
                        printf ( "|%i-%i|\n\t",temp->inizio,temp->fine );
                        temp=M2_sch;
                        printf ( "\n\tPos\tJob\tTipo\tStart\tEnd\tCamp\tLat.\tTardy" );
                        i=0;
                        while ( temp->next!=NULL )
                        {
                                printf ( "\n\t|%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i|",i++,temp->ID_job,temp->tipo,temp->inizio,
                                         temp->fine,temp->index_camp,temp->Lmax,temp->Tardy );
                                temp = temp->next;
                        }
                        printf ( "\n\t|%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i|\n\t",i,temp->ID_job,temp->tipo,temp->inizio,
                                 temp->fine,temp->index_camp,temp->Lmax,temp->Tardy );
                        printf ( "__________________________________________________________________\n\n" );
                }
                if ( num_macchine==3 )
                {
                        temp = M3_sch;
                        temp1 = M3;
                        printf ( "\n\tMacchina 3:\n\t" );
                        printf ( "Indisponibilitï¿½ï¿½?\n\t" );
                        while ( temp1->next!=NULL )
                        {
                                printf ( "|%i-%i|__",temp1->inizio,temp1->fine );
                                temp1 = temp1->next;
                        }
                        printf ( "|%i-%i|\n\t",temp1->inizio,temp1->fine );
                        printf ( "------------------------------------------------------------------" );
                        printf ( "\n\tSchedula:\n\t" );
                        while ( temp->next!=NULL )
                        {
                                printf ( "|%i-%i|__",temp->inizio,temp->fine );
                                temp = temp->next;
                        }
                        printf ( "|%i-%i|\n\t",temp->inizio,temp->fine );
                        temp=M3_sch;
                        printf ( "\n\tPos\tJob\tTipo\tStart\tEnd\tCamp\tLat.\tTardy" );
                        i=0;
                        while ( temp->next!=NULL )
                        {
                                printf ( "\n\t|%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i|",i++,temp->ID_job,temp->tipo,temp->inizio,
                                         temp->fine,temp->index_camp,temp->Lmax,temp->Tardy );
                                temp = temp->next;
                        }
                        printf ( "\n\t|%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i|\n\t",i,temp->ID_job,temp->tipo,temp->inizio,
                                 temp->fine,temp->index_camp,temp->Lmax,temp->Tardy );
                        printf ( "__________________________________________________________________\n\n" );
                }
        }
        printf ( "\tLmax: %i\tCmax: %i\tNum. Tardy Jobs: %i\tFeas.: %i\n",Lmax,Cmax,Tardy,Feasible );

// 	printf("\ttempo inizio : %f\n",(double)tempo_inizio/CLOCKS_PER_SEC);
// 	printf("\ttempo fine : %f\n",(double)tempo_fine/CLOCKS_PER_SEC);
        if ( flag ==0 )
        {
                printf ( "\tTempo impiegato Rollout : %.3f\n", ( double ) ( tempo_fine1-tempo_inizio1 ) );
        }
        else if ( flag == 1 )
        {
                printf ( "\tTempo impiegato VNS : %.3f\n", ( double ) ( tempo_fine2-tempo_inizio2 ) /CLOCKS_PER_SEC );
                printf ( "\tTempo Totale (Rollout + VNS): %.3f\n", ( double ) ( tempo_fine1-tempo_inizio1 ) + ( double ) ( tempo_fine2-tempo_inizio2 ) /CLOCKS_PER_SEC );
        }
        else
        {
                printf ( "\tTempo impiegato Bilanc. : %.3f\n", ( double ) ( tempo_fine3-tempo_inizio3 ) );
// 		printf("\tTempo impiegato Bilanc. : %.3f\n",(double)(tempo_fine3-tempo_inizio3)/CLOCKS_PER_SEC);
// 		printf("\tTempo Totale (Rollout + VNS + Bilanc.): %.3f\n",(double)(tempo_fine3-tempo_inizio1)/CLOCKS_PER_SEC);
                printf ( "\tTempo Totale (Rollout + VNS + Bilanc.): %.3f\n", ( double ) ( tempo_fine1-tempo_inizio1 ) + ( double ) ( tempo_fine3-tempo_inizio2 ) /CLOCKS_PER_SEC );
        }
}
void QRollout::stampa_risultati_su_file ( FILE *f_out, char *instance_file,int flag,int force )
{

        if ( flag ==0 )
        {
                fprintf ( f_out,"%s\t",instance_file );
                fprintf ( f_out,"%i\n\t",Feasible );
                if (rollout_or_heuristic==1)//euristica
                        fprintf ( f_out,"%.3f\t", ( double ) ( tempo_fine1-tempo_inizio1 )/CLOCKS_PER_SEC );
                else
                        fprintf ( f_out,"%.3f\t", ( double ) ( tempo_secondi_fine1-tempo_secondi_inizio1 ) );
        }
        else if ( flag == 1 )
        {
                fprintf ( f_out,"%.3f\t", ( double ) ( tempo_fine2-tempo_inizio2 ) /CLOCKS_PER_SEC );

                if (rollout_or_heuristic==1)//euristica
                        fprintf ( f_out,"%.3f\t", ( double )( ( tempo_fine1-tempo_inizio1 ) + ( tempo_fine2-tempo_inizio2 )) /CLOCKS_PER_SEC );
                else
                        fprintf ( f_out,"%.3f\t", ( double )( tempo_secondi_fine1-tempo_secondi_inizio1 ) + (double)(( tempo_fine2-tempo_inizio2 )) /CLOCKS_PER_SEC );


        }
        else
        {
                fprintf ( f_out,"%.3f\t", ( double ) ( tempo_fine3-tempo_inizio3 ) );
                if (rollout_or_heuristic==1)//euristica
                        fprintf ( f_out,"%.3f\t", ( double )( ( tempo_fine1-tempo_inizio1 ) + ( tempo_fine3-tempo_inizio2 )) /CLOCKS_PER_SEC );
                else
                        fprintf ( f_out,"%.3f\t", ( double )( tempo_secondi_fine1-tempo_secondi_inizio1 ) + (double)(( tempo_fine3-tempo_inizio2 )) /CLOCKS_PER_SEC );



        }
        fprintf ( f_out,"%i\t",num_macchine );
        fprintf ( f_out,"%i\t",Lmax );
        fprintf ( f_out,"%i\t",Cmax );
        fprintf ( f_out,"%i\t",Tardy );
        if ( flag==0 )
        {
                fprintf ( f_out,"%i\t",Feasible );
                fprintf ( f_out,"force = %i\t|\n",force );
        }
        else
        {
                fprintf ( f_out,"%i\t\t\t|\n",Feasible );
        }
        if ( flag== 2 )
        {
                fprintf ( f_out,"\n" );
        }
}
int QRollout::min ( int a, int b )
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
int QRollout::max ( int a, int b )
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
void copia_schelule ( schedula *M_sch,schedula *M_sch_locale )
{
        /*int ID_job;
                int tipo;
                int inizio;
                int fine;
                int Lmax;
                int Tardy;
                int index_camp;
                struct sch *next;*/

        schedula *temp = M_sch_locale;
        while ( M_sch->next != NULL )
        {
                temp->ID_job = M_sch->ID_job;
                temp->tipo = M_sch->tipo;
                temp->inizio = M_sch->inizio;
                temp->fine = M_sch->fine;
                temp->Lmax = M_sch->Lmax;
                temp->Tardy = M_sch->Tardy;
                temp->index_camp = M_sch->index_camp;

                M_sch = M_sch->next;
                temp->next = new schedula;
                temp = temp->next;
        }
        temp->ID_job = M_sch->ID_job;
        temp->tipo = M_sch->tipo;
        temp->inizio = M_sch->inizio;
        temp->fine = M_sch->fine;
        temp->Lmax = M_sch->Lmax;
        temp->Tardy = M_sch->Tardy;
        temp->index_camp = M_sch->index_camp;//finisco di copiare l'ultimo elemento
        temp->next = NULL;

}
//***************************************************************************************************************************
// lo scopo di questa funzione ï¿½ï¿½?quello di eseguire uno spostamento di un job della macchina X da una posizione ad un'altra.
//
// **************************************************************************************************************************
schedula *mossa ( schedula *M_sch, elem *M, int pos_iniziale, int pos_finale )
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
        schedula *schedula_di_lavoro;// ovviamente nn lavoro sulla schedula ma su una sua copia.
        schedula *temp1,*temp2;
        job *job_temp;

        job_temp = new job;

        if ( pos_finale<pos_iniziale )
        {
                if ( pos_finale>0 ) // copio nella schedula di lavoro gli elementi che restano inalterati
                {
                        schedula_di_lavoro = new schedula;
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
                        for ( i=1;i<pos_finale;i++ )
                        {
                                temp2->next = new schedula;
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
                        schedula_di_lavoro=new schedula;
                        temp2 = schedula_di_lavoro;
                        temp2->ID_job = -3;
                        temp2->tipo = 0;
                        temp2->inizio = 0;
                        temp2->fine = 0;
                        temp2->Lmax = 0;
                        temp2->Tardy = 0;
                        temp2->index_camp = 0;
                        temp2->next = NULL;
                }
//_____________________________________________________________________________________________________________________
// ora devo considerare il job nella posizione iniziale e spostarlo in quella finale.
// devo tener conto della possibilitï¿½ï¿½?di incorrere in setup o inisponibilitï¿½ï¿½?
// uso la funzione aggiungi_schedula
                temp1 = M_sch;
                for ( i=0;i<pos_iniziale;i++ )
                {
                        temp1 = temp1->next;
                }
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
                while ( i<num_job )
                {
                        if ( temp1->ID_job == array_job[i].ID )
                        {
                                job_temp[0] = array_job[i];
                                break;
                        }
                        i++;
                }
                verifica_macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
                aggiungi_schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
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
                                while ( i<num_job )
                                {
                                        if ( temp1->ID_job == array_job[i].ID )
                                        {
                                                job_temp[0] = array_job[i];
                                                break;
                                        }
                                        i++;
                                }
                                verifica_macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
                                aggiungi_schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
                                tipo_predecessore = tipo;// il predecessore ï¿½ï¿½?ora il job appena schedulato
                        }
                        temp1 = temp1->next;
                }
        }
        else
        {
                if ( pos_iniziale>0 ) // copio nella schedula di lavoro gli elementi che restano inalterati
                {
                        schedula_di_lavoro = new schedula;
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
                                temp2->next = new schedula;
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
                        schedula_di_lavoro = new schedula;
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
                int disp =0;
                disponibilita =&disp;
                if ( pos_iniziale==0 )
                {
                        tipo_predecessore = 0;//nn ho predecessori
                }
                else
                {
                        tipo_predecessore = temp1->tipo;
                }
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
                        {
                                setup_vett[0] = 0;
                        }
                        else
                        {
                                setup_vett[0] = 1;
                        }
                        int jj=0;
                        while ( jj<num_job )
                        {
                                if ( temp1->ID_job == array_job[jj].ID )
                                {
                                        job_temp[0] = array_job[jj];
                                        break;
                                }
                                jj++;
                        }
                        verifica_macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
                        aggiungi_schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
                        tipo_predecessore = tipo;// il predecessore ï¿½ï¿½?ora il job appena schedulato
                        temp1 = temp1->next;
                }
                i=0;
                while ( i<num_job )
                {
                        if ( ID_job_da_spostare == array_job[i].ID )
                        {
                                job_temp[0] = array_job[i];
                                break;
                        }
                        i++;
                }
                verifica_macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
                aggiungi_schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
                tipo_predecessore = tipo_job_da_spostare;// il predecessore ï¿½ï¿½?ora il job appena schedulato
                while ( temp1!=NULL ) //aggiungo tutti gli altri
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
                        while ( i<num_job )
                        {
                                if ( temp1->ID_job == array_job[i].ID )
                                {
                                        job_temp[0] = array_job[i];
                                        break;
                                }
                                i++;
                        }
                        verifica_macchina ( schedula_di_lavoro,M,disponibilita,setup_vett,0,job_temp,0 );
                        aggiungi_schedula ( schedula_di_lavoro,job_temp[0],disponibilita[0],setup_vett[0] );
                        tipo_predecessore = tipo;// il predecessore ï¿½ï¿½?ora il job appena schedulato
                        temp1 = temp1->next;
                }

        }
        free ( job_temp );
        return schedula_di_lavoro;
}
//VNS euristica di esplorazione di vicinati multipli
// restituisce 1 se sono stati trovati miglioramenti 0 atrimenti.
int VNS ( schedula *M1_sch_buffer,schedula *M2_sch_buffer,schedula *M3_sch_buffer )
{
        int ris = 0;
        ris += VNS_per_macchina ( M1_sch_buffer,M1 );
        if ( num_macchine >= 2 )
        {
                ris += VNS_per_macchina ( M2_sch_buffer,M2 );
        }
        if ( num_macchine==3 )
        {
                ris += VNS_per_macchina ( M3_sch_buffer,M3 );
        }
        return ris;
}
//VNS_per_macchina implementa lo schema VNS per la singola macchina considerata.
int VNS_per_macchina ( schedula *M_sch,elem *M )
{
// 	per prima cosa devo trovare i job con lateness positiva decrescente
// 	ed ordinarli in modo decrescente (dal + ritardatario al meno ritardatario)
// struct sch //questa struttura conterrï¿½ï¿½?le schedulazioni per ogni macchina
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
        schedula *temp;
        schedula *temp1,*temp2;
        schedula *schedula_di_lavoro;
        quaterna *quaterna_di_lavoro;
        quaterna *quaterna_migliore;
        int *job_vett;
        job_vett=new int[num_job];
        int i,j;
     //   int ris = 0;//risultato
        int Lat_max,pos,cont = 0;
        for ( i=0;i<num_job;i++ )
        {
                job_vett[i]=0;
        }
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
        quaterna_migliore = valuta_singola_schedula ( M_sch );
        for ( j=0;j<cont;j++ )
        {
                Lat_max = 0;
                pos = -1;
                for ( i=0;i<num_job;i++ )
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
                        for ( i=pos+1;i<num_job;i++ )
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
                                schedula_di_lavoro = mossa ( M_sch,M,pos,pos-1 );
// 				stampa_sequenza_macchina( M_sch);
// 				stampa_sequenza_macchina( schedula_di_lavoro);
                                quaterna_di_lavoro = valuta_singola_schedula ( schedula_di_lavoro );
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
// 						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
// 						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
// 						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
// 						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;

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
// 							free(temp1);//cancello la struttura creata
                                                        temp1 = temp2;
                                                }
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_migliore );
                                                free ( quaterna_di_lavoro );
                                                free ( job_vett );
                                                VNS_per_macchina ( M_sch,M );
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
// 						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
// 						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
// 						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
// 						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
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
// 							free(temp1);//cancello la struttura creata
                                                        temp1 = temp2;
                                                }
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_migliore );
                                                free ( quaterna_di_lavoro );
                                                free ( job_vett );
                                                VNS_per_macchina ( M_sch,M );
                                                //applico ricorsivamente l'algoritmo
                                                return 1;
                                        }
                                        else
                                        {
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_di_lavoro );
                                        }

                                }
                                else
                                {//devo distruggere la shedula di lavoro
                                        elimina_schedula ( schedula_di_lavoro );
                                        free ( quaterna_di_lavoro );
                                }
                        }
                }

        }
        for ( i=0;i<num_job;i++ )
        {
                job_vett[i]=0;
        }
        int k=0;
        cont = 0;
        temp = M_sch;
        while ( temp!= NULL )
        {
                for ( i=0;i<num_job;i++ )
                {
                        if ( temp->ID_job== array_job[i].ID )
                        {
                                if
                                (
                                    ( array_job[i].deadline!=0 )
                                    &&
                                    (
                                        ( temp->fine-array_job[i].deadline )
                                        >0
                                    )
                                )
                                {
                                        job_vett[k] = ( temp->fine-array_job[i].deadline );
                                        cont++;
                                        break;
                                }
                        }
                }
                k++;
                temp = temp->next;
        }
// 	devo ora trovare il job con lateness maggiore
// 	free(quaterna_migliore);
// 	quaterna_migliore = valuta_singola_schedula(M_sch);

        for ( j=0;j<cont;j++ )
        {
                int violazione_max =0;
                pos = -1;
                for ( i=0;i<num_job;i++ )
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
                        for ( i=pos+1;i<num_job;i++ )
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
                                schedula_di_lavoro = mossa ( M_sch,M,pos,pos_finale );
                                /*stampa_sequenza_macchina ( M_sch );*/
                                /*stampa_sequenza_macchina( schedula_di_lavoro );*/
                                quaterna_di_lavoro = valuta_singola_schedula ( schedula_di_lavoro );
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
// 						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
// 						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
// 						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
// 						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
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
// 							free(temp1);//cancello la struttura creata
                                                        temp1 = temp2;
                                                }
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_migliore );
                                                free ( quaterna_di_lavoro );
                                                free ( job_vett );
                                                VNS_per_macchina ( M_sch,M );
                                                //applico ricorsivamente l'algoritmo
                                                return 1;
                                        }
                                        else
                                        {
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_di_lavoro );

                                        }
                                }
                                else
                                {//devo distruggere la shedula di lavoro
                                        elimina_schedula ( schedula_di_lavoro );
                                        free ( quaterna_di_lavoro );
                                }
                        }
                }
        }
        //ultima parte
        for ( i=0;i<num_job;i++ )
        {
                job_vett[i]=0;
        }
        k=0;
        cont = 0;
        temp = M_sch;
        while ( temp!= NULL )
        {
                for ( i=0;i<num_job;i++ )
                {
                        if ( temp->ID_job== array_job[i].ID )
                        {
                                if
                                (
                                    ( array_job[i].deadline!=0 )
                                    &&
                                    (
                                        ( temp->fine-array_job[i].deadline )
                                        <0
                                    )
                                )
                                {
                                        job_vett[k] = - ( temp->fine-array_job[i].deadline );
                                        cont++;
                                        break;
                                }
                        }
                }
                k++;
                temp = temp->next;
        }
// 	free(quaterna_migliore);
// 	quaterna_migliore = valuta_singola_schedula(M_sch);

        for ( j=0;j<cont;j++ )
        {
                int margine_max =0;
                pos = -1;
                for ( i=0;i<num_job;i++ )
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
                        for ( i=pos+1;i<num_job;i++ )
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
                                schedula_di_lavoro = mossa ( M_sch,M,pos,pos+1 );

// 				stampa_sequenza_macchina( M_sch);
// 				stampa_sequenza_macchina( schedula_di_lavoro);
                                quaterna_di_lavoro = valuta_singola_schedula ( schedula_di_lavoro );
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
                                                /*						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
                                                                                                quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
                                                                                                quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
                                                                                                quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;*/
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
// 							free(temp1);//cancello la struttura creata
                                                        temp1 = temp2;
                                                }
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_migliore );
                                                free ( quaterna_di_lavoro );
                                                free ( job_vett );
                                                VNS_per_macchina ( M_sch,M );
                                                //applico ricorsivamente l'algoritmo
                                                return 1;
                                        }
                                        else
                                        {
                                                elimina_schedula ( schedula_di_lavoro );
                                                free ( quaterna_di_lavoro );
                                        }
                                }
                                else
                                {//devo distruggere la shedula di lavoro
                                        elimina_schedula ( schedula_di_lavoro );
                                        free ( quaterna_di_lavoro );
                                }
                        }
                }
        }

        free ( quaterna_migliore );
        free ( job_vett );
        return 0;//se nn ho fatto modifiche alla sequenza
}
//lo scopo di questa funzione ï¿½ï¿½?quello di valutare una singola schedula invece di fare una analisi globale cone
//valuta schedula che tra l'altro fissa anche il valore delle variabili globali Lmax, Cmax e Tardy
quaterna *valuta_singola_schedula ( schedula *M_sch )
{
        quaterna *quaterna_di_lavoro;
        quaterna_di_lavoro = new quaterna;
        quaterna_di_lavoro->Lmax = -65000;
        quaterna_di_lavoro->Tardy = 0;
        quaterna_di_lavoro->Feasible = 1;
        schedula *temp,*temp1;
        temp=M_sch;
        int k;
        while ( temp!=NULL )
        {
                for ( k=0;k<num_job;k++ )
                {
                        if ( temp->ID_job==array_job[k].ID )
                        {
                                break;
                        }
                }
                if
                (
                    ( array_job[k].duedate!=0 )
                    &&
                    (
                        ( temp->fine - array_job[k].duedate )
                        >
                        quaterna_di_lavoro->Lmax
                    )
                )
                {
                        quaterna_di_lavoro->Lmax = ( temp->fine - array_job[k].duedate );
                }
                if
                (
                    ( array_job[k].duedate!= 0 )
                    &&
                    ( ( temp->fine - array_job[k].duedate ) > 0 )
                )
                {
                        quaterna_di_lavoro->Tardy++;
                }
                if
                (
                    ( array_job[k].deadline != 0 )
                    &&
                    ( ( temp->fine - array_job[k].deadline ) > 0 )
                )
                {
                        quaterna_di_lavoro->Feasible = 0;
                }
                temp1=temp;
                temp=temp->next;
        }

        quaterna_di_lavoro->Cmax=temp1->fine;
        return quaterna_di_lavoro;
}
int bilanciamento_schedule ( schedula *M1_sch_locale,schedula *M2_sch_locale,schedula *M3_sch_locale )
{
//le schedule M1_sch_locale, M2_sch_locale,M3_sch_locale sono state modificate ed adesso devono essere ribilanciate.
        schedula **vett_sch;
        vett_sch = ( schedula ** ) new schedula*[num_macchine];
        elem **vett_indisp;
        vett_indisp = ( elem ** ) malloc ( num_macchine*sizeof ( elem * ) );
        int *vett_C_fine;
        vett_C_fine = new int[num_macchine];
        int *vett_C_inizio;
        vett_C_inizio = new int[num_macchine];
        schedula **punt_fine_sch;
        punt_fine_sch = ( schedula ** ) new schedula*[num_macchine];
        int i,done = 0;
        job *perm;
        perm=new job;
        if ( num_macchine == 1 )
        {
                vett_sch[0] = M1_sch_locale;
                vett_indisp[0] = M1;
        }
        if ( num_macchine == 2 )
        {
                vett_sch[0] = M1_sch_locale;
                vett_indisp[0] = M1;
                vett_sch[1] = M2_sch_locale;
                vett_indisp[1] = M2;
        }
        else if ( num_macchine == 3 )
        {
                vett_sch[0] = M1_sch_locale;
                vett_indisp[0] = M1;
                vett_sch[1] = M2_sch_locale;
                vett_indisp[1] = M2;
                vett_sch[2] = M3_sch_locale;
                vett_indisp[2] = M3;
        }
        while ( !done )
        {
                for ( i = 0;i<num_macchine;i++ )
                {

                        punt_fine_sch[i] = vett_sch[i];
                        while ( punt_fine_sch[i]->next!=NULL )
                        {
                                punt_fine_sch[i] = punt_fine_sch[i]->next;
                        }
                        //ora sto puntando all'ultimo elemento schedulato
                        vett_C_fine[i] = punt_fine_sch[i]->fine;
                        vett_C_inizio[i] = punt_fine_sch[i]->inizio;
                }
                int min = 65000;
                int pos = -1;
                int *setup_vett;
                int st_vt = 0;
                setup_vett = &st_vt;
                for ( i = 0; i < num_macchine; i++ )
                {
                        if ( min > vett_C_fine[i] )
                        {
                                min = vett_C_fine[i];
                                pos = i;
                        }
                }//trovo la schedula + corta
                for ( i = 0; i < num_macchine; i++ )
                {
                        if
                        (
                            ( i != pos )
                            &&
                            ( vett_C_inizio[i] > vett_C_fine[pos] )
                        )
                        {
// 				devo spostare il job schedulato alla macchina puntata da i in quella puntata da pos
// 				nn ï¿½ï¿½?detto che spostarlo sia utile devo verificare.
                                int ID = punt_fine_sch[i]->ID_job;
                                int j;
                                for ( j = 0; j < num_job; j++ )
                                {
                                        if ( ID == array_job[j].ID )
                                        {
                                                perm[0]=array_job[j];
                                                break;
                                        }
                                }
                                int *disponibilita;
                                int disp = 0;
                                disponibilita = &disp;
                                verifica_macchina ( vett_sch[pos],vett_indisp[pos],disponibilita,setup_vett,0,perm,0 );
                                if ( disponibilita[0]<vett_C_inizio[i] )
                                {
                                        aggiungi_schedula ( vett_sch[pos],perm[0],disponibilita[0],setup_vett[0] );
                                        quaterna *ris = valuta_singola_schedula ( vett_sch[pos] );
                                        if ( ris->Feasible >=Feasible )
                                        {
                                                if
                                                (
                                                    ( ris->Lmax < Lmax )
                                                    ||
                                                    (
                                                        ( ris->Lmax == Lmax )
                                                        &&
                                                        ( ris->Cmax < Cmax )
                                                    )
                                                    ||
                                                    (
                                                        ( ris->Lmax == Lmax )
                                                        &&
                                                        ( ris->Cmax == Cmax )
                                                        &&
                                                        ( ris->Tardy < Tardy )
                                                    )
                                                )
                                                {
// 						devo eliminare il job dalla schedula i
                                                        punt_fine_sch[i] = vett_sch[i];
                                                        while ( punt_fine_sch[i]->next->next!=NULL )
                                                        {
                                                                punt_fine_sch[i] = punt_fine_sch[i]->next;
                                                        }
                                                        free ( punt_fine_sch[i]->next );
                                                        punt_fine_sch[i]->next = NULL;
                                                        punt_fine_sch[pos] = vett_sch[pos];
                                                        while ( punt_fine_sch[pos]->next!=NULL ) //update
                                                        {
                                                                punt_fine_sch[pos] = punt_fine_sch[pos]->next;
                                                        }
                                                        next_elem *prossimo1 = new next_elem;
                                                        valuta_schedula ( M1_sch_locale,M2_sch_locale,M3_sch_locale,prossimo1 );//ricalcolo i valori delle variabili globali
                                                        free ( prossimo1 );
                                                        done = 1;
                                                        break;
                                                }
                                        }
                                        else
                                        {
                                                punt_fine_sch[pos] = vett_sch[pos];
                                                while ( punt_fine_sch[pos]->next->next!=NULL )
                                                {
                                                        punt_fine_sch[pos] = punt_fine_sch[pos]->next;
                                                }
                                                free ( punt_fine_sch[pos]->next );
                                                punt_fine_sch[pos]->next = NULL;
                                        }
                                }
                        }
                }
                if ( done == 1 ) //ho effettuato almeno una modifica e quindi provo a riapplicare il bilanciamento
                {
                        done =0;
                }
                else
                {
                        done = 1;
                }
        }
        free ( vett_sch );
        free ( vett_indisp );
        free ( vett_C_fine );
        free ( vett_C_inizio );
        free ( punt_fine_sch );
        free ( perm );
}
void QRollout::stampa_percentuali_utilizzo_a_video ( void )
{
        int i;
        for ( i = 0;i<num_heur;i++ )
        {
                printf ( "\n %i %.3f% :", i, ( ( float ) funzioni[i].perc_utilizzo/num_job ) *100 );
        }

}
int seleziona_prossimo_job ( next_elem *lista_prossimi_vincitori,int cont_livelli )
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
        Feasible = lista_prossimi_vincitori[pos].feasible;
        while ( pos< cont_livelli-1 )
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
                pos++;
        }
        return best_pos;
}
void QRollout::stampa_permutazioni ( job *permutazione,int dim )
{
        int i;
// 	printf("\n");
        for ( i=0;i<dim;i++ )
        {
                printf ( " %i ",permutazione[i].ID );
        }
// 	printf("\n");
}
int trova_posizione_assoluta ( job *array,int pos_rel )
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
        pos_assoluta=j-1;
        return pos_assoluta;
}
void QRollout::verifica_cambiamento_macchina ( int *primo_passo_M1,int *primo_passo_M2,int *primo_passo_M3 )
{
        schedula *temp;
        if ( primo_passo_M1[0]==0 )
        {
                temp = M1_sch;
                if ( temp->ID_job!=-3 )
                {
                        primo_passo_M1[0]=1;
                }
        }
        if ( primo_passo_M2[0]==0 )
        {
                if ( num_macchine>=2 )
                {
                        temp = M2_sch;
                        if ( temp->ID_job!=-3 )
                        {
                                primo_passo_M2[0]=1;
                        }
                }
        }
        if ( primo_passo_M3[0]==0 )
        {
                if ( num_macchine>=3 )
                {
                        temp = M3_sch;
                        if ( temp->ID_job!=-3 )
                        {
                                primo_passo_M3[0]=1;
                        }
                }
        }
        if ( primo_passo_M1[0]==1 )
        {
                temp = M1_sch;
                if ( temp->ID_job==-3 )
                {
                        primo_passo_M1[0]=0;
                }
        }
        if ( primo_passo_M2[0]==1 )
        {
                if ( temp->ID_job==-3 )
                {
                        temp = M2_sch;
                        if ( temp->next==NULL )
                        {
                                primo_passo_M2[0]=0;
                        }
                }
        }
        if ( primo_passo_M3[0]==1 )
        {
                if ( num_macchine>=3 )
                {
                        temp = M3_sch;
                        if ( temp->ID_job==-3 )
                        {
                                primo_passo_M3[0]=0;
                        }
                }
        }
}
/***********************************************************************************************/

/***********************************************************************************************/

void salva_se_meglio ( job* permutazioni,job *best_perm,int cont_livelli,int iter_for )
{
      //  job *temp;
      //  int j;
        int i=0;
        if
        (
            ( Feasible>Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax<Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax<Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy<Tardy_best )
            )
        )
        {
                for ( i=0;i<num_job;i++ )
                {
                        permutazione_buffer[i]=permutazioni[i];
                }
                Feasible_best=Feasible;
                Lmax_best=Lmax;
                Cmax_best=Cmax;
                Tardy_best=Tardy;
        }


}
void QRollout::stampa_sequenza_macchina ( schedula *M )
{
        schedula *temp;
        printf ( "\n" );
        temp=M;
        while ( temp->next!=NULL )
        {
                printf ( "%i ",temp->ID_job );
                temp=temp->next;
        }
        printf ( "\n" );
}
void QRollout::verifica_se_elementi_uguali ( job *perm_di_passaggio,int num_job )
{
        int j=0,i=0;
        for ( i=0;i<num_job;i++ )
        {
                for ( j=i+1;j<num_job;j++ )
                {
                        if ( perm_di_passaggio[i].ID==perm_di_passaggio[j].ID )
                        {
                                printf ( "\n ERRORE JOB UGUALI %i pos %i e pos %i num %i \n\a",perm_di_passaggio[i].ID,i+1,j+1,perm_di_passaggio[j].ID );
                                exit ( 0 );
                        }
                }
        }
}
job *QRollout::rollout_old ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo


        int i,iter  = 0;
        int j = 0;
        job *permutazioni[num_heur];
        int primo_passo_M1 = 0;
        int primo_passo_M2 = 0;
        int primo_passo_M3 = 0;//non ho fatto ancora nessuna assegnazione alla macchina 3
        job *array_job_attuale;
        schedula *M1_sch_attuale;
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
        M1_sch_attuale = M1_sch;
        schedula *temp1;
        schedula *temp2;
        int fine = 0;
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {

                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];
        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
        iter=0;
        //carica_euristiche()
        while ( iter < num_job )
        {
                next_elem *prossimo = NULL;
                next_elem *prossimo1;

                for ( i = 0;i<num_heur;i++ ) //num_heur dovrï¿½ï¿½?essere cambiato con il numero di heuristiche effettivamente usato
                {
                        permutazioni[i]=NULL;
                        permutazioni[i] = funzioni[i].funz ( array_job_attuale,num_job-iter );
//          stampa_permutazioni(permutazioni[i],num_job-iter);

                        prossimo1 = new next_elem;
                        if ( prossimo == NULL )
                        {
                                prossimo = new next_elem;
                                prossimo->ID_job = permutazioni[i][0].ID;
                                prossimo->ID_heur= funzioni[i].ID_heur;
                                prossimo->next = NULL;
                        }
                        else
                        {
                                next_elem *temp;
                                next_elem *temp_prox;
                                temp = prossimo;
                                while ( temp->next!=NULL )
                                {
                                        temp = temp->next;
                                }
                                temp_prox=new next_elem;
                                temp->next = temp_prox;
                                temp = temp->next;
                                temp->ID_job = permutazioni[i][0].ID;
                                temp->ID_heur= funzioni[i].ID_heur;
                                temp->next = NULL;

                        }
                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter );
//          stampa_risultati_a_video(0);
                        //devo riportare la macchina nelle condizioni orginarie
                        if ( force == 1 )
                        {
                                M1_sch_buffer=new schedula;
                                M2_sch_buffer=new schedula;
                                M3_sch_buffer=new schedula;
                                copia_schelule ( M1_sch,M1_sch_buffer );
                                if ( num_macchine>=2 )
                                {
                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                }
                                if ( num_macchine==3 )
                                {
                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                }
                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                elimina_schedula ( M1_sch_buffer );
                                if ( num_macchine>=2 )
                                {
                                        elimina_schedula ( M2_sch_buffer );
                                }
                                if ( num_macchine==3 )
                                {

                                        elimina_schedula ( M3_sch_buffer );
                                }
                                next_elem *tmp_prox;
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
                                        tmp_prox->Lmax=prossimo1->Lmax;
                                        tmp_prox->Cmax=prossimo1->Cmax;
                                        tmp_prox->Tardy=prossimo1->Tardy;
                                        tmp_prox->feasible=prossimo1->feasible;
                                }
                                free ( prossimo1 );
                        }
                        if
                        (
                            ( M1_sch_attuale == M1_sch )
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
                                                free ( temp1 );
                                                break;
                                        }
                                        while ( temp1->next!=NULL )
                                        {
                                                temp2=temp1;
                                                temp1=temp1->next;
                                        }
                                        free ( temp1 );
                                        temp2->next = NULL;
                                }
                                M1_sch = new schedula;
                                {
                                        M1_sch->ID_job = -3;
                                        M1_sch->tipo = 0;
                                        M1_sch->inizio = 0;
                                        M1_sch->fine = 0;
                                        M1_sch->index_camp = 0;
                                        M1_sch->Lmax = 0;
                                        M1_sch->Tardy = 0;
                                        M1_sch->next = NULL;
                                        M1_sch_attuale = M1_sch;
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
                                                temp2=temp1;
                                                temp1=temp1->next;
                                        }
                                        free ( temp1 );
                                        temp2->next = NULL;
                                }
                        }
                        if ( num_macchine>=2 )
                        {
                                if
                                (
                                    ( M2_sch_attuale == M2_sch )
                                    &&
                                    ( primo_passo_M2 == 0 ) //nn ho ancora schedulato niente su M2
                                )
                                {
                                        fine = 0;
                                        while ( !fine )
                                        {
                                                temp1 = M2_sch_attuale;
                                                if ( temp1->next==NULL )
                                                {
                                                        free ( temp1 );
                                                        break;
                                                }
                                                while ( temp1->next!=NULL )
                                                {
                                                        temp2=temp1;
                                                        temp1=temp1->next;
                                                }
                                                free ( temp1 );
                                                temp2->next = NULL;
                                        }
                                        M2_sch = new schedula;
                                        {
                                                M2_sch->ID_job = -3;
                                                M2_sch->tipo = 0;
                                                M2_sch->inizio = 0;
                                                M2_sch->fine = 0;
                                                M2_sch->Lmax = 0;
                                                M2_sch->Tardy = 0;
                                                M2_sch->index_camp = 0;
                                                M2_sch->next = NULL;
                                                M2_sch_attuale = M2_sch;
                                        }
                                }
                                else
                                {
                                        fine = 0;
                                        while ( !fine )
                                        {
                                                temp1 = M2_sch_attuale;
                                                if ( temp1->next==NULL )
                                                {
                                                        fine = 1;
                                                        break;
                                                }
                                                while ( temp1->next!=NULL )
                                                {
                                                        temp2=temp1;
                                                        temp1=temp1->next;
                                                }
                                                free ( temp1 );
                                                temp2->next = NULL;
                                        }
                                }
                        }
                        if ( num_macchine == 3 )
                        {
                                if
                                (
                                    ( M3_sch_attuale == M3_sch )
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
                                                        free ( temp1 );
                                                        break;
                                                }
                                                while ( temp1->next!=NULL )
                                                {
                                                        temp2=temp1;
                                                        temp1=temp1->next;
                                                }
                                                free ( temp1 );
                                                temp2->next = NULL;
                                        }
                                        M3_sch = new schedula;
                                        {
                                                M3_sch->ID_job = -3;
                                                M3_sch->tipo = 0;
                                                M3_sch->inizio = 0;
                                                M3_sch->fine = 0;
                                                M3_sch->Lmax = 0;
                                                M3_sch->Tardy = 0;
                                                M3_sch->index_camp = 0;
                                                M3_sch->next = NULL;
                                                M3_sch_attuale = M3_sch;
                                        }
                                }
                                else
                                {
                                        fine = 0;
                                        while ( !fine )
                                        {
                                                temp1 = M3_sch_attuale;
                                                if ( temp1->next==NULL )
                                                {
                                                        fine = 1;
                                                        break;
                                                }
                                                while ( temp1->next!=NULL )
                                                {
                                                        temp2=temp1;
                                                        temp1=temp1->next;
                                                }
                                                free ( temp1 );
                                                temp2->next = NULL;
                                        }
                                }
                        }
                }
                //se la schedula non ï¿½ï¿½?feseable deve essere penalizzata rispetto alle altre.
                //devo ridurre il numero di job che rimangono da schedulare
                //devo trovare il job con la Lateness + alta
//       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
//       infine con il numero + basso di Tardy job
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
                Feasible = prossimo->feasible;
                next_elem *temp = prossimo ;
                while ( temp->next!=NULL )
                {
                        if ( Feasible <= temp->next->feasible )
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
                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
//       e su quale macchina ï¿½ï¿½?stato schedulato
                printf ( "\n (%i) %i %i %i %i",iter, ID_heur, L_max, C_max,tardy );
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
                                        temp1->next = new schedula;
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
                                        {
                                                temp1 = temp1->next;
                                        }
                                        temp1->next = new schedula;
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
                                        {
                                                temp1 = temp1->next;
                                        }
                                        temp1->next = new schedula;
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
                for ( i=0;i< ( num_job-iter );i++ )
                {
                        best_perm[iter+i].ID=permutazioni[ID_heur][i].ID;
                        best_perm[iter+i].tipo=permutazioni[ID_heur][i].tipo;
                        best_perm[iter+i].proc_time=permutazioni[ID_heur][i].proc_time;
                        best_perm[iter+i].duedate=permutazioni[ID_heur][i].duedate;
                        best_perm[iter+i].deadline=permutazioni[ID_heur][i].deadline;
                        best_perm[iter+i].priority=permutazioni[ID_heur][i].priority;
                        best_perm[iter+i].rel_time=permutazioni[ID_heur][i].rel_time;
                }
//       faccio l'update dell'utilizzo delle singole funzioni
                funzioni[ID_heur].perc_utilizzo++;
                //devo ora eliminare il job selezionato dall'insieme dei job schedulabili
                i=0;
                j=0;
                while ( j<num_job-iter )
                {
                        if ( array_job_attuale[j].ID!=ID_job )
                        {
                                array_job_attuale[i] = array_job_attuale[j];
                                i++;
                        }
                        j++;
                }
                //marco i job nn utilizzabili
                for ( j=i;j<num_job;j++ )
                {
                        array_job_attuale[j].ID = -1;
                }

                // aggiungo il job nella sequenza di quelli schdulati.
//       permutazione[iter].ID = ID_job;
//       permutazione[iter].tipo = tipo;
//       permutazione[iter].proc_time = proc_time;
//       permutazione[iter].rel_time = rel_time;
//       permutazione[iter].duedate = duedate;
//       permutazione[iter].deadline = deadline;
//       permutazione[iter].priority = priority;//salvo le info sul job prescelto
                next_elem *temp_pr;
                for ( i=0;i<num_heur;i++ )
                {
                        temp_pr = prossimo->next;
                        free ( prossimo );//questa si deve fare meglio, molto meglio
                        prossimo = temp_pr;
                }
                iter++;
                for ( i=0;i<num_heur;i++ )
                {
                        free ( permutazioni[i] );
                }
        }


        return   best_perm;
}
//*************************************************************************************************
//       ROLLOUT Main function modified 18-5-2006
//***********************************************************************************************
job *QRollout::rollout_modificato1 ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

     //   next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int num_job_daschedulare;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
    //    int num_job_relativo = num_job;
    //    int j = 0;
        job **permutazioni;
        permutazioni= new job*[num_heur];
    //    job *job_da_inserire;
    //    int primo_passo_M1 = 0;
    //    int primo_passo_M2 = 0;
    //    int primo_passo_M3 = 0;
        //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
        job *array_job_attuale;
        job *array_job_attuale_temp;
        schedula *M1_sch_attuale;
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
    //    schedula *M1_sch_buffer1;
    //    schedula *M2_sch_buffer1;
    //    schedula *M3_sch_buffer1;
    //    schedula **vettore_schedule_rollout;
    //    schedula *punt_vett_roll;
    //    next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout
        M1_sch_attuale = M1_sch;
    //    schedula *temp1;
    //    schedula *temp2;
    //    int fine = 0;
        int num_next_job=1;
        int indice_job=0;
        int scelto;
        int *punt_job_scelti;
        permutazione_buffer=new job[num_job];
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {

                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];

        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
//     job_fisso = (next_elem *)malloc(sizeof(next_elem));
        job * perm_di_passaggio;
        num_job_daschedulare=num_job;
        while ( cont_livelli>0 )
        {
                punt_job_scelti=NULL;
                next_elem lista_prossimi_vincitori[500];
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
                //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perï¿½ï¿½?devo valutare tutte le sotto schedule

                if ( indice_job==0 ) //questo vuol dire che devo scegliere solo i job migliori
                {
                        indice_job=0;
                        next_elem *prossimo2;
                        prossimo2=NULL;
                        for ( scelto=0;scelto<num_job;scelto++ )
                        {
                                if ( array_job_attuale[scelto].ID!=-1 )
                                {
                                        azzera_schedule();
                                        perm_di_passaggio=new job[num_job-cont_livelli ];
//                 inizializza_perutazione_migliore(perm_di_passaggio);
                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                        {
                                                perm_di_passaggio[pp]=best_perm[pp];
                                        }
                                        pp++;
                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[scelto];// aggiungo in coda il job prescelto
                                        if ( prossimo2==NULL )
                                        {
                                                prossimo2= new next_elem;
                                                prossimo2->next=NULL;
                                                prossimo2->ID_job = array_job[scelto].ID;
                                        }
                                        else
                                        {
                                                next_elem *temp;
                                                next_elem *temp_prox_prova;
                                                temp = prossimo2;
                                                while ( temp->next!=NULL )
                                                {
                                                        temp = temp->next;
                                                }
                                                temp_prox_prova=new next_elem;
                                                temp->next = temp_prox_prova;
                                                temp = temp->next;
                                                temp->next = NULL;
                                                temp->ID_job = array_job[scelto].ID;
                                        }
                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job-cont_livelli );
                                        free ( perm_di_passaggio );
                                }
                        }
//             devo ora individuare quei job che determinano valori migliori della schedula.
                        // tropvo il primo job non segnato
                        punt_job_scelti=new int[num_next_job];
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
                                next_elem *temp = prossimo2 ;
                                while ( temp->ID_job==-1 )
                                {
                                        temp=temp->next;
                                }
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
                                next_elem *temporaneo = prossimo2 ;
                                while ( temporaneo->ID_job!=ID_job )
                                {
                                        temporaneo=temporaneo->next;
                                }
                                temporaneo->ID_job=-1;
                                punt_job_scelti[scelto]=ID_job;
                        }
//             for(scelto=0;scelto<num_next_job;scelto++)
//             {
//                 printf("\n %i ",punt_job_scelti[scelto]);
//             }
//             printf("\n");
                        //_______________________________________________________________________________
                        next_elem *temp_pr;
                        temp_pr = prossimo2;
                        while ( temp_pr->next!=NULL )
                        {
                                temp_pr = temp_pr->next;
                                free ( prossimo2 );
                                prossimo2 = temp_pr;
                        }
                        //_______________________________________________________________________________


                }
                for ( iter_for=0;iter_for<num_job;iter_for++ )
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
                                int bou;
                                if ( indice_job==1 )
                                {
                                        num_job_daschedulare=1;
                                }
                                else
                                {
                                        num_job_daschedulare=num_next_job;
                                }
                                for ( bou=0;bou<num_job_daschedulare;bou++ )
                                {
                                        if (
                                            ( indice_job==1 )
                                            ||
                                            ( array_job_attuale[iter_for].ID==punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                                        )
                                        {
                                                array_job_attuale[iter_for].ID =-1;
                                                array_job_attuale_temp=new job[cont_livelli];
                                                iter=0;
                                                for ( kk=0;kk<cont_livelli;kk++ )
                                                {
                                                        while ( array_job_attuale[iter].ID==-1 )
                                                        {
                                                                iter++;
                                                        }
                                                        array_job_attuale_temp[kk]=array_job_attuale[iter];
                                                        iter++;
                                                }
                                                iter=0;
//                 free(job_da_inserire);
                                                next_elem *prossimo = NULL;
                                                next_elem *prossimo1;
                                                for ( i = 0;i<num_heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                                                {
// Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                                                        azzera_schedule();
                                                        perm_di_passaggio=new job[num_job];
                                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                                        {
                                                                perm_di_passaggio[pp]=best_perm[pp];
                                                        }
                                                        pp++;
                                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];// aggiungo in coda il job prescelto
                                                        next_elem *prossimo2;
                                                        prossimo2= new next_elem;
                                                        prossimo2->next=NULL;
                                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job );
                                                        free ( prossimo2 );
// _____________________________________________________________________________________________________________________________
                                                        azzera_schedule();
                                                        permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                                        if ( prossimo == NULL )
                                                        {
                                                                prossimo = new next_elem;
                                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                                prossimo->next = NULL;
                                                        }
                                                        else
                                                        {
                                                                next_elem *temp;
                                                                next_elem *temp_prox;
                                                                temp = prossimo;
                                                                while ( temp->next!=NULL )
                                                                {
                                                                        temp = temp->next;
                                                                }
                                                                temp_prox=new next_elem;
                                                                temp->next = temp_prox;
                                                                temp = temp->next;
                                                                temp->ID_job = permutazioni[i][0].ID;
                                                                temp->ID_heur= funzioni[i].ID_heur;
                                                                temp->next = NULL;
                                                        }
                                                        for ( jj=0;pp<num_job;pp++,jj++ )
                                                        {
                                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                                        } //aggiungo i job proposti dall'euristica
                                                        azzera_schedule();
                                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );
                                                        free ( perm_di_passaggio );

                                                        //_____________________________________________________________________________________________________________________
                                                        //devo riportare la macchina nelle condizioni orginarie
                                                        if ( force == 1 )
                                                        {
                                                                prossimo1 = new next_elem;
                                                                M1_sch_buffer=new schedula;
                                                                M2_sch_buffer=new schedula;
                                                                M3_sch_buffer=new schedula;
                                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                                if ( num_macchine>=2 )
                                                                {
                                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                                }
                                                                if ( num_macchine==3 )
                                                                {
                                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                                }
                                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                                elimina_schedula ( M1_sch_buffer );
                                                                if ( num_macchine>=2 ) {elimina_schedula ( M2_sch_buffer );}
                                                                if ( num_macchine==3 ) {elimina_schedula ( M3_sch_buffer );}
                                                                next_elem *tmp_prox;
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
                                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                                        tmp_prox->feasible=prossimo1->feasible;
                                                                }
                                                                free ( prossimo1 );
                                                        }
                                                }
                                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                                //devo ridurre il numero di job che rimangono da schedulare
                                                //devo trovare il job con la Lateness + alta
                                                //       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                                //       infine con il numero + basso di Tardy job
                                                array_job_attuale[iter_for].ID =iter_for;
                                                free ( array_job_attuale_temp );
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
                                                Feasible = prossimo->feasible;
                                                next_elem *temp = prossimo ;
                                                while ( temp->next!=NULL )
                                                {
                                                        if ( Feasible <= temp->next->feasible )
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
                                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                                //       e su quale macchina ï¿½ï¿½?stato schedulato
                                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                                lista_prossimi_vincitori[index].feasible = Feasible;
                                                index++;
                                                // fine salvataggio ___________________________________________________________

//             if(prossimo!=NULL)
//             {
                                                next_elem *temp_pr;
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        temp_pr = prossimo->next;
                                                        free ( prossimo );
                                                        prossimo = temp_pr;
                                                }

                                                /* }*/
                                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        free ( permutazioni[i] );
                                                }
//          array_job_attuale[iter_for].ID=iter_for;
                                        }
                                }
                        }

//       verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );
//_____________________________________________________________________________________________________________________
                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,num_next_job );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
//       printf("\nla posizione_vincitore ï¿½ï¿½?: %i Pos_assoluta ï¿½ï¿½?: %i",Pos_vincitore,Pos_assoluta);
//       printf("\n l'id del job successore a quello da inserire ï¿½ï¿½?:%i \n",lista_prossimi_vincitori[Pos_vincitore].ID_job);
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale[Pos_assoluta].ID=-1;
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
//          printf("\a\nhelp!\n");
                }
//       printf("\n P_vincitore %i job aggiunto= %i \n",Pos_vincitore,best_perm[num_job-cont_livelli-1].ID);
//       free(lista_prossimi_vincitori);
//       printf("\a\n___!\n");
//       stampa_permutazioni( permutazione_buffer,num_job);
//       printf("\a\n___!\n");
                cont_livelli--;
                if ( punt_job_scelti!=NULL )
                {
                        free ( punt_job_scelti );
                }
        }


        //    stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale[i].ID;
//    printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
        array_job_attuale[i].ID=-1;
//     free(job_fisso);
        free ( array_job_attuale );
        azzera_schedule();
        next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
//       printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return   best_perm;
        }
}

//*************************************************************************************************
//       ROLLOUT Main function modified 19-5-2006
//***********************************************************************************************
job *QRollout::rollout_modificato2 ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

    //    next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int num_job_daschedulare;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
   //     int num_job_relativo = num_job;
   //     int j = 0;
        job **permutazioni;
        permutazioni= new job*[num_heur];
    //    job *job_da_inserire;
    //    int primo_passo_M1 = 0;
    //    int primo_passo_M2 = 0;
    //    int primo_passo_M3 = 0;
        //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
        job *array_job_attuale;
        job *array_job_attuale_temp;
        schedula *M1_sch_attuale;
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
     //   schedula *M1_sch_buffer1;
     //   schedula *M2_sch_buffer1;
     //   schedula *M3_sch_buffer1;
     //   schedula **vettore_schedule_rollout;
     //   schedula *punt_vett_roll;
     //   next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout
        M1_sch_attuale = M1_sch;
    //    schedula *temp1;
    //    schedula *temp2;
    //    int fine = 0;
        int num_next_job=1;
        int indice_job=0;
        int scelto;
        int *punt_job_scelti;
        int *lista_migliori_passo_precedente;
        int * lista_purificata;
        int *dimensione_lista_purificata=new int;
        permutazione_buffer=new job[num_job];
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {

                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];

        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
//     job_fisso = (next_elem *)malloc(sizeof(next_elem));
        job * perm_di_passaggio;
        num_job_daschedulare=num_job;
        while ( cont_livelli>0 )
        {
                punt_job_scelti=NULL;
                next_elem lista_prossimi_vincitori[500];
                next_elem lista_prossimi_vincitori_swap[500];
                index=0;
                index1=0;
                //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perï¿½ï¿½?devo valutare tutte le sotto schedule
                if ( num_next_job==1 ) //ho un ultimo job da schedulare e poi ricomincio con la totalitï¿½ï¿½?di quelli mancanti
                {
                        num_next_job=cont_livelli+1;//adesso considero i job trascurati
                        indice_job=1;
                }
                else
                {
                        num_next_job = ceil ( ( float ) num_next_job/4 );
                        indice_job=0;
                }

                if ( indice_job==1 ) //questo equivale a dire che non ho ancora schedulato niente
                {
                }
                else //questo vuol dire che devo scegliere solo i job migliori
                {
                        indice_job=0;
                        next_elem *prossimo2;
                        prossimo2=NULL;
                        for ( scelto=0;scelto<num_job;scelto++ )
                        {
                                if ( array_job_attuale[scelto].ID!=-1 )
                                {
                                        azzera_schedule();
                                        perm_di_passaggio=new job[num_job-cont_livelli ];
                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                        {
                                                perm_di_passaggio[pp]=best_perm[pp];
                                        }
                                        pp++;
                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[scelto];// aggiungo in coda il job prescelto
                                        if ( prossimo2==NULL )
                                        {
                                                prossimo2= new next_elem;
                                                prossimo2->next=NULL;
                                                prossimo2->ID_job = array_job[scelto].ID;
                                        }
                                        else
                                        {
                                                next_elem *temp;
                                                next_elem *temp_prox_prova;
                                                temp = prossimo2;
                                                while ( temp->next!=NULL )
                                                {
                                                        temp = temp->next;
                                                }
                                                temp_prox_prova=new next_elem;
                                                temp->next = temp_prox_prova;
                                                temp = temp->next;
                                                temp->next = NULL;
                                                temp->ID_job = array_job[scelto].ID;
                                        }
                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job-cont_livelli );
                                        free ( perm_di_passaggio );
                                }
                        }
//             devo ora individuare quei job che determinano valori migliori della schedula.
                        // tropvo il primo job non segnato
                        punt_job_scelti= new int[2*num_next_job];
                        for ( scelto=0;scelto<dimensione_lista_purificata[0];scelto++ )
                        {
                                punt_job_scelti[scelto]=lista_purificata[scelto];
                        }
                        for ( scelto=0;scelto<2*num_next_job-dimensione_lista_purificata[0];scelto++ )
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
                                next_elem *temp = prossimo2 ;
                                while ( temp->ID_job==-1 )
                                {
                                        temp=temp->next;
                                }
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
                                next_elem *temporaneo = prossimo2 ;
                                while ( temporaneo->ID_job!=ID_job )
                                {
                                        temporaneo=temporaneo->next;
                                }
                                temporaneo->ID_job=-1;
                                punt_job_scelti[dimensione_lista_purificata[0]+scelto]=ID_job;
                        }
                        free ( lista_migliori_passo_precedente );
                        free ( lista_purificata );
                        //_______________________________________________________________________________
                        next_elem *temp_pr;
                        temp_pr = prossimo2;
                        while ( temp_pr->next!=NULL )
                        {
                                temp_pr = temp_pr->next;
                                free ( prossimo2 );
                                prossimo2 = temp_pr;
                        }
                        //_______________________________________________________________________________

                }
                for ( iter_for=0;iter_for<num_job;iter_for++ )
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
                                int bou;
                                if ( indice_job==1 )
                                {
                                        num_job_daschedulare=1;
                                }
                                else
                                {
                                        num_job_daschedulare=2*num_next_job;
                                }
                                for ( bou=0;bou<num_job_daschedulare;bou++ )
                                {
                                        if (
                                            ( indice_job==1 )
                                            ||
                                            ( array_job_attuale[iter_for].ID==punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                                        )
                                        {
                                                array_job_attuale[iter_for].ID =-1;

                                                array_job_attuale_temp=new job[cont_livelli];
                                                iter=0;
                                                for ( kk=0;kk<cont_livelli;kk++ )
                                                {
                                                        while ( array_job_attuale[iter].ID==-1 )
                                                        {
                                                                iter++;
                                                        }
                                                        array_job_attuale_temp[kk]=array_job_attuale[iter];
                                                        iter++;
                                                }
                                                iter=0;
//                 free(job_da_inserire);
                                                next_elem *prossimo = NULL;
                                                next_elem *prossimo1;
                                                for ( i = 0;i<num_heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                                                {
// Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                                                        azzera_schedule();
                                                        perm_di_passaggio=new job[num_job];
                                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                                        {
                                                                perm_di_passaggio[pp]=best_perm[pp];
                                                        }
                                                        pp++;
                                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];// aggiungo in coda il job prescelto
                                                        next_elem *prossimo2;
                                                        prossimo2= new next_elem;
                                                        prossimo2->next=NULL;
                                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job );
                                                        free ( prossimo2 );
// _____________________________________________________________________________________________________________________________
                                                        azzera_schedule();
                                                        permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                                        if ( prossimo == NULL )
                                                        {
                                                                prossimo = new next_elem;
                                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                                prossimo->next = NULL;
                                                        }
                                                        else
                                                        {
                                                                next_elem *temp;
                                                                next_elem *temp_prox;
                                                                temp = prossimo;
                                                                while ( temp->next!=NULL )
                                                                {
                                                                        temp = temp->next;
                                                                }
                                                                temp_prox=new next_elem;
                                                                temp->next = temp_prox;
                                                                temp = temp->next;
                                                                temp->ID_job = permutazioni[i][0].ID;
                                                                temp->ID_heur= funzioni[i].ID_heur;
                                                                temp->next = NULL;
                                                        }
                                                        for ( jj=0;pp<num_job;pp++,jj++ )
                                                        {
                                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                                        } //aggiungo i job proposti dall'euristica
                                                        azzera_schedule();
                                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );
                                                        free ( perm_di_passaggio );

                                                        //_____________________________________________________________________________________________________________________
                                                        //devo riportare la macchina nelle condizioni orginarie
                                                        if ( force == 1 )
                                                        {
                                                                prossimo1 = new next_elem;
                                                                M1_sch_buffer=new schedula;
                                                                M2_sch_buffer=new schedula;
                                                                M3_sch_buffer=new schedula;
                                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                                if ( num_macchine>=2 )
                                                                {
                                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                                }
                                                                if ( num_macchine==3 )
                                                                {
                                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                                }
                                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                                elimina_schedula ( M1_sch_buffer );
                                                                if ( num_macchine>=2 ) {elimina_schedula ( M2_sch_buffer );}
                                                                if ( num_macchine==3 ) {elimina_schedula ( M3_sch_buffer );}
                                                                next_elem *tmp_prox;
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
                                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                                        tmp_prox->feasible=prossimo1->feasible;
                                                                }
                                                                free ( prossimo1 );
                                                        }
                                                }
                                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                                //devo ridurre il numero di job che rimangono da schedulare
                                                //devo trovare il job con la Lateness + alta
                                                //       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                                //       infine con il numero + basso di Tardy job
                                                array_job_attuale[iter_for].ID =iter_for;
                                                free ( array_job_attuale_temp );
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
                                                Feasible = prossimo->feasible;
                                                next_elem *temp = prossimo ;
                                                while ( temp->next!=NULL )
                                                {
                                                        if ( Feasible <= temp->next->feasible )
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
                                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                                //       e su quale macchina ï¿½ï¿½?stato schedulato
                                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                                lista_prossimi_vincitori[index].feasible = Feasible;
                                                index++;
                                                // fine salvataggio ___________________________________________________________

                                                //qui salvo i job che si sono dimostrati i migliori.C_max

                                                next_elem *temp_pr;
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        temp_pr = prossimo->next;
                                                        free ( prossimo );
                                                        prossimo = temp_pr;
                                                }

                                                /* }*/
                                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        free ( permutazioni[i] );
                                                }
//          array_job_attuale[iter_for].ID=iter_for;
                                        }
                                }
                        }
//       verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );
//_____________________________________________________________________________________________________________________
//         questa funzione restituisce i migliori elementi della lista ad esclusione del migliore.
                for ( i=0;i<500;i++ )
                {
                        lista_prossimi_vincitori_swap[i]=lista_prossimi_vincitori[i];
                }
                lista_migliori_passo_precedente=trova_migliori ( lista_prossimi_vincitori_swap,cont_livelli,num_next_job );
                lista_purificata=purifica_lista_job_prescelti ( lista_migliori_passo_precedente,num_next_job,dimensione_lista_purificata );
                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,num_next_job );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
//       printf("\nla posizione_vincitore ï¿½ï¿½?: %i Pos_assoluta ï¿½ï¿½?: %i",Pos_vincitore,Pos_assoluta);
//       printf("\n l'id del job successore a quello da inserire ï¿½ï¿½?:%i \n",lista_prossimi_vincitori[Pos_vincitore].ID_job);
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale[Pos_assoluta].ID=-1;
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
//          printf("\a\nhelp!\n");
                }
                cont_livelli--;
                if ( punt_job_scelti!=NULL )
                {
                        free ( punt_job_scelti );
                }
        }


        //    stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale[i].ID;
//    printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
        array_job_attuale[i].ID=-1;
//     free(job_fisso);
        free ( array_job_attuale );
        azzera_schedule();
        next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
//       printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return   best_perm;
        }
}
int *QRollout::trova_migliori ( next_elem *lista_prossimi_vincitori,int cont_livelli,int num_next_job )
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
                lista_migliori_passo_precedente[j]=lista_prossimi_vincitori[best_pos].ID_job;
                lista_prossimi_vincitori[best_pos].ID_job=-1;
                j++;
        }
        return lista_migliori_passo_precedente;


}
job *QRollout::rollout_modificato3 ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

   //     next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int num_job_daschedulare;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
   //     int num_job_relativo = num_job;
   //     int j = 0;
        job **permutazioni;
        permutazioni= new job*[num_heur];
   //     job *job_da_inserire;
   //     int primo_passo_M1 = 0;
   //     int primo_passo_M2 = 0;
   //     int primo_passo_M3 = 0;
        //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
        job *array_job_attuale;
        job *array_job_attuale_temp;
        schedula *M1_sch_attuale;
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
   //     schedula *M1_sch_buffer1;
   //     schedula *M2_sch_buffer1;
   //     schedula *M3_sch_buffer1;
   //     schedula **vettore_schedule_rollout;
   //     schedula *punt_vett_roll;
   //     next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout
        M1_sch_attuale = M1_sch;
   //     schedula *temp1;
   //     schedula *temp2;
   //     int fine = 0;
        int num_next_job=1;
        int indice_job=0;
        int scelto;
        int *punt_job_scelti;
        int *lista_migliori_passo_precedente;
        int *dimensione_lista_purificata;
        int *lista_purificata;
        FILE *f_log;
        char nome_file[256];
        strcpy ( nome_file,instance_file );
        strcat ( nome_file,".roll_filtro3.txt" );
        f_log=fopen ( nome_file,"a+" );
        dimensione_lista_purificata=new int;
        permutazione_buffer=new job[num_job];
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {
                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];

        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
//     job_fisso = (next_elem *)malloc(sizeof(next_elem));
        job * perm_di_passaggio;
        num_job_daschedulare=num_job;
        while ( cont_livelli>0 )
        {
                punt_job_scelti=NULL;
                next_elem lista_prossimi_vincitori[500];
                next_elem lista_prossimi_vincitori_swap[500];
                index=0;
                index1=0;
                //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perï¿½ï¿½?devo valutare tutte le sotto schedule
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
                if ( indice_job==1 ) //questo equivale a dire che non ho ancora schedulato niente
                {
                }
                else //questo vuol dire che devo scegliere solo i job migliori
                {
                        indice_job=0;
                        punt_job_scelti= new int[dimensione_lista_purificata[0]];
                        for ( scelto=0;scelto<dimensione_lista_purificata[0];scelto++ )
                        {
                                punt_job_scelti[scelto]=lista_purificata[scelto];
                        }
//             printf("\n");
//             for(scelto=0;scelto<num_next_job;scelto++)
//             {
//                 printf(" %i",punt_job_scelti[scelto]);
//             }
//             printf("\n");
                        if ( dimensione_lista_purificata[0]==0 )
                        {
                                indice_job=1;
                                num_next_job=cont_livelli+1;
                        }
                        else
                        {
                                num_next_job=dimensione_lista_purificata[0];
                        }
                        free ( lista_migliori_passo_precedente );
                        free ( lista_purificata );
                }
                for ( iter_for=0;iter_for<num_job;iter_for++ )
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
                                int bou;
                                if ( indice_job==1 )
                                {
                                        num_job_daschedulare=1;
                                }
                                else
                                {
                                        num_job_daschedulare=num_next_job;
                                }
                                for ( bou=0;bou<num_job_daschedulare;bou++ )
                                {
                                        if (
                                            ( indice_job==1 )
                                            ||
                                            ( array_job_attuale[iter_for].ID==punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                                        )
                                        {
                                                array_job_attuale[iter_for].ID =-1;
                                                array_job_attuale_temp=new job[cont_livelli];
                                                iter=0;
                                                for ( kk=0;kk<cont_livelli;kk++ )
                                                {
                                                        while ( array_job_attuale[iter].ID==-1 )
                                                        {
                                                                iter++;
                                                        }
                                                        array_job_attuale_temp[kk]=array_job_attuale[iter];
                                                        iter++;
                                                }
                                                iter=0;
//                 free(job_da_inserire);
                                                next_elem *prossimo = NULL;
                                                next_elem *prossimo1;
                                                for ( i = 0;i<num_heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                                                {
// Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                                                        azzera_schedule();
                                                        perm_di_passaggio=new job[num_job];
                                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                                        {
                                                                perm_di_passaggio[pp]=best_perm[pp];
                                                        }
                                                        pp++;
                                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];// aggiungo in coda il job prescelto
//                             next_elem *prossimo2;
//                             prossimo2= malloc(sizeof(next_elem));
//                             prossimo2->next=NULL;
//                             costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
//                             free(prossimo2);
// _____________________________________________________________________________________________________________________________
                                                        azzera_schedule();
                                                        permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                                        if ( prossimo == NULL )
                                                        {
                                                                prossimo = new next_elem;
                                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                                prossimo->next = NULL;
                                                        }
                                                        else
                                                        {
                                                                next_elem *temp;
                                                                next_elem *temp_prox;
                                                                temp = prossimo;
                                                                while ( temp->next!=NULL )
                                                                {
                                                                        temp = temp->next;
                                                                }
                                                                temp_prox=new next_elem;
                                                                temp->next = temp_prox;
                                                                temp = temp->next;
                                                                temp->ID_job = permutazioni[i][0].ID;
                                                                temp->ID_heur= funzioni[i].ID_heur;
                                                                temp->next = NULL;
                                                        }
                                                        for ( jj=0;pp<num_job;pp++,jj++ )
                                                        {
                                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                                        } //aggiungo i job proposti dall'euristica
                                                        azzera_schedule();
                                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );
                                                        free ( perm_di_passaggio );

                                                        //_____________________________________________________________________________________________________________________
                                                        //devo riportare la macchina nelle condizioni orginarie
                                                        if ( force == 1 )
                                                        {
                                                                prossimo1 = new next_elem;
                                                                M1_sch_buffer=new schedula;
                                                                M2_sch_buffer=new schedula;
                                                                M3_sch_buffer=new schedula;
                                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                                if ( num_macchine>=2 )
                                                                {
                                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                                }
                                                                if ( num_macchine==3 )
                                                                {
                                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                                }
                                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                                elimina_schedula ( M1_sch_buffer );
                                                                if ( num_macchine>=2 ) {elimina_schedula ( M2_sch_buffer );}
                                                                if ( num_macchine==3 ) {elimina_schedula ( M3_sch_buffer );}
                                                                next_elem *tmp_prox;
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
                                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                                        tmp_prox->feasible=prossimo1->feasible;
                                                                }
                                                                free ( prossimo1 );
                                                        }
                                                }
                                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                                //devo ridurre il numero di job che rimangono da schedulare
                                                //devo trovare il job con la Lateness + alta
                                                //       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                                //       infine con il numero + basso di Tardy job
                                                array_job_attuale[iter_for].ID =iter_for;
                                                free ( array_job_attuale_temp );
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
                                                Feasible = prossimo->feasible;
                                                next_elem *temp = prossimo ;
                                                while ( temp->next!=NULL )
                                                {
                                                        if ( Feasible <= temp->next->feasible )
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
                                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                                //       e su quale macchina ï¿½ï¿½?stato schedulato
                                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                                lista_prossimi_vincitori[index].feasible = Feasible;
                                                index++;
                                                // fine salvataggio ___________________________________________________________

                                                //qui salvo i job che si sono dimostrati i migliori.C_max

                                                next_elem *temp_pr;
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        temp_pr = prossimo->next;
                                                        free ( prossimo );
                                                        prossimo = temp_pr;
                                                }

                                                /* }*/
                                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        free ( permutazioni[i] );
                                                }
//          array_job_attuale[iter_for].ID=iter_for;
                                        }
                                }
                        }
//       verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );
//_____________________________________________________________________________________________________________________
//         questa funzione restituisce i migliori elementi della lista ad esclusione del migliore.
                for ( i=0;i<500;i++ )
                {
                        lista_prossimi_vincitori_swap[i]=lista_prossimi_vincitori[i];
                }
                lista_migliori_passo_precedente=trova_migliori ( lista_prossimi_vincitori_swap,cont_livelli,num_next_job );
                lista_purificata=purifica_lista_job_prescelti ( lista_migliori_passo_precedente,num_next_job,dimensione_lista_purificata );
                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,num_next_job );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
//       printf("\nla posizione_vincitore ï¿½ï¿½?: %i Pos_assoluta ï¿½ï¿½?: %i",Pos_vincitore,Pos_assoluta);
//       printf("\n l'id del job successore a quello da inserire ï¿½ï¿½?:%i \n",lista_prossimi_vincitori[Pos_vincitore].ID_job);
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale[Pos_assoluta].ID=-1;
                        fprintf ( f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy );
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
//          printf("\a\nhelp!\n");
                        fprintf ( f_log,"%i %i %i %i \n",Feasible_best,Lmax_best,Cmax_best,Tardy_best );
                }
                cont_livelli--;
                if ( punt_job_scelti!=NULL )
                {
                        free ( punt_job_scelti );
                }
        }


        //    stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale[i].ID;
//    printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
        array_job_attuale[i].ID=-1;
//     free(job_fisso);
        free ( array_job_attuale );
        azzera_schedule();
        next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
//       printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return   best_perm;
        }
}
int *QRollout::purifica_lista_job_prescelti ( int *lista_migliori_passo_precedente,int dim_lista, int *prossima_dimensione )
{
        int j,i;
        int *lista_purificata;
        int contatore=0;
        int dim_lista_locale=ceil ( ( float ) dim_lista/2 ) +1;
        for ( i=0;i<dim_lista_locale-1;i++ )
        {
                for ( j=i+1;j<dim_lista_locale;j++ )
                {
                        if (
                            ( lista_migliori_passo_precedente[i]!=-1 )
                            &&
                            ( lista_migliori_passo_precedente[i]==lista_migliori_passo_precedente[j] )
                        )
                        {
                                lista_migliori_passo_precedente[j]=-1;
                                contatore++;
                        }
                }
        }
        lista_purificata=new int[dim_lista_locale-contatore-1];
        j=0;
        for ( i=1;i<dim_lista_locale;i++ )
        {
                if ( lista_migliori_passo_precedente[i]!=-1 )
                {
                        lista_purificata[j]=lista_migliori_passo_precedente[i];
                        j++;
                }
        }
        prossima_dimensione[0]= ( dim_lista_locale-contatore-1 );
        return lista_purificata;
}
job *QRollout::rollout_modificato4 ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

   //     next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int num_job_daschedulare;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
   //     int num_job_relativo = num_job;
   //     int j = 0;
        job **permutazioni;
        permutazioni= new job*[num_heur];
   //     job *job_da_inserire;
   //     int primo_passo_M1 = 0;
   //     int primo_passo_M2 = 0;
   //     int primo_passo_M3 = 0;
        //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
        job *array_job_attuale;
        job *array_job_attuale_temp;
        schedula *M1_sch_attuale;
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
   //     schedula *M1_sch_buffer1;
   //     schedula *M2_sch_buffer1;
   //     schedula *M3_sch_buffer1;
   //     schedula **vettore_schedule_rollout;
   //     schedula *punt_vett_roll;
   //     next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout
        M1_sch_attuale = M1_sch;
   //     schedula *temp1;
   //     schedula *temp2;
   //     int fine = 0;
        int num_next_job=1;
        int indice_job=0;
        int scelto;
        int *punt_job_scelti;
        int *lista_migliori_passo_precedente;
        int *dimensione_lista_purificata;
   //     int *lista_purificata;
        FILE *f_log;
        char nome_file[256];
        int *lista_migliori_passo_precedente_posizione_assoluta;
        strcpy ( nome_file,instance_file );
        strcat ( nome_file,".roll_filtro4.txt" );
        f_log=fopen ( nome_file,"a+" );
        dimensione_lista_purificata=new int;
        permutazione_buffer=new job[num_job];
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {
                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];

        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
//     job_fisso = (next_elem *)malloc(sizeof(next_elem));
        job * perm_di_passaggio;
        num_job_daschedulare=num_job;
        while ( cont_livelli>0 )
        {
                punt_job_scelti=NULL;
                next_elem lista_prossimi_vincitori[500];
                next_elem lista_prossimi_vincitori_swap[500];
                index=0;
                index1=0;
                //qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perï¿½ï¿½?devo valutare tutte le sotto schedule
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
                if ( indice_job==1 ) //questo equivale a dire che non ho ancora schedulato niente
                {
                }
                else //questo vuol dire che devo scegliere solo i job migliori al passo precedente.
                {
                        indice_job=0;
                        punt_job_scelti=new int[num_next_job];
                        for ( scelto=0;scelto<num_next_job;scelto++ )
                        {
                                punt_job_scelti[scelto]=lista_migliori_passo_precedente_posizione_assoluta[scelto];
                        }
                        free ( lista_migliori_passo_precedente );
                        free ( lista_migliori_passo_precedente_posizione_assoluta );
//             free(lista_purificata);
                }
                for ( iter_for=0;iter_for<num_job;iter_for++ )
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
                                int bou;
                                if ( indice_job==1 )
                                {
                                        num_job_daschedulare=1;
                                }
                                else
                                {
                                        num_job_daschedulare=num_next_job;
                                }
                                for ( bou=0;bou<num_job_daschedulare;bou++ )
                                {
                                        if (
                                            ( indice_job==1 )
                                            ||
                                            ( array_job_attuale[iter_for].ID==punt_job_scelti[bou] ) //se ï¿½ï¿½?selezionabile
                                        )
                                        {
                                                array_job_attuale[iter_for].ID =-1;
                                                array_job_attuale_temp=new job[cont_livelli];
                                                iter=0;
                                                for ( kk=0;kk<cont_livelli;kk++ )
                                                {
                                                        while ( array_job_attuale[iter].ID==-1 )
                                                        {
                                                                iter++;
                                                        }
                                                        array_job_attuale_temp[kk]=array_job_attuale[iter];
                                                        iter++;
                                                }
                                                iter=0;
//                 free(job_da_inserire);
                                                next_elem *prossimo = NULL;
                                                next_elem *prossimo1;
                                                for ( i = 0;i<num_heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                                                {
// Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                                                        azzera_schedule();
                                                        perm_di_passaggio=new job[num_job];
                                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                                        {
                                                                perm_di_passaggio[pp]=best_perm[pp];
                                                        }
                                                        pp++;
                                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];// aggiungo in coda il job prescelto
//                             next_elem *prossimo2;
//                             prossimo2= malloc(sizeof(next_elem));
//                             prossimo2->next=NULL;
//                             costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
//                             free(prossimo2);
// _____________________________________________________________________________________________________________________________
                                                        azzera_schedule();
                                                        permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                                        if ( prossimo == NULL )
                                                        {
                                                                prossimo = new next_elem;
                                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                                prossimo->next = NULL;
                                                        }
                                                        else
                                                        {
                                                                next_elem *temp;
                                                                next_elem *temp_prox;
                                                                temp = prossimo;
                                                                while ( temp->next!=NULL )
                                                                {
                                                                        temp = temp->next;
                                                                }
                                                                temp_prox=new next_elem;
                                                                temp->next = temp_prox;
                                                                temp = temp->next;
                                                                temp->ID_job = permutazioni[i][0].ID;
                                                                temp->ID_heur= funzioni[i].ID_heur;
                                                                temp->next = NULL;
                                                        }
                                                        for ( jj=0;pp<num_job;pp++,jj++ )
                                                        {
                                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                                        } //aggiungo i job proposti dall'euristica
                                                        azzera_schedule();
                                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );
                                                        free ( perm_di_passaggio );

                                                        //_____________________________________________________________________________________________________________________
                                                        //devo riportare la macchina nelle condizioni orginarie
                                                        if ( force == 1 )
                                                        {
                                                                prossimo1 = new next_elem;
                                                                M1_sch_buffer=new schedula;
                                                                M2_sch_buffer=new schedula;
                                                                M3_sch_buffer=new schedula;
                                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                                if ( num_macchine>=2 )
                                                                {
                                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                                }
                                                                if ( num_macchine==3 )
                                                                {
                                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                                }
                                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                                elimina_schedula ( M1_sch_buffer );
                                                                if ( num_macchine>=2 ) {elimina_schedula ( M2_sch_buffer );}
                                                                if ( num_macchine==3 ) {elimina_schedula ( M3_sch_buffer );}
                                                                next_elem *tmp_prox;
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
                                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                                        tmp_prox->feasible=prossimo1->feasible;
                                                                }
                                                                free ( prossimo1 );
                                                        }
                                                }
                                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                                //devo ridurre il numero di job che rimangono da schedulare
                                                //devo trovare il job con la Lateness + alta
                                                //       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                                //       infine con il numero + basso di Tardy job
                                                array_job_attuale[iter_for].ID =iter_for;
                                                free ( array_job_attuale_temp );
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
                                                Feasible = prossimo->feasible;
                                                next_elem *temp = prossimo ;
                                                while ( temp->next!=NULL )
                                                {
                                                        if ( Feasible <= temp->next->feasible )
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
                                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                                //       e su quale macchina ï¿½ï¿½?stato schedulato
                                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                                lista_prossimi_vincitori[index].feasible = Feasible;
                                                index++;
                                                // fine salvataggio ___________________________________________________________

                                                //qui salvo i job che si sono dimostrati i migliori.C_max

                                                next_elem *temp_pr;
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        temp_pr = prossimo->next;
                                                        free ( prossimo );
                                                        prossimo = temp_pr;
                                                }

                                                /* }*/
                                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                                for ( i=0;i<num_heur;i++ )
                                                {
                                                        free ( permutazioni[i] );
                                                }
//          array_job_attuale[iter_for].ID=iter_for;
                                        }
                                }
                        }
//       verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );
//_____________________________________________________________________________________________________________________
//         questa funzione restituisce i migliori elementi della lista ad esclusione del migliore.
                for ( i=0;i<500;i++ )
                {
                        lista_prossimi_vincitori_swap[i]=lista_prossimi_vincitori[i];
                }
                lista_migliori_passo_precedente=trova_prossimi_migliori ( lista_prossimi_vincitori_swap,cont_livelli,num_next_job );
//         printf("\n");
//         for(i=0;i<ceil((float)num_next_job/2)+1;i++)
//         {
//             printf("%i ",lista_migliori_passo_precedente[i]);
//         }
//         printf("\n");
                lista_migliori_passo_precedente_posizione_assoluta=new int[(int)ceil(( float )num_next_job/2.0)];
                for ( i=0;i<ceil ( ( float ) num_next_job/2 );i++ )
                {
                        lista_migliori_passo_precedente_posizione_assoluta[i]=trova_posizione_assoluta ( array_job_attuale,lista_migliori_passo_precedente[i+1] );
                }
                printf ( "\n" );
                for ( i=0;i<ceil ( ( float ) num_next_job/2 );i++ )
                {
                        printf ( "%i ",lista_migliori_passo_precedente_posizione_assoluta[i] );
                }
                printf ( "\n" );
                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,num_next_job );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale[Pos_assoluta].ID=-1;
                        fprintf ( f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy );
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
//          printf("\a\nhelp!\n");
//             fprintf(f_log,"%i %i %i %i \n",Feasible_best,Lmax_best,Cmax_best,Tardy_best);
                }
                cont_livelli--;
                if ( punt_job_scelti!=NULL )
                {
                        free ( punt_job_scelti );
                }
        }


        //    stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale[i].ID;
//    printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
        array_job_attuale[i].ID=-1;
//     free(job_fisso);
        free ( array_job_attuale );
        azzera_schedule();
        next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
//       printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return   best_perm;
        }
}
int *QRollout::trova_prossimi_migliori ( next_elem *lista_prossimi_vincitori,int cont_livelli,int num_next_job )
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
// 	inserirÃ² nella lista dei job papabili solo quelli che hanno release date inferiore al massimo
// 	tempo di completamento delle schedule attuali.
//***********************************************************************************************
job *QRollout::rollout_modificato5 ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

        next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int num_job_daschedulare;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
        int num_job_relativo = num_job;
        int j = 0;
        job **permutazioni;
        permutazioni= new job*[num_heur];
        job *job_da_inserire;
        int primo_passo_M1 = 0;
        int primo_passo_M2 = 0;
        int primo_passo_M3 = 0;
        //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
        job *array_job_attuale;
        job *array_job_attuale_temp;
        job *array_job_attuale_swap;//questo array conterrÃ  una copia del array_job_attuale al passo precedente per poterlo ripristinare
        schedula *M1_sch_attuale;
        schedula **vett_schedule_parziali; //questo vettore conterrÃ  i puntatori alle schedule parziali via via costruite dal rollout.
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
        schedula *M1_sch_buffer1;
        schedula *M2_sch_buffer1;
        schedula *M3_sch_buffer1;
        schedula **vettore_schedule_rollout;
        schedula *punt_vett_roll;
        next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout
        M1_sch_attuale = M1_sch;
        schedula *temp1;
        schedula *temp2;
        int fine = 0;
        int num_next_job=1;
        int indice_job=0;
        int scelto;
        int *punt_job_scelti;
        int *lista_migliori_passo_precedente;
        int *dimensione_lista_purificata;
        int *lista_purificata;
//     FILE *f_log;
        char nome_file[256];
        int *lista_migliori_passo_precedente_posizione_assoluta;
//     strcpy(nome_file,instance_file);
//     strcat(nome_file,".roll_filtro5.txt");
//     f_log=fopen(nome_file,"a+");
        dimensione_lista_purificata=new int;
        permutazione_buffer=new job[num_job];
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2

        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {
                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];

        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
//     job_fisso = (next_elem *)malloc(sizeof(next_elem));
        job * perm_di_passaggio;
        num_job_daschedulare=num_job;
        vett_schedule_parziali=new schedula*[num_macchine];
        array_job_attuale_swap = new job[num_job];
        while ( cont_livelli>0 )
        {
                punt_job_scelti=NULL;
                next_elem lista_prossimi_vincitori[500];
//         next_elem lista_prossimi_vincitori_swap[500];
                index=0;
                index1=0;

// Per prima cosa devo calcolare il max tempo di completamento delle schedule parziali.
                int tempo_massimo_di_completamento=0; //in questo filtro non tengo conto delle indisponibilitÃ  delle macchine.
                int tempo_minimo_di_release=10000000;//ï¿½? un numero arbitrariamente grande
//qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perÃ² devo verificare che il tempo di rilascio sia inferiore al max dei tempi di completamento delle schedule parziali.

// devo quindi salvare in una struttura tutti i job validi e in un'altra tutti quegli elementi che pur non essendo ancora stati selezionati non verificano il criterio del filtro.
                if ( cont_livelli==num_job-1 ) //
                {
                        vett_schedule_parziali[0]=M1_sch;
                        if ( num_macchine==2 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                        }
                        if ( num_macchine==3 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                                vett_schedule_parziali[2]=M3_sch;
                        }
                        tempo_massimo_di_completamento=MaxCmaxSchedula ( vett_schedule_parziali,num_macchine );

                        for ( i = 0;i<num_job;i++ )
                        {
                                array_job_attuale_swap[i] = array_job_attuale[i];
                        }
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 )
                                {
                                        if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                                        {
                                                tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                                        }
                                }
                        }
                        tempo_massimo_di_completamento=max ( tempo_massimo_di_completamento,tempo_minimo_di_release );
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_massimo_di_completamento )
                                {
                                        array_job_attuale[i].ID=-1;//non schedulabile
                                }
                        }

                }
                else
                {
                        azzera_schedule();
                        perm_di_passaggio=new job[num_job-cont_livelli-1];
// 	        inizializza_perutazione_migliore(perm_di_passaggio);
                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                        {
                                perm_di_passaggio[pp]=best_perm[pp];
                                printf ( " %i",perm_di_passaggio[pp].ID );
// 			if(perm_di_passaggio[pp].ID ==64)
// 			{
// 						printf("\n");
// 			}
                        }
// 		printf("\n");
                        stampa_permutazioni ( permutazione_buffer,num_job );
                        pp++;
                        next_elem *prossimo2;
                        prossimo2= new next_elem;
                        prossimo2->next=NULL;
                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job-cont_livelli-1 );
                        free ( prossimo2 );
                        free ( perm_di_passaggio );
                        vett_schedule_parziali[0]=M1_sch;
                        if ( num_macchine==2 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                        }
                        if ( num_macchine==3 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                                vett_schedule_parziali[2]=M3_sch;
                        }
                        tempo_massimo_di_completamento=MaxCmaxSchedula ( vett_schedule_parziali,num_macchine );
                        for ( i = 0;i<num_job;i++ )
                        {
                                array_job_attuale[i] = array_job_attuale_swap[i];// ho modificato l'array_job_attuale_swap in modo che indichi come non schedulabile il job scelto al passo precedente.
                        }
                        tempo_minimo_di_release=10000000;//ï¿½? un numero arbitrariamente grande
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 )
                                {
                                        if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                                        {
                                                tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                                        }
                                }
                        }
                        tempo_massimo_di_completamento=max ( tempo_massimo_di_completamento,tempo_minimo_di_release );
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_massimo_di_completamento )
                                {
                                        array_job_attuale[i].ID=-1;//non schedulabile
                                }
                        }
                }
                for ( iter_for=0;iter_for<num_job;iter_for++ )
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

                                array_job_attuale_temp=new job[cont_livelli];
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
//                 free(job_da_inserire);
                                next_elem *prossimo = NULL;
                                next_elem *prossimo1;
                                for ( i = 0;i<num_heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                                {
// Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                                        azzera_schedule();
                                        perm_di_passaggio=new job[num_job];
                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                        {
                                                perm_di_passaggio[pp]=best_perm[pp];
                                        }
                                        pp++;
                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];// aggiungo in coda il job prescelto
//                             next_elem *prossimo2;
//                             prossimo2= malloc(sizeof(next_elem));
//                             prossimo2->next=NULL;
//                             costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
//                             free(prossimo2);
// _____________________________________________________________________________________________________________________________
                                        azzera_schedule();
                                        permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                        if ( prossimo == NULL )
                                        {
                                                prossimo = new next_elem;
                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                prossimo->next = NULL;
                                        }
                                        else
                                        {
                                                next_elem *temp;
                                                next_elem *temp_prox;
                                                temp = prossimo;
                                                while ( temp->next!=NULL )
                                                {
                                                        temp = temp->next;
                                                }
                                                temp_prox=new next_elem;
                                                temp->next = temp_prox;
                                                temp = temp->next;
                                                temp->ID_job = permutazioni[i][0].ID;
                                                temp->ID_heur= funzioni[i].ID_heur;
                                                temp->next = NULL;
                                        }
                                        for ( jj=0;pp<num_job;pp++,jj++ )
                                        {
                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                        } //aggiungo i job proposti dall'euristica
                                        azzera_schedule();
                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );//qui modifica permutazione_buffer
                                        free ( perm_di_passaggio );

                                        //_____________________________________________________________________________________________________________________
                                        //devo riportare la macchina nelle condizioni orginarie
                                        if ( force == 1 )
                                        {
                                                prossimo1 = new next_elem;
                                                M1_sch_buffer=new schedula;
                                                M2_sch_buffer=new schedula;
                                                M3_sch_buffer=new schedula;
                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                if ( num_macchine>=2 )
                                                {
                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                }
                                                if ( num_macchine==3 )
                                                {
                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                }
                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                elimina_schedula ( M1_sch_buffer );
                                                if ( num_macchine>=2 ) {elimina_schedula ( M2_sch_buffer );}
                                                if ( num_macchine==3 ) {elimina_schedula ( M3_sch_buffer );}
                                                next_elem *tmp_prox;
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
                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                        tmp_prox->feasible=prossimo1->feasible;
                                                }
                                                free ( prossimo1 );
                                        }
                                }
                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                //devo ridurre il numero di job che rimangono da schedulare
                                //devo trovare il job con la Lateness + alta
                                //       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                //       infine con il numero + basso di Tardy job
                                array_job_attuale_swap[iter_for].ID =iter_for;
                                free ( array_job_attuale_temp );
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
                                Feasible = prossimo->feasible;
                                next_elem *temp = prossimo ;
                                while ( temp->next!=NULL )
                                {
                                        if ( Feasible <= temp->next->feasible )
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
                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                //       e su quale macchina ï¿½ï¿½?stato schedulato
                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                lista_prossimi_vincitori[index].feasible = Feasible;
                                index++;
                                // fine salvataggio ___________________________________________________________

                                //qui salvo i job che si sono dimostrati i migliori.C_max

                                next_elem *temp_pr;
                                for ( i=0;i<num_heur;i++ )
                                {
                                        temp_pr = prossimo->next;
                                        free ( prossimo );
                                        prossimo = temp_pr;
                                }

                                /* }*/
                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                for ( i=0;i<num_heur;i++ )
                                {
                                        free ( permutazioni[i] );
                                }
//          array_job_attuale[iter_for].ID=iter_for;


                        }
//       verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );

                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,num_next_job );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale_swap[Pos_assoluta].ID=-1;
//              fprintf(f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy);
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale_swap[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
//          printf("\a\nhelp!\n");
//             fprintf(f_log,"%i %i %i %i \n",Feasible_best,Lmax_best,Cmax_best,Tardy_best);
                }
                cont_livelli--;
                if ( punt_job_scelti!=NULL )
                {
                        free ( punt_job_scelti );
                }
        }


        //    stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale_swap[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale_swap[i].ID;
//    printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale_swap[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale_swap[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale_swap[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale_swap[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale_swap[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale_swap[i].rel_time;
        array_job_attuale[i].ID=-1;
//     free(job_fisso);
        free ( array_job_attuale );
        free ( array_job_attuale_swap );
        azzera_schedule();
        next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
//       printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return   best_perm;
        }
}
int QRollout::CmaxSchedula ( schedula *M_sch )
{
// 	lo scopo di questa funzione ï¿½? quello di individuare il valore del tempo di completamento di una schedula passatagli come parametro.
        if ( M_sch==NULL ) return 0;

        while ( M_sch->next!=NULL )
        {
                M_sch=M_sch->next;
        }
        return M_sch->fine;
}

int QRollout::MaxCmaxSchedula ( schedula **vett_schedule,int num_macchine )
{
// questa funzione considera la schedule separatamente e poi calcola il massimo del loro tempo di completamento.
        int *vettore_dei_tempi; //conterrÃ  i tempi di completamento delle macchine.
        int massimo; //conterrÃ  il valore del massimo.
        vettore_dei_tempi=new int[num_macchine];
        int i=0;
        for ( i=0;i<num_macchine;i++ )
        {
                vettore_dei_tempi[i]=CmaxSchedula ( vett_schedule[i] );
        }
// a questo punto devo calcolare il max di tali elementi.
        massimo=vettore_dei_tempi[0];
        for ( i=1;i<num_macchine;i++ )
        {
                if ( vettore_dei_tempi[i]>massimo )
                {
                        massimo = vettore_dei_tempi[i];
                }
        }
        free ( vettore_dei_tempi );
        return massimo;
}

int QRollout::MinCmaxSchedula ( schedula **vett_schedule,int num_macchine )
{
// questa funzione considera la schedule separatamente e poi calcola il minimo del loro tempo di completamento.
        int *vettore_dei_tempi; //conterrÃ  i tempi di completamento delle macchine.
        int minimo; //conterrÃ  il valore del minimo.
        vettore_dei_tempi=new int[num_macchine];
        int i=0;
        for ( i=0;i<num_macchine;i++ )
        {
                vettore_dei_tempi[i]=CmaxSchedula ( vett_schedule[i] );
        }
// a questo punto devo calcolare il max di tali elementi.
        minimo=vettore_dei_tempi[0];
        for ( i=1;i<num_macchine;i++ )
        {
                if ( vettore_dei_tempi[i]<minimo )
                {
                        minimo = vettore_dei_tempi[i];
                }
        }
        free ( vettore_dei_tempi );
        return minimo;
}
//*************************************************************************************************
//       ROLLOUT Main function modified 9-8-2006
// 	inserirÃ² nella lista dei job papabili solo quelli che hanno release date inferiore al minimo
// 	tempo di completamento delle schedule attuali.
//***********************************************************************************************
job *QRollout::rollout_modificato6 ( int force,char *instance_file )
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

        next_elem *job_fisso ;
        int i,iter,kk  = 0;
        int num_job_daschedulare;
        int pp=0,jj=0;
        int iter_for = 0;
        int index,index1 =0;
        int cont_livelli= num_job-1;
        int Pos_vincitore = 0;
        int Pos_assoluta=0;
        int num_job_relativo = num_job;
        int j = 0;
        job **permutazioni;
        permutazioni= new job*[num_heur];
        job *job_da_inserire;
        int primo_passo_M1 = 0;
        int primo_passo_M2 = 0;
        int primo_passo_M3 = 0;
        //non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
        job *array_job_attuale;
        job *array_job_attuale_temp;
        job *array_job_attuale_swap;//questo array conterrÃ  una copia del array_job_attuale al passo precedente per poterlo ripristinare
        schedula *M1_sch_attuale;
        schedula **vett_schedule_parziali; //questo vettore conterrÃ  i puntatori alle schedule parziali via via costruite dal rollout.
        schedula *M1_sch_buffer;
        schedula *M2_sch_buffer;
        schedula *M3_sch_buffer;
        schedula *M1_sch_buffer1;
        schedula *M2_sch_buffer1;
        schedula *M3_sch_buffer1;
        schedula **vettore_schedule_rollout;
        schedula *punt_vett_roll;
        next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout
        M1_sch_attuale = M1_sch;
        schedula *temp1;
        schedula *temp2;
        int fine = 0;
        int num_next_job=1;
        int indice_job=0;
        int scelto;
        int *punt_job_scelti;
        int *lista_migliori_passo_precedente;
        int *dimensione_lista_purificata;
        int *lista_purificata;
//     FILE *f_log;
        char nome_file[256];
        int *lista_migliori_passo_precedente_posizione_assoluta;
//     strcpy(nome_file,instance_file);
//     strcat(nome_file,".roll_filtro6.txt");
//     f_log=fopen(nome_file,"a+");
        dimensione_lista_purificata=new int;
        permutazione_buffer=new job[num_job];
        schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2

        if ( num_macchine >= 2 )
        {
                M2_sch_attuale = M2_sch;
        }
        schedula *M3_sch_attuale;
        if ( num_macchine == 3 )
        {
                M3_sch_attuale = M3_sch;
        }
        array_job_attuale = new job[num_job];

        for ( i = 0;i<num_job;i++ )
        {
                array_job_attuale[i] = array_job[i];
        }
//     job_fisso = (next_elem *)malloc(sizeof(next_elem));
        job * perm_di_passaggio;
        num_job_daschedulare=num_job;
        vett_schedule_parziali=new schedula*[num_macchine];
        array_job_attuale_swap = new job[num_job];
        while ( cont_livelli>0 )
        {
                punt_job_scelti=NULL;
                next_elem lista_prossimi_vincitori[500];
//         next_elem lista_prossimi_vincitori_swap[500];
                index=0;
                index1=0;

// Per prima cosa devo calcolare il max tempo di completamento delle schedule parziali.
                int tempo_minimo_di_completamento=0; //in questo filtro non tengo conto delle indisponibilitÃ  delle macchine.
                int tempo_minimo_di_release=10000000;//ï¿½? un numero arbitrariamente grande
//qui devo creare un vettore contenente solo i job che devo provare nel rollout, per farlo perÃ² devo verificare che il tempo di rilascio sia inferiore al max dei tempi di completamento delle schedule parziali.

// devo quindi salvare in una struttura tutti i job validi e in un'altra tutti quegli elementi che pur non essendo ancora stati selezionati non verificano il criterio del filtro.
                if ( cont_livelli==num_job-1 ) //
                {
                        vett_schedule_parziali[0]=M1_sch;
                        if ( num_macchine==2 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                        }
                        if ( num_macchine==3 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                                vett_schedule_parziali[2]=M3_sch;
                        }
                        tempo_minimo_di_completamento=MinCmaxSchedula ( vett_schedule_parziali,num_macchine );

                        for ( i = 0;i<num_job;i++ )
                        {
                                array_job_attuale_swap[i] = array_job_attuale[i];
                        }
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 )
                                {
                                        if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                                        {
                                                tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                                        }
                                }
                        }
                        tempo_minimo_di_completamento=max ( tempo_minimo_di_completamento,tempo_minimo_di_release );
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_minimo_di_completamento )
                                {
                                        array_job_attuale[i].ID=-1;//non schedulabile
                                }
                        }
                }
                else
                {
                        azzera_schedule();
                        perm_di_passaggio=new job[num_job-cont_livelli-1];
// 	        inizializza_perutazione_migliore(perm_di_passaggio);
                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                        {
                                perm_di_passaggio[pp]=best_perm[pp];
// 			printf(" % i",perm_di_passaggio[pp].ID);
// 			int prova;
// 	        	if(perm_di_passaggio[pp].ID==192)
// 			{
// 			prova++;
// 			}
                        }
// 		printf("\n");

                        pp++;
                        next_elem *prossimo2;
                        prossimo2= new next_elem;
                        prossimo2->next=NULL;
                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job-cont_livelli-1 );
                        free ( prossimo2 );
                        free ( perm_di_passaggio );
                        vett_schedule_parziali[0]=M1_sch;
                        if ( num_macchine==2 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                        }
                        if ( num_macchine==3 )
                        {
                                vett_schedule_parziali[1]=M2_sch;
                                vett_schedule_parziali[2]=M3_sch;
                        }
                        tempo_minimo_di_completamento=MinCmaxSchedula ( vett_schedule_parziali,num_macchine );
                        for ( i = 0;i<num_job;i++ )
                        {
                                array_job_attuale[i] = array_job_attuale_swap[i];// ho modificato l'array_job_attuale_swap in modo che indichi come non schedulabile il job scelto al passo precedente.
                        }
                        tempo_minimo_di_release=10000000;//ï¿½? un numero arbitrariamente grande
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 )
                                {
                                        if ( array_job_attuale[i].rel_time<tempo_minimo_di_release )
                                        {
                                                tempo_minimo_di_release=array_job_attuale[i].rel_time; //calcolo il tempo minimo di release
                                        }
                                }
                        }
                        tempo_minimo_di_completamento=max ( tempo_minimo_di_completamento,tempo_minimo_di_release );
                        for ( i=0;i<num_job;i++ )
                        {
                                if ( array_job_attuale[i].ID !=-1 && array_job_attuale[i].rel_time>tempo_minimo_di_completamento )
                                {
                                        array_job_attuale[i].ID=-1;//non schedulabile
                                }
                        }
                }
                for ( iter_for=0;iter_for<num_job;iter_for++ )
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

                                array_job_attuale_temp=new job[cont_livelli];
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
//                 free(job_da_inserire);
                                next_elem *prossimo = NULL;
                                next_elem *prossimo1;
                                for ( i = 0;i<num_heur;i++ ) //num_heur contiene il numero di heuristiche effettivamente usato
                                {
// Inizializzo le schedule con tramite i job che ho scelto finora_____________________________________________________________
                                        azzera_schedule();
                                        perm_di_passaggio=new job[num_job];
                                        inizializza_perutazione_migliore ( perm_di_passaggio );
                                        for ( pp=0;pp<num_job-cont_livelli-1;pp++ )
                                        {
                                                perm_di_passaggio[pp]=best_perm[pp];
                                        }
                                        pp++;
                                        perm_di_passaggio[ ( num_job-cont_livelli-1 ) ]=array_job[iter_for];// aggiungo in coda il job prescelto
//                             next_elem *prossimo2;
//                             prossimo2= malloc(sizeof(next_elem));
//                             prossimo2->next=NULL;
//                             costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
//                             free(prossimo2);
// _____________________________________________________________________________________________________________________________
                                        azzera_schedule();
                                        permutazioni[i]=NULL;//calcolo tramite una euristica la parte mancante della permutazione
                                        permutazioni[i] = funzioni[i].funz ( array_job_attuale_temp,cont_livelli );
                                        if ( prossimo == NULL )
                                        {
                                                prossimo = new next_elem;
                                                prossimo->ID_job = permutazioni[i][0].ID;
                                                prossimo->ID_heur= funzioni[i].ID_heur;
                                                prossimo->next = NULL;
                                        }
                                        else
                                        {
                                                next_elem *temp;
                                                next_elem *temp_prox;
                                                temp = prossimo;
                                                while ( temp->next!=NULL )
                                                {
                                                        temp = temp->next;
                                                }
                                                temp_prox=new next_elem;
                                                temp->next = temp_prox;
                                                temp = temp->next;
                                                temp->ID_job = permutazioni[i][0].ID;
                                                temp->ID_heur= funzioni[i].ID_heur;
                                                temp->next = NULL;
                                        }
                                        for ( jj=0;pp<num_job;pp++,jj++ )
                                        {
                                                perm_di_passaggio[pp]=permutazioni[i][jj];
                                        } //aggiungo i job proposti dall'euristica
                                        azzera_schedule();
                                        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job );
                                        printf ( " %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli );
                                        salva_se_meglio ( perm_di_passaggio,best_perm,cont_livelli,iter_for );
                                        free ( perm_di_passaggio );

                                        //_____________________________________________________________________________________________________________________
                                        //devo riportare la macchina nelle condizioni orginarie
                                        if ( force == 1 )
                                        {
                                                prossimo1 = new next_elem;
                                                M1_sch_buffer=new schedula;
                                                M2_sch_buffer=new schedula;
                                                M3_sch_buffer=new schedula;
                                                copia_schelule ( M1_sch,M1_sch_buffer );
                                                if ( num_macchine>=2 )
                                                {
                                                        copia_schelule ( M2_sch,M2_sch_buffer );
                                                }
                                                if ( num_macchine==3 )
                                                {
                                                        copia_schelule ( M3_sch,M3_sch_buffer );
                                                }
                                                VNS ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );
                                                bilanciamento_schedule ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer );//bilancio
                                                valuta_schedula ( M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1 );

                                                elimina_schedula ( M1_sch_buffer );
                                                if ( num_macchine>=2 ) {elimina_schedula ( M2_sch_buffer );}
                                                if ( num_macchine==3 ) {elimina_schedula ( M3_sch_buffer );}
                                                next_elem *tmp_prox;
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
                                                        tmp_prox->Lmax=prossimo1->Lmax;
                                                        tmp_prox->Cmax=prossimo1->Cmax;
                                                        tmp_prox->Tardy=prossimo1->Tardy;
                                                        tmp_prox->feasible=prossimo1->feasible;
                                                }
                                                free ( prossimo1 );
                                        }
                                }
                                //se la schedula non ï¿½ï¿½?feasible deve essere penalizzata rispetto alle altre.
                                //devo ridurre il numero di job che rimangono da schedulare
                                //devo trovare il job con la Lateness + alta
                                //       in condizioni di paritï¿½ï¿½?quello con la Cmax +bassa
                                //       infine con il numero + basso di Tardy job
                                array_job_attuale_swap[iter_for].ID =iter_for;
                                free ( array_job_attuale_temp );
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
                                Feasible = prossimo->feasible;
                                next_elem *temp = prossimo ;
                                while ( temp->next!=NULL )
                                {
                                        if ( Feasible <= temp->next->feasible )
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
                                }//ora sappiamo qual ï¿½ï¿½?L'ID del Job da eliminare dalla lista dei job da schedulare
                                //       e su quale macchina ï¿½ï¿½?stato schedulato
                                printf ( "\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy );
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
                                lista_prossimi_vincitori[index].feasible = Feasible;
                                index++;
                                // fine salvataggio ___________________________________________________________

                                //qui salvo i job che si sono dimostrati i migliori.C_max

                                next_elem *temp_pr;
                                for ( i=0;i<num_heur;i++ )
                                {
                                        temp_pr = prossimo->next;
                                        free ( prossimo );
                                        prossimo = temp_pr;
                                }

                                /* }*/
                                /*elimino i vari candidati di questo step e procedo allo step successivo*/
                                for ( i=0;i<num_heur;i++ )
                                {
                                        free ( permutazioni[i] );
                                }
//          array_job_attuale[iter_for].ID=iter_for;


                        }
//       verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);
                }
                printf ( "\n %i____________________\n",cont_livelli );

                Pos_vincitore=seleziona_prossimo_job ( lista_prossimi_vincitori,num_next_job );//da fare
                //Pos_vincitore indica la posizione relativa del job da schedulare
                Pos_assoluta=trova_posizione_assoluta ( array_job_attuale,Pos_vincitore );
                // devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
                if
                (
                    ( lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best )
                    )
                    ||
                    (
                        ( lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best )
                        &&
                        ( lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best )
                    )
                )
                {
                        best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
                        //       printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
                        best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
                        best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
                        array_job_attuale_swap[Pos_assoluta].ID=-1;
//              fprintf(f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy);
                }
                else
                {
                        best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
                        best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
                        best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
                        best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
                        best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
                        best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
                        best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
                        array_job_attuale_swap[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
//          printf("\a\nhelp!\n");
//             fprintf(f_log,"%i %i %i %i \n",Feasible_best,Lmax_best,Cmax_best,Tardy_best);
                }
                cont_livelli--;
                if ( punt_job_scelti!=NULL )
                {
                        free ( punt_job_scelti );
                }
        }


        //    stampa_risultati_a_video(0);
        i=0;
        while ( array_job_attuale_swap[i].ID==-1 )
        {
                i++;
        }
        best_perm[num_job-1].ID=array_job_attuale_swap[i].ID;
//    printf(" %i\n",best_perm[num_job-1].ID);
        best_perm[num_job-1].tipo=array_job_attuale_swap[i].tipo;
        best_perm[num_job-1].proc_time=array_job_attuale_swap[i].proc_time;
        best_perm[num_job-1].duedate=array_job_attuale_swap[i].duedate;
        best_perm[num_job-1].deadline=array_job_attuale_swap[i].deadline;
        best_perm[num_job-1].priority=array_job_attuale_swap[i].priority;
        best_perm[num_job-1].rel_time=array_job_attuale_swap[i].rel_time;
        array_job_attuale[i].ID=-1;
//     free(job_fisso);
        free ( array_job_attuale );
        free ( array_job_attuale_swap );
        azzera_schedule();
        next_elem *prossimo1;
        prossimo1= new next_elem;
        prossimo1->next=NULL;
        costruisci_e_valuta_schedula ( M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job );
        free ( prossimo1 );
        if
        (
            ( Feasible<Feasible_best )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax>Lmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax>Cmax_best )
            )
            ||
            (
                ( Feasible==Feasible_best )
                &&
                ( Lmax==Lmax_best )
                &&
                ( Cmax==Cmax_best )
                &&
                ( Tardy>Tardy_best )
            )
        )
        {
//       printf("\n Uso La permutazione_buffer\n\a");
                free ( permutazioni );
                return permutazione_buffer;
        }
        else
        {
                free ( permutazioni );
                free ( permutazione_buffer );
                return   best_perm;
        }
}


QRollout::QRollout()
{
    num_job=0;
    num_tipi=0;
    Lmax = 0;// massima lateness
    Cmax = 0;//Makespan
    Tardy = 0;//numero di tardy jobs per schedula
    Feasible = 1; // mi dice se la schedula costruita ï¿½feasible
    Lmax_best=-65000;
    Cmax_best=-65000;
    Tardy_best=-65000;
    Feasible_best=-2;
    swap_lat_tard=0;
    tempo_secondi_inizio1 = 0 ;
    tempo_inizio1=0;tempo_inizio2 = 0;tempo_inizio3 = 0;tempo_inizio4=0;
    num_macchine=0;
    tempo_secondi_fine1 = 0;
    tempo_fine1=0;tempo_fine2 = 0;tempo_fine3 = 0;

}

int QRollout::getNumJob(){
    return num_job;
}

//void QRollout::run(){}

