#pragma once

#include <cpr/cpr.h>
#include <filesystem>
#include <expected>
#include <iostream>
#include <nlohmann/json.hpp>

#include "APIInfo.h"
#include "CashInfo.h"


class DataProcessing {
public:
    void OutputResult();

    void UpdateCityData();

    void SetApiKey(const std::string &api_key);
    void SetDate(const std::string &date);
    void SetFirstCity(const std::string &name_city);
    void SetSecondCity(const std::string &name_city);
    void SwapCity();

private:
    std::string GenerateNameFile();
    std::string GetDataInNormalFormat(const std::string &date);
    void DisplayData(const nlohmann::json &file_info);
    
    std::expected<void, std::string> ParseRequest(const cpr::Response &request, nlohmann::json *json_file);
    std::expected<void, std::string> ParseCityCodes(const cpr::Response &request, nlohmann::json *json_file);

    QueryData api_data_;
    Cash cash_info_;

    const std::string kNameCodes_ = "City_codes.json";

    std::string date_;
    std::string first_city_;
    std::string second_city_;

};
