#ifndef ENV_LOADER_H
#define ENV_LOADER_H

#include <QString>
#include <QMap>

class EnvLoader
{
public:
    static EnvLoader& instance();

    bool load(const QString& filePath = ".env");
    QString value(const QString& key, const QString& defaultValue = QString()) const;
    QString configPath();

private:
    EnvLoader() = default;
    QMap<QString, QString> m_values;
};

#endif // ENV_LOADER_H