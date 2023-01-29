#ifndef MAIN_H
#define MAIN_H
#include <iostream>

#include "json.h"

using ExtendedTypeSupport::TypeToStr;
using reflection::Super;
using u8 = uint8_t;

namespace Rest {
enum class Method { GET, PATCH };
struct Url {
    std::string_view url;
};
template <typename T>
struct PathParam {
    std::string_view param;
};
struct ControllerWrapper {};
static constexpr ControllerWrapper Controller{};
}  // namespace Rest

class SuperA {
public:
    SuperA() : superVal(0) {}

    int superVal;

    REFLECT(SuperA, superVal)
};

NOTE(OtherSuperA)
class OtherSuperA {
public:
    REFLECT_EMPTY(OtherSuperA)
};

class Composed {
public:
    Composed() : composedVal(0) {}

    int composedVal;

    REFLECT(Composed, composedVal)
};

NOTE(A, Super<SuperA>(json::Name{"SupA"}), Super<OtherSuperA>)
class A : public SuperA, public OtherSuperA {
public:
    enum_t(TestEnum, u8, {first, second});
    static const std::unordered_map<std::string, TestEnum> TestEnumCache;
    static const std::unordered_map<std::string, TestEnum> TestEnumCacheCustom;

    A() : testEnum(TestEnum::first), first(0), firstReference(first), ptr(nullptr), composed(), boolean(false), putCache(false), customInt(0), str("") {
        ray[0] = 0;
        ray[1] = 0;
    }

    TestEnum testEnum;
    int first;
    int& firstReference;
    static int second;
    static int& secondReference;
    int* ptr;
    Composed composed;
    NOTE(boolean, json::Ignore)
    bool boolean;
    bool putCache;
    uint16_t customInt;
    NOTE(str, json::Name{"string"})
    std::string str;
    std::map<std::string, std::string> map;
    std::vector<std::vector<int>> vecVec;
    int ray[2];
    std::shared_ptr<json::Value> runtime;
    std::vector<std::shared_ptr<int>> autoAllocate;
    std::shared_ptr<json::Generic::FieldCluster> unknownFields;

    REFLECT_NOTED(A, testEnum, composed, first, firstReference, second, secondReference, ptr, boolean, putCache, customInt, str, map, vecVec, ray, runtime, autoAllocate, unknownFields)
};

std::istream& operator>>(std::istream& is, A::TestEnum& testEnum);

template <>
struct json::Output::Customize<A, uint16_t, A::Class::IndexOf::customInt> {
    static bool As(OutStreamType& os, Context& context, const A& object, const uint16_t& value) {
        if (value > 9000) json::Put::String(os, "Over 9000!");

        return value > 9000;
    }
};

struct EnhancedContext : public json::Context {
    virtual ~EnhancedContext() {}

    EnhancedContext(int enhanced) : enhanced(enhanced) {}

    int enhanced;
};

// In a .cpp file you can usually only use explicit instantiation for templates (under normal circumstances)
// In a .h or .hpp file you could use proper template arguments (e.g. have the method apply to any index, or to the same type in any object at any index)
template <size_t FieldIndex>
struct json::Input::Customize<A, A::TestEnum, FieldIndex> {
    static bool As(std::istream& is, Context& context, const A& object, A::TestEnum& value) {
        std::string input = json::Read::String<>(is);
        auto found = A::TestEnumCacheCustom.find(input);
        if (found != A::TestEnumCacheCustom.end()) {
            value = found->second;
            return true;
        } else
            return false;
    }
};

template <>
struct json::Input::CustomizeType<A> {
    static bool As(std::istream& is, Context& context, A& object) {
        char c = '\0';
        json::Read::ObjectPrefix(is, c);
        if (!json::Read::TryObjectSuffix(is)) {
            do {
                std::string fieldName = json::Read::FieldName(is, c);
                json::Read::Field(is, context, c, object, fieldName);
            } while (json::Read::FieldSeparator(is));
        }
        return true;
    }
};

template <>
struct json::Output::Customize<A, A::TestEnum> {
    static bool As(json::OutStreamType& os, Context& context, const A& object, const A::TestEnum& value) {
        try {
            EnhancedContext& enhanced = dynamic_cast<EnhancedContext&>(context);
            switch (value) {
                case A::TestEnum::first:
                    json::Put::String(os, "firstCustom" + std::to_string(enhanced.enhanced));
                    return true;
                case A::TestEnum::second:
                    json::Put::String(os, "secondCustom" + std::to_string(enhanced.enhanced));
                    return true;
            }
            return true;
        } catch (std::bad_cast&) {
            return false;
        }
    }
};

#endif