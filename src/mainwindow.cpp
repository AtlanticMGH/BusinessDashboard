#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include "kundenuebersicht.h"
#include "sql_manager.h"
#include "auftrag.h"
#include "kundenHinzufuegenWindow.h"
#include <QDebug>
#include <QMessageBox>
#include "kundenbearbeitenwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pluginManager = new PluginManager(this, ui->menubar, this);
    m_pluginManager->loadPlugins();
    m_pluginManager->setupExtrasMenu();

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->tableWidget, &QTableWidget::customContextMenuRequested,
                this, &MainWindow::onTableWidgetContextMenu);

    connect(ui->addButton, &QPushButton::clicked,
                this, &MainWindow::onAddButtonClicked);

    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &MainWindow::onSearchTextChanged);

    this->setStyleSheet(
        // 1. Fenster und Tabellen-Hintergrund in klassischem Dunkelgrau
        "QMainWindow {"
        "    background-color: #2b2b2b;"
        "}"

        "QTableWidget {"
        "    background-color: #262626;"           // Sehr dunkles Grau für die Tabelle
        "    alternate-background-color: #2d2d2d;" // Etwas helleres Grau für den Zebra-Look
        "    color: white;"                        // Weiße Schrift
        "    gridline-color: #3f3f3f;"             // Neutrale, graue Gitterlinien
        "    border: 1px solid #3f3f3f;"
        "    selection-background-color: #8344ad;" // Das Firmen-Violett bleibt für Markierungen!
        "    selection-color: white;"
        "}"

        // 2. Spaltenüberschriften mit Firmen-Akzenten (Cyan & Violett)
        "QHeaderView::section {"
        "    background-color: #2d2d2d;"         // Passt sich dem grauen Theme an
        "    color: #29b6f6;"                    // Firmen-Cyan für den Text
        "    font-weight: bold;"
        "    font-size: 13px;"
        "    border: none;"
        "    border-right: 1px solid #3f3f3f;"
        "    border-bottom: 2px solid #8344ad;"  // Violetter Firmen-Unterstrich
        "    padding: 6px;"
        "}"

        // 3. Ecke oben links anpassen
        "QTableCornerButton::section {"
        "    background-color: #2d2d2d;"
        "    border: none;"
        "}"
    );

    setWindowTitle("Auftragsübersicht");

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);

    // Datenabfrage
    QList<Auftrag> auftragsListe = SqlManager::instance().getAllCustomerShortData();

    //Zeige Daten an
    ui->tableWidget->setRowCount(auftragsListe.size());
    for (int zeile = 0; zeile < auftragsListe.size(); ++zeile) {
        const Auftrag &aktuellerAuftrag = auftragsListe[zeile];
        ui->tableWidget->setItem(zeile, 0, new QTableWidgetItem(aktuellerAuftrag.kunde));
        ui->tableWidget->setItem(zeile, 1, new QTableWidgetItem(aktuellerAuftrag.branche));
        QString preisText = QString::number(aktuellerAuftrag.preis, 'f', 2) + " €";
        ui->tableWidget->setItem(zeile, 2, new QTableWidgetItem(preisText));
        ui->tableWidget->setItem(zeile, 3, new QTableWidgetItem(aktuellerAuftrag.auftrag));
    }

    // Wenn doppelt auf eine Zeile geklickt wird, öffne das Detailfenster
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, [=](int row, int column) {
        Q_UNUSED(column); // Spalte ignorieren, uns interessiert die ganze Zeile
        zeigeKundenDetails(row);
    });
}


void MainWindow::on_actionHinzuefegen_triggered()
{
    kundeHinzufuegen();
}

void MainWindow::onTableWidgetContextMenu(const QPoint &pos)
{
    // Welches Item wurde angeklickt?
    QTableWidgetItem *item = ui->tableWidget->itemAt(pos);
    if (!item) {
        return;   // Rechtsklick auf leere Fläche → kein Menü
    }

    int row = item->row();
    QString kundenName = ui->tableWidget->item(row, 0)->text();

    QMenu contextMenu(this);
    QAction *bearbeitenAction = contextMenu.addAction("Bearbeiten");
    QAction *loeschenAction = contextMenu.addAction("Löschen");
    contextMenu.addSeparator();
    QAction *detailsAction = contextMenu.addAction("Details anzeigen");

    // Globale Position berechnen (pos ist relativ zum Widget)
    QAction *selectedAction = contextMenu.exec(ui->tableWidget->viewport()->mapToGlobal(pos));

    if (selectedAction == bearbeitenAction) {
        // z.B. Bearbeiten-Dialog öffnen
        kundeBearbeiten(row);
    } else if (selectedAction == loeschenAction) {
        QString kunde = ui->tableWidget->item(row, 0)->text();
        std::vector<QString> customerShortData = SqlManager::instance().getCustomerShortData(kunde);
        if (SqlManager::instance().deleteCustomer(customerShortData[4].toInt())) {
                ui->tableWidget->removeRow(row);
            } else {
                QMessageBox::warning(this, "Fehler", "Kunde konnte nicht gelöscht werden.");
            }
        ui->tableWidget->removeRow(row);
    } else if (selectedAction == detailsAction) {
        zeigeKundenDetails(row);
    }
}

void MainWindow::zeigeKundenDetails(int row)
{
    if (row < 0 || !ui->tableWidget->item(row, 0)) {
        return;
    }

    QString kunde   = ui->tableWidget->item(row, 0)->text();
    QString branche = ui->tableWidget->item(row, 1)->text();
    QString preis   = ui->tableWidget->item(row, 2)->text();
    QString auftrag = ui->tableWidget->item(row, 3)->text();

    std::vector<QString> customerShortData = SqlManager::instance().getCustomerShortData(kunde);

    if (customerShortData.size() < 5) {
        QMessageBox::warning(this, "Fehler", "Kundendaten konnten nicht geladen werden.");
        return;
    }

    KundenUebersicht detailWindow(this);
    detailWindow.setAuftragsDetails(customerShortData[0], customerShortData[1],
                                     customerShortData[2], customerShortData[3],
                                     customerShortData[4].toInt());
    detailWindow.exec();
}

void MainWindow::kundeBearbeiten(int row)
{
    if (row < 0 || !ui->tableWidget->item(row, 0)) {
        return;
    }

    QString kunde = ui->tableWidget->item(row, 0)->text();

    // Kurzdaten (Name, Branche, Preis, Auftragsart, KundenID) abfragen
    std::vector<QString> shortData = SqlManager::instance().getCustomerShortData(kunde);
    if (shortData.size() < 5) {
        QMessageBox::warning(this, "Fehler", "Kundendaten konnten nicht geladen werden.");
        return;
    }

    int kundenID = shortData[4].toInt();

    // Langdaten (Adresse, Beschreibung) anhand der KundenID abfragen
    std::vector<QString> longData = SqlManager::instance().getCustomerLongData(kundenID);
    if (longData.size() < 2) {
        QMessageBox::warning(this, "Fehler", "Kundendaten konnten nicht geladen werden.");
        return;
    }

    kundenBearbeitenWindow dialog(this);

    // Dialogfelder vorbefüllen
    dialog.setKundenDaten(shortData[0],            // name
                          shortData[1],            // branche
                          shortData[2].toDouble(), // preis
                          shortData[3],            // auftragsart
                          longData[1],             // adresse
                          longData[0]);             // beschreibung

    while (dialog.exec() == QDialog::Accepted) {
        QString name         = dialog.getName();
        QString branche      = dialog.getBranche();
        double  preis        = dialog.getPreis();
        QString auftragsart  = dialog.getAuftragsart();
        QString adresse      = dialog.getAdresse();
        QString beschreibung = dialog.getBeschreibung();

        if (SqlManager::instance().updateCustomer(name, branche, preis, auftragsart,
                                                    adresse, beschreibung, kundenID)) {
            // Erfolg: Zeile in der Tabelle aktualisieren
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(branche));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(preis, 'f', 2) + " €"));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(auftragsart));
            break;
            onSearchTextChanged(ui->searchEdit->text());
        } else {
            QMessageBox::warning(this, "Fehler",
                "Die Änderungen konnten nicht gespeichert werden.\nBitte versuchen Sie es erneut.");
        }
    }
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    const QString suchtext = text.trimmed();

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *nameItem = ui->tableWidget->item(row, 0); // Spalte "Kunde"
        if (!nameItem) {
            continue;
        }

        bool treffer = suchtext.isEmpty()
                       || nameItem->text().contains(suchtext, Qt::CaseInsensitive);

        ui->tableWidget->setRowHidden(row, !treffer);
    }
}

void MainWindow::onAddButtonClicked()
{
    kundeHinzufuegen();
}

void MainWindow::kundeHinzufuegen(){
    kundenHinzufuegenWindow dialog(this);

    while (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName();
        QString branche = dialog.getBranche();
        double preis = dialog.getPreis();
        QString auftragsart = dialog.getAuftragsart();
        QString adresse = dialog.getAdresse();
        QString beschreibung = dialog.getBeschreibung();

        if (SqlManager::instance().addCustomer(name, branche, preis, auftragsart, adresse, beschreibung)) {
            // Erfolg: Tabelle aktualisieren und Schleife verlassen
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(branche));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(preis, 'f', 2)));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(auftragsart));
            break;   // fertig, Schleife verlassen
            onSearchTextChanged(ui->searchEdit->text());
        } else {
            // Fehler: Nutzer informieren, Dialog bleibt mit den Daten offen (nächster Schleifendurchlauf)
            QMessageBox::warning(this, "Fehler",
                "Der Kunde konnte nicht gespeichert werden.\nBitte versuchen Sie es erneut.");
            // kein break/return → while-Schleife ruft dialog.exec() erneut auf
        }
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}
