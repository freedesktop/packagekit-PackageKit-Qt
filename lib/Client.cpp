#include "Client.h"
#include "constants.h"
#include "Status.h"

using namespace PackageKit;

Client::Client(QObject *parent) : QObject(parent) {
	proxy = new DBusProxy(PK_NAME, PK_PATH, QDBusConnection::systemBus(), this);
	_promiscuous = false; // Don't listen on all signals by default
	getTid();

	connect(proxy, SIGNAL(Package(const QString&, const QString&, const QString&, const QString&)), this,
									SLOT(Package_cb(const QString&, const QString&, const QString&, const QString&)));
	connect(proxy, SIGNAL(UpdateDetail(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&,
				const QString&, const QString&, const QString&)), this, SLOT(UpdateDetail_cb(const QString&, const QString&,
				const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)));
	connect(proxy, SIGNAL(Finished(const QString&, const QString&, uint)), this, SLOT(Finished_cb(const QString&, const QString&, uint)));
	connect(proxy, SIGNAL(ProgressChanged(const QString&, uint, uint, uint, uint)), this,
														SLOT(ProgressChanged_cb(const QString&, uint, uint, uint, uint)));
	connect(proxy, SIGNAL(Description(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, qulonglong)),
			this, SLOT(Description_cb(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, qulonglong)));
	connect(proxy, SIGNAL(Files(const QString&, const QString&, const QString&)), this,
														SLOT(Files_cb(const QString&, const QString&, const QString&)));
	connect(proxy, SIGNAL(ErrorCode(const QString&, const QString&, const QString&)), this,
													SLOT(ErrorCode_cb(const QString&, const QString&, const QString&)));
	connect(proxy, SIGNAL(Message(const QString&, const QString&, const QString&)), this,
													SLOT(Message_cb(const QString&, const QString&, const QString&)));
	connect(proxy, SIGNAL(Locked(bool)), this, SIGNAL(Locked(bool)));
	connect(proxy, SIGNAL(AllowCancel(const QString&, bool)), this, SLOT(AllowCancel_cb(const QString&, bool)));
	connect(proxy, SIGNAL(StatusChanged(const QString&, const QString&)), this, SLOT(StatusChanged_cb(const QString&, const QString&)));
	connect(proxy, SIGNAL(RequireRestart(const QString&, const QString&, const QString&)), this,
												SLOT(RequireRestart_cb(const QString&, const QString&, const QString&)));
	connect(proxy, SIGNAL(UpdatesChanged(const QString&)), this, SLOT(UpdatesChanged_cb(const QString&)));
}

Client::~Client() {

}

bool Client::setPromiscuous(bool enabled) {
	if(_tid != QString()) { // The doc says we can only set promiscuous mode on a tid-less instance
		return false;
	}
	_promiscuous = enabled;
	return true;
}

// Tid related functions

void Client::getTid() {
	_tid = proxy->GetTid().value();
}

void Client::setTid(QString newTid) {
	_tid = newTid;
}

const QString& Client::tid() {
	return _tid;
}

bool Client::allowCancel() {
	if(_tid == QString()) return false;
	return proxy->GetAllowCancel(_tid).value();
}

Status::Value Client::status() {
	if(_tid == QString()) getTid();
	return (Status::Value)EnumFromString<Status>(proxy->GetStatus(_tid).value());
}

Role::Value Client::role(QString &package_id) {
	if(_tid == QString()) getTid();
	return (Role::Value)EnumFromString<Role>(proxy->GetRole(_tid, package_id));
}

void Client::searchName(const QString& filter, const QString& name) {
	if(_tid == QString()) getTid();
	proxy->SearchName(_tid, filter, name);
}

void Client::searchDetails(const QString& filter, const QString& search) {
	if(_tid == QString()) getTid();
	proxy->SearchDetails(_tid, filter, search);
}

void Client::searchGroup(const QString& filter, const QString& group) {
	if(_tid == QString()) getTid();
	proxy->SearchGroup(_tid, filter, group);
}

void Client::searchFile(const QString& filter, const QString& file) {
	if(_tid == QString()) getTid();
	proxy->SearchFile(_tid, filter, file);
}

void Client::getPackage(const QString& package_id) {
	proxy->GetPackage(package_id);
}

void Client::getDescription(const QString& package_id) {
	if(_tid == QString()) getTid();
	proxy->GetDescription(_tid, package_id);
}

void Client::getDescription(Package *p) {
	getDescription(p->id());
}

void Client::getDepends(Package *p, bool recursive) {
	if(_tid == QString()) getTid();
	proxy->GetDepends(_tid, p->id(), recursive);
}

void Client::getRequires(Package *p, bool recursive) {
	if(_tid == QString()) getTid();
	proxy->GetRequires(_tid, p->id(), recursive);
}

void Client::installPackage(Package *p) {
	if(_tid == QString()) getTid();
	proxy->InstallPackage(_tid, p->id());
}

void Client::installFile(const QString& path) {
	if(_tid == QString()) getTid();
	proxy->InstallFile(_tid, path);
}

void Client::getUpdates() {
	if(_tid == QString()) getTid();
	proxy->GetUpdates(_tid);
}

void Client::updatePackage(Package *p) {
	if(_tid == QString()) getTid();
	proxy->UpdatePackage(_tid, p->id());
}

void Client::updateSystem() {
	if(_tid == QString()) getTid();
	proxy->UpdateSystem(_tid);
}

void Client::getUpdateDetail(Package *p) {
	if(_tid == QString()) getTid();
	proxy->GetUpdateDetail(_tid, p->id());
}

void Client::cancel() {
	if(_tid == QString()) return;
	proxy->Cancel(_tid);
}

void Client::getProgress() {
	if(_tid == QString()) return;
	uint percentage, subpercentage, elapsed, remaining;
	percentage = proxy->GetProgress(_tid, subpercentage, elapsed, remaining);
	emit ProgressChanged(percentage, subpercentage, elapsed, remaining);
}

void Client::refreshCache(bool force) {
	if(_tid == QString()) getTid();
	proxy->RefreshCache(_tid, force);
}

void Client::backendDetails(QString *name, QString *author) {
	QString n, a;
	n = proxy->GetBackendDetail(a);
	name = &n;
	author = &a;
}

QStringList Client::getActions() {
	QString actions = proxy->GetActions();
	return actions.split(";");
}

QStringList Client::getFilters() {
	QString filters = proxy->GetFilters();
	return filters.split(";");
}

QStringList Client::getGroups() {
	QString groups = proxy->GetGroups();
	return groups.split(";");
}

//// Signal callbacks
void Client::AllowCancel_cb(const QString& tid, bool allow_cancel) {
	if(!_promiscuous && tid != _tid) return;
	emit AllowCancel(allow_cancel);
}

void Client::Package_cb(const QString& tid, const QString& info, const QString& package_id, const QString& summary) {
	if(!_promiscuous && tid != _tid) return;
	emit newPackage(new Package(package_id, info, summary));
}

void Client::UpdateDetail_cb(const QString& tid, const QString& package_id, const QString& updates, const QString& obsoletes, const QString& vendor_url, const QString& bugzilla_url, const QString& cve_url, const QString& restart, const QString& update_text) {
	if(!_promiscuous && tid != _tid) return;
	emit updateDetail(new Package(package_id, this), updates, obsoletes, vendor_url, bugzilla_url, cve_url, restart, update_text);
}

void Client::Description_cb(const QString &tid, const QString &package_id, const QString &license, const QString &group,
															const QString &detail, const QString &url, qulonglong size) {
	if(!_promiscuous && tid != _tid) return;
	emit Description(new Package(package_id), license, group, detail, url, size);
}

void Client::Finished_cb(const QString& tid, const QString& status, uint runtime) {
	if(!_promiscuous && tid != _tid) return;
	emit Finished((Exit::Value)EnumFromString<Exit>(status), runtime);
	_tid = QString();
}

void Client::ProgressChanged_cb(const QString& tid, uint percentage, uint subpercentage, uint elapsed, uint remaining) {
	if(!_promiscuous && tid != _tid) return;
	emit ProgressChanged(percentage, subpercentage, elapsed, remaining);
}

void Client::Files_cb(const QString& tid, const QString& package_id, const QString& files) {
	if(!_promiscuous && tid != _tid) return;
	emit Files(new Package(package_id), files.split(";"));
}

void Client::ErrorCode_cb(const QString& tid, const QString& code, const QString& details) {
	if(!_promiscuous && tid != _tid) return;
	emit ErrorCode(code, details);
}

void Client::Message_cb(const QString& tid, const QString& message, const QString& details) {
	if(!_promiscuous && tid != _tid) return;
	emit Message(message, details);
}

void Client::StatusChanged_cb(const QString& tid, const QString& status) {
	if(!_promiscuous && tid != _tid) return;
	emit StatusChanged((Status::Value)EnumFromString<Status>(status));
}

void Client::RequireRestart_cb(const QString& tid, const QString& type, const QString& details) {
	if(!_promiscuous && tid != _tid) return;
	emit RequireRestart((Restart::Value)EnumFromString<Restart>(type), details);
}

void Client::UpdatesChanged_cb(const QString& tid) {
	if(!_promiscuous && tid != _tid) return;
	emit UpdatesChanged();
}