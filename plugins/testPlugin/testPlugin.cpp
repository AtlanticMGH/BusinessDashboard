#include "testPlugin.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

QString TestPlugin::getPluginName() const
{
    return QStringLiteral("Test-Plugin");
}

QString TestPlugin::getPluginVersion() const
{
    return QStringLiteral("1.0.0");
}

QWidget* TestPlugin::createWidget(QWidget *parent)
{
    auto *widget = new QWidget(parent);
    auto *layout = new QVBoxLayout(widget);

    auto *label = new QLabel(QStringLiteral("Hallo von TestPlugin!"), widget);
    label->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(label);

    auto *button = new QPushButton(QStringLiteral("Klick mich"), widget);
    layout->addWidget(button);

    QObject::connect(button, &QPushButton::clicked, widget, [widget]() {
        QMessageBox::information(widget, QStringLiteral("TestPlugin"),
                                  QStringLiteral("Das Plugin funktioniert!"));
    });

    return widget;
}