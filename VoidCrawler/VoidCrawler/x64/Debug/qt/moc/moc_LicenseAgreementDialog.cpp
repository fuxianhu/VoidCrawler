/****************************************************************************
** Meta object code from reading C++ file 'LicenseAgreementDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../LicenseAgreementDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LicenseAgreementDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS = QtMocHelpers::stringData(
    "LicenseAgreementDialog",
    "onViewLicenseClicked",
    "",
    "onAcceptClicked",
    "onRejectClicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS_t {
    uint offsetsAndSizes[10];
    char stringdata0[23];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS_t qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS = {
    {
        QT_MOC_LITERAL(0, 22),  // "LicenseAgreementDialog"
        QT_MOC_LITERAL(23, 20),  // "onViewLicenseClicked"
        QT_MOC_LITERAL(44, 0),  // ""
        QT_MOC_LITERAL(45, 15),  // "onAcceptClicked"
        QT_MOC_LITERAL(61, 15)   // "onRejectClicked"
    },
    "LicenseAgreementDialog",
    "onViewLicenseClicked",
    "",
    "onAcceptClicked",
    "onRejectClicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLicenseAgreementDialogENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x08,    1 /* Private */,
       3,    0,   33,    2, 0x08,    2 /* Private */,
       4,    0,   34,    2, 0x08,    3 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LicenseAgreementDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLicenseAgreementDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LicenseAgreementDialog, std::true_type>,
        // method 'onViewLicenseClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAcceptClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRejectClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LicenseAgreementDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LicenseAgreementDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onViewLicenseClicked(); break;
        case 1: _t->onAcceptClicked(); break;
        case 2: _t->onRejectClicked(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *LicenseAgreementDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LicenseAgreementDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLicenseAgreementDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int LicenseAgreementDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
