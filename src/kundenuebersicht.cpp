#include "kundenuebersicht.h"
#include "ui_kundenuebersicht.h"
#include "sql_manager.h"
#include <QDesktopServices>
#include <QUrl>
#include <vector>

KundenUebersicht::KundenUebersicht(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KundenUebersicht)
{
    ui->setupUi(this);

    ui->listWidgetDateien->addItem("Angebot_Website.pdf");

    connect(ui->listWidgetDateien, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item) {

        QString dateiName = item->text();

        QString ordnerPfad = "";

        QString kompletterDateipfad = ordnerPfad + dateiName;

        QDesktopServices::openUrl(QUrl::fromLocalFile(kompletterDateipfad));
    });
}




KundenUebersicht::~KundenUebersicht()
{
    delete ui;
}

void KundenUebersicht::setAuftragsDetails(const QString &kunde, const QString &branche, const QString &preis, const QString &auftrag, const int ID)
{
    ui->lblKunde->setText("Kunde: " + kunde);
    ui->lblBranche->setText("Branche: " + branche);
    ui->lblPreis->setText("Preis: " + preis);
    ui->lblAuftrag->setText("Auftrag: " + auftrag);

    std::vector<QString> customerLongData = SqlManager::instance().getCustomerLongData(ID);


    ui->lblBeschreibung->setText("Beschreibung: " + customerLongData[0]);
}
