#ifndef SQL_MANAGER_H
#define SQL_MANAGER_H

#include <QSqlDatabase>
#include <QString>
#include "auftrag.h"

class SqlManager
{
public:
    static SqlManager& instance();

    bool open();
    void close();
    bool isOpen() const;
    std::vector<QString> getCustomerShortData(QString KundenName);
    std::vector<QString> getCustomerLongData(int KundenID);
    QList<Auftrag> getAllCustomerShortData();
    bool addCustomer(QString name, QString branche, double preis, QString auftragsArt, QString adresse, QString beschribung);
    bool deleteCustomer(int KundenID);
    QSqlDatabase& database();
    bool updateCustomer(QString name, QString branche, double preis, QString auftragsArt,
                         QString adresse, QString beschreibung, int KundenID);


private:
    SqlManager() = default;
    ~SqlManager() = default;
    SqlManager(const SqlManager&) = delete;
    SqlManager& operator=(const SqlManager&) = delete;
    std::optional<int> getLastCustomerID();
    void insertShortCustomerInfo(QString name, QString branche, double preis, QString auftragsArt, int customerID);
    void insertLongCustomerInfo(QString adresse, QString beschreibung, int customerID);
    QSqlQuery executeQuery(const QString& sql, const QMap<QString, QVariant>& bindings);

    QSqlDatabase m_db;
};

#endif // SQL_MANAGER_H