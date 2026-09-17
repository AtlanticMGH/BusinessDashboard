#include <Python.h>
#include "PythonBridge.h"
#include <QDebug>

PythonBridge &PythonBridge::instance()
{
    static PythonBridge bridge;
    return bridge;
}

bool PythonBridge::initialize(const QString &pluginsDir)
{
    if (m_initialized)
        return true;

    Py_Initialize();

    // Plugin-Ordner in sys.path aufnehmen, damit `import meinplugin` klappt,
    // ohne dass Plugin-Autoren irgendetwas installieren müssen.
    PyObject *sysPath = PySys_GetObject("path"); // Borrowed reference
    PyObject *dirStr = PyUnicode_FromString(pluginsDir.toUtf8().constData());
    PyList_Append(sysPath, dirStr);
    Py_DECREF(dirStr);

    if (PyErr_Occurred()) {
        PyErr_Print();
        qWarning() << "Python-Interpreter konnte nicht initialisiert werden.";
        return false;
    }

    m_initialized = true;
    qDebug() << "Python-Interpreter gestartet, Plugin-Pfad:" << pluginsDir;
    return true;
}

void PythonBridge::shutdown()
{
    if (!m_initialized)
        return;

    Py_Finalize();
    m_initialized = false;
}
