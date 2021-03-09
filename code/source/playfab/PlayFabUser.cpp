#include <stdafx.h>
#include <playfab/PlayFabUser.h>

namespace PlayFab
{
User::User(SharedPtr<PlayFabApiSettings> settings, SharedPtr<PlayFabAuthenticationContext> authContext) :
    ClientApi{ settings, authContext },
    m_authContext{ authContext }
{
}
}