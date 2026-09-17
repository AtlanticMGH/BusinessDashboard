#include "kundenbearbeitenwindow.h"
#include "ui_kundenbearbeitenwindow.h"
kundenBearbeitenWindow::kundenBearbeitenWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kundenBearbeitenWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}
QString kundenBearbeitenWindow::getName() const
{
    return ui->nameTextEdit->text();
}
QString kundenBearbeitenWindow::getBranche() const
{
    return ui->brancheTextEdit->text();
}
double kundenBearbeitenWindow::getPreis() const
{
    return ui->doubleSpinBox->value();
}
QString kundenBearbeitenWindow::getAuftragsart() const
{
    return ui->comboBox->currentText();
}
QString kundenBearbeitenWindow::getAdresse() const
{
    return ui->adresseTextEdit->text();
}
QString kundenBearbeitenWindow::getBeschreibung() const
{
    return ui->beschreibungTextEdit->toPlainText();
}
void kundenBearbeitenWindow::setName(const QString& name)
{
    ui->nameTextEdit->setText(name);
}
void kundenBearbeitenWindow::setBranche(const QString& branche)
{
    ui->brancheTextEdit->setText(branche);
}
void kundenBearbeitenWindow::setPreis(double preis)
{
    ui->doubleSpinBox->setValue(preis);
}
void kundenBearbeitenWindow::setAuftragsart(const QString& auftragsart)
{
    int index = ui->comboBox->findText(auftragsart);
    if (index >= 0) {
        ui->comboBox->setCurrentIndex(index);
    }
}
void kundenBearbeitenWindow::setAdresse(const QString& adresse)
{
    ui->adresseTextEdit->setText(adresse);
}
void kundenBearbeitenWindow::setBeschreibung(const QString& beschreibung)
{
    ui->beschreibungTextEdit->setPlainText(beschreibung);
}
void kundenBearbeitenWindow::setKundenDaten(const QString& name, const QString& branche,
                                             double preis, const QString& auftragsart,
                                             const QString& adresse, const QString& beschreibung)
{
    setName(name);
    setBranche(branche);
    setPreis(preis);
    setAuftragsart(auftragsart);
    setAdresse(adresse);
    setBeschreibung(beschreibung);
}
kundenBearbeitenWindow::~kundenBearbeitenWindow()
{
    delete ui;
}
