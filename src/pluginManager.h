#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QWidget>
#include <QPluginLoader>
#include "PluginInterface.h"

struct LoadedPlugin {
    QPluginLoader *loader;
    PluginInterface *interface;
    QString filePath;
};

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QWidget *parentWidget, QMenuBar *menuBar, QObject *parent = nullptr);
    ~PluginManager() override;

    void loadPlugins();
    void setupExtrasMenu();

private:
    QString pluginsDirPath() const;
    void loadSinglePlugin(const QString &filePath);
    void refreshPluginList();
    void addPluginFile();
    void removePlugin(int index);
    void openPluginWidget(int index);
    void showPluginContextMenu(const QPoint &pos);

    QWidget *m_parentWidget = nullptr;   // zum Parenten von Dialogen/MessageBoxen
    QMenuBar *m_menuBar = nullptr;
    QMenu *m_extrasMenu = nullptr;
    QAction *m_addPluginAction = nullptr;
    QAction *m_separatorAction = nullptr;
    QList<LoadedPlugin> m_loadedPlugins;

    void loadSinglePythonPlugin(const QString &filePath);
};

#endif // PLUGINMANAGER_H