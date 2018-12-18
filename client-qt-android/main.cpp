#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "myprob.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<MyProb>("MyProb", 1, 0, "MyProb");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
