#pragma once

#include <memory>

#include "ReferenceHandling.h"
#include "ConstHandling.h"


class UniversalHolder {
public:
    UniversalHolder() = default;

    template<typename T>
    UniversalHolder(const T& value) : ptr_(std::make_unique<DataType<T>>(value)) {}

    UniversalHolder(const UniversalHolder& other) : ptr_(other.ptr_->GetCopy()) {}
    UniversalHolder(UniversalHolder&& other) = default;

    ~UniversalHolder() = default;

    UniversalHolder& operator=(const UniversalHolder& other) {
        if (this != &other) {
            ptr_ = other.ptr_ ? other.ptr_->GetCopy() : nullptr;
        }
        return *this;
    }

    UniversalHolder& operator=(UniversalHolder&& other) {
        if (this != &other) {
            ptr_ = std::move(other.ptr_);
        }
        return *this;
    }

    template<typename T>
    T UniversalHolderCast(UniversalHolder& obj) {
        auto* ptr_cast = dynamic_cast<DataType<T>*>(obj.ptr_.get());
        if (!ptr_cast) {
            throw std::bad_cast();
        }
        return ptr_cast->value;
    }

    template<typename T>
    T Cast() {
        if (!ptr_) {
            throw std::bad_cast();
        }

        using ValueType = ConstHandling::remove_const_t<ReferenceHandling::remove_reference_t<T>>;
        auto* result = dynamic_cast<DataType<ValueType>*>(ptr_.get());

        if (!result) {
            throw std::bad_cast();
        }

        if constexpr (ReferenceHandling::is_reference_v<ConstHandling::remove_const_t<T>>) {
            return static_cast<T>(result->GetValue());
        } else {
            return result->GetValue();
        }
    }

private:
    class WrapperDataType {
    public:
        virtual ~WrapperDataType() = default;
        virtual std::unique_ptr<WrapperDataType> GetCopy() = 0;

    };

    template<typename T>
    class DataType : public WrapperDataType {
    public:
        DataType(const T& value) : value_(value) {}
        ~DataType() = default;

        std::unique_ptr<WrapperDataType> GetCopy() override {
            return std::make_unique<DataType<T>>(value_);
        }

        T& GetValue() {
            return value_;
        }

    private:
        T value_;

    };

    std::unique_ptr<WrapperDataType> ptr_ = nullptr;
    
};
