#include "sql_manager.h"
#include "env_loader.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QStandardPaths>
#include <QSqlRecord>
#include <QDir>
#include <vector>
#include "auftrag.h"
#include <QDebug>

SqlManager& SqlManager::instance()
{
    static SqlManager instance;
    return instance;
}

bool SqlManager::open()
{
    // Falls schon eine Verbindung existiert, erst mal aufräumen
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QMYSQL");
    }

    m_db.setHostName(EnvLoader::instance().value("DB_HOST", "localhost"));
    m_db.setPort(3306);
    m_db.setDatabaseName(EnvLoader::instance().value("DB_NAME"));
    m_db.setUserName(EnvLoader::instance().value("DB_USER"));
    m_db.setPassword(EnvLoader::instance().value("DB_PASS"));

    QString ordner = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(ordner);

    QString zertifikatPfad = ordner + "/server_cert.pem";
    QString connectOptions = QString("SSL_CA=%1;MYSQL_OPT_SSL_VERIFY_SERVER_CERT=0").arg(zertifikatPfad);
    m_db.setConnectOptions(connectOptions);

    if (!m_db.open()) {
        qWarning() << "DB-Verbindung fehlgeschlagen:" << m_db.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery SqlManager::executeQuery(const QString& sql, const QMap<QString, QVariant>& bindings){
    if (!m_db.isOpen() && !open()) {
        qWarning() << "Keine DB-Verbindung verfügbar";
        return QSqlQuery();
    }

    QSqlQuery query(SqlManager::instance().database());
    query.prepare(sql);

    for(auto it = bindings.constBegin(); it != bindings.constEnd(); ++it){
        query.bindValue(it.key(), it.value());
    }
    if(!query.exec()){
        qWarning()  << "Query fehlgeschlagen:" << query.lastError().text()
                    << "SQL:" << sql;
    }
    return query;
}

std::vector<QString> SqlManager::getCustomerShortData(QString KundenName)
{
    std::vector<QString> queryVec;
    QSqlQuery query = executeQuery(
                    "SELECT * FROM KundenKurzUebersicht WHERE Name = :name",
                {{":name", KundenName}}
                );

    int spaltenAnzahl = query.record().count();
    while (query.next()) {
        for (int i = 0; i < spaltenAnzahl; ++i) {
            queryVec.push_back(query.value(i).toString());
        }
    }
    return queryVec;
}

QList<Auftrag> SqlManager::getAllCustomerShortData()
{
    QList<Auftrag> auftraege;
    QSqlQuery query = executeQuery("SELECT Name, Branche, Preis, Auftrag, KundenID FROM KundenKurzUebersicht", {{}});
    while (query.next()) {
        Auftrag a;
        a.kunde    = query.value("Name").toString();
        a.branche  = query.value("Branche").toString();
        a.preis    = query.value("Preis").toDouble();
        a.auftrag  = query.value("Auftrag").toString();
        a.userID   = query.value("KundenID").toString();

        auftraege.append(a);
    }
    return auftraege;
}

std::vector<QString> SqlManager::getCustomerLongData(int KundenID){
    std::vector<QString> queryVec;
    QSqlQuery query = executeQuery("SELECT * FROM KundenBeschreibung WHERE KundenID = :ID",
                                   {{":ID", KundenID}});
    int spaltenAnzahl = query.record().count();
    while (query.next()) {
        for (int i = 0; i < spaltenAnzahl; ++i) {
            queryVec.push_back(query.value(i).toString());
        }
    }
    return queryVec;
}

std::optional<int> SqlManager::getLastCustomerID(){
    int lastID;
    QSqlQuery query = executeQuery("SELECT MAX(KundenID) FROM KundenBeschreibung", {{}});
    if (query.next()) {
        //qDebug() << query.value(0);
        return query.value(0).toInt();
    }
    return std::nullopt;
}

void SqlManager::insertShortCustomerInfo(QString name, QString branche, double preis, QString auftragsArt, int customerID){
    QSqlQuery query = executeQuery("INSERT INTO KundenKurzUebersicht (Name, Branche, Preis, Auftrag, KundenID) VALUES (:name, :branche, :preis, :auftragsArt, :kundenID)",
                                   {{":name", name}, {":branche", branche}, {":preis", preis}, {":auftragsArt", auftragsArt}, {":kundenID", customerID}});
}

void SqlManager::insertLongCustomerInfo(QString adresse, QString beschreibung, int customerID){
    QSqlQuery query = executeQuery("INSERT INTO KundenBeschreibung (Beschreibung, Adresse, KundenID) VALUES (:beschreibung, :adresse, :kundenID)",
                                   {{":beschreibung", beschreibung}, {":adresse", adresse}, {":kundenID", customerID}});
}

bool SqlManager::addCustomer(QString name, QString branche, double preis, QString auftragsArt, QString adresse, QString beschreibung){
    std::optional<int> lastID = getLastCustomerID();
    //qDebug() << "letzte ID der Datenbank:" << customerID;

    if(!lastID.has_value()){
        qWarning() << "KundenID konnte nicht ermittelt werden!";
        return false;
    }

    //customerID erhöhen, für neuen kunden
    int customerID = lastID.value() + 1;
    insertShortCustomerInfo(name, branche, preis, auftragsArt, customerID);
    insertLongCustomerInfo(adresse, beschreibung, customerID);
    return true;
}

bool SqlManager::deleteCustomer(int KundenID)
{
    if (!open()) return false;

    m_db.transaction();

    QSqlQuery query1 = executeQuery("DELETE FROM KundenBeschreibung WHERE KundenID = :ID",
                                     {{":ID", KundenID}});
    QSqlQuery query2 = executeQuery("DELETE FROM KundenKurzUebersicht WHERE KundenID = :ID",
                                     {{":ID", KundenID}});

    bool ok = query1.lastError().type() == QSqlError::NoError
           && query2.lastError().type() == QSqlError::NoError;

    if (ok) {
        return m_db.commit();
    } else {
        m_db.rollback();
        return false;
    }
}

bool SqlManager::updateCustomer(QString name, QString branche, double preis, QString auftragsArt, QString adresse, QString beschreibung, int KundenID){
    if (!open()) return false;
    m_db.transaction();

    QSqlQuery query1 = executeQuery("UPDATE KundenKurzUebersicht SET Name = :name, Branche = :branche, Preis = :preis, Auftrag = :auftragsart WHERE KundenID = :ID",
                                         {{":name", name},{":branche", branche},{":preis", preis},{":auftragsart", auftragsArt},{":ID", KundenID}});

    QSqlQuery query2 = executeQuery("UPDATE KundenBeschreibung SET Adresse = :adresse, Beschreibung = :beschreibung WHERE KundenID = :ID",
                                     {{":adresse", adresse},{":beschreibung", beschreibung},{":ID", KundenID}});

    bool ok = query1.lastError().type() == QSqlError::NoError
           && query2.lastError().type() == QSqlError::NoError;

    if (ok) {
        return m_db.commit();
    } else {
        m_db.rollback();
        return false;
    }
}

void SqlManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool SqlManager::isOpen() const
{
    return m_db.isOpen();
}

QSqlDatabase& SqlManager::database()
{
    return m_db;
}