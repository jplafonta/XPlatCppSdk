#pragma once

#include <playfab/PlayFabAuthenticationContext.h>
#include <playfab/PlayFabClientInstanceApi.h>

namespace PlayFab
{
class User
{
public:
    User(SharedPtr<PlayFabApiSettings> settings, SharedPtr<PlayFabAuthenticationContext> authContext, const ClientModels::UserSettings& userSettings);

    PlayFabClientInstanceAPI ClientApi;
private:

    SharedPtr<PlayFabAuthenticationContext> m_authContext;
    ClientModels::UserSettings const m_userSettings;
};
}

struct PlayFabUser
{
    PlayFabUser(PlayFab::SharedPtr<PlayFab::User> _user) : user(std::move(_user)) {};
    PlayFab::SharedPtr<PlayFab::User> user;
};
