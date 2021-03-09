#pragma once

#include <playfab/PlayFabAuthenticationContext.h>
#include <playfab/PlayFabClientInstanceApi.h>

namespace PlayFab
{
class User
{
public:
    User(SharedPtr<PlayFabApiSettings> settings, SharedPtr<PlayFabAuthenticationContext> authContext);

    PlayFabClientInstanceAPI ClientApi;
private:
    SharedPtr<PlayFabAuthenticationContext> m_authContext;
};
}

struct PlayFabUserHolder
{
    PlayFab::SharedPtr<PlayFab::User> user;
};