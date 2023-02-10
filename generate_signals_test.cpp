#include "generate_signals_test.h"

#include <generate_signals.hpp>

#include <QMetaObject>
#include <QObject>
#include <iostream>

Test::Test(QObject *parent)
  : QObject(parent)
{
}

int
main(void)
{
    auto qmlstorage = generate_signals::QmlMessageStroage();
    qmlstorage.get_message<Test>();
    qmlstorage.get_message<QObject>();
    qDebug() << qmlstorage.toJson();
}
