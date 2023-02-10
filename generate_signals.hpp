#pragma once
#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_GENERATE_SIGNALS
#define GENERATE_SIGNALS_PUBLIC __declspec(dllexport)
#else
#define GENERATE_SIGNALS_PUBLIC __declspec(dllimport)
#endif
#else
#ifdef BUILDING_GENERATE_SIGNALS
#define GENERATE_SIGNALS_PUBLIC __attribute__((visibility("default")))
#else
#define GENERATE_SIGNALS_PUBLIC
#endif
#endif

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QMetaMethod>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <concepts>
#include <generate_signals.hpp>
#include <type_traits>
namespace generate_signals {

template<typename T>
concept QMetaAble = std::is_base_of_v<QObject, T>;

struct QmlMessage
{
    QString Name;
    QList<QString> Signals;
    QList<QPair<QString, QString>> Slots;
    QJsonObject toJson() const
    {
        return {{"name", Name},
                {"signals", std::invoke([this]() -> QJsonArray {
                     QJsonArray array;
                     for (auto sig : Signals) {
                         array.append(sig);
                     }
                     return array;
                 })},
                {"slots", std::invoke([this]() -> QJsonArray {
                     QJsonArray array;
                     for (auto slot : Slots) {
                         array.append({{slot.first, slot.second}});
                     }
                     return array;
                 })}};
    }
};

class QmlMessageStroage : public QObject
{
    Q_OBJECT
public:
    explicit QmlMessageStroage(QObject *parent = nullptr);
    ~QmlMessageStroage() = default;
    template<QMetaAble T>
    void get_message()
    {
        const QMetaObject metaObj = T::staticMetaObject;
        QString name              = metaObj.className();
        QList<QPair<QString, QString>> slotSignatures;
        QList<QString> signalSignatures;
        for (int methodIdx = metaObj.methodOffset(); methodIdx < metaObj.methodCount();
             ++methodIdx) {
            QMetaMethod mmTest = metaObj.method(methodIdx);
            switch (mmTest.methodType()) {
            case QMetaMethod::Signal:
                signalSignatures.append(QString(mmTest.methodSignature()));
                break;
            case QMetaMethod::Slot:
                slotSignatures.append(
                  qMakePair(mmTest.returnMetaType().name(), QString(mmTest.methodSignature())));
                break;
            default:
                break;
            }
        }
        this->messages_.append(QmlMessage{
          .Name    = name,
          .Signals = signalSignatures,
          .Slots   = slotSignatures,
        });
    }
    [[nodiscard]] QJsonArray toJson() const
    {
        QJsonArray array;
        for (auto message : messages_) {
            array.append(message.toJson());
        }
        return array;
    }

private:
    QList<QmlMessage> messages_;
};

}
