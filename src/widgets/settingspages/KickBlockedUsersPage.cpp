// SPDX-FileCopyrightText: 2026 Contributors to Chatterino <https://chatterino.com>
//
// SPDX-License-Identifier: MIT

#include "widgets/settingspages/KickBlockedUsersPage.hpp"

#include "providers/kick/KickIgnoredUser.hpp"
#include "singletons/Settings.hpp"
#include "util/LayoutCreator.hpp"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

#include <algorithm>

namespace chatterino {

KickBlockedUsersPage::KickBlockedUsersPage()
    : model_(new QStandardItemModel(this))
{
    LayoutCreator<KickBlockedUsersPage> layoutCreator(this);
    auto layout = layoutCreator.setLayoutType<QVBoxLayout>();

    auto info = layout.emplace<QLabel>(
        "Locally blocked Kick users. Messages from these users are hidden "
        "before rendering.");
    info->setWordWrap(true);

    auto buttons = layout.emplace<QHBoxLayout>().withoutMargin();
    auto unblockSelected =
        buttons.emplace<QPushButton>("Unblock selected").getElement();
    buttons->addStretch(1);

    QObject::connect(unblockSelected, &QPushButton::clicked, [this] {
        this->unblockSelectedUsers();
    });

    this->table_ = layout.emplace<QTableView>().getElement();
    this->table_->setModel(this->model_);
    this->table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table_->verticalHeader()->setVisible(false);
    this->table_->horizontalHeader()->setSectionsClickable(false);
    this->table_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Interactive);
    this->table_->horizontalHeader()->setSectionResizeMode(0,
                                                           QHeaderView::Stretch);

    this->model_->setColumnCount(2);
    this->model_->setHorizontalHeaderLabels({"Username", "User ID"});

    this->managedConnections_.managedConnect(
        getSettings()->kickIgnoredUsers.delayedItemsChanged, [this] {
            this->reloadUsers();
        });

    this->reloadUsers();
}

void KickBlockedUsersPage::reloadUsers()
{
    this->model_->removeRows(0, this->model_->rowCount());

    auto blockedUsers = getSettings()->kickIgnoredUsers.readOnly();

    std::vector<KickIgnoredUser> users(blockedUsers->begin(), blockedUsers->end());
    std::sort(users.begin(), users.end(), [](const auto &a, const auto &b) {
        return QString::compare(a.getUsername(), b.getUsername(),
                                Qt::CaseInsensitive) < 0;
    });

    for (const auto &user : users)
    {
        auto *nameItem = new QStandardItem(user.getUsername());
        auto *idItem = new QStandardItem(QString::number(user.getUserID()));
        this->model_->appendRow({nameItem, idItem});
    }

    this->table_->resizeColumnsToContents();
}

void KickBlockedUsersPage::unblockSelectedUsers()
{
    auto selection = this->table_->selectionModel()->selectedRows(1);
    if (selection.isEmpty())
    {
        return;
    }

    std::vector<uint64_t> ids;
    ids.reserve(selection.size());

    for (const auto &index : selection)
    {
        bool ok = false;
        auto id = index.data(Qt::DisplayRole).toString().toULongLong(&ok);
        if (ok)
        {
            ids.push_back(id);
        }
    }

    for (auto id : ids)
    {
        getSettings()->unblockKickUser(id);
    }

    this->reloadUsers();
}

void KickBlockedUsersPage::onShow()
{
    this->reloadUsers();
}

bool KickBlockedUsersPage::filterElements(const QString &query)
{
    bool found = false;

    for (int row = 0; row < this->model_->rowCount(); row++)
    {
        const auto user = this->model_->index(row, 0).data().toString();
        const auto id = this->model_->index(row, 1).data().toString();

        const bool match = query.isEmpty() ||
                           user.contains(query, Qt::CaseInsensitive) ||
                           id.contains(query, Qt::CaseInsensitive);

        this->table_->setRowHidden(row, !match);
        found |= match;
    }

    return found || (this->model_->rowCount() == 0 && query.isEmpty());
}

}  // namespace chatterino
