#include <generate_signals.hpp>
namespace generate_signals {
QmlMessageStroage::QmlMessageStroage(QObject *parent)
  : QObject(parent)
  , messages_{}
{
}
}
