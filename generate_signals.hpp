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

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
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

enum class RegisterType
{
    SingleTon,
    ObjectType,
    UncreateableType,
};

[[nodiscard]] inline QString
registertype_to_string(RegisterType type)
{
    switch (type) {
    case RegisterType::SingleTon:
        return QString("SingleTon");
    case RegisterType::ObjectType:
        return QString("ObjectType");
    case RegisterType::UncreateableType:
        return QString("UncreateableType");
    default:
        return QString();
    }
}

struct QmlMessage
{
    QString Name;
    QString Uri;
    QString ReType;
    int VersionMajor;
    int VersionMinor;
    QList<QString> Signals;
    QList<QPair<QString, QString>> Slots;
    QJsonObject toJson() const
    {
        return {{"name", Name},
                {"uri", Uri},
                {"type", ReType},
                {"versionMajor", VersionMajor},
                {"versionMinor", VersionMinor},
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
    void get_message(const char *uri,
                     const RegisterType rgtype,
                     int versionMajor,
                     int versionMinor,
                     const char *qmlname)
    {
        const QMetaObject metaObj = T::staticMetaObject;
        const QString name        = QString(qmlname);
        const QString reUri       = QString(uri);
        const QString qmltype     = registertype_to_string(rgtype);
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
          .Name         = name,
          .Uri          = reUri,
          .ReType       = qmltype,
          .VersionMajor = versionMajor,
          .VersionMinor = versionMinor,
          .Signals      = signalSignatures,
          .Slots        = slotSignatures,
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
    void writeToFile()
    {
        QJsonArray array = toJson();
        QJsonDocument document;
        document.setArray(array);
        auto dir         = qApp->applicationDirPath();
        QByteArray bytes = document.toJson(QJsonDocument::Indented);
        QFile file(QString("%1/types.json").arg(dir));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream iStream(&file);
            iStream.setEncoding(QStringConverter::Encoding::Utf8);
            iStream << bytes;
            file.close();
            emit quit(1);
        } else {
            emit quit(0);
        }
    }
signals:
    void quit(int);

private:
    QList<QmlMessage> messages_;
};

}
