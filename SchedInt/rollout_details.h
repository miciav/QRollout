#ifndef ROLLOUT_DETAILS_H
#define ROLLOUT_DETAILS_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QTableWidget>
#include "qRolloutThread.h"

namespace Ui {
    class Rollout_Details;
}

class Rollout_Details : public QDialog
{
    Q_OBJECT

public:
    explicit Rollout_Details(QWidget *parent = 0);
    ~Rollout_Details();

private slots:

    void on_CleanButton_clicked();
    void on_CheckBoxAlgoritmi_StateChanged(QTableWidgetItem* item);
    void on_CheckBoxEuristiche_StateChanged(QTableWidgetItem* item);
    void on_CheckBoxLS_StateChanged(QTableWidgetItem* item);
    void on_CheckBoxPolitiche_StateChanged(QTableWidgetItem* item);

    void on_OpenFileButton_clicked();

    void on_ExeButton_clicked();    

    void ScriveAschermo(QString pTesto);

private:
    Ui::Rollout_Details *ui;
    QStringList* FListaNomiFile;        /*lista con i nomi dei file*/
    QRolloutThread* AlgoritmoRollout;   /*Algoritmo rollout o euristica*/
    QStringList* FlistaNomiAlgoritmi;   /*Lista con i nomi degli  algoritmi*/
    QStringList* FlistaNomiEuristiche;  /*Lista con i nomi delle euristiche*/
    QStringList* FlistaNomiLS;          /*Lista con i nomi delle ricerche locali*/
    QStringList* FListaNomiPolitiche;   /*Lista con i nomi delle politiche di pruning*/
    QLabel* FLabel;
    QTableWidgetItem * FChek;
    int FTipoAlgoritmo; // tipo algoritmo
    int FTipoEuristica; // tipo euristica
    int FTipoLS;        // tipo LS
    int FTipoPolitica;  // tipo Politica

    /*questa funzione e' ricorsiva e serve per riempire la lista dei nomi dei file*/
    void CrealistaNomiFile(QString pPath);
    void CreaFileConfigurazione();
    void LimpiarColores();
    void LimpiarConsola();

};

#endif // ROLLOUT_DETAILS_H
