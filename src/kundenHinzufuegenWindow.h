#ifndef KUNDENHINZUFUEGENWINDOW_H
#define KUNDENHINZUFUEGENWINDOW_H

#include <QDialog>

namespace Ui {
class kundenHinzufuegenWindow;
}

class kundenHinzufuegenWindow : public QDialog
{
    Q_OBJECT

public:
    explicit kundenHinzufuegenWindow(QWidget *parent = nullptr);
    ~kundenHinzufuegenWindow();

    QString getName() const;
    QString getBranche() const;
    double getPreis() const;
    QString getAuftragsart() const;
    QString getAdresse() const;
    QString getBeschreibung() const;


private:
    Ui::kundenHinzufuegenWindow *ui;
};

#endif // KUNDENHINZUFUEGENWINDOW_H
