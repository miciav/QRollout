#include "mygeneralthread.h"

/*Questa classe è generica e serve per poter lanciare ogni tipo di algoritmo
l'unica cosa che si deve fare è programmare un metodo create ed un metodo basico
*/

MyGeneralThread::MyGeneralThread(QObject *parent) :
    QThread(parent)
{
}
