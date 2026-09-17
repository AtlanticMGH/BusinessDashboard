#include "env_loader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>


EnvLoader& EnvLoader::instance()
{
    static EnvLoader instance;
    return instance;
}

bool EnvLoader::load(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Konnte .env Datei nicht öffnen:" << filePath;
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Leere Zeilen und Kommentare überspringen
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        int splitIndex = line.indexOf('=');
        if (splitIndex == -1) {
            continue;
        }

        QString key = line.left(splitIndex).trimmed();
        QString value = line.mid(splitIndex + 1).trimmed();

        // Optional: Anführungszeichen um den Wert entfernen
        if (value.startsWith('"') && value.endsWith('"')) {
            value = value.mid(1, value.length() - 2);
        }

        m_values.insert(key, value);
    }

    file.close();
    return true;
}

QString EnvLoader::configPath()
{
    QString ordner = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(ordner);  // Ordner anlegen, falls nicht vorhanden
    return ordner + "/.env";
}

QString EnvLoader::value(const QString& key, const QString& defaultValue) const
{
    return m_values.value(key, defaultValue);
}