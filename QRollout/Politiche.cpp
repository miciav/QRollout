#include <qRolloutThread.h>
#include "Globals.h"


/*sceglie quale politica di esclusione delle euristiche applicare*/
void QRolloutThread::EscludiEuristica(TStruttura_Funz *pFunzioni,
                                      int pPolitica,
                                      int pCont_Livelli)
{
        switch (pPolitica)
        {
            case 0:
            {
                    //no exclusion
                    break;
            }
            case 1:
            {
                    PoliticaNeverWin(pFunzioni);
                    break;
            }
            case 2:
            {
                    PoliticaLess_3percReset(pFunzioni);
                    break;

            }
            case 3:
            {
                    PoliticaLess_5percReset(pFunzioni);
                    break;

            }
            case 4:
            {
                    PoliticaLess_7percReset(pFunzioni);
                    break;

            }
            case 5:
            {
                    PoliticaLess_3perc(pFunzioni);
                    break;

            }
            case 6:
            {
                    PoliticaLess_5perc(pFunzioni);
                    break;

            }
            case 7:
            {
                    PoliticaLess_7perc(pFunzioni);
                    break;

            }
            case 8:
            {
                    PoliticaLinearSpread(pFunzioni, pCont_Livelli);
                    break;
            }
            case 9:
            {
                    PoliticaLinearSpreadReset(pFunzioni, pCont_Livelli);
                    break;
            }
            case 10:
            {
                    PoliticaNeverWinOneAtATime(pFunzioni);
                    break;
            }
        }
}

/*esclude dalle euristiche da applicare quelle che dopo uno stadio di rollout non hanno mai vinto*/
void QRolloutThread::PoliticaNeverWin(TStruttura_Funz *pFunzioni)
{
    int i;
    float *percentuali;
    int flag=0;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i=0;i<Fnum_Heur;i++)
    {
        if ((percentuali[i] == 0.0) &&
            (pFunzioni[i].perc_utilizzo != -1))
        {
            pFunzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
             flag=1;
        }
    }
    //reset
    if (flag == 1)
        for(i = 0;i < Fnum_Heur; i++)
        {
            if (pFunzioni[i].perc_utilizzo != -1)
                pFunzioni[i].perc_utilizzo = 0;
        }
}

/*esclude quelle euristiche che dopo uno stadio di rollout hanno vinto meno del 3% delle volte*/
void QRolloutThread::PoliticaLess_3perc(TStruttura_Funz *pFunzioni)
{
    int i, flag=0;
    float *percentuali;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i = 0;i < Fnum_Heur;i++)
    {
        if ((percentuali[i]<3.0) && (pFunzioni[i].perc_utilizzo != -1))
        {
            pFunzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;

            flag=1;
        }
    }

}

/*esclude quelle euristiche che dopo uno stadio di rollout hanno vinto meno del 5% delle volte*/
void QRolloutThread::PoliticaLess_5perc(TStruttura_Funz *pFunzioni)
{
    int i, flag=0;
    float *percentuali = NULL;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i = 0;i < Fnum_Heur;i++)
    {
        if ((percentuali[i]<5.0) && (pFunzioni[i].perc_utilizzo != -1))
        {
            pFunzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
            flag=1;
        }
    }
}

/*esclude quelle euristiche che dopo uno stadio di rollout hanno vinto meno del 7% delle volte*/
void QRolloutThread::PoliticaLess_7perc(TStruttura_Funz *pFunzioni)
{
    int i, flag=0;
    float *percentuali;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i = 0;i < Fnum_Heur;i++)
    {
        if ((percentuali[i] < 7.0) && (pFunzioni[i].perc_utilizzo != -1))
        {
            pFunzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
            flag=1;
        }
    }
}

/*esclude quelle euristiche che hanno vinto meno del 3% delle volte in quello stadio di rollout*/
void QRolloutThread::PoliticaLess_3percReset(TStruttura_Funz *funzioni)
{
    int i, flag=0;
    float *percentuali;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();

    //scelgo le euristiche da eliminare
    for (i = 0;i < Fnum_Heur;i++)
    {
        if ((percentuali[i] < 3.0) && (funzioni[i].perc_utilizzo != -1))
        {
            funzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
            flag=1;
        }
    }
    //reset
    if (flag == 1 && FNum_Heur_Used > 1)
        for(i=0;i<Fnum_Heur;i++)
        {
            if (funzioni[i].perc_utilizzo!=-1)
                funzioni[i].perc_utilizzo=0;
        }
}

/*esclude quelle euristiche che hanno vinto meno del 5% delle volte in quello stadio di rollout*/
void QRolloutThread::PoliticaLess_5percReset(TStruttura_Funz *funzioni)
{
    int i, flag=0;
    float *percentuali;

    percentuali=new float[Fnum_Heur];
    percentuali=CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i=0;i<Fnum_Heur;i++)
    {
        if ((percentuali[i]<5.0) && (funzioni[i].perc_utilizzo != -1))
        {
            funzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
            flag=1;
        }
    }
    //reset
    if (flag==1 && FNum_Heur_Used > 1)
        for(i=0;i<Fnum_Heur;i++){
            if (funzioni[i].perc_utilizzo!=-1)
                funzioni[i].perc_utilizzo=0;
        }
}

/*esclude quelle euristiche che hanno vinto meno del 7% delle volte in quello stadio di rollout*/
void QRolloutThread::PoliticaLess_7percReset(TStruttura_Funz *funzioni)
{
    int i, flag=0;
    float *percentuali;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i = 0;i < Fnum_Heur;i++)
    {
        if ((percentuali[i]<7.0) && (funzioni[i].perc_utilizzo != -1))
        {
            funzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
            //printf("\n Escludo l'euristica numero %i\n",i);
            flag=1;
        }
    }
    //reset
    if (flag == 1 && FNum_Heur_Used > 1)
        for(i = 0;i < Fnum_Heur; i++)
        {
            if (funzioni[i].perc_utilizzo!=-1)
                funzioni[i].perc_utilizzo=0;
        }
}

/*esclude l'euristica peggiore dopo ogni (num_job/num_heur) volte
* in questo modo negli ultimi stadi resta solo l'euristica migliore
*/
void QRolloutThread::PoliticaLinearSpread(TStruttura_Funz *funzioni, int cont_livelli)
{
    int i, j, tmp;
    int rank[Fnum_Heur];
    int somma = 0;

    if ((Fnum_Heur>GNum_Job) || ((GNum_Job-cont_livelli)%(GNum_Job/Fnum_Heur)==0 && (FNum_Heur_Used>1))) {
        //calcola il ranking
        for (i=0;i<Fnum_Heur;i++)
        {
            if (funzioni[i].perc_utilizzo != -1)
                somma+=funzioni[i].perc_utilizzo;
        }
        for(i = 0;i < Fnum_Heur;i++)
            rank[i] = i;

        for(i = 1;i < Fnum_Heur;i++)
            for(j = 0;j < Fnum_Heur;j++)
            {
                if (funzioni[rank[i]].perc_utilizzo>funzioni[rank[j]].perc_utilizzo) {
                    tmp=rank[j];
                    rank[j]=rank[i];
                    rank[i]=tmp;
                }
            }
        //trova indice j del rank da eliminare
        j = -1;
        i = Fnum_Heur-1;
        while(i > 0 && j == -1)
        {
            if(funzioni[rank[i]].perc_utilizzo != -1)
                j=i;
            i--;
        }
        //elimina la funzione che tra quelle attualmente utilizzate ha una percentuale di utilizzo minore
        funzioni[rank[j]].perc_utilizzo=-1;
        FNum_Heur_Used--;
    }
}

/*esclude l'euristica peggiore dopo ogni (num_job/num_heur) volte
* in questo modo negli ultimi stadi resta solo l'euristica migliore
* faccio il reset delle percentuali d'utilizzo dopo ogni stadio di rollout
* la funzione assume un carattere simile a quello del racing (esclusione a confronti)
*/
void QRolloutThread::PoliticaLinearSpreadReset(TStruttura_Funz *funzioni, int cont_livelli){
    int i, j, tmp, rank[Fnum_Heur], somma=0;

    if ((Fnum_Heur>GNum_Job) || ((GNum_Job-cont_livelli)%(GNum_Job/Fnum_Heur)==0 && (FNum_Heur_Used>1))) {
        //calcola il ranking
        for (i = 0;i < Fnum_Heur;i++)
        {
            if (funzioni[i].perc_utilizzo != -1)
                somma+=funzioni[i].perc_utilizzo;
        }
        for(i = 0;i < Fnum_Heur;i++)
            rank[i] = i;

        for(i = 1; i < Fnum_Heur;i++)
            for(j = 0;j < Fnum_Heur; j++)
            {
                if (funzioni[rank[i]].perc_utilizzo>funzioni[rank[j]].perc_utilizzo) {
                    tmp=rank[j];
                    rank[j]=rank[i];
                    rank[i]=tmp;
                }
            }
        //trova indice j del rank da eliminare
        j = -1;
        i = Fnum_Heur - 1;
        while(i > 0 && j == -1)
        {
            if(funzioni[rank[i]].perc_utilizzo != -1)
                j=i;
            i--;
        }
        //elimina la funzione che tra quelle attualmente utilizzate ha una percentuale di utilizzo minore
        funzioni[rank[j]].perc_utilizzo=-1;
        FNum_Heur_Used--;
        //reset
        for(i = 0;i < Fnum_Heur; i++)
        {
            if (funzioni[i].perc_utilizzo!=-1)
                funzioni[i].perc_utilizzo=0;
        }
    }
}

/*esclude una tra le euristiche che dopo uno stadio di rollout non hanno mai vinto*/
void QRolloutThread::PoliticaNeverWinOneAtATime(TStruttura_Funz *funzioni)
{
    int i;
    float *percentuali;
    int flag=0;

    percentuali = new float[Fnum_Heur];
    percentuali = CalcolaStatistiche();
    //scelgo le euristiche da eliminare
    for (i=0;i<Fnum_Heur;i++)
    {
        if ((percentuali[i]==0.0) && (funzioni[i].perc_utilizzo != -1) && (flag != 1))
        {
            funzioni[i].perc_utilizzo=-1;
            FNum_Heur_Used--;
            //printf("\n Escludo l'euristica numero %i\n",i);
            flag=1;
        }
    }
    //reset
    if ((flag==1) && (FNum_Heur_Used > 1))
        for(i=0;i<Fnum_Heur;i++){
            if (funzioni[i].perc_utilizzo!=-1)
                funzioni[i].perc_utilizzo=0;
        }
    //printf("\n");
}



