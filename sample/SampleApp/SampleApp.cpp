// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <string>
#include <playfab/PlayFabSettings.h>
#include <playfab/PlayFabApiSettings.h>
#include "playfab_c/PlayFabClientApi_c.h"

PlayFabStateHandle stateHandle{ nullptr };
PlayFabUserHandle userHandle{ nullptr };

int main()
{
    HRESULT hr = PlayFabInitialize(&stateHandle);
    assert(SUCCEEDED(hr));

    {
        PlayFabLoginWithCustomIDRequest request{};
        request.createAccount = false;
        request.customId = "GettingStartedGuide";
        request.titleId = "E18D7";

        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            PlayFabResultHandle resultHandle{ nullptr };
            PlayFabLoginResult* loginResult{ nullptr };
            HRESULT hr = PlayFabLoginWithCustomIDGetResult(async, &resultHandle, &loginResult);
            if (SUCCEEDED(hr))
            {
                hr = PlayFabUserDuplicateHandle(loginResult->userHandle, &userHandle);
                assert(SUCCEEDED(hr));
            }
            PlayFabResultCloseHandle(resultHandle);
        };

        HRESULT hr = PlayFabLoginWithCustomIDAsync(stateHandle, &request, &async);
        assert(SUCCEEDED(hr));

        hr = XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    {
        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            PlayFabResultHandle handle{ nullptr };
            PlayFabPlayerProfileResult* result{ nullptr };
            HRESULT hr = PlayFabGetPlayerProfileGetResult(async, &handle, &result);
            if (SUCCEEDED(hr))
            {
                printf("Got player profile!\n");
            }
            PlayFabResultCloseHandle(handle);
        };

        PlayFabGetPlayerProfileRequest request{};

        HRESULT hr = PlayFabGetPlayerProfileAsync(userHandle, &request, &async);
        assert(SUCCEEDED(hr));

        hr = XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    {
        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            size_t requiredBufferSize;
            HRESULT hr = PlayFabCreateSharedGroupGetResultSize(async, &requiredBufferSize);
            if (SUCCEEDED(hr))
            {
                std::vector<char> buffer(requiredBufferSize);
                PlayFabCreateSharedGroupResult* result;
                hr = PlayFabCreateSharedGroupGetResult(async, buffer.size(), buffer.data(), &result, nullptr);
                assert(SUCCEEDED(hr));
            }
        };

        PlayFabCreateSharedGroupRequest request{};

        HRESULT hr = PlayFabCreateSharedGroupAsync(userHandle, &request, &async);
        assert(SUCCEEDED(hr));

        hr = XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    {
        XAsyncBlock async{};
        async.callback = [](XAsyncBlock* async)
        {
            PlayFabGetTimeResult result;
            HRESULT hr = PlayFabGetTimeGetResult(async, &result);
            assert(SUCCEEDED(hr));
        };

        HRESULT hr = PlayFabGetTimeAsync(userHandle, &async);
        assert(SUCCEEDED(hr));

        hr = XAsyncGetStatus(&async, true);
        assert(SUCCEEDED(hr));
    }

    PlayFabUserCloseHandle(userHandle);

    {
        XAsyncBlock async{};
        PlayFabCleanupAsync(&async, stateHandle);
        XAsyncGetStatus(&async, true);
    }

    printf("Press enter to exit\n");
    getchar();
    return 0;
}