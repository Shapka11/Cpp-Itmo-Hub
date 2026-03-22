#pragma once

#include <string>
#include <vector>
#include <typeinfo>

namespace ArgumentParser {

class DataType {
public:
    virtual ~DataType() = default;

    bool is_initialized = false;
    bool is_multi_value = false;
    size_t multi_count_value = 0;
    int count_value = 0;


    virtual const char *GetType() = 0;
};

template <typename T>
class TypedArgument : public DataType {
public:
    virtual ~TypedArgument() override = default;

    std::vector<T>* p_vector_data = nullptr;
    T value;
    T* p_value = &value;

    virtual const char *GetType() override {
        return typeid(T).name();
    }
};

template <>
class TypedArgument<std::string> : public DataType {
public:
    virtual ~TypedArgument() override = default;

    std::vector<std::string>* p_vector_data = nullptr;
    std::string value;
    std::string* p_value = &value;
    std::string description = "";

    virtual const char *GetType() override {
        return typeid(std::string).name();
    }
};

template <>
class TypedArgument<int> : public DataType {
public:
    virtual ~TypedArgument() override = default;

    std::vector<int>* p_vector_data = nullptr;
    int value;
    int* p_value = &value;
    std::string description = "";

    virtual const char *GetType() override {
        return typeid(int).name();
    }
};

template <>
class TypedArgument<bool> : public DataType {
public:
    virtual ~TypedArgument() override = default;

    std::vector<bool>* p_vector_data = nullptr;
    bool value;
    bool* p_value = &value;
    std::string description = "";

    virtual const char *GetType() override {
        return typeid(bool).name();
    }
};

struct HelpArgument {
    std::string short_name = "";
    std::string long_name = "";
    std::string description = "";
    std::string type = "";
    std::string default_value = "";
    std::string min_count_args = "";
};

}
