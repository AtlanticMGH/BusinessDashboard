#pragma once
#include <QString>

/**
 * Verwaltet einen einzigen eingebetteten Python-Interpreter für die gesamte
 * Anwendung (CPython darf pro Prozess nur einmal initialisiert werden).
 *
 * Aufruf-Reihenfolge in main():
 *   1. initialize()  ganz am Anfang, vor MainWindow-Erzeugung
 *   2. app.exec()
 *   3. shutdown()    ganz am Ende, NACHDEM alle Python-Widgets bereits
 *                     zerstört wurden (also nach dem exec()-Rücksprung)
 */
class PythonBridge
{
public:
    static PythonBridge &instance();

    bool initialize(const QString &pluginsDir);
    void shutdown();

    bool isInitialized() const { return m_initialized; }

private:
    PythonBridge() = default;
    bool m_initialized = false;
};
