// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "playfab/PlayFabError.h"
#include "playfab/PlayFabClientDataModels.h"
#include "playfab/PlayFabClientApi.h"
#include "playfab/PlayFabSettings.h"
#include "playfab/PlayFabApiSettings.h"
#include "playfab/PlayFabPluginManager.h"
#include "playfab_gdk/GDK_PlayFabClientApi.h"
#include "playfab/PlayFabHCHttpPlugin.h"
#include <windows.h>

using namespace PlayFab;
using namespace ClientModels;

bool callComplete = false;
LoginResult loginResult{};

void OnLoginSuccess(const LoginResult& result, void* customData)
{
    printf("Congratulations, you made your first successful API call!\n");
    loginResult = result;
    callComplete = true;
}

void OnLoginFail(const PlayFabError& error, void* customData)
{
    printf("Api error!\n");
}

int main()
{
    PlayFabSettings::staticSettings->titleId = ("E18D7");

    {
        callComplete = false;
        LoginWithCustomIDRequest request;
        request.CreateAccount = false;
        request.CustomId = "GettingStartedGuide";

        PlayFabClientAPI::LoginWithCustomID(request, OnLoginSuccess, OnLoginFail);

        while (!callComplete)
        {
            PlayFabClientAPI::Update();
            Sleep(1);
        }
    }

    {
        // Get Player profile with existing client API
        GetPlayerProfileRequest request;
        request.PlayFabId = loginResult.PlayFabId;

        PlayFabClientAPI::GetPlayerProfile(request, 
            [] (const GetPlayerProfileResult& result, void* /*customData*/)
            {
                printf("Got player profile!\n");
            },
            [](const PlayFabError& error, void* /*customData*/)
            {
                printf("GetPlayerProfile error");
            }
            );

        while (PlayFabClientAPI::Update() != 0)
        {
            Sleep(1);
        }
    }

    {
        // Get Player profile with GDK API && HC HttpPlugin
        PlayFabPluginManager::SetPlugin(std::make_shared<PlayFabHCHttpPlugin>(), PlayFabPluginContract::PlayFab_Transport);

        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            PlayFabPlayerProfile profile{};
            PlayFabResultHandle handle{ nullptr };
            HRESULT hr = PlayFabGetPlayerProfileResult(async, &profile, &handle);
            if (SUCCEEDED(hr))
            {
                printf("Got player profile!\n");
                PlayFabReleaseResult(handle);
            }
        };

        HRESULT hr = PlayFabGetPlayerProfileAsync(nullptr, loginResult.PlayFabId.data(), &async);
        assert(SUCCEEDED(hr));

        XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    printf("Press enter to exit\n");
    getchar();
    return 0;
}