#include <Python.h>

#include "pythonPluginAdapter.h"
#include <QDebug>
#include <QFileInfo>
#include <QWidget>

namespace {

QString pyErrorString()
{
    if (!PyErr_Occurred())
        return {};
    PyErr_Print(); // volle Ausgabe auf stderr, hilfreich beim Debuggen
    return QStringLiteral("Python-Fehler (siehe Konsolen-/Log-Ausgabe)");
}

QString callStringMethod(PyObject *instance, const char *methodName)
{
    PyObject *result = PyObject_CallMethod(instance, methodName, nullptr);
    if (!result) {
        qWarning() << "Aufruf von" << methodName << "fehlgeschlagen:" << pyErrorString();
        return {};
    }
    QString value = QString::fromUtf8(PyUnicode_AsUTF8(result));
    Py_DECREF(result);
    return value;
}

} // namespace

PythonPluginAdapter *PythonPluginAdapter::load(const QString &scriptPath, QObject *parent)
{
    PyGILState_STATE gil = PyGILState_Ensure();

    const QString moduleName = QFileInfo(scriptPath).completeBaseName();

    PyObject *module = PyImport_ImportModule(moduleName.toUtf8().constData());
    if (!module) {
        qWarning() << "Konnte Python-Modul nicht importieren:" << moduleName << pyErrorString();
        PyGILState_Release(gil);
        return nullptr;
    }

    PyObject *baseModule = PyImport_ImportModule("plugin_base");
    PyObject *baseClass = baseModule ? PyObject_GetAttrString(baseModule, "PluginBase") : nullptr;

    PyObject *dict = PyModule_GetDict(module); // Borrowed reference
    PyObject *key = nullptr;
    PyObject *value = nullptr;
    Py_ssize_t pos = 0;
    PyObject *pluginClass = nullptr;

    while (PyDict_Next(dict, &pos, &key, &value)) {
        if (PyType_Check(value) && baseClass &&
            PyObject_IsSubclass(value, baseClass) == 1 && value != baseClass) {
            pluginClass = value; // Borrowed reference (gehört zum Modul-Dict)
            break;
        }
    }

    if (!pluginClass) {
        qWarning() << "Keine PluginBase-Unterklasse in" << scriptPath << "gefunden.";
        Py_XDECREF(baseClass);
        Py_XDECREF(baseModule);
        Py_DECREF(module);
        PyGILState_Release(gil);
        return nullptr;
    }

    PyObject *instance = PyObject_CallObject(pluginClass, nullptr);

    Py_XDECREF(baseClass);
    Py_XDECREF(baseModule);
    Py_DECREF(module);

    if (!instance) {
        qWarning() << "Konnte Plugin-Klasse nicht instanziieren:" << pyErrorString();
        PyGILState_Release(gil);
        return nullptr;
    }

    PyGILState_Release(gil);

    // Referenz `instance` geht an den Adapter über (kein weiteres DECREF hier).
    return new PythonPluginAdapter(instance, parent);
}

PythonPluginAdapter::PythonPluginAdapter(PyObject *instance, QObject *parent)
    : QObject(parent)
    , m_instance(instance)
{
}

PythonPluginAdapter::~PythonPluginAdapter()
{
    PyGILState_STATE gil = PyGILState_Ensure();
    for (PyObject *obj : std::as_const(m_widgetRefs)) {
        Py_XDECREF(obj);
    }
    m_widgetRefs.clear();
    if (m_instance) {
        Py_DECREF(m_instance);
    }
    PyGILState_Release(gil);
}

QString PythonPluginAdapter::getPluginName() const
{
    PyGILState_STATE gil = PyGILState_Ensure();
    QString name = callStringMethod(m_instance, "get_plugin_name");
    PyGILState_Release(gil);
    return name;
}

QString PythonPluginAdapter::getPluginVersion() const
{
    PyGILState_STATE gil = PyGILState_Ensure();
    QString version = callStringMethod(m_instance, "get_plugin_version");
    PyGILState_Release(gil);
    return version;
}

QWidget *PythonPluginAdapter::createWidget(QWidget *parent)
{
    PyGILState_STATE gil = PyGILState_Ensure();

    PyObject *widgetObj = PyObject_CallMethod(m_instance, "create_widget", nullptr);
    if (!widgetObj) {
        qWarning() << "create_widget() fehlgeschlagen:" << pyErrorString();
        PyGILState_Release(gil);
        return nullptr;
    }

    PyObject *shibokenModule = PyImport_ImportModule("shiboken6");
    if (!shibokenModule) {
        qWarning() << "shiboken6 nicht verfügbar:" << pyErrorString();
        Py_DECREF(widgetObj);
        PyGILState_Release(gil);
        return nullptr;
    }

    // <-- HIER war der disown()-Block, jetzt komplett weg

    PyObject *getPtrFn = PyObject_GetAttrString(shibokenModule, "getCppPointer");
    PyObject *ptrTuple = getPtrFn ? PyObject_CallFunctionObjArgs(getPtrFn, widgetObj, nullptr)
                                   : nullptr;
    Py_XDECREF(getPtrFn);
    Py_DECREF(shibokenModule);

    QWidget *widget = nullptr;
    if (ptrTuple && PyTuple_Check(ptrTuple) && PyTuple_Size(ptrTuple) > 0) {
        unsigned long long address = PyLong_AsUnsignedLongLong(PyTuple_GetItem(ptrTuple, 0));
        widget = reinterpret_cast<QWidget *>(address);
    } else {
        qWarning() << "Konnte C++-Pointer nicht aus Python-Widget extrahieren:" << pyErrorString();
    }
    Py_XDECREF(ptrTuple);

    if (!widget) {
        Py_DECREF(widgetObj);
        PyGILState_Release(gil);
        return nullptr;
    }

    // WICHTIG: widgetObj NICHT freigeben (kein Py_DECREF hier)!
    m_widgetRefs.insert(widget, widgetObj);
    connect(widget, &QObject::destroyed, this, [this, widget]() {
        PyGILState_STATE innerGil = PyGILState_Ensure();
        PyObject *obj = m_widgetRefs.take(widget);
        Py_XDECREF(obj);
        PyGILState_Release(innerGil);
    });

    PyGILState_Release(gil);

    if (parent) {
        widget->setParent(parent);
    }

    return widget;
}