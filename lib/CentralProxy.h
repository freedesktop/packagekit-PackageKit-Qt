/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -c CentralProxy -p CentralProxy.h /home/madcat/code/packagekit/src/pk-interface.xml org.freedesktop.PackageKit
 *
 * dbusxml2cpp is Copyright (C) 2006 Trolltech ASA. All rights reserved.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef CENTRALPROXY_H_1212967231
#define CENTRALPROXY_H_1212967231

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

namespace PackageKit {

/*
 * Proxy class for interface org.freedesktop.PackageKit
 */
class CentralProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.PackageKit"; }

public:
    CentralProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~CentralProxy();

public Q_SLOTS: // METHODS
    inline QDBusReply<QString> GetActions()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetActions"), argumentList);
    }

    inline QDBusReply<QString> GetBackendDetail(QString &author)
    {
        QList<QVariant> argumentList;
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QLatin1String("GetBackendDetail"), argumentList);
        if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
            author = qdbus_cast<QString>(reply.arguments().at(1));
        }
        return reply;
    }

    inline QDBusReply<QString> GetFilters()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetFilters"), argumentList);
    }

    inline QDBusReply<QString> GetGroups()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetGroups"), argumentList);
    }

    inline QDBusReply<QString> GetNetworkState()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetNetworkState"), argumentList);
    }

    inline QDBusReply<QString> GetTid()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetTid"), argumentList);
    }

    inline QDBusReply<uint> GetTimeSinceAction(const QString &role)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(role);
        return callWithArgumentList(QDBus::Block, QLatin1String("GetTimeSinceAction"), argumentList);
    }

    inline QDBusReply<QStringList> GetTransactionList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetTransactionList"), argumentList);
    }

    inline QDBusReply<void> SetProxy(const QString &proxy_http, const QString &proxy_ftp)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(proxy_http) << qVariantFromValue(proxy_ftp);
        return callWithArgumentList(QDBus::Block, QLatin1String("SetProxy"), argumentList);
    }

    inline QDBusReply<void> StateHasChanged(const QString &reason)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(reason);
        return callWithArgumentList(QDBus::Block, QLatin1String("StateHasChanged"), argumentList);
    }

    inline QDBusReply<void> SuggestDaemonQuit()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("SuggestDaemonQuit"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void Locked(bool is_locked);
    void NetworkStateChanged(const QString &state);
    void RepoListChanged();
    void RestartSchedule();
    void TransactionListChanged(const QStringList &transactions);
    void UpdatesChanged();
};

} // End namespace PackageKit
#endif


