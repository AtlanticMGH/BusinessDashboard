#pragma once

#include <QObject>
#include <QtPlugin>
#include "PluginInterface.h"

class TestPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "de.business.PluginInterface/1.0" FILE "testPlugin.json")
    Q_INTERFACES(PluginInterface)

public:
    QString getPluginName() const override;
    QString getPluginVersion() const override;
    QWidget* createWidget(QWidget *parent = nullptr) override;
};