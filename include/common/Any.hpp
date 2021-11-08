#pragma once

#include <memory>
#include <typeindex>

namespace tibus {
namespace common {

class Any {
public:
    Any() : type(std::type_index(typeid(void)))
    {
    }

    Any(const Any& other) : object(other.Clone()), type(other.type)
    {
    }

    Any(Any&& other) noexcept : object(std::move(other.object)), type(other.type)
    {
    }

    template <typename T, typename D = typename std::decay<T>::type,
        class = typename std::enable_if<!std::is_same<D, Any>::value, T>::type>
    Any(T&& value) // When creating a unique_ptr, for generic types, use std::decay
    // to remove reference(&) and cv(const and volatile) to get the original type.
        : object(std::make_unique<SpecificObject<D>>(std::forward<T>(value)))
        , type(std::type_index(typeid(D)))
    {
    }

    Any& operator=(const Any& other)
    {
        if (object == other.object) {
            return *this;
        }
        object = other.Clone();
        type = other.type;
        return *this;
    }

    Any& operator=(Any&& other) noexcept
    {
        if (object == other.object) {
            return *this;
        }
        object = std::move(other.object);
        type = other.type;
        return *this;
    }

    bool IsNull() const
    {
        return !object; // unique_ptr override the `operator bool()`
    }

    template <typename T>
    bool IsType() const
    {
        return (type == std::type_index(typeid(T)));
    }

    template <typename T>
    T& AnyCast() const
    {
        if (!IsType<T>()) {
            throw std::bad_cast();
        }
        return *dynamic_cast<SpecificObject<T>*>(object.get());
    }

private:
    class AnyObject {
    public:
        virtual std::unique_ptr<AnyObject> Clone() const = 0;
        virtual ~AnyObject() = default;

    protected:
        AnyObject() = default;
    };

    template <typename T>
    class SpecificObject : public AnyObject {
    public:
        template <typename T>
        SpecificObject(T&& value) : value(std::forward<T>(value))
        {
        }

        std::unique_ptr<AnyObject> Clone() const
        {
            return std::unique_ptr<SpecificObject<T>>(value);
        }

        operator T& ()
        {
            return value;
        }

    private:
        T value;
    };

    std::unique_ptr<AnyObject> Clone() const
    {
        if (object) {
            return object->Clone();
        }
        return nullptr;
    }

    std::unique_ptr<AnyObject> object;
    std::type_index type;
};

}
}
