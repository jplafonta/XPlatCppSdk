#include <stdafx.h>
#include <playfab/PlayFabUser.h>

namespace PlayFab
{
User::User(std::shared_ptr<PlayFabAuthenticationContext> authContext) :
    ClientApi{ authContext },
    m_authContext{ authContext }
{
}
}