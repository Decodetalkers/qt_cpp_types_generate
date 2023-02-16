#include "generate_signals_test.h"
#include <generate_signals.hpp>

#include <QCoreApplication>
#include <QMetaObject>
#include <QObject>
#include <iostream>

Test::Test(QObject *parent)
  : QObject(parent)
{
}

int
main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    auto qmlstorage = generate_signals::QmlMessageStroage();
    qmlstorage.get_message<Test>(
      "im.nheko", generate_signals::RegisterType::SingleTon, 1, 0, "test");
    qmlstorage.get_message<Test>(
      "im.nheko", generate_signals::RegisterType::SingleTon, 1, 0, "test2");
    QObject::connect(
      &qmlstorage,
      &generate_signals::QmlMessageStroage::quit,
      &app,
      [&app](int code) {
          if (code == 0) {
              qDebug() << "Cannot generate";
          }
          app.quit();
      },
      Qt::QueuedConnection);
    qmlstorage.writeToFile();
    return app.exec();
}
