#pragma once

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <sstream>
#include <unordered_map>


std::string GetCurrentDate();

class Cash {
public:
    void CleanCash();
    void CleanAllCash();
    bool IsFileExist(const std::string &filename);

    nlohmann::json *GetFile(const std::string &filename);
    const std::string &GetPathName();

    void SetFile(const nlohmann::json &json_file, const std::string &filename);

private:
    const std::string kPathName_ = "../../DataRoutes/";
    std::unordered_map<std::string, nlohmann::json*> map_data_;

};
