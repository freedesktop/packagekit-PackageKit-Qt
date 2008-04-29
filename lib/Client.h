#ifndef CLIENT_H
#define CLIENT_H

#include <QtCore>

#include "DBusProxy.h"
#include "Status.h"
#include "Role.h"
#include "Exit.h"
#include "Restart.h"
#include "Package.h"

namespace PackageKit {

class Client : public QObject {

	Q_OBJECT

public:
	Client(QObject *parent = 0);
	~Client();

	bool setPromiscuous(bool enabled);
	void setTid(QString newTid);
	const QString& tid();
	bool allowCancel();

	Status::Value status();
	Role::Value role(QString &package_id);

	void searchName(const QString& filter, const QString& name);
	void searchDetails(const QString& filter, const QString& search);
	void searchGroup(const QString& filter, const QString& group);
	void searchFile(const QString& filter, const QString& file);
	void getPackage(const QString& package_id);

	void getDescription(const QString& package_id);
	void getDescription(Package *p);
	void getDepends(Package *p, bool recursive);
	void getRequires(Package *p, bool recursive);

	void installPackage(Package *p);
	void installFile(const QString& path);

	void getUpdates();
	void updatePackage(Package *p);
	void updateSystem();
	void getUpdateDetail(Package *p);

	void cancel();
	void getProgress();
	void refreshCache(bool force);

	void backendDetails(QString *name, QString *author);
	QStringList getActions();
	QStringList getFilters();
	QStringList getGroups();

signals:
	void Locked(bool);
	void AllowCancel(bool allow_cancel);
	void newPackage(Package *p);
	void updateDetail(Package *p, const QString& updates, const QString& obsoletes, const QString& vendor_url, const QString& bugzilla_url, const QString& cve_url, const QString& restart, const QString& update_text);
	void Description(Package *p, const QString& license, const QString& group, const QString& detail, const QString& url, qulonglong size);
	void Files(Package *p, QStringList files);
	void Finished(Exit::Value status, uint runtime);
	void ProgressChanged(uint percentage, uint subpercentage, uint elapsed, uint remaining);
	void ErrorCode(const QString& code, const QString& details);
	void Message(const QString& message, const QString& details);
	void StatusChanged(Status::Value status);
	void RequireRestart(Restart::Value restart, const QString& details);
	void UpdatesChanged();

private:
	DBusProxy *proxy;
	QString _tid; // TID this instance is tied to. QString() means no tying
	void getTid();
	bool _promiscuous; // Weither this instance listens on all transactions or not

private slots:
	void AllowCancel_cb(const QString& tid, bool allow_cancel);
	void Package_cb(const QString& tid, const QString& info, const QString& package_id, const QString& summary);
	void UpdateDetail_cb(const QString& tid, const QString& package_id, const QString& updates, const QString& obsoletes, const QString& vendor_url, const QString& bugzilla_url, const QString& cve_url, const QString& restart, const QString& update_text);
    void Description_cb(const QString& tid, const QString& package_id, const QString& license, const QString& group, const QString& detail, const QString& url, qulonglong size);
	void Files_cb(const QString& tid, const QString& package_id, const QString& files);
	void Finished_cb(const QString& tid, const QString& status, uint runtime);
	void ProgressChanged_cb(const QString& tid, uint percentage, uint subpercentage, uint elapsed, uint remaining);
	void ErrorCode_cb(const QString& tid, const QString& code, const QString& details);
	void Message_cb(const QString& tid, const QString& message, const QString& details);
	void StatusChanged_cb(const QString& tid, const QString& status);
	void RequireRestart_cb(const QString& tid, const QString& type, const QString& details);
	void UpdatesChanged_cb(const QString& tid);

};

} // End namespace PackageKit

#endif