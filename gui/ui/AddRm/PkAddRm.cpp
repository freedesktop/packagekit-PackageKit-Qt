/***************************************************************************
 *   Copyright (C) 2008 by Daniel Nicoletti   *
 *   mirttex85-pk@yahoo.com.br   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <KLocale>
#include <KStandardDirs>
#include <QPalette>
#include <QColor>

#include "PkTransaction.h"
#include "PkAddRm.h"

#define UNIVERSAL_PADDING 6

PkAddRm::PkAddRm( QWidget *parent )
 : QWidget( parent ), m_viewWidth(0)
{
    setupUi( this );

    //initialize the model, delegate, client and  connect it's signals
    packageView->setItemDelegate(pkg_delegate = new PkDelegate(this));
    packageView->setModel(m_pkg_model_main = new PkAddRmModel(this));
    updateColumnsWidth(true);

    // Create a new daemon
    m_daemon = new Daemon(this);
qDebug() << m_daemon->getActions();
    // create the install transaction
    m_pkClient_install = m_daemon->newTransaction();
    connect( m_pkClient_install, SIGNAL(GotPackage(Package *)), m_pkg_model_main, SLOT(addPackage(Package *)) );
    connect( m_pkClient_install, SIGNAL(Finished(Exit::Value, uint)), this, SLOT(Finished(Exit::Value, uint)) );
    connect( m_pkClient_install, SIGNAL(Files(Package *, QStringList)), this, SLOT(Files(Package *, QStringList)) );
    connect( m_pkClient_install, SIGNAL( Message(const QString&, const QString&) ), this, SLOT( Message(const QString&, const QString&) ) );
    connect( m_pkClient_install, SIGNAL( ErrorCode(const QString&, const QString&) ), this, SLOT( Message(const QString&, const QString&) ) );

    // create the main transaction
    m_pkClient_main = m_daemon->newTransaction();
    connect( m_pkClient_main, SIGNAL(GotPackage(Package *)), m_pkg_model_main, SLOT(addPackage(Package *)) );
    connect( m_pkClient_main, SIGNAL(Finished(Exit::Value, uint)), this, SLOT(Finished(Exit::Value, uint)) );
    connect( m_pkClient_main, SIGNAL( Message(const QString&, const QString&) ), this, SLOT( Message(const QString&, const QString&) ) );

    //initialize the groups
    //TODO map everything and fix search group.
    for (int i = 0; i < m_daemon->getGroups().size(); ++i) {
        if ( m_daemon->getGroups().at(i) == "accessories" )
	    groupsCB->addItem(KIcon("applications-accessories"), i18n("Accessories"));
	else if ( m_daemon->getGroups().at(i) == "games" )
	    groupsCB->addItem(KIcon("applications-games"), i18n("Games"));
	else if ( m_daemon->getGroups().at(i) == "graphics" )
	    groupsCB->addItem(KIcon("applications-graphics"), i18n("Graphics"));
	else if ( m_daemon->getGroups().at(i) == "internet" )
	    groupsCB->addItem(KIcon("applications-internet"), i18n("Internet"));
	else if ( m_daemon->getGroups().at(i) == "office" )
	    groupsCB->addItem(KIcon("applications-office"), i18n("Office"));
	else if ( m_daemon->getGroups().at(i) == "other" )
	    groupsCB->addItem(KIcon("applications-other"), i18n("Other"));
	else if ( m_daemon->getGroups().at(i) == "programming" )
	    groupsCB->addItem(KIcon("applications-development"), i18n("Development"));
	else if ( m_daemon->getGroups().at(i) == "multimedia" )
	    groupsCB->addItem(KIcon("applications-multimedia"), i18n("Multimedia"));
	else if ( m_daemon->getGroups().at(i) == "system" )
	    groupsCB->addItem(KIcon("applications-system"), i18n("System"));
    }
    // install the backend filters
    FilterMenu( m_daemon->getFilters() );

    // create the description transaction
    m_pkClient_desc = m_daemon->newTransaction();
    connect( m_pkClient_desc, SIGNAL(Details(Package *,  const QString, const QString, const QString,
        const QString, qulonglong) ),
        this, SLOT( Description(Package *, const QString, const QString, const QString,
        const QString, qulonglong) ) );

    // create the files transaction
    m_pkClient_files = m_daemon->newTransaction();
    connect( m_pkClient_files, SIGNAL( Files(Package *, QStringList) ), this, SLOT( Files(Package *, QStringList) ) );

    // create a transaction for the dependecies, and its model.
    m_pkClient_dep = m_daemon->newTransaction();
    dependsOnLV->setModel(m_pkg_model_dep = new PkAddRmModel(this));
    connect( m_pkClient_dep, SIGNAL(GotPackage(Package *)), m_pkg_model_dep, SLOT(addPackage(Package *)) );
 
    // create a transaction for the requirements, and its model.
    m_pkClient_req = m_daemon->newTransaction();
    requiredByLV->setModel(m_pkg_model_req = new PkAddRmModel(this));
    connect( m_pkClient_req, SIGNAL(GotPackage(Package *)), m_pkg_model_req, SLOT(addPackage(Package *)) );

    // connect the timer...
    connect(&m_notifyT, SIGNAL(timeout()), this, SLOT(notifyUpdate()));

    infoHide();
}

void PkAddRm::resizeEvent ( QResizeEvent * event )
{
    updateColumnsWidth();

    QWidget::resizeEvent(event);
}

bool PkAddRm::event ( QEvent * event )
{
    switch (event->type()) {
        case QEvent::PolishRequest:
        case QEvent::Polish:
            updateColumnsWidth(true);
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

void PkAddRm::updateColumnsWidth(bool force)
{
    m_viewWidth = packageView->viewport()->width();

    if (force) {
        m_viewWidth -= style()->pixelMetric(QStyle::PM_ScrollBarExtent) + UNIVERSAL_PADDING;
    }

    packageView->setColumnWidth(0, pkg_delegate->columnWidth(0, m_viewWidth));
    packageView->setColumnWidth(1, pkg_delegate->columnWidth(1, m_viewWidth));
}

PkAddRm::~PkAddRm()
{
    delete m_pkg_model_main;
    delete pkg_delegate;
    delete m_pkClient_main;
    delete m_toolQM;
    delete m_pkg_model_dep;
    delete m_pkClient_dep;
    delete m_pkg_model_req;
    delete m_pkClient_req;
}

void PkAddRm::infoHide()
{
    // hides the description to have more space.
    descriptionDW->setVisible(false);
    actionPB->hide();
    notifyF->hide();
    // cleans the models
    m_currPkg = 0;
    m_pkg_model_main->clear();
    m_pkg_model_req->clear();
    m_pkg_model_dep->clear();
}

void PkAddRm::infoShow()
{
    descriptionDW->setVisible(true);
    notifyF->show();
    actionPB->show();
}

void PkAddRm::on_searchPB_clicked()
{
    infoHide();
//     qDebug() << "Search Name " << filters() ;
    m_pkClient_main->searchName( filters(), lineEdit->text() );
}

void PkAddRm::Message(const QString &one, const QString &two)
{
    qDebug() << "Error code: " << one << " two: " << two;
}

void PkAddRm::on_groupsCB_currentIndexChanged( const QString & text )
{
    //TODO fix this mapping
    qDebug() << "Search Group " << text.toLower();
    infoHide();
    m_pkClient_main->searchGroup( filters(), text.toLower() );
}

void PkAddRm::on_packageView_pressed( const QModelIndex & index )
{
    m_pkClient_desc->getDetails(m_pkg_model_main->package(index));
    m_currPkg = m_pkg_model_main->package(index);
    if (m_currPkg) {
       if (m_currPkg->info() == "installed")
           actionPB->setText( i18n("Remove") );
       else
           actionPB->setText( i18n("Install") );
    }
    qDebug() << index.model()->data(index, PkAddRmModel::IdRole).toString();
}

void PkAddRm::on_actionPB_clicked()
{
packageView->resizeColumnToContents(1);
//     Transaction *trans = m_daemon->newTransaction();
//     if (m_daemon->getActions().contains("get-depends") ) {
//         trans->getDepends("~installed", m_currPkg, true);
// //     connect( m_pkClient_req, SIGNAL( GotPackage(Package *) ), m_pkg_model_req, SLOT( addPackage(Package *) ) );
// //     connect( m_pkClient_req, SIGNAL( Finished(Exit::Value, uint) ), this, SLOT( reqFinished(Exit::Value, uint) ) );
//     }
//         
//     PkAddRmTransaction *frm = new PkAddRmTransaction(, this);
//     frm->exec();
//     delete frm;
//     qDebug() << "mainEXEC()";
}

void PkAddRm::Finished(Exit::Value status, uint runtime)
{
    notifyF->show();
    QPalette teste;
    switch(status) {
        case Exit::Success :
	    notifyL->setText("Search finished in " + KGlobal::locale()->formatDuration(runtime) );
            teste.setColor( QPalette::Normal, QPalette::Window, QColor(0,255,0,150));
            notifyL->setPalette(teste);
            notifyL->setAutoFillBackground(true);
            m_notifyT.start(100);
	    break;
	case Exit::Failed :
	    notifyL->setText("Search Failed " + KGlobal::locale()->formatDuration(runtime) );
            teste.setColor(QPalette::Normal, QPalette::Window, QColor(255,0,0,150));
            notifyL->setPalette(teste);
            notifyL->setAutoFillBackground(true);
            m_notifyT.start(50);
	    break;
	case Exit::Quit : break;
	case Exit::Kill : break;
	case Exit::Unknown : break;
    }
}

void PkAddRm::notifyUpdate()
{
    QPalette palleteN(notifyL->palette());
    QColor colorN(palleteN.color(QPalette::Normal, QPalette::Window));
    if ( colorN.alpha() <= 0 ) {
        m_notifyT.stop();
        notifyL->setAutoFillBackground(false);
	if ( !actionPB->isVisible() )
	    notifyF->hide();
    }
    else {
        colorN.setAlpha(colorN.alpha() - 5);
        palleteN.setColor(QPalette::Normal, QPalette::Window, colorN);
        notifyL->setPalette(palleteN);
    }
}

void PkAddRm::Description(Package *p, const QString& license, const QString& group, const QString& detail, const QString& url, qulonglong size)
{
    qDebug() << p->id();
    //ask required by packages
    m_pkClient_req->getRequires("none", p, false);

    //ask depends on packages
    m_pkClient_dep->getDepends("none", p, false);

    //ask files in packages
    m_pkClient_files->getFiles(p);

    //format and show description
    QString description;
    description += "<b>" + i18n("Package Name") + ":</b> " + p->name() + "<br />";
    if ( !license.isEmpty() && license != "unknown" )
        description += "<b>" + i18n("License") + ":</b> " + license + "<br />";
    if ( !group.isEmpty() && group != "unknown" )
        description += "<b>" + i18n("Group") + ":</b> " + group + "<br />";
    if ( !detail.isEmpty() )
        description += "<b>" + i18n("Details") + ":</b> " + detail + "<br />";
    if ( !url.isEmpty() )
        description += "<b>" + i18n("Home Page") + ":</b> <a href=\"" + url + "\">" + url + "</a><br />";
    if ( size > 0 )
        description += "<b>" + i18n("Size") + ":</b> " + KGlobal::locale()->formatByteSize(size);
    descriptionKTB->setHtml(description);
    infoShow();
}

void PkAddRm::Files(Package *, QStringList files)
{
    filesPTE->clear();
    for (int i = 0; i < files.size(); ++i)
        filesPTE->appendPlainText(files.at(i));
    infoShow();
}

void PkAddRm::FilterMenu(const QStringList &filters)
{
    if(filters.size() != 0) {
        m_toolQM = new QMenu(this);
        filtersTB->setMenu(m_toolQM);

        if(filters.contains("installed")) {
            // Installed
            QMenu *menuInstalled = new QMenu(i18n("Installed"), m_toolQM);
            m_toolQM->addMenu(menuInstalled);
            QActionGroup *installedGroup = new QActionGroup(menuInstalled);
            installedGroup->setExclusive(true);
            QAction *installedTrue = new QAction(i18n("installed"), installedGroup);
            installedTrue->setCheckable(true);
            installedTrue->setData("installed");
            installedGroup->addAction(installedTrue);
            menuInstalled->addAction(installedTrue);

            QAction *installedFalse = new QAction(i18n("~installed"), installedGroup);
            installedFalse->setCheckable(true);
            installedFalse->setData("~installed");
            installedGroup->addAction(installedFalse);
            menuInstalled->addAction(installedFalse);

            QAction *installedNone = new QAction(i18n("No filter"), installedGroup);
            installedNone->setCheckable(true);
            installedNone->setChecked(true);
            installedGroup->addAction(installedNone);
            menuInstalled->addAction(installedNone);

            actions << installedTrue << installedFalse << installedNone;
        }
        if(filters.contains("devel")) {
            // Development
            QMenu *menuDevelopment = new QMenu(i18n("Development"), m_toolQM);
            m_toolQM->addMenu(menuDevelopment);
            QActionGroup *developmentGroup = new QActionGroup(menuDevelopment);
            developmentGroup->setExclusive(true);

            QAction *developmentTrue = new QAction(i18n("development"), developmentGroup);
            developmentTrue->setCheckable(true);
            developmentTrue->setData("development");
            developmentGroup->addAction(developmentTrue);
            menuDevelopment->addAction(developmentTrue);

            QAction *developmentFalse = new QAction(i18n("~development"), developmentGroup);
            developmentFalse->setCheckable(true);
            developmentFalse->setData("~development");
            developmentGroup->addAction(developmentFalse);
            menuDevelopment->addAction(developmentFalse);

            QAction *developmentNone = new QAction(i18n("No filter"), developmentGroup);
            developmentNone->setCheckable(true);
            developmentNone->setChecked(true);
            developmentGroup->addAction(developmentNone);
            menuDevelopment->addAction(developmentNone);

            actions << developmentTrue << developmentFalse << developmentNone;
        }
        if(filters.contains("gui")) {
            // Graphical
            QMenu *menuGui = new QMenu(i18n("Graphical"), m_toolQM);
            m_toolQM->addMenu(menuGui);
            QActionGroup *guiGroup = new QActionGroup(menuGui);
            guiGroup->setExclusive(true);

            QAction *guiTrue = new QAction(i18n("gui"), guiGroup);
            guiTrue->setCheckable(true);
            guiTrue->setData("gui");
            guiGroup->addAction(guiTrue);
            menuGui->addAction(guiTrue);

            QAction *guiFalse = new QAction(i18n("~gui"), guiGroup);
            guiFalse->setCheckable(true);
            guiFalse->setData("~gui");
            guiGroup->addAction(guiFalse);
            menuGui->addAction(guiFalse);

            QAction *guiNone = new QAction(i18n("No filter"), guiGroup);
            guiNone->setCheckable(true);
            guiNone->setChecked(true);
            guiGroup->addAction(guiNone);
            menuGui->addAction(guiNone);

            actions << guiTrue << guiFalse << guiNone;
        }
        if(filters.contains("free")) {
            // Free
            QMenu *menuFree = new QMenu(i18n("Free"), m_toolQM);
            m_toolQM->addMenu(menuFree);
            QActionGroup *freeGroup = new QActionGroup(menuFree);
            freeGroup->setExclusive(true);

            QAction *freeTrue = new QAction(i18n("free"), freeGroup);
            freeTrue->setCheckable(true);
            freeTrue->setData("free");
            freeGroup->addAction(freeTrue);
            menuFree->addAction(freeTrue);

            QAction *freeFalse = new QAction(i18n("~free"), freeGroup);
            freeFalse->setCheckable(true);
            freeFalse->setData("~free");
            freeGroup->addAction(freeFalse);
            menuFree->addAction(freeFalse);

            QAction *freeNone = new QAction(i18n("No filter"), freeGroup);
            freeNone->setCheckable(true);
            freeNone->setChecked(true);
            freeGroup->addAction(freeNone);
            menuFree->addAction(freeNone);

            actions << freeTrue << freeFalse << freeNone;
        }
        if(filters.contains("basename")) {
            m_toolQM->addSeparator();
            QAction *basename = new QAction(i18n("Hide subpackages"), m_toolQM);
            basename->setCheckable(true);
            basename->setData("basename");
            m_toolQM->addAction(basename);

            actions << basename;
        }

    }
    else {
        filtersTB->setDisabled(true);
    }
}

QString PkAddRm::filters()
{
    QStringList buffer;
    for(int i = 0 ; i < actions.size() ; ++i) {
        if(actions.at(i)->isChecked())
            if(actions.at(i)->data() != QVariant())
                buffer << actions.at(i)->data().toString();
    }
    if(buffer.size() == 0) buffer << "none";
        return buffer.join(";");
}

#include "PkAddRm.moc"
