#include <QtGui>
#include "qRolloutThread.h"
#include "rollout_details.h"
#include "ui_rollout_details.h"
#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QFile>

Rollout_Details::Rollout_Details(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rollout_Details)
{
    ui->setupUi(this);

    this->setGeometry(0,0,647,430);

    FListaNomiFile = new QStringList;

    AlgoritmoRollout = new QRolloutThread;


    /*Lista Nomi algoritmi*/
    {
        FlistaNomiAlgoritmi = AlgoritmoRollout->GetListaAlgoritmos();

        ui->TabellaAlgoritmo->setRowCount(FlistaNomiAlgoritmi->count());

        for (int var = 0; var < FlistaNomiAlgoritmi->count(); ++var)
        {
            FLabel = new QLabel(FlistaNomiAlgoritmi->at(var));
            FChek  = new QTableWidgetItem();
            FChek->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if (var == 0)
            {
                FChek->setCheckState(Qt::Checked);
                FTipoAlgoritmo = 0;
            }
            else
                FChek->setCheckState(Qt::Unchecked);

            ui->TabellaAlgoritmo->setItem(var,0,FChek);
            ui->TabellaAlgoritmo->setCellWidget(var,1,FLabel );

        }
        connect(ui->TabellaAlgoritmo,
                SIGNAL(itemChanged(QTableWidgetItem*)),
                this,
                SLOT(on_CheckBoxAlgoritmi_StateChanged(QTableWidgetItem*))
                );


    }
    /*Lista nomi euristiche*/
    {
        FlistaNomiEuristiche = AlgoritmoRollout->GetListaEuristicas();

        ui->TabellaEuristica->setRowCount(FlistaNomiEuristiche->count());
        ui->TabellaEuristica->setColumnWidth(1,150);

        for (int var = 0; var < FlistaNomiEuristiche->count(); ++var)
        {
            FLabel = new QLabel(FlistaNomiEuristiche->at(var));
            FChek  = new QTableWidgetItem();
            FChek->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if (var == 0) {
                FChek->setCheckState(Qt::Checked);
                FTipoEuristica = 0;
            }
            else
                FChek->setCheckState(Qt::Unchecked);

            ui->TabellaEuristica->setItem(var,0,FChek);
            ui->TabellaEuristica->setCellWidget(var,1,FLabel );

        }
        connect(ui->TabellaEuristica,
                SIGNAL(itemChanged(QTableWidgetItem*)),
                this,
                SLOT(on_CheckBoxEuristiche_StateChanged(QTableWidgetItem*))
                );

    }
    ui->Result->setText("Risultati.txt");
    /*Lista nomi ricerche locali*/
    {
        FlistaNomiLS = AlgoritmoRollout->GetListaRicercheLocali();
        ui->TableLS->setRowCount(FlistaNomiLS->count());
        ui->TableLS->setColumnWidth(1,150);
        for (int var = 0; var < FlistaNomiLS->count(); ++var)
        {
            FLabel = new QLabel(FlistaNomiLS->at(var));
            FChek  = new QTableWidgetItem();
            FChek->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if (var == 0)
            {
                FChek->setCheckState(Qt::Checked);
                FTipoLS = 0;
            }
            else
                FChek->setCheckState(Qt::Unchecked);

            ui->TableLS->setItem(var,0,FChek);
            ui->TableLS->setCellWidget(var,1,FLabel );

        }
        connect(ui->TableLS,
                SIGNAL(itemChanged(QTableWidgetItem*)),
                this,
                SLOT(on_CheckBoxLS_StateChanged(QTableWidgetItem*))
                );

    }

    /*Lista nomi politiche di pruning*/
    {
        FListaNomiPolitiche = AlgoritmoRollout->GetListaPolitiche();
        ui->tablePruning->setRowCount(FListaNomiPolitiche->count());
        ui->tablePruning->setColumnWidth(1,150);
        for (int var = 0; var < FListaNomiPolitiche->count(); ++var)
        {
            FLabel = new QLabel(FListaNomiPolitiche->at(var));
            FChek  = new QTableWidgetItem();
            FChek->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if (var == 0)
            {
                FChek->setCheckState(Qt::Checked);
                FTipoLS = 0;
            }
            else
                FChek->setCheckState(Qt::Unchecked);

            ui->tablePruning->setItem(var,0,FChek);
            ui->tablePruning->setCellWidget(var,1,FLabel );

        }
        connect(ui->tablePruning,
                SIGNAL(itemChanged(QTableWidgetItem*)),
                this,
                SLOT(on_CheckBoxPolitiche_StateChanged(QTableWidgetItem*))
                );

    }



    /*Progress bar*/
    ui->progressBar->setVisible(false);

    connect(AlgoritmoRollout,
            SIGNAL(ScriviASchermo(QString)),
            this,
            SLOT(ScriveAschermo(QString))
            );

}

Rollout_Details::~Rollout_Details()
{
    delete ui;
}


void Rollout_Details::CrealistaNomiFile(QString pPath)
{


    QFileInfoList lListaInfoFile;
    QDir lDirIniziale(pPath);

    /*credo che questa parte Ã¨ inutile*/
    lDirIniziale.setCurrent(pPath);

    lListaInfoFile = lDirIniziale.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);


    for (int i = 0; i < lListaInfoFile.length(); i++)
    {
        if (lListaInfoFile.at(i).isDir())
            /*se e' una directory chiamo la funzione ricorsivamente*/
            CrealistaNomiFile(lListaInfoFile.at(i).filePath());
        else /*e' un file*/
        {
            FListaNomiFile->append(lListaInfoFile.at(i).filePath()); /*aggiungo alla lista*/
            ui->VisualList->addItem(lListaInfoFile.at(i).filePath());
        }
    }

}
void Rollout_Details::on_CheckBoxPolitiche_StateChanged(QTableWidgetItem* item)
{
    int lsumaCheck = 0;
    /*questa azione serve per il checkbox*/
    for (int i = 0; i < ui->tablePruning->rowCount(); ++i)
    {
        if (ui->tablePruning->item(i,0)->checkState() == Qt::Checked   )
            lsumaCheck++;
    }
    if (lsumaCheck > 1)
    { /*permettiamo il cambio e azzeriamo gli altri*/
        FTipoPolitica = item->row();
        for (int i = 0; i < ui->tablePruning->rowCount(); ++i)
        {
            if (    (ui->tablePruning->item(i,0) != item) &&
                    (ui->tablePruning->item(i,0)->checkState() == Qt::Checked)   )
            {
                ui->tablePruning->item(i,0)->setCheckState(Qt::Unchecked);
            }
        }
    }
}

void Rollout_Details::on_CheckBoxLS_StateChanged(QTableWidgetItem* item)
{
    int lsumaCheck = 0;
    /*questa azione serve per il checkbox*/
    for (int i = 0; i < ui->TableLS->rowCount(); ++i)
    {
        if (ui->TableLS->item(i,0)->checkState() == Qt::Checked   )
            lsumaCheck++;
    }
    if (lsumaCheck >1)
    { /*permettiamo il cambio e azzeriamo gli altri*/
        FTipoLS = item->row();
        for (int i = 0; i < ui->TableLS->rowCount(); ++i)
        {
            if (    (ui->TableLS->item(i,0) != item) &&
                    (ui->TableLS->item(i,0)->checkState() == Qt::Checked)   )
            {
                ui->TableLS->item(i,0)->setCheckState(Qt::Unchecked);
            }
        }
    }
}

void Rollout_Details::on_CheckBoxEuristiche_StateChanged(QTableWidgetItem *item)
{
    int lsumaCheck = 0;
    /*questa azione serve per il checkbox*/
    for (int i = 0; i < ui->TabellaEuristica->rowCount(); ++i)
    {
        if (ui->TabellaEuristica->item(i,0)->checkState() == Qt::Checked   )
            lsumaCheck++;
    }
    if (lsumaCheck >1)
    { /*permettiamo il cambio e azzeriamo gli altri*/
        FTipoEuristica = item->row();
        for (int i = 0; i < ui->TabellaEuristica->rowCount(); ++i)
        {
            if (    (ui->TabellaEuristica->item(i,0) != item) &&
                    (ui->TabellaEuristica->item(i,0)->checkState() == Qt::Checked)   )
            {
                ui->TabellaEuristica->item(i,0)->setCheckState(Qt::Unchecked);
            }
        }
    }

}

void Rollout_Details::on_CheckBoxAlgoritmi_StateChanged(QTableWidgetItem* item)
{

    int lsumaCheck=0;
    /*questa azione serve per il checkbox*/
    for (int i = 0; i < ui->TabellaAlgoritmo->rowCount(); ++i) {
        if (ui->TabellaAlgoritmo->item(i,0)->checkState() == Qt::Checked   ) {
            lsumaCheck++;
        }
    }
    if (lsumaCheck >1) { /*permettiamo il cambio e azzeriamo gli altri*/
        if (item->checkState()== Qt::Checked) {
            FTipoAlgoritmo = item->row();
        }

        FTipoAlgoritmo = item->row();
        for (int i = 0; i < ui->TabellaAlgoritmo->rowCount(); ++i) {
            if (    (ui->TabellaAlgoritmo->item(i,0) != item) &&
                    (ui->TabellaAlgoritmo->item(i,0)->checkState() == Qt::Checked)   ) {
                ui->TabellaAlgoritmo->item(i,0)->setCheckState(Qt::Unchecked);
            }
        }
    }


}

void Rollout_Details::on_OpenFileButton_clicked()
{
    /*esta funcion sirve para elegir el directorio*/
    /*serve per aprire la directory con le istanze*/

    QString dirname = QFileDialog::getExistingDirectory(   this,
                                                           tr("Scegli la directory con le instanze"),
                                                           QDir::currentPath() );
    if( !dirname.isNull() )
    {
        /*qui bisognerÃ  scandagliare la directory e le subdirectory e mettere
        *il percorso delle instanze in una lista*/
        CrealistaNomiFile(dirname);
        ui->ExeButton->setEnabled(true);
        ui->CleanButton->setEnabled (true);
    }

}

void Rollout_Details::on_ExeButton_clicked()
{
    // per prima cosa devo creare un file di testo.
    LimpiarColores();
    LimpiarConsola();
    CreaFileConfigurazione();

    QString lConfigFileName = QCoreApplication::applicationDirPath()+"//Configurazione.txt";
    ui->tabWidget->setCurrentWidget(ui->tab_4); // attivo il tab che mostra i risultati
    ui->progressBar->setVisible(true);
    ui->progressBar->reset();
    ui->progressBar->setMaximum(FListaNomiFile->count());


    for (int i = 0; i < FListaNomiFile->count(); ++i)
    {
        QString lSolutionName = FListaNomiFile->at(i);
        int status = AlgoritmoRollout->QRolloutThreadInicialize(lConfigFileName,lSolutionName);
        if(status != 1)
        {
            ui->VisualList->item(i)->setBackground(Qt::red);
            continue;
        }
        AlgoritmoRollout->start();
        AlgoritmoRollout->wait();
        QCoreApplication::processEvents();
        ui->progressBar->setValue(i+1);
        ui->VisualList->item(i)->setBackground(Qt::green);
    }

    ui->progressBar->setVisible(false);




}
void Rollout_Details::LimpiarConsola()
{
    ui->textEdit->clear();
}

void Rollout_Details::LimpiarColores()
{
    for (int i = 0; i < FListaNomiFile->count(); ++i)
        ui->VisualList->item(i)->setBackground(Qt::white);

}

void Rollout_Details::CreaFileConfigurazione()
{
    QString lDirName = QCoreApplication::applicationDirPath();
    QFile file(lDirName+"//Configurazione.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    /*algortimo o euristica*/
    if (FTipoAlgoritmo > 0) out << "0\n";
    else out << "1\n";

    /*tipo euristica*/
   out << QString::number(FTipoEuristica)+"\n";

   /*tipo rollout*/
   if (FTipoAlgoritmo > 0)
       out << QString::number(FTipoAlgoritmo-1)+ "\n";
   else
       out << "0\n";

   /*tipo euristica rollout*/
   out << QString::number(FTipoEuristica)+"\n";

   /*tempo ma non serve*/
   out << "0\n";

   /*file dei risultati*/
   out << lDirName + ui->Result->text()+"\n";

   /*force */
   if (ui->ChForce->isChecked())
       out <<"1\n";
   else
       out <<"0\n";

   /*fin*/
   out << "0\n";


   /*local search*/
   out << QString::number(FTipoLS)+ "\n";

   file.close();
}



/*questa funzione serve per scrivere a schermo e risponde ad un evento*/
void Rollout_Details::ScriveAschermo(QString pTesto)
{
    ui->textEdit->append(pTesto);

}

void Rollout_Details::on_CleanButton_clicked()
{
    ui->VisualList->clear ();
    ui->ExeButton->setEnabled (false);
    ui->OpenFileButton->setEnabled (true);
    ui->CleanButton->setEnabled (false);
    FListaNomiFile->clear ();

}
