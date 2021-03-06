#pragma once

#include <playfab/PlayFabPlatformMacros.h>
#include <playfab/PlayFabPlatformTypes.h>
#include <playfab/PlayFabJsonHeaders.h>
#include <playfab/PlayFabPlatformTypes.h>
#include <playfab/PlayFabPlatformUtils.h>
#include <playfab/internal_memory.h>
#include <playfab/std_optional.h>
#include <playfab_c/PlayFabBaseModel_c.h>

#include <assert.h>
#include <functional>
#include <list>
#include <memory>

namespace PlayFab
{
    class PlayFabAuthenticationContext;

    template <typename BoxedType>
    class Boxed
    {
    public:
        Boxed() : boxedValue(), isSet(false) {}
        Boxed(BoxedType value) : boxedValue(value), isSet(true) {}

        inline Boxed& operator=(BoxedType value) { boxedValue = value; isSet = true; return *this; }
        inline operator const BoxedType& () const { assert(notNull()); return *operator->(); }
        inline BoxedType* operator->() { return isSet ? &boxedValue : nullptr; }
        inline const BoxedType* operator->() const { return isSet ? &boxedValue : nullptr; }

        inline void setNull() { isSet = false; }
        inline bool notNull() const { return isSet; }
        inline bool isNull() const { return !isSet; }
    private:
        BoxedType boxedValue;
        bool isSet;
    };

    template<typename ResType> using ProcessApiCallback = std::function<void(const ResType& result, void* customData)>;

    /// <summary>
    /// Base class for all PlayFab Models
    /// Provides interface for converting to/from json
    /// </summary>
    struct PlayFabBaseModel
    {
        virtual ~PlayFabBaseModel() {}
        virtual void FromJson(const Json::Value& input) = 0;
        virtual Json::Value ToJson() const = 0;
    };

    /// <summary>
    /// Base class for all PlayFab Models
    /// Provides interface for converting to/from json
    /// Renamed to Model to have consistent naming conventions (non-namespaced public types should be
    /// prefixed with PlayFab, internal types will be in PlayFab namespace and not be prefixed)
    /// </summary>
    struct BaseModel
    {
        virtual ~BaseModel() {}
        virtual void FromJson(const Json::Value& input) = 0;
        virtual Json::Value ToJson() const = 0;
    };

    /// <summary>
    /// Base class for all PlayFab Requests
    /// </summary>
    struct PlayFabRequestCommon : public BaseModel {
        /// <summary>
        /// An optional authentication context, it can used in multi-user scenarios
        /// </summary>
        std::shared_ptr<PlayFabAuthenticationContext> authenticationContext;
    };

    /// <summary>
    /// Base class for all PlayFab Requests
    /// </summary>
    struct RequestCommon : public BaseModel
    {
        /// Do Requests need FromJson method?
    };

    /// <summary>
    /// Base class for all PlayFab Results
    /// </summary>
    struct PlayFabResultCommon : public PlayFabBaseModel {
        Json::Value Request;
    };

    /// <summary>
    /// Base class for all PlayFab Results
    /// </summary>
    struct ResultCommon : public BaseModel
    {
        Json::Value request;
    };

    /// <summary>
    /// Base class for all PlayFab Login method Results
    /// </summary>
    struct PlayFabLoginResultCommon : public PlayFabResultCommon {
        /// <summary>
        /// An authentication context returned by Login methods, it can used in multi-user scenarios
        /// </summary>
        std::shared_ptr<PlayFabAuthenticationContext> authenticationContext;
    };

    /// <summary>
    /// Helper class for managing C-style arrays of pointers
    /// </summary>
    template<typename PointerT, typename ObjectT> class PointerArray : public BaseModel
    {
    public:
        static_assert(std::is_base_of_v<PointerT, ObjectT> || std::is_same_v<PointerT, ObjectT>, "ObjectT must be a PointerT");
        static_assert(std::is_base_of_v<BaseModel, ObjectT>, "ObjectT must be a PlayFab::BaseModel");

        PointerArray() = default;
        virtual ~PointerArray() = default;

        PointerArray(const PointerArray& src)
            : m_objects(src.m_objects)
        {
            m_pointers.reserve(m_objects.size());
            for (const auto& o : m_objects)
            {
                m_pointers.push_back((PointerT*)&o);
            }
        }

        PointerArray(PointerArray&& src)
            : m_pointers(std::move(src.m_pointers)),
            m_objects(std::move(src.m_objects))
        {
        }

        PointerArray& operator=(PointerArray src)
        {
            m_pointers = std::move(src.m_pointers);
            m_objects = std::move(src.m_objects);
            return *this;
        }

        PointerArray& operator=(PointerArray&& src)
        {
            m_pointers = std::move(src.m_pointers);
            m_objects = std::move(src.m_objects);
            return *this;
        }

        bool Empty() const
        {
            return m_pointers.empty();
        }

        const PointerT** Data()
        {
            return m_pointers.data();
        }

        size_t Size() const
        {
            assert(m_pointers.size() == m_objects.size());
            return m_pointers.size();
        }

        void Clear()
        {
            m_pointers.clear();
            m_objects.clear();
        }

        void FromJson(const Json::Value& input)
        {
            Clear();
            FromJsonUtilO(input, m_objects);

            m_pointers.reserve(m_objects.size());
            for (const auto& o : m_objects)
            {
                m_pointers.push_back((PointerT*)&o);
            }
        }

        Json::Value ToJson() const override
        {
            // TODO
            return Json::Value();
        }

    private:
        Vector<const PointerT*> m_pointers;
        List<ObjectT> m_objects;
    };

    template <typename EntryT, typename ValueT>
    class AssociativeArray : public BaseModel
    {
    public:
        void FromJson(const Json::Value& src) override {};
        Json::Value ToJson() const override { return Json::Value(); }
    };

    // Utilities for [de]serializing time_t to/from json
    inline void ToJsonUtilT(const time_t input, Json::Value& output)
    {
        output = Json::Value(TimeTToIso8601String(input));
    }
    inline void FromJsonUtilT(const Json::Value& input, time_t& output)
    {
        if (input == Json::Value::null)
        {
            return;
        }
        output = Iso8601StringToTimeT(input.asString());
    }

    inline void ToJsonUtilT(const Boxed<time_t>& input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            ToJsonUtilT(static_cast<time_t>(input), output);
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, Boxed<time_t>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            time_t outputVal = {};
            FromJsonUtilT(input, outputVal);
            output = outputVal;
        }
    }

    inline void ToJsonUtilT(const time_t* input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value();
        }
        else
        {
            ToJsonUtilT(*input, output);
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, StdExtra::optional<time_t>& output, time_t*& outputPointer)
    {
        if (input == Json::Value::null)
        {
            output.reset();
            outputPointer = nullptr;
        }
        else
        {
            time_t outputVal = {};
            FromJsonUtilT(input, outputVal);
            output = outputVal;
            outputPointer = output.operator->();
        }
    }

    inline void ToJsonUtilT(const std::list<time_t>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilT(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, std::list<time_t>& output)
    {
        output.clear();
        if (input == Json::Value::null || !input.isArray())
        {
            return;
        }

        time_t eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilT(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    inline void ToJsonUtilT(const std::map<std::string, time_t>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilT(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    inline void FromJsonUtilT(const Json::Value& input, std::map<std::string, time_t>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        time_t eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilT(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    // Utilities for [de]serializing EnumType to/from json
    template <typename EnumType> inline void ToJsonUtilE(const Boxed<EnumType>& input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            ToJsonEnum(input, output);
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, Boxed<EnumType>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            EnumType outputVal;
            FromJsonEnum(input, outputVal);
            output = outputVal;
        }
    }

    template <typename EnumType> inline void ToJsonUtilE(const EnumType* input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value();
        }
        else
        {
            // Will need to move all ToJsonEnum methods to a common namespace.
            // They were found by ADL (https://en.cppreference.com/w/cpp/language/adl) previously but enums have been moved to global namespace
            PlayFab::ClientModels::ToJsonEnum(*input, output);
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, StdExtra::optional<EnumType>& output, EnumType*& outputPointer)
    {
        if (input == Json::Value::null)
        {
            output.reset();
            outputPointer = nullptr;
        }
        else
        {
            EnumType outputVal;
            ClientModels::FromJsonEnum(input, outputVal);
            output = outputVal;
            outputPointer = output.operator->();
        }
    }

    template <typename EnumType> inline void ToJsonUtilE(const std::list<EnumType>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonEnum(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, std::list<EnumType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        EnumType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonEnum(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename EnumType> inline void ToJsonUtilE(const std::map<std::string, EnumType>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonEnum(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    template <typename EnumType> inline void FromJsonUtilE(const Json::Value& input, std::map<std::string, EnumType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        EnumType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonEnum(iter, eachOutput);
            output[iter.key()] = eachOutput;
        }
    }

    // Utilities for [de]serializing string to/from json
    inline void ToJsonUtilS(const std::string& input, Json::Value& output)
    {
        if (input.length() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(input);
        }
    }

    inline void ToJsonUtilS(const char* input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(input);
        }
    }

    inline void ToJsonUtilS(const String& input, Json::Value& output)
    {
        if (input.empty())
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(input.data());
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, std::string& output)
    {
        if (input == Json::Value::null)
        {
            output.clear();
        }
        else
        {
            output = input.asString();
        }
    }

    inline void ToJsonUtilS(const std::list<std::string>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilS(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    // Consider something like std::span for these arrays
    inline void ToJsonUtilS(const char** input, size_t inputCount, Json::Value& output)
    {
        if (!input || inputCount == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            for (auto i = 0u; i < inputCount; ++i)
            {
                ToJsonUtilS(input[i], eachOutput);
                output[i] = eachOutput;
            }
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, std::list<std::string>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        std::string eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilS(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    inline void ToJsonUtilS(const std::map<std::string, std::string>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilS(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, std::map<std::string, std::string>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        std::string eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilS(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    inline void FromJsonUtilS(const Json::Value& input, String& output, const char*& outputPointer)
    {
        if (input == Json::Value::null)
        {
            output.clear();
            outputPointer = nullptr;
        }
        else
        {
            output = input.asCString();
            outputPointer = output.data();
        }
    }

    // Utilities for [de]serializing PlayFabObjects to/from json
    inline void ToJsonUtilO(const PlayFabBaseModel& input, Json::Value& output)
    {
        output = input.ToJson();
    }

    inline void ToJsonUtilO(const BaseModel& input, Json::Value& output)
    {
        output = input.ToJson();
    }

    inline void FromJsonUtilO(const Json::Value& input, PlayFabBaseModel& output)
    {
        output.FromJson(input);
    }

    inline void FromJsonUtilO(const Json::Value& input, BaseModel& output)
    {
        output.FromJson(input);
    }

    template <typename ObjectType> inline void ToJsonUtilO(const Boxed<ObjectType> input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value();
        }
        else
        {
            output = static_cast<ObjectType>(input).ToJson();
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const StdExtra::optional<ObjectType>& input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value::null;
        }
        else
        {
            output = input->ToJson();
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const ObjectType* input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value::null;
        }
        else
        {
            output = PlayFab::JsonUtils::ToJsonObject(*input);
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, Boxed<ObjectType>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            ObjectType outputTemp;
            outputTemp.FromJson(input);
            output = outputTemp;
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, StdExtra::optional<ObjectType>& output)
    {
        if (input == Json::Value::null)
        {
            output.reset();
        }
        else
        {
            ObjectType outputTemp;
            outputTemp.FromJson(input);
            output = outputTemp;
        }
    }

    template <typename ObjectType, typename PointerType> inline void FromJsonUtilO(const Json::Value& input, StdExtra::optional<ObjectType>& output, PointerType*& outputPointer)
    {
        if (input == Json::Value::null)
        {
            output.reset();
            outputPointer = nullptr;
        }
        else
        {
            ObjectType outputTemp;
            outputTemp.FromJson(input);
            output = outputTemp;
            outputPointer = output.operator->();
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const std::list<ObjectType>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilO(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, std::list<ObjectType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        ObjectType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilO(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, List<ObjectType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        ObjectType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilO(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const std::map<std::string, ObjectType>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilO(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    template <typename ObjectType> inline void FromJsonUtilO(const Json::Value& input, std::map<std::string, ObjectType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        ObjectType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilO(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    template <typename ObjectType> inline void ToJsonUtilO(const ObjectType** input, size_t inputCount, Json::Value& output)
    {
        if (!input || inputCount == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            for (auto i = 0u; i < inputCount; ++i)
            {
                ToJsonUtilO(input[i], eachOutput);
                output[i] = eachOutput;
            }
        }
    }

    // Utilities for [de]serializing primitives to/from json
    template <typename PrimitiveType> inline void ToJsonUtilP(const PrimitiveType& input, Json::Value& output)
    {
        output = Json::Value(input);
    }

    inline void FromJsonUtilP(const Json::Value& input, bool& output)
    {
        output = input == Json::Value::null ? false : input.asBool();
    }

    inline void FromJsonUtilP(const Json::Value& input, Int16& output)
    {
        output = input == Json::Value::null ? 0 : static_cast<Int16>(input.asInt());
    }

    inline void FromJsonUtilP(const Json::Value& input, Uint16& output)
    {
        output = input == Json::Value::null ? static_cast<Uint16>(0) : static_cast<Uint16>(input.asInt());
    }

    inline void FromJsonUtilP(const Json::Value& input, Int32& output)
    {
        output = input == Json::Value::null ? 0 : input.asInt();
    }

    inline void FromJsonUtilP(const Json::Value& input, Uint32& output)
    {
        output = input == Json::Value::null ? 0 : input.asUInt();
    }

    inline void FromJsonUtilP(const Json::Value& input, Int64& output)
    {
        output = input == Json::Value::null ? 0 : input.asInt64();
    }

    inline void FromJsonUtilP(const Json::Value& input, Uint64& output)
    {
        output = input == Json::Value::null ? 0 : input.asUInt64();
    }

    inline void FromJsonUtilP(const Json::Value& input, float& output)
    {
        output = input == Json::Value::null ? 0 : input.asFloat();
    }

    inline void FromJsonUtilP(const Json::Value& input, double& output)
    {
        output = input == Json::Value::null ? 0 : input.asDouble();
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const Boxed<PrimitiveType>& input, Json::Value& output)
    {
        if (input.isNull())
        {
            output = Json::Value::null;
        }
        else
        {
            ToJsonUtilP(static_cast<PrimitiveType>(input), output);
        }
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const StdExtra::optional<PrimitiveType>& input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value::null;
        }
        else
        {
            ToJsonUtilP(*input, output);
        }
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const PrimitiveType* input, Json::Value& output)
    {
        if (!input)
        {
            output = Json::Value::null;
        }
        else
        {
            ToJsonUtilP(*input, output);
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, Boxed<PrimitiveType>& output)
    {
        if (input == Json::Value::null)
        {
            output.setNull();
        }
        else
        {
            PrimitiveType outputVal;
            FromJsonUtilP(input, outputVal);
            output = outputVal;
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, StdExtra::optional<PrimitiveType>& output, PrimitiveType*& outputPointer)
    {
        if (input == Json::Value::null)
        {
            output.reset();
            outputPointer = nullptr;
        }
        else
        {
            PrimitiveType outputVal;
            FromJsonUtilP(input, outputVal);
            output = outputVal;
            outputPointer = output.operator->();
        }
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const std::list<PrimitiveType>& input, Json::Value& output)
    {
        if (input.size() == 0)
        {
            output = Json::Value::null;
        }
        else
        {
            output = Json::Value(Json::arrayValue);
            Json::Value eachOutput;
            int index = 0;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                ToJsonUtilP(*iter, eachOutput);
                output[index++] = eachOutput;
            }
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, std::list<PrimitiveType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        PrimitiveType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilP(*iter, eachOutput);
            output.push_back(eachOutput);
        }
    }

    template <typename PrimitiveType> inline void ToJsonUtilP(const std::map<std::string, PrimitiveType>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            ToJsonUtilP(iter->second, eachOutput);
            output[iter->first] = eachOutput;
        }
    }

    template <typename PrimitiveType> inline void FromJsonUtilP(const Json::Value& input, std::map<std::string, PrimitiveType>& output)
    {
        output.clear();
        if (input == Json::Value::null)
        {
            return;
        }

        PrimitiveType eachOutput;
        for (auto iter = input.begin(); iter != input.end(); ++iter)
        {
            FromJsonUtilP(*iter, eachOutput);
            output[iter.key().asString()] = eachOutput;
        }
    }

    template<> class PointerArray<const char*, String> : public BaseModel
    {
    public:
        PointerArray() = default;
        virtual ~PointerArray() = default;

        PointerArray(const PointerArray& src)
            : m_strings(src.m_strings)
        {
            m_pointers.reserve(m_strings.size());
            for (const auto& o : m_strings)
            {
                m_pointers.push_back((const char*)&o);
            }
        }

        PointerArray(PointerArray&& src)
            : m_pointers(std::move(src.m_pointers)),
            m_strings(std::move(src.m_strings))
        {
        }

        PointerArray& operator=(PointerArray src)
        {
            m_pointers = std::move(src.m_pointers);
            m_strings = std::move(src.m_strings);
            return *this;
        }

        PointerArray& operator=(PointerArray&& src)
        {
            m_pointers = std::move(src.m_pointers);
            m_strings = std::move(src.m_strings);
            return *this;
        }

        bool Empty() const
        {
            return m_pointers.empty();
        }

        const char** Data()
        {
            return m_pointers.data();
        }

        size_t Size() const
        {
            assert(m_pointers.size() == m_strings.size());
            return m_pointers.size();
        }

        void Clear()
        {
            m_pointers.clear();
            m_strings.clear();
        }

        void FromJson(const Json::Value& input) override
        {
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                String output;
                const char* outputPointer;
                FromJsonUtilS(*iter, output, outputPointer);
                m_pointers.push_back(outputPointer);
                m_strings.push_back(std::move(output));
            }
        }

        Json::Value ToJson() const override
        {
            // TODO
            return Json::Value{};
        }

    private:
        Vector<const char*> m_pointers;
        List<String> m_strings;
    };


    template<typename PublicT, typename InternalT> inline void FromJsonUtilA(
        const Json::Value& input,
        PointerArray<PublicT, InternalT>& output,
        const PublicT**& outputPointer,
        size_t& outputCount
    )
    {
        output.Clear();
        if (input == Json::Value::null)
        {
            outputPointer = nullptr;
            outputCount = 0;
            return;
        }

        output.FromJson(input);
        outputPointer = output.Data();
        outputCount = output.Size();
    }

    inline void FromJsonUtilA(
        const Json::Value& input,
        PointerArray<const char*, String>& output,
        const char**& outputPointer,
        size_t& outputCount
    )
    {
        output.Clear();
        if (input == Json::Value::null)
        {
            outputPointer = nullptr;
            outputCount = 0;
            return;
        }

        output.FromJson(input);
        outputPointer = output.Data();
        outputCount = output.Size();
    }

    template <> class AssociativeArray<PlayFabDictionaryEntry, String> : public BaseModel
    {
    public:
        bool Empty() const
        {
            return m_sequentialEntries.empty();
        }

        PlayFabDictionaryEntry* Data()
        {
            return m_sequentialEntries.data();
        }

        const PlayFabDictionaryEntry* Data() const
        {
            return m_sequentialEntries.data();
        }

        size_t Size() const
        {
            return m_sequentialEntries.size();
        }

        void Clear()
        {
            m_sequentialEntries.clear();
            m_map.clear();
        }

        void FromJson(const Json::Value& input) override
        {
            Clear();

            String eachOutput;
            for (auto iter = input.begin(); iter != input.end(); ++iter)
            {
                PlayFabDictionaryEntry entry;
                FromJsonUtilS(*iter, eachOutput, entry.value);
                m_map[iter.key().asCString()] = eachOutput;
            }
        }

        Json::Value ToJson() const override
        {
            // TODO
            return Json::Value{};
        }

    private:
        Vector<PlayFabDictionaryEntry> m_sequentialEntries;
        UnorderedMap<String, String> m_map;
    };

    // TODO fix constness
    inline void ToJsonUtilA(const AssociativeArray<PlayFabDictionaryEntry, String>& input, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        // consider adding iterator to AssociativeArray
        for (auto i = 0u; i < input.Size(); ++i)
        {
            auto& entry = input.Data()[i];
            ToJsonUtilS(entry.value, eachOutput);
            output[entry.key] = eachOutput;
        }
    }

    // TODO consider something like std::span here
    inline void ToJsonUtilA(const PlayFabDictionaryEntry* input, size_t inputCount, Json::Value& output)
    {
        output = Json::Value(Json::objectValue);
        Json::Value eachOutput;
        for (auto i = 0u; i < inputCount; ++i)
        {
            auto& entry = input[i];
            ToJsonUtilS(entry.value, eachOutput);
            output[entry.key] = eachOutput;
        }
    }

    inline void FromJsonUtilA(const Json::Value& input, AssociativeArray<PlayFabDictionaryEntry, String>& output, PlayFabDictionaryEntry*& outputPointer, size_t& outputCount)
    {
        output.Clear();
        if (input == Json::Value::null)
        {
            outputPointer = nullptr;
            outputCount = 0;
        }
        else
        {
            output.FromJson(input);
            outputPointer = output.Data();
            outputCount = output.Size();
        }
    }
}

/// <summary>
/// Result holder which will be returned to callers
/// </summary>
struct PlayFabResultHolder
{
    std::shared_ptr<PlayFab::PlayFabResultCommon> result;
};
