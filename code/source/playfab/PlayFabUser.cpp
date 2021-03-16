#include <stdafx.h>
#include <playfab/PlayFabUser.h>
#include <playfab/PlayFabClientDataModels.h>
#include <playfab/PlayFabSettings.h>

namespace PlayFab
{
User::User(
    SharedPtr<PlayFabApiSettings> settings,
    SharedPtr<PlayFabAuthenticationContext> authContext,
    const ClientModels::UserSettings& userSettings
) :
    ClientApi{ settings, authContext },
    m_authContext{ std::move(authContext) },
    m_userSettings{ std::move(userSettings) }
{
    // Handle multi step client login
    // TODO do we not care about handling errors here?
    if (m_userSettings.NeedsAttribution && !m_authContext->disableAdvertising && m_authContext->advertisingIdType.length() > 0 && m_authContext->advertisingIdValue.length() > 0)
    {
        ClientModels::AttributeInstallRequest request;
        if (m_authContext->advertisingIdType == PlayFabSettings::AD_TYPE_IDFA)
        {
            request.Idfa = m_authContext->advertisingIdValue;
        }
        else if (m_authContext->advertisingIdType == PlayFabSettings::AD_TYPE_ANDROID_ID)
        {
            request.Adid = m_authContext->advertisingIdValue;
        }
        ClientApi.AttributeInstall(request, nullptr, nullptr);
    }
}

}