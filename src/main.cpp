#include "mainwindow.h"
#include "env_loader.h"
#include "sql_manager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QFile>
#include <QTextStream>
#include "PythonBridge.h"
#include <QCoreApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PythonBridge::instance().initialize(
            QDir(QCoreApplication::applicationDirPath()).filePath("plugins"));

    QString envPath = EnvLoader::instance().configPath();
    if (!EnvLoader::instance().load(envPath)) {
        qWarning() << "Keine Konfiguration gefunden unter:" << envPath;
    }
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BusinessDashboard_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    //Stylesheet laden
    QFile styleFile(":/styles/business-style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
        styleFile.close();
    } else {
        qWarning() << "Stylesheet konnte nicht geladen werden: :/styles/business-style.qss";
    }


    MainWindow w;
    w.show();
    SqlManager::instance().close();
    int result = QApplication::exec();
    PythonBridge::instance().shutdown();
    return result;
}
