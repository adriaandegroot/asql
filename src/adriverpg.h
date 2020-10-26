/* 
 * SPDX-FileCopyrightText: (C) 2020 Daniel Nicoletti <dantti12@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef ADRIVERPG_H
#define ADRIVERPG_H

#include <adriver.h>
#include <libpq-fe.h>

#include "aresult.h"
#include "apreparedquery.h"

#include <QQueue>
#include <QPointer>
#include <QHash>

typedef struct pg_conn PGconn;

class AResultPg : public AResultPrivate
{
public:
    AResultPg();
    virtual ~AResultPg();

    virtual bool lastResulSet() const override;
    virtual bool error() const override;
    virtual QString errorString() const override;

    virtual int size() const override;
    virtual int fields() const override;
    virtual int numRowsAffected() const override;

    virtual QString fieldName(int column) const override;
    virtual QVariant value(int row, int column) const override;

    virtual bool toBool(int row, int column) const override;
    virtual int toInt(int row, int column) const override;
    virtual int toLongLong(int row, int column) const override;
    virtual int toULongLong(int row, int column) const override;
    virtual int toDouble(int row, int column) const override;
    virtual QString toString(int row, int column) const override;
    virtual QDate toDate(int row, int column) const override;
    virtual QTime toTime(int row, int column) const override;
    virtual QDateTime toDateTime(int row, int column) const override;
    virtual QByteArray toByteArray(int row, int column) const override;

    void processResult();

    QString m_errorString;
    PGresult *m_result = nullptr;
    bool m_error = false;
    bool m_lastResultSet = true;
};

class APGQuery
{
public:
    APGQuery() : result(QSharedPointer<AResultPg>(new AResultPg))
    { }
    QString query;
    APreparedQuery preparedQuery;
    QSharedPointer<AResultPg> result;
    QVariantList params;
    AResultFn cb;
    QSharedPointer<ADatabasePrivate> db;
    QPointer<QObject> receiver;
    QObject *checkReceiver;
    bool preparing = false;
    bool prepared = false;
    bool setSingleRow = false;

    inline void done() {
        AResult r(result);
        if (cb && (!checkReceiver || !receiver.isNull())) {
            cb(r);
        }
    }
};

class ADriverPg : public ADriver
{
    Q_OBJECT
public:
    ADriverPg();
    virtual ~ADriverPg();

    virtual void open(std::function<void(bool isOpen, const QString &error)> cb) override;
    virtual bool isOpen() const override;

    void setState(ADatabase::State state, const QString &status);
    virtual ADatabase::State state() const override;
    virtual void onStateChanged(std::function<void(ADatabase::State state, const QString &status)> cb) override;

    virtual void begin(QSharedPointer<ADatabasePrivate> db, AResultFn cb, QObject *receiver) override;
    virtual void commit(QSharedPointer<ADatabasePrivate> db, AResultFn cb, QObject *receiver) override;
    virtual void rollback(QSharedPointer<ADatabasePrivate> db, AResultFn cb, QObject *receiver) override;

    virtual void exec(QSharedPointer<ADatabasePrivate> db, const QString &query, const QVariantList &params, AResultFn cb, QObject *receiver) override;
    virtual void exec(QSharedPointer<ADatabasePrivate> db, const APreparedQuery &query, const QVariantList &params, AResultFn cb, QObject *receiver) override;

    virtual void setLastQuerySingleRowMode();

    virtual void subscribeToNotification(QSharedPointer<ADatabasePrivate> db, const QString &name, ANotificationFn cb, QObject *receiver) override;
    virtual void unsubscribeFromNotification(QSharedPointer<ADatabasePrivate> db, const QString &name, QObject *receiver) override;

private:
    void nextQuery();
    void finishConnection();
    void finishQueries(const QString &error);
    inline void doExec(APGQuery &pgQuery);
    inline void doExecParams(APGQuery &query);
    inline void setSingleRowMode();
    inline void cmdFlush();

    PGconn *m_conn = nullptr;
    ADatabase::State m_state = ADatabase::Disconnected;
    bool m_connected = false;
    bool m_flush = false;
    bool m_queryRunning = false;
    std::function<void (ADatabase::State, const QString &)> m_stateChangedCb;
    QHash<QString, ANotificationFn> m_subscribedNotifications;
    QQueue<APGQuery> m_queuedQueries;
    QSocketNotifier *m_writeNotify = nullptr;
    QSocketNotifier *m_readNotify = nullptr;
    QStringList m_preparedQueries;
};

#endif // ADRIVERPG_H
