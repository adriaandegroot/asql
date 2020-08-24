#ifndef ADRIVER_H
#define ADRIVER_H

#include <QString>
#include <QSocketNotifier>
#include <QUrl>

#include <adatabase.h>

#include <functional>

class AResult;

class ADriver
{
public:
    ADriver();

    QUrl connectionInfo() const;
    void setConnectionInfo(const QUrl &connectionInfo);

    virtual void open(std::function<void(bool isOpen, const QString &error)> cb) = 0;

    virtual bool isOpen() const = 0;

    virtual void begin(QSharedPointer<ADatabasePrivate> db, AResultFn cb, QObject *receiver) = 0;
    virtual void commit(QSharedPointer<ADatabasePrivate> db, AResultFn cb, QObject *receiver) = 0;
    virtual void rollback(QSharedPointer<ADatabasePrivate> db, AResultFn cb, QObject *receiver) = 0;

    virtual bool exec(QSharedPointer<ADatabasePrivate> db, const QString &query, AResultFn cb, QObject *receiver) = 0;
    virtual bool exec(QSharedPointer<ADatabasePrivate> db, const QString &query, const QVariantList &params, AResultFn cb, QObject *receiver) = 0;

    virtual void subscribeToNotification(QSharedPointer<ADatabasePrivate> db, const QString &name, ANotificationFn cb, QObject *receiver) = 0;
    virtual void unsubscribeFromNotification(QSharedPointer<ADatabasePrivate> db, const QString &name, QObject *receiver) = 0;

private:
    QUrl m_info;
};

#endif // ADRIVER_H