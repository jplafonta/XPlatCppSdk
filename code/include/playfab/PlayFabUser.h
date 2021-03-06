#pragma once

#include <playfab/PlayFabAuthenticationContext.h>
#include <playfab/PlayFabClientInstanceApi.h>

namespace PlayFab
{
class User
{
public:
    User() = default;
    User(std::shared_ptr<PlayFabAuthenticationContext> authContext);

    PlayFabClientInstanceAPI ClientApi;
private:
    std::shared_ptr<PlayFabAuthenticationContext> m_authContext;
};
}

struct PlayFabUserHolder
{
    std::shared_ptr<PlayFab::User> user;
};