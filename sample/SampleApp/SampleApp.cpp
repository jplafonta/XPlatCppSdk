// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <string>
#include <playfab/PlayFabSettings.h>
#include <playfab/PlayFabApiSettings.h>
#include "playfab_c/PlayFabClientApi_c.h"

PlayFabUserHandle userHandle{ nullptr };
std::string playFabId;

int main()
{
    HRESULT hr = PlayFabInitialize("E18D7");
    assert(SUCCEEDED(hr));

    {
        PlayFabLoginWithCustomIDRequest request{};
        request.createAccount = false;
        request.customId = "GettingStartedGuide";

        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            PlayFabResultHandle resultHandle{ nullptr };
            HRESULT hr = PlayFabGetResultHandle(async, &resultHandle);
            if (SUCCEEDED(hr))
            {
                PlayFabLoginWithCustomIDResultGetUserHandle(resultHandle, &userHandle);
                const char* playFabIdPtr;
                PlayFabLoginWithCustomIdResultGetPlayFabId(resultHandle, &playFabIdPtr);
                playFabId = playFabIdPtr;
            }
            PlayFabResultCloseHandle(resultHandle);
        };

        HRESULT hr = PlayFabLoginWithCustomIDAsync(&request, &async);
        assert(SUCCEEDED(hr));

        hr = XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    {
        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            PlayFabResultHandle handle{ nullptr };
            HRESULT hr = PlayFabGetResultHandle(async, &handle);
            if (SUCCEEDED(hr))
            {
                PlayFabPlayerProfileModel* profile{ nullptr };
                hr = PlayFabGetPlayerProfileResultGetProfile(handle, &profile);
                if (SUCCEEDED(hr))
                {
                    printf("Got player profile!\n");
                }
            }
            PlayFabResultCloseHandle(handle);
        };

        PlayFabGetPlayerProfileRequest request{};
        request.playFabId = playFabId.data();

        HRESULT hr = PlayFabGetPlayerProfileAsync(userHandle, &request, &async);
        assert(SUCCEEDED(hr));

        XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    {
        XAsyncBlock async{};
        PlayFabCleanupAsync(&async);
        XAsyncGetStatus(&async, true);
    }

    printf("Press enter to exit\n");
    getchar();
    return 0;
}