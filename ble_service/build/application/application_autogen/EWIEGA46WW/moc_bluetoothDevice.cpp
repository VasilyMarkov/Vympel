/****************************************************************************
** Meta object code from reading C++ file 'bluetoothDevice.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../application/bluetoothDevice.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bluetoothDevice.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_app__ble__DeviceInfo_t {
    const uint offsetsAndSize[10];
    char stringdata0[61];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_app__ble__DeviceInfo_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_app__ble__DeviceInfo_t qt_meta_stringdata_app__ble__DeviceInfo = {
    {
QT_MOC_LITERAL(0, 20), // "app::ble::DeviceInfo"
QT_MOC_LITERAL(21, 13), // "deviceChanged"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 10), // "deviceName"
QT_MOC_LITERAL(47, 13) // "deviceAddress"

    },
    "app::ble::DeviceInfo\0deviceChanged\0\0"
    "deviceName\0deviceAddress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_app__ble__DeviceInfo[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       2,   21, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   20,    2, 0x06,    3 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::QString, 0x00015001, uint(0), 0,
       4, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void app::ble::DeviceInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeviceInfo *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->deviceChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DeviceInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceInfo::deviceChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<DeviceInfo *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getAddress(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

const QMetaObject app::ble::DeviceInfo::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_app__ble__DeviceInfo.offsetsAndSize,
    qt_meta_data_app__ble__DeviceInfo,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_app__ble__DeviceInfo_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<DeviceInfo, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *app::ble::DeviceInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *app::ble::DeviceInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_app__ble__DeviceInfo.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int app::ble::DeviceInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void app::ble::DeviceInfo::deviceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_app__ble__BLEInterface_t {
    const uint offsetsAndSize[54];
    char stringdata0[532];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_app__ble__BLEInterface_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_app__ble__BLEInterface_t qt_meta_stringdata_app__ble__BLEInterface = {
    {
QT_MOC_LITERAL(0, 22), // "app::ble::BLEInterface"
QT_MOC_LITERAL(23, 15), // "sendTemperature"
QT_MOC_LITERAL(39, 0), // ""
QT_MOC_LITERAL(40, 15), // "deviceConnected"
QT_MOC_LITERAL(56, 18), // "requestTemperature"
QT_MOC_LITERAL(75, 9), // "addDevice"
QT_MOC_LITERAL(85, 20), // "QBluetoothDeviceInfo"
QT_MOC_LITERAL(106, 14), // "onScanFinished"
QT_MOC_LITERAL(121, 17), // "onDeviceScanError"
QT_MOC_LITERAL(139, 37), // "QBluetoothDeviceDiscoveryAgen..."
QT_MOC_LITERAL(177, 19), // "onServiceDiscovered"
QT_MOC_LITERAL(197, 14), // "QBluetoothUuid"
QT_MOC_LITERAL(212, 17), // "onServiceScanDone"
QT_MOC_LITERAL(230, 17), // "onControllerError"
QT_MOC_LITERAL(248, 27), // "QLowEnergyController::Error"
QT_MOC_LITERAL(276, 17), // "onDeviceConnected"
QT_MOC_LITERAL(294, 20), // "onDeviceDisconnected"
QT_MOC_LITERAL(315, 21), // "onServiceStateChanged"
QT_MOC_LITERAL(337, 31), // "QLowEnergyService::ServiceState"
QT_MOC_LITERAL(369, 23), // "onCharacteristicChanged"
QT_MOC_LITERAL(393, 24), // "QLowEnergyCharacteristic"
QT_MOC_LITERAL(418, 20), // "onCharacteristicRead"
QT_MOC_LITERAL(439, 21), // "onCharacteristicWrite"
QT_MOC_LITERAL(461, 12), // "serviceError"
QT_MOC_LITERAL(474, 31), // "QLowEnergyService::ServiceError"
QT_MOC_LITERAL(506, 4), // "read"
QT_MOC_LITERAL(511, 20) // "searchCharacteristic"

    },
    "app::ble::BLEInterface\0sendTemperature\0"
    "\0deviceConnected\0requestTemperature\0"
    "addDevice\0QBluetoothDeviceInfo\0"
    "onScanFinished\0onDeviceScanError\0"
    "QBluetoothDeviceDiscoveryAgent::Error\0"
    "onServiceDiscovered\0QBluetoothUuid\0"
    "onServiceScanDone\0onControllerError\0"
    "QLowEnergyController::Error\0"
    "onDeviceConnected\0onDeviceDisconnected\0"
    "onServiceStateChanged\0"
    "QLowEnergyService::ServiceState\0"
    "onCharacteristicChanged\0"
    "QLowEnergyCharacteristic\0onCharacteristicRead\0"
    "onCharacteristicWrite\0serviceError\0"
    "QLowEnergyService::ServiceError\0read\0"
    "searchCharacteristic"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_app__ble__BLEInterface[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  122,    2, 0x06,    1 /* Public */,
       3,    0,  125,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,  126,    2, 0x0a,    4 /* Public */,
       5,    1,  127,    2, 0x08,    5 /* Private */,
       7,    0,  130,    2, 0x08,    7 /* Private */,
       8,    1,  131,    2, 0x08,    8 /* Private */,
      10,    1,  134,    2, 0x08,   10 /* Private */,
      12,    0,  137,    2, 0x08,   12 /* Private */,
      13,    1,  138,    2, 0x08,   13 /* Private */,
      15,    0,  141,    2, 0x08,   15 /* Private */,
      16,    0,  142,    2, 0x08,   16 /* Private */,
      17,    1,  143,    2, 0x08,   17 /* Private */,
      19,    2,  146,    2, 0x08,   19 /* Private */,
      21,    2,  151,    2, 0x08,   22 /* Private */,
      22,    2,  156,    2, 0x08,   25 /* Private */,
      23,    1,  161,    2, 0x08,   28 /* Private */,
      25,    0,  164,    2, 0x08,   30 /* Private */,
      26,    0,  165,    2, 0x08,   31 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void, 0x80000000 | 20, QMetaType::QByteArray,    2,    2,
    QMetaType::Void, 0x80000000 | 20, QMetaType::QByteArray,    2,    2,
    QMetaType::Void, 0x80000000 | 20, QMetaType::QByteArray,    2,    2,
    QMetaType::Void, 0x80000000 | 24,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void app::ble::BLEInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BLEInterface *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sendTemperature((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->deviceConnected(); break;
        case 2: _t->requestTemperature(); break;
        case 3: _t->addDevice((*reinterpret_cast< std::add_pointer_t<QBluetoothDeviceInfo>>(_a[1]))); break;
        case 4: _t->onScanFinished(); break;
        case 5: _t->onDeviceScanError((*reinterpret_cast< std::add_pointer_t<QBluetoothDeviceDiscoveryAgent::Error>>(_a[1]))); break;
        case 6: _t->onServiceDiscovered((*reinterpret_cast< std::add_pointer_t<QBluetoothUuid>>(_a[1]))); break;
        case 7: _t->onServiceScanDone(); break;
        case 8: _t->onControllerError((*reinterpret_cast< std::add_pointer_t<QLowEnergyController::Error>>(_a[1]))); break;
        case 9: _t->onDeviceConnected(); break;
        case 10: _t->onDeviceDisconnected(); break;
        case 11: _t->onServiceStateChanged((*reinterpret_cast< std::add_pointer_t<QLowEnergyService::ServiceState>>(_a[1]))); break;
        case 12: _t->onCharacteristicChanged((*reinterpret_cast< std::add_pointer_t<QLowEnergyCharacteristic>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 13: _t->onCharacteristicRead((*reinterpret_cast< std::add_pointer_t<QLowEnergyCharacteristic>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 14: _t->onCharacteristicWrite((*reinterpret_cast< std::add_pointer_t<QLowEnergyCharacteristic>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 15: _t->serviceError((*reinterpret_cast< std::add_pointer_t<QLowEnergyService::ServiceError>>(_a[1]))); break;
        case 16: _t->read(); break;
        case 17: _t->searchCharacteristic(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QBluetoothDeviceInfo >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QBluetoothUuid >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLowEnergyController::Error >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLowEnergyService::ServiceState >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLowEnergyCharacteristic >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLowEnergyCharacteristic >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLowEnergyCharacteristic >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLowEnergyService::ServiceError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BLEInterface::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BLEInterface::sendTemperature)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BLEInterface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BLEInterface::deviceConnected)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject app::ble::BLEInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_app__ble__BLEInterface.offsetsAndSize,
    qt_meta_data_app__ble__BLEInterface,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_app__ble__BLEInterface_t
, QtPrivate::TypeAndForceComplete<BLEInterface, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QBluetoothDeviceInfo &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QBluetoothDeviceDiscoveryAgent::Error, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QBluetoothUuid &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QLowEnergyController::Error, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QLowEnergyService::ServiceState, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QLowEnergyCharacteristic &, std::false_type>, QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QLowEnergyCharacteristic &, std::false_type>, QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QLowEnergyCharacteristic &, std::false_type>, QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QLowEnergyService::ServiceError, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *app::ble::BLEInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *app::ble::BLEInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_app__ble__BLEInterface.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int app::ble::BLEInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void app::ble::BLEInterface::sendTemperature(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void app::ble::BLEInterface::deviceConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
