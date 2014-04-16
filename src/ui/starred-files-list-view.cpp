#include <QtGui>
#include <QHeaderView>
#include <QDesktopServices>
#include <QEvent>
#include <QShowEvent>
#include <QHideEvent>

#include "QtAwesome.h"
#include "utils/utils.h"
#include "seafile-applet.h"
#include "account-mgr.h"
#include "rpc/rpc-client.h"
#include "rpc/local-repo.h"
#include "starred-file-item.h"
#include "starred-files-list-model.h"

#include "starred-files-list-view.h"


StarredFilesListView::StarredFilesListView(QWidget *parent)
    : QListView(parent)
{
#ifdef Q_WS_MAC
    setAttribute(Qt::WA_MacShowFocusRect, 0);
#endif

    createActions();
}

void StarredFilesListView::createActions()
{
    open_file_action_ = new QAction(tr("&Open"), this);
    // open_file_action_->setIcon(QIcon(":/images/info.png"));
    // open_file_action_->setIconVisibleInMenu(true);
    open_file_action_->setStatusTip(tr("Open this file"));
    connect(open_file_action_, SIGNAL(triggered()), this, SLOT(openLocalFile()));

    view_file_on_web_action_ = new QAction(tr("view on &Web"), this);
    // view_file_on_web_action_->setIcon(QIcon(":/images/download.png"));
    // view_file_on_web_action_->setIconVisibleInMenu(true);
    view_file_on_web_action_->setStatusTip(tr("view this file on seahub"));
    connect(view_file_on_web_action_, SIGNAL(triggered()), this, SLOT(viewFileOnWeb()));

    // sync_now_action_ = new QAction(tr("Sync &Now"), this);
    // sync_now_action_->setIcon(QIcon(":/images/sync_now.png"));
    // sync_now_action_->setStatusTip(tr("Sync this library immediately"));
    // sync_now_action_->setIconVisibleInMenu(true);
    // connect(sync_now_action_, SIGNAL(triggered()), this, SLOT(syncRepoImmediately()));

    // cancel_download_action_ = new QAction(tr("&Cancel download"), this);
    // cancel_download_action_->setIcon(QIcon(":/images/remove.png"));
    // cancel_download_action_->setStatusTip(tr("Cancel download of this library"));
    // cancel_download_action_->setIconVisibleInMenu(true);
    // connect(cancel_download_action_, SIGNAL(triggered()), this, SLOT(cancelDownload()));

    // open_local_folder_action_ = new QAction(tr("&Open folder"), this);
    // open_local_folder_action_->setIcon(QIcon(":/images/folder-open.png"));
    // open_local_folder_action_->setStatusTip(tr("open local folder"));
    // open_local_folder_action_->setIconVisibleInMenu(true);
    // connect(open_local_folder_action_, SIGNAL(triggered()), this, SLOT(openLocalFolder()));

    // unsync_action_ = new QAction(tr("&Unsync"), this);
    // unsync_action_->setStatusTip(tr("unsync this library"));
    // unsync_action_->setIcon(QIcon(":/images/minus.png"));
    // unsync_action_->setIconVisibleInMenu(true);
    // connect(unsync_action_, SIGNAL(triggered()), this, SLOT(unsyncRepo()));

    // toggle_auto_sync_action_ = new QAction(tr("Enable auto sync"), this);
    // toggle_auto_sync_action_->setStatusTip(tr("Enable auto sync"));
    // toggle_auto_sync_action_->setIconVisibleInMenu(true);
    // connect(toggle_auto_sync_action_, SIGNAL(triggered()), this, SLOT(toggleRepoAutoSync()));

    // view_on_web_action_ = new QAction(tr("&View on cloud"), this);
    // view_on_web_action_->setIcon(QIcon(":/images/cloud.png"));
    // view_on_web_action_->setStatusTip(tr("view this library on seahub"));
    // view_on_web_action_->setIconVisibleInMenu(true);

    // connect(view_on_web_action_, SIGNAL(triggered()), this, SLOT(viewRepoOnWeb()));
}

void StarredFilesListView::openLocalFile()
{
}

void StarredFilesListView::viewFileOnWeb()
{
}

void StarredFilesListView::updateActions()
{
    StarredFileItem *item = NULL;
    QItemSelection selected = selectionModel()->selection();
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() != 0) {
        const QModelIndex& index = indexes.at(0);
        QStandardItem *it = ((StarredFilesListModel *)model())->itemFromIndex(index);
        item = (StarredFileItem *)it;
    }

    if (!item) {
        // No item is selected
        open_file_action_->setEnabled(false);
        view_file_on_web_action_->setEnabled(false);
        return;
    }

    const StarredFile& file = item->file();

    open_file_action_->setData(QVariant::fromValue(file));
    view_file_on_web_action_->setData(QVariant::fromValue(file));
}

void StarredFilesListView::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    if (!index.isValid()) {
        // Not clicked at a repo item
        return;
    }

    QStandardItem *item = getFileItem(index);
    if (!item) {
        return;
    }
    updateActions();
    QMenu *menu = prepareContextMenu((StarredFileItem *)item);
    pos = viewport()->mapToGlobal(pos);
    menu->exec(pos);
}

QStandardItem*
StarredFilesListView::getFileItem(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return NULL;
    }
    const StarredFilesListModel *model = (const StarredFilesListModel*)index.model();
    QStandardItem *item = model->itemFromIndex(index);
    return item;
}

QMenu* StarredFilesListView::prepareContextMenu(const StarredFileItem *item)
{
    QMenu *menu = new QMenu(this);

    menu->addAction(open_file_action_);
    menu->addAction(view_file_on_web_action_);

    return menu;
}
