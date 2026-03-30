// SPDX-FileCopyrightText: 2024 Contributors to Chatterino <https://chatterino.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "util/RapidjsonHelpers.hpp"
#include "util/RapidJsonSerializeQString.hpp"

#include <pajlada/serialize.hpp>
#include <QString>

#include <cstdint>

namespace chatterino {

/// @brief Represents a Kick user that should be ignored/blocked in chat
class KickIgnoredUser
{
public:
    bool operator==(const KickIgnoredUser &other) const
    {
        return this->userID_ == other.userID_;
    }

    KickIgnoredUser(uint64_t userID, const QString &username = "")
        : userID_(userID)
        , username_(username)
    {
    }

    uint64_t getUserID() const
    {
        return this->userID_;
    }

    const QString &getUsername() const
    {
        return this->username_;
    }

    void setUsername(const QString &username)
    {
        this->username_ = username;
    }

private:
    uint64_t userID_;
    QString username_;
};

}  // namespace chatterino

namespace pajlada {

template <>
struct Serialize<chatterino::KickIgnoredUser> {
    static rapidjson::Value get(const chatterino::KickIgnoredUser &value,
                                rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(rapidjson::kObjectType);

        chatterino::rj::set(ret, "userID", value.getUserID(), a);
        chatterino::rj::set(ret, "username", value.getUsername(), a);

        return ret;
    }
};

template <>
struct Deserialize<chatterino::KickIgnoredUser> {
    static chatterino::KickIgnoredUser get(const rapidjson::Value &value,
                                            bool *error = nullptr)
    {
        uint64_t userID = 0;
        QString username;

        if (!value.IsObject())
        {
            PAJLADA_REPORT_ERROR(error)
            return chatterino::KickIgnoredUser(0);
        }

        if (!chatterino::rj::getSafe(value, "userID", userID))
        {
            PAJLADA_REPORT_ERROR(error)
            return chatterino::KickIgnoredUser(0);
        }

        chatterino::rj::getSafe(value, "username", username);

        return chatterino::KickIgnoredUser(userID, username);
    }
};

}  // namespace pajlada
