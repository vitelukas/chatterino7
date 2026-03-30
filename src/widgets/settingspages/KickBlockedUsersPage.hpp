// SPDX-FileCopyrightText: 2026 Contributors to Chatterino <https://chatterino.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "widgets/settingspages/SettingsPage.hpp"

class QStandardItemModel;
class QTableView;

namespace chatterino {

class KickBlockedUsersPage : public SettingsPage
{
public:
    KickBlockedUsersPage();

    void onShow() final;
    bool filterElements(const QString &query) final;

private:
    void reloadUsers();
    void unblockSelectedUsers();

    QStandardItemModel *model_{};
    QTableView *table_{};
};

}  // namespace chatterino
