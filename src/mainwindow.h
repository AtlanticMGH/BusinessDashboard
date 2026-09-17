#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QPluginLoader>
#include "pluginManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_actionHinzuefegen_triggered();
    void onTableWidgetContextMenu(const QPoint &pos);
    void kundeBearbeiten(int KundenID);
    void onAddButtonClicked();
    void onSearchTextChanged(const QString &text);


private:
    Ui::MainWindow *ui;
    void zeigeKundenDetails(int row);
    void kundeHinzufuegen();
    PluginManager *m_pluginManager = nullptr;

};
#endif // MAINWINDOW_H
