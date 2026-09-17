#include "pluginManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QFileDialog>
#include "PluginInterface.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include "pythonPluginAdapter.h"

PluginManager::PluginManager(QWidget *parentWidget, QMenuBar *menuBar, QObject *parent)
    : QObject(parent)
    , m_parentWidget(parentWidget)
    , m_menuBar(menuBar)
{
}

PluginManager::~PluginManager()
{
    for (const LoadedPlugin &lp : std::as_const(m_loadedPlugins)) {
        if (lp.loader) {
            lp.loader->unload();
        } else if (auto *obj = dynamic_cast<QObject*>(lp.interface)) {
            obj->deleteLater();
        }
    }
}

QString PluginManager::pluginsDirPath() const
{
    return QDir(QCoreApplication::applicationDirPath()).filePath("plugins");
}

void PluginManager::loadPlugins()
{
    QDir pluginsDir(pluginsDirPath());
    if (!pluginsDir.exists()) {
        pluginsDir.mkpath(".");
    }

    const QStringList entries = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entries) {
        const QString filePath = pluginsDir.absoluteFilePath(fileName);
        if (fileName.endsWith(".py")) {
            if (fileName == "plugin_base.py")
                continue; // Basisklasse, kein eigenständiges Plugin
            loadSinglePythonPlugin(filePath);
        } else if (QLibrary::isLibrary(fileName)) {
            loadSinglePlugin(filePath);
        }
    }
}

void PluginManager::loadSinglePythonPlugin(const QString &filePath)
{
    auto *adapter = PythonPluginAdapter::load(filePath, this);
    if (!adapter) {
        QMessageBox::warning(m_parentWidget, tr("Plugin-Fehler"),
                              tr("Python-Plugin konnte nicht geladen werden:\n%1").arg(filePath));
        return;
    }
    qDebug() << "Python-Plugin geladen:" << adapter->getPluginName();
    m_loadedPlugins.append({nullptr, adapter, filePath}); // loader = nullptr!
}

void PluginManager::loadSinglePlugin(const QString &filePath)
{
    auto *loader = new QPluginLoader(filePath, this);
    QObject *pluginInstance = loader->instance();

    if (!pluginInstance) {
        qWarning() << "Fehler beim Laden von" << filePath << ":" << loader->errorString();
        QMessageBox::warning(m_parentWidget, tr("Plugin-Fehler"),
                              tr("Konnte Plugin nicht laden:\n%1").arg(loader->errorString()));
        loader->deleteLater();
        return;
    }

    if (auto *interface = qobject_cast<PluginInterface*>(pluginInstance)) {
        qDebug() << "Plugin geladen:" << interface->getPluginName();
        m_loadedPlugins.append({loader, interface, filePath});
    } else {
        qWarning() << "Falsches Interface:" << filePath;
        QMessageBox::warning(m_parentWidget, tr("Plugin-Fehler"),
                              tr("Die Datei ist kein gültiges Business-Plugin."));
        loader->unload();
        loader->deleteLater();
    }
}

void PluginManager::setupExtrasMenu()
{
    m_extrasMenu = m_menuBar->addMenu(tr("&Extras"));

    // Rechtsklick auf einen Eintrag soll ein Kontextmenü öffnen (zum Entfernen)
    m_extrasMenu->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_extrasMenu, &QMenu::customContextMenuRequested,
            this, &PluginManager::showPluginContextMenu);

    m_addPluginAction = m_extrasMenu->addAction(tr("Plugin hinzufügen …"));
    connect(m_addPluginAction, &QAction::triggered, this, &PluginManager::addPluginFile);

    m_separatorAction = m_extrasMenu->addSeparator();

    refreshPluginList();
}

void PluginManager::refreshPluginList()
{
    // Alle Einträge nach dem Separator entfernen, dann neu aufbauen
    const QList<QAction*> actions = m_extrasMenu->actions();
    for (QAction *action : actions) {
        if (action == m_addPluginAction || action == m_separatorAction)
            continue;
        m_extrasMenu->removeAction(action);
        action->deleteLater();
    }

    if (m_loadedPlugins.isEmpty()) {
        QAction *noneAction = m_extrasMenu->addAction(tr("Keine Plugins geladen"));
        noneAction->setEnabled(false);
        return;
    }

    for (int i = 0; i < m_loadedPlugins.size(); ++i) {
        QAction *action = m_extrasMenu->addAction(m_loadedPlugins[i].interface->getPluginName());
        // Index am QAction speichern, damit wir ihn im Kontextmenü wiederfinden
        action->setData(i);
        connect(action, &QAction::triggered, this, [this, i]() { openPluginWidget(i); });
    }
}

void PluginManager::showPluginContextMenu(const QPoint &pos)
{
    QAction *action = m_extrasMenu->actionAt(pos);
    if (!action || !action->data().isValid())
        return;   // Rechtsklick war nicht auf einem Plugin-Eintrag

    int index = action->data().toInt();
    if (index < 0 || index >= m_loadedPlugins.size())
        return;

    QMenu contextMenu(m_parentWidget);
    QAction *removeAction = contextMenu.addAction(tr("Entfernen"));
    QAction *chosen = contextMenu.exec(m_extrasMenu->mapToGlobal(pos));

    if (chosen == removeAction) {
        removePlugin(index);
    }
}

void PluginManager::addPluginFile()
{
#if defined(Q_OS_WIN)
    const QString filter = tr("Plugin-Dateien (*.dll)");
#elif defined(Q_OS_MAC)
    const QString filter = tr("Plugin-Dateien (*.dylib)");
#else
    const QString filter = tr("Plugin-Dateien (*.so)");
#endif

    const QString sourcePath = QFileDialog::getOpenFileName(
        m_parentWidget, tr("Plugin auswählen"), QString(), filter);
    if (sourcePath.isEmpty())
        return;

    QDir pluginsDir(pluginsDirPath());
    const QString fileName = QFileInfo(sourcePath).fileName();
    const QString destPath = pluginsDir.absoluteFilePath(fileName);

    if (QFile::exists(destPath)) {
        QMessageBox::warning(m_parentWidget, tr("Plugin hinzufügen"),
                              tr("Ein Plugin mit diesem Dateinamen existiert bereits."));
        return;
    }

    if (!QFile::copy(sourcePath, destPath)) {
        QMessageBox::warning(m_parentWidget, tr("Plugin hinzufügen"),
                              tr("Datei konnte nicht kopiert werden."));
        return;
    }

    loadSinglePlugin(destPath);
    refreshPluginList();
}

void PluginManager::removePlugin(int index)
{
    if (index < 0 || index >= m_loadedPlugins.size())
        return;

    const LoadedPlugin lp = m_loadedPlugins[index];
    const QString name = lp.interface->getPluginName();

    auto reply = QMessageBox::question(m_parentWidget, tr("Plugin entfernen"),
        tr("Plugin \"%1\" wirklich entladen und löschen?").arg(name));
    if (reply != QMessageBox::Yes)
        return;

    if (lp.loader) {
        lp.loader->unload();
        lp.loader->deleteLater();
    } else if (auto *obj = dynamic_cast<QObject*>(lp.interface)) {
        obj->deleteLater();
    }

    if (!QFile::remove(lp.filePath)) {
        qWarning() << "Datei konnte nicht gelöscht werden:" << lp.filePath;
    }

    m_loadedPlugins.removeAt(index);
    refreshPluginList();
}

void PluginManager::openPluginWidget(int index)
{
    if (index < 0 || index >= m_loadedPlugins.size())
        return;

    PluginInterface *plugin = m_loadedPlugins[index].interface;

    auto *dialog = new QDialog(m_parentWidget);
    dialog->setWindowTitle(plugin->getPluginName());
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    auto *layout = new QVBoxLayout(dialog);
    QWidget *pluginUi = plugin->createWidget(dialog);
    if (pluginUi) {
        layout->addWidget(pluginUi);
    }

    dialog->resize(400, 300);
    dialog->show();
}
