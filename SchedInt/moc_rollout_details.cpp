/****************************************************************************
** Meta object code from reading C++ file 'rollout_details.h'
**
** Created: Thu Jun 16 23:40:44 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "rollout_details.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rollout_details.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Rollout_Details[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      47,   42,   16,   16, 0x08,
     100,   42,   16,   16, 0x08,
     154,   42,   16,   16, 0x08,
     200,   42,   16,   16, 0x08,
     253,   16,   16,   16, 0x08,
     281,   16,   16,   16, 0x08,
     311,  304,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Rollout_Details[] = {
    "Rollout_Details\0\0on_CleanButton_clicked()\0"
    "item\0on_CheckBoxAlgoritmi_StateChanged(QTableWidgetItem*)\0"
    "on_CheckBoxEuristiche_StateChanged(QTableWidgetItem*)\0"
    "on_CheckBoxLS_StateChanged(QTableWidgetItem*)\0"
    "on_CheckBoxPolitiche_StateChanged(QTableWidgetItem*)\0"
    "on_OpenFileButton_clicked()\0"
    "on_ExeButton_clicked()\0pTesto\0"
    "ScriveAschermo(QString)\0"
};

const QMetaObject Rollout_Details::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Rollout_Details,
      qt_meta_data_Rollout_Details, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Rollout_Details::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Rollout_Details::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Rollout_Details::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Rollout_Details))
        return static_cast<void*>(const_cast< Rollout_Details*>(this));
    return QDialog::qt_metacast(_clname);
}

int Rollout_Details::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_CleanButton_clicked(); break;
        case 1: on_CheckBoxAlgoritmi_StateChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 2: on_CheckBoxEuristiche_StateChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 3: on_CheckBoxLS_StateChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: on_CheckBoxPolitiche_StateChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 5: on_OpenFileButton_clicked(); break;
        case 6: on_ExeButton_clicked(); break;
        case 7: ScriveAschermo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
