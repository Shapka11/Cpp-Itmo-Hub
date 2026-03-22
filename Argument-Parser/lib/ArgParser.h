#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace ArgumentParser {

class ArgParser {
public:
    ArgParser();
    ArgParser(const std::string& name);
    ~ArgParser();

    bool Parse(const int& argc, char** &argv);
    bool Parse(const std::vector<std::string>& argv);

    ArgParser& AddStringArgument(const char& short_name = '\0', const std::string& long_name = "", const std::string& description = "");
    ArgParser& AddStringArgument(const std::string& long_name = "", const std::string& description = "");

    ArgParser& AddIntArgument(const char& short_name = '\0', const std::string& long_name = "", const std::string& description = "");
    ArgParser& AddIntArgument(const std::string& long_name = "", const std::string& description = "");

    ArgParser& AddFlag(const char& short_name = '\0', const std::string& long_name = "", const std::string& description = "");
    ArgParser& AddFlag(const std::string& long_name = "", const std::string& description = "");

    ArgParser& AddHelp(const char& short_name = '\0', const std::string& long_name = "", const std::string& description = "");
    ArgParser& AddHelp(const std::string& long_name = "", const std::string& description = "");
    bool Help();
    std::string HelpDescription();

    ArgParser& MultiValue(const size_t MinArgsCount = 0);
    bool MinCountValue();

    ArgParser& StoreValue(std::string& str_value);
    ArgParser& StoreValue(int& int_value);
    ArgParser& StoreValue(bool& flag_value);

    ArgParser& StoreValues(std::vector<std::string>& str_value);
    ArgParser& StoreValues(std::vector<int>& int_value);
    ArgParser& StoreValues(std::vector<bool>& flag_value);

    std::string GetStringValue(const std::string& name, const int& index = -1);
    int GetIntValue(const std::string& name, const int& index = -1);
    bool GetFlag(const std::string& name, const int& index = -1);

    ArgParser& Default(const char* value);
    ArgParser& Default(const int& value);
    ArgParser& Default(const bool& value);

    ArgParser& Positional();

private:
    std::string name_parser_;

    struct NodeString {
        std::vector<std::string> vector_str;
        std::vector<std::string>* p_vector_str = nullptr;
        std::string value;
        std::string* p_str = &value;
        bool is_value = false;
    };

    struct NodeInt {
        std::vector<int> vector_int;
        std::vector<int>* p_vector_int = nullptr;
        int value;
        int* p_int = &value;
        bool is_value = false;
    };

    struct NodeFlag {
        std::vector<bool> vector_bool;
        std::vector<bool>* p_vector_bool = nullptr;
        bool flag;
        bool* p_flag = &flag;
        bool is_value = false;
    };

    struct NodeHelpInfo {
        std::string short_name = "     ";
        std::string long_name = "";
        std::string description = "";
        std::string type = "";
        std::string default_value = "";
        std::string multi_value = "";
        NodeString* node_str_pointer = nullptr;
        NodeInt* node_int_pointer = nullptr;
        NodeFlag* node_flag_pointer = nullptr;
    };

    std::string pos_key_ = "";
    std::vector<std::string> curr_key_;
    std::vector<std::string> res_help_val_;
    std::vector<NodeHelpInfo*> help_info_;
    int index_help_info_vector_ = -1;

    std::unordered_map<std::string, NodeString*> map_str_;
    std::unordered_map<std::string, NodeInt*> map_int_;
    std::unordered_map<std::string, NodeFlag*> map_flag_;
    std::unordered_map<std::string, size_t> map_multi_value_;

    ArgParser& AddString(const std::string& long_name, const std::string& description, NodeString* new_node);
    ArgParser& AddInt(const std::string& long_name, const std::string& description, NodeInt* new_node);
    ArgParser& AddFlagArg(const std::string& long_name, const std::string& description, NodeFlag* new_node);
    ArgParser& AddHelpArg(const std::string& long_name, const std::string& description);

    bool IsAllValue();

    void PushInFlagMap(const std::string& key, bool& is_iter);
    void PushInIntMap(const std::string& key, const std::string& value, bool& is_iter);
    void PushInStringMap(const std::string& key, const std::string value, bool& is_iter);
    void PushInPos(const std::string& value);
};

}