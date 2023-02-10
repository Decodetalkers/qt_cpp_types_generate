#include "Test.h"
#include <QObject>
#include <generate_signals.hpp>
#include <iostream>
#include <QMetaObject>
Test::Test(QObject *parent)
  : QObject(parent)
{
}

int
main(int argc, char **argv)
{
    auto qmlstorage = generate_signals::QmlMessageStroage();
    qmlstorage.get_message<Test>();
    qmlstorage.get_message<QObject>();
    qDebug() << qmlstorage.toJson();
}
