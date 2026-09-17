#pragma once

#include <QObject>
#include <QHash>
#include "PluginInterface.h"

struct _object;
typedef _object PyObject;

class PythonPluginAdapter : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:
    static PythonPluginAdapter *load(const QString &scriptPath, QObject *parent = nullptr);

    ~PythonPluginAdapter() override;

    QString getPluginName() const override;
    QString getPluginVersion() const override;
    QWidget *createWidget(QWidget *parent = nullptr) override;

private:
    explicit PythonPluginAdapter(PyObject *instance, QObject *parent = nullptr);

    PyObject *m_instance = nullptr; // starke Referenz auf das Python-Plugin-Objekt

    // Hält je erzeugtem Widget die zugehörige Python-Objektreferenz am Leben,
    // bis Qt das Widget zerstört (siehe createWidget()/destroyed-Handler in .cpp).
    // Verhindert, dass Pythons GC das C++-Objekt vorzeitig einsammelt.
    QHash<QWidget*, PyObject*> m_widgetRefs;
};