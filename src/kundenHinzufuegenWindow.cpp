#include "kundenHinzufuegenWindow.h"
#include "ui_kundenHinzufuegenWindow.h"
kundenHinzufuegenWindow::kundenHinzufuegenWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kundenHinzufuegenWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}
QString kundenHinzufuegenWindow::getName() const
{
    return ui->nameTextEdit->text();
}
QString kundenHinzufuegenWindow::getBranche() const
{
    return ui->brancheTextEdit->text();
}
double kundenHinzufuegenWindow::getPreis() const
{
    return ui->doubleSpinBox->value();
}
QString kundenHinzufuegenWindow::getAuftragsart() const
{
    return ui->comboBox->currentText();
}
QString kundenHinzufuegenWindow::getAdresse() const
{
    return ui->adresseTextEdit->text();
}
QString kundenHinzufuegenWindow::getBeschreibung() const
{
    return ui->beschreibungTextEdit->toPlainText();
}
kundenHinzufuegenWindow::~kundenHinzufuegenWindow()
{
    delete ui;
}
