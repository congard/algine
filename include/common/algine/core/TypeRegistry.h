#ifndef ALGINE_TYPEREGISTRY_H
#define ALGINE_TYPEREGISTRY_H

#include <algine/core/RawPtr.h>
#include <algine/platform.h>

#include <tulz/util/static_initializer.h>

#include <string>
#include <map>

namespace algine {
class AL_EXPORT TypeRegistry {
    STATIC_INITIALIZER_DECL

public:
    using Deleter = void(*)(void*);

    template<typename T = void, typename ...Args>
    class Creator {
    public:
        virtual T* create(Args&&...args) = 0;
        virtual Deleter getDeleter() = 0;
        virtual ~Creator() = default;
    };

public:
    static void registerType(const std::string &name, void *creator);

    template<typename T, typename ...Args>
    static void registerType(const std::string &name) {
        class Impl: public Creator<T, Args...> {
        public:
            T* create(Args&&...args) override {
                return new T(std::forward<Args>(args)...);
            }

            Deleter getDeleter() override {
                return [](void *ptr) { delete (T*) ptr; };
            }
        };

        registerType(name, new Impl);
    }

    static void unregister(const std::string &name);

    static bool contains(const std::string &name);

    template<typename T, typename ...Args>
    static RawPtr<T> create(const std::string &name, Args&&...args) {
        using Type = Creator<T, Args...>;
        auto implType = (Type*) m_registry[name];
        return implType->create(std::forward<Args>(args)...);
    }

    static Deleter getDeleter(const std::string &name);

    static void clear();

private:
    static std::map<std::string, void*> m_registry;
};
}

#define alTypeRegistrySignature(type, ...) #type "(" #__VA_ARGS__ ")"

/**
 * The same as `alTypeRegistrySignature`,
 * but `typeStr` must be of type `std::string`
 */
#define alTypeRegistrySignatureTStr(typeStr, ...) (typeStr + ("(" #__VA_ARGS__ ")"))

#define alRegisterType(...) algine::TypeRegistry::registerType<__VA_ARGS__>(alTypeRegistrySignature(__VA_ARGS__))
#define alUnregisterType(...) algine::TypeRegistry::unregister(alTypeRegistrySignature(__VA_ARGS__))

#endif //ALGINE_TYPEREGISTRY_H
