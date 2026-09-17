#ifndef KUNDENBEARBEITENWINDOW_H
#define KUNDENBEARBEITENWINDOW_H

#include <QDialog>

namespace Ui {
class kundenBearbeitenWindow;
}

class kundenBearbeitenWindow : public QDialog
{
    Q_OBJECT

public:
    explicit kundenBearbeitenWindow(QWidget *parent = nullptr);
    ~kundenBearbeitenWindow();

    QString getName() const;
    QString getBranche() const;
    double getPreis() const;
    QString getAuftragsart() const;
    QString getAdresse() const;
    QString getBeschreibung() const;
    void setName(const QString& name);
    void setBranche(const QString& branche);
    void setPreis(double preis);
    void setAuftragsart(const QString& auftragsart);
    void setAdresse(const QString& adresse);
    void setBeschreibung(const QString& beschreibung);

    void setKundenDaten(const QString& name, const QString& branche, double preis,
                             const QString& auftragsart, const QString& adresse,
                             const QString& beschreibung);


private:
    Ui::kundenBearbeitenWindow *ui;
};

#endif // KUNDENBEARBEITENWINDOW_H
