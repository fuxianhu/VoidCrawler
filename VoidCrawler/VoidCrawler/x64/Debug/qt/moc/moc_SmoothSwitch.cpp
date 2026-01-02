/****************************************************************************
** Meta object code from reading C++ file 'SmoothSwitch.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../SmoothSwitch.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SmoothSwitch.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSSmoothSwitchENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSSmoothSwitchENDCLASS = QtMocHelpers::stringData(
    "SmoothSwitch",
    "toggled",
    "",
    "checked",
    "clicked",
    "trackColor",
    "thumbColor",
    "activeTrackColor",
    "thumbPosition"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSSmoothSwitchENDCLASS_t {
    uint offsetsAndSizes[18];
    char stringdata0[13];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[8];
    char stringdata5[11];
    char stringdata6[11];
    char stringdata7[17];
    char stringdata8[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSSmoothSwitchENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSSmoothSwitchENDCLASS_t qt_meta_stringdata_CLASSSmoothSwitchENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "SmoothSwitch"
        QT_MOC_LITERAL(13, 7),  // "toggled"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 7),  // "checked"
        QT_MOC_LITERAL(30, 7),  // "clicked"
        QT_MOC_LITERAL(38, 10),  // "trackColor"
        QT_MOC_LITERAL(49, 10),  // "thumbColor"
        QT_MOC_LITERAL(60, 16),  // "activeTrackColor"
        QT_MOC_LITERAL(77, 13)   // "thumbPosition"
    },
    "SmoothSwitch",
    "toggled",
    "",
    "checked",
    "clicked",
    "trackColor",
    "thumbColor",
    "activeTrackColor",
    "thumbPosition"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSmoothSwitchENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       5,   30, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    6 /* Public */,
       4,    0,   29,    2, 0x06,    8 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Bool, 0x00015103, uint(0), 0,
       5, QMetaType::QColor, 0x00015103, uint(-1), 0,
       6, QMetaType::QColor, 0x00015103, uint(-1), 0,
       7, QMetaType::QColor, 0x00015103, uint(-1), 0,
       8, QMetaType::QReal, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject SmoothSwitch::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSSmoothSwitchENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSmoothSwitchENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSmoothSwitchENDCLASS_t,
        // property 'checked'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'trackColor'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'thumbColor'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'activeTrackColor'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'thumbPosition'
        QtPrivate::TypeAndForceComplete<qreal, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SmoothSwitch, std::true_type>,
        // method 'toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SmoothSwitch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SmoothSwitch *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SmoothSwitch::*)(bool );
            if (_t _q_method = &SmoothSwitch::toggled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SmoothSwitch::*)();
            if (_t _q_method = &SmoothSwitch::clicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<SmoothSwitch *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isChecked(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->trackColor(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = _t->thumbColor(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = _t->activeTrackColor(); break;
        case 4: *reinterpret_cast< qreal*>(_v) = _t->thumbPosition(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<SmoothSwitch *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setChecked(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setTrackColor(*reinterpret_cast< QColor*>(_v)); break;
        case 2: _t->setThumbColor(*reinterpret_cast< QColor*>(_v)); break;
        case 3: _t->setActiveTrackColor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: _t->setThumbPosition(*reinterpret_cast< qreal*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *SmoothSwitch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SmoothSwitch::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSmoothSwitchENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SmoothSwitch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SmoothSwitch::toggled(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SmoothSwitch::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
