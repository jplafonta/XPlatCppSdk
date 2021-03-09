#pragma once

#include <playfab_c/PlayFabBaseModel_c.h>

enum class PlayFabEmailVerificationStatus : uint32_t
{
    Unverified = 0,
    Pending = 1,
    Confirmed = 2
};

enum class PlayFabLoginIdentityProvider : uint32_t
{
    Unknown = 0,
    PlayFab = 1,
    Custom = 2,
    GameCenter = 3,
    GooglePlay = 4,
    Steam = 5,
    XBoxLive = 6,
    PSN = 7,
    Kongregate = 8,
    Facebook = 9,
    IOSDevice = 10,
    AndroidDevice = 11,
    Twitch = 12,
    WindowsHello = 13,
    GameServer = 14,
    CustomServer = 15,
    NintendoSwitch = 16,
    FacebookInstantGames = 17,
    OpenIdConnect = 18,
    Apple = 19,
    NintendoSwitchAccount = 20
};

enum class PlayFabContinentCode : uint32_t
{
    AF = 0,
    AN = 1,
    AS = 2,
    EU = 3,
    NA = 4,
    OC = 5,
    SA = 6
};

enum class PlayFabCountryCode
{
    AF = 0,
    AX,
    AL,
    DZ,
    AS,
    AD,
    AO,
    AI,
    AQ,
    AG,
    AR,
    AM,
    AW,
    AU,
    AT,
    AZ,
    BS,
    BH,
    BD,
    BB,
    BY,
    BE,
    BZ,
    BJ,
    BM,
    BT,
    BO,
    BQ,
    BA,
    BW,
    BV,
    BR,
    IO,
    BN,
    BG,
    BF,
    BI,
    KH,
    CM,
    CA,
    CV,
    KY,
    CF,
    TD,
    CL,
    CN,
    CX,
    CC,
    CO,
    KM,
    CG,
    CD,
    CK,
    CR,
    CI,
    HR,
    CU,
    CW,
    CY,
    CZ,
    DK,
    DJ,
    DM,
    DO,
    EC,
    EG,
    SV,
    GQ,
    ER,
    EE,
    ET,
    FK,
    FO,
    FJ,
    FI,
    FR,
    GF,
    PF,
    TF,
    GA,
    GM,
    GE,
    DE,
    GH,
    GI,
    GR,
    GL,
    GD,
    GP,
    GU,
    GT,
    GG,
    GN,
    GW,
    GY,
    HT,
    HM,
    VA,
    HN,
    HK,
    HU,
    IS,
#pragma push_macro("IN")
#undef IN
    IN,
#pragma pop_macro("IN")
    ID,
    IR,
    IQ,
    IE,
    IM,
    IL,
    IT,
    JM,
    JP,
    JE,
    JO,
    KZ,
    KE,
    KI,
    KP,
    KR,
    KW,
    KG,
    LA,
    LV,
    LB,
    LS,
    LR,
    LY,
    LI,
    LT,
    LU,
    MO,
    MK,
    MG,
    MW,
    MY,
    MV,
    ML,
    MT,
    MH,
    MQ,
    MR,
    MU,
    YT,
    MX,
    FM,
    MD,
    MC,
    MN,
    ME,
    MS,
    MA,
    MZ,
    MM,
    NA,
    NR,
    NP,
    NL,
    NC,
    NZ,
    NI,
    NE,
    NG,
    NU,
    NF,
    MP,
    NO,
    OM,
    PK,
    PW,
    PS,
    PA,
    PG,
    PY,
    PE,
    PH,
    PN,
    PL,
    PT,
    PR,
    QA,
    RE,
    RO,
    RU,
    RW,
    BL,
    SH,
    KN,
    LC,
    MF,
    PM,
    VC,
    WS,
    SM,
    ST,
    SA,
    SN,
    RS,
    SC,
    SL,
    SG,
    SX,
    SK,
    SI,
    SB,
    SO,
    ZA,
    GS,
    SS,
    ES,
    LK,
    SD,
    SR,
    SJ,
    SZ,
    SE,
    CH,
    SY,
    TW,
    TJ,
    TZ,
    TH,
    TL,
    TG,
    TK,
    TO,
    TT,
    TN,
    TR,
    TM,
    TC,
    TV,
    UG,
    UA,
    AE,
    GB,
    US,
    UM,
    UY,
    UZ,
    VU,
    VE,
    VN,
    VG,
    VI,
    WF,
    EH,
    YE,
    ZM,
    ZW
};

enum class PlayFabSubscriptionProviderStatus : uint32_t
{
    NoError = 0,
    Cancelled = 1,
    UnknownError = 2,
    BillingError = 3,
    ProductUnavailable = 4,
    CustomerDidNotAcceptPriceChange = 5,
    FreeTrial = 6,
    PaymentPending = 7
};

enum class PlayFabPushNotificationPlatform : uint32_t
{
    ApplePushNotificationService = 0,
    GoogleCloudMessaging = 1
};

struct PlayFabPlayerProfileViewConstraints
{
    bool showAvatarUrl;
    bool showBannedUntil;
    bool showCampaignAttributions;
    bool showContactEmailAddresses;
    bool showCreated;
    bool showDisplayName;
    bool showExperimentVariants;
    bool showLastLogin;
    bool showLinkedAccounts;
    bool showLocations;
    bool showMemberships;
    bool showOrigination;
    bool showPushNotificationRegistrations;
    bool showStatistics;
    bool showTags;
    bool showTotalValueToDateInUsd;
    bool showValuesToDate;
};

struct PlayFabGetPlayerCombinedInfoRequestParams
{
    bool getCharacterInventories;
    bool getCharacterList;
    bool getPlayerProfile;
    bool getPlayerStatistics;
    bool getTitleData;
    bool getUserAccountInfo;
    bool getUserData;
    bool getUserInventory;
    bool getUserReadOnlyData;
    bool getUserVirtualCurrency;
    const char** playerStatisticNames;
    size_t playerStatisticNamesCount;
    PlayFabPlayerProfileViewConstraints* profileConstraints;
    const char** titleDataKeys;
    size_t titleDataKeysCount;
    const char** userDataKeys;
    size_t userDataKeysCount;
    const char** userReadOnlyDataKeys;
    size_t userReadOnlyDataKeysCount;
};

struct PlayFabLoginWithCustomIDRequest
{
    bool* createAccount;
    const char* customId;
    PlayFabDictionaryEntry* customTags;
    size_t customTagsCount;
    const char* encryptedRequest;
    PlayFabGetPlayerCombinedInfoRequestParams* infoRequestParameters;
    const char* playerSecret;
    const char* titleId;
};

struct PlayFabAdCampaignAttributionModel
{
    time_t attributedAt;
    const char* campaignId;
    const char* platform;
};

struct PlayFabContactEmailInfoModel
{
    const char* emailAddress;
    const char* name;
    PlayFabEmailVerificationStatus* verificationStatus;
};

struct PlayFabLinkedPlatformAccountModel
{
    const char* email;
    PlayFabLoginIdentityProvider* platform;
    const char* platformUserId;
    const char* username;
};

struct PlayFabLocationModel
{
    const char* city;
    PlayFabContinentCode* pfContinentCode;
    PlayFabCountryCode* pfCountryCode;
    double* latitude;
    double* longitude;
};

struct PlayFabSubscriptionModel
{
    time_t expiration;
    time_t initialSubscriptionTime;
    bool isActive;
    PlayFabSubscriptionProviderStatus* status;
    const char* subscriptionId;
    const char* subscriptionItemId;
    const char* subscriptionProvider;
};

struct PlayFabMembershipModel
{
    bool isActive;
    time_t membershipExpiration;
    const char* membershipId;
    time_t* overrideExpiration;
    PlayFabSubscriptionModel** subscriptions;
    size_t subscriptionsCount;
};

struct PlayFabPushNotificationRegistrationModel
{
    const char* notificationEndpointARN;
    PlayFabPushNotificationPlatform* platform;
};

struct PlayFabStatisticModel
{
    const char* name;
    uint32_t value;
    uint32_t version;
};

struct PlayFabTagModel
{
    const char* tagValue;
};

struct PlayFabValueToDateModel
{
    const char* currency;
    uint32_t totalValue;
    const char* totalValueAsDecimal;
};

struct PlayFabPlayerProfileModel
{
    PlayFabAdCampaignAttributionModel** adCampaignAttributions;
    size_t adCampaignAttributionsCount;
    const char* avatarUrl;
    time_t* bannedUntil;
    PlayFabContactEmailInfoModel** contactEmailAddresses;
    size_t contactEmailAddressesCount;
    time_t* created;
    const char* displayName;
    const char** experimentVariants;
    size_t experimentVariantsCount;
    time_t* lastLogin;
    PlayFabLinkedPlatformAccountModel** linkedAccounts;
    size_t linkedAccountsCount;
    PlayFabLocationModel** locations;
    size_t locationsCount;
    PlayFabMembershipModel** memberships;
    size_t membershipsCount;
    PlayFabLoginIdentityProvider* origination;
    const char* playerId;
    const char* publisherId;
    PlayFabPushNotificationRegistrationModel** pushNotificationRegistrations;
    size_t pushNotificationRegistrationsCount;
    PlayFabStatisticModel** statistics;
    size_t statisticsCount;
    PlayFabTagModel** tags;
    size_t tagsCount;
    const char* titleId;
    uint32_t* totalValueToDateInUSD;
    PlayFabValueToDateModel** valuesToDate;
    size_t valuesToDateCount;
};

struct PlayFabGetPlayerProfileRequest
{
    PlayFabDictionaryEntry* customTags;
    size_t customTagsCount;
    const char* playFabId;
    PlayFabPlayerProfileViewConstraints* profileConstraints;
};