#pragma once

#include <playfab/PlayFabCallRequestContainer.h>
#include <playfab/PlayFabPluginManager.h>
#include <httpClient/pal.h>
#include <httpClient/httpClient.h>

namespace PlayFab
{
    /// <summary>
    /// PlayFabWinHttpPlugin is an https implementation to interact with PlayFab services using libHttpClient (HC).
    /// </summary>
    class PlayFabHCHttpPlugin : public IPlayFabHttpPlugin
    {
    public:
        PlayFabHCHttpPlugin() = default;
        PlayFabHCHttpPlugin(const PlayFabHCHttpPlugin& other) = delete;
        PlayFabHCHttpPlugin(PlayFabHCHttpPlugin&& other) = delete;
        PlayFabHCHttpPlugin& operator=(PlayFabHCHttpPlugin&& other) = delete;
        virtual ~PlayFabHCHttpPlugin() = default;

        virtual void MakePostRequest(std::unique_ptr<CallRequestContainerBase> requestContainer) override;
        virtual size_t Update() override;
    };
}