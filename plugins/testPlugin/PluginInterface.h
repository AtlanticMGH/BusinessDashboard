#pragma once
#include <QString>
#include <QWidget>
#include <QtPlugin>

class PluginInterface {
public:
    virtual ~PluginInterface() = default;

    virtual QString getPluginName() const = 0;
    virtual QString getPluginVersion() const = 0;   // hilfreich für Debugging/Logging
    virtual QWidget* createWidget(QWidget *parent = nullptr) = 0;
};

Q_DECLARE_INTERFACE(PluginInterface, "de.business.PluginInterface/1.0")
