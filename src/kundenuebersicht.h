#ifndef KUNDENUEBERSICHT_H
#define KUNDENUEBERSICHT_H

#include <QDialog>

namespace Ui {
class KundenUebersicht;
}

class KundenUebersicht : public QDialog
{
    Q_OBJECT

public:
    explicit KundenUebersicht(QWidget *parent = nullptr);
    ~KundenUebersicht();

    void setAuftragsDetails(const QString &kunde, const QString &branche, const QString &preis, const QString &auftrag, const int ID);

private:
    Ui::KundenUebersicht *ui;
};

#endif // KUNDENUEBERSICHT_H
