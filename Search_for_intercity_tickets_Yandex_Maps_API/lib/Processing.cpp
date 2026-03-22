#include "Processing.h"


void DataProcessing::SetApiKey(const std::string &api_key) {
    api_data_.SetApiKey(api_key);
}

void DataProcessing::SetDate(const std::string &date) {
    date_ = date;
}

void DataProcessing::SetFirstCity(const std::string &name_city) {
    first_city_ = name_city;
}

void DataProcessing::SetSecondCity(const std::string &name_city) {
    second_city_ = name_city;
}

void DataProcessing::SwapCity() {
    std::swap(first_city_, second_city_);
}


void DataProcessing::UpdateCityData() {
    cpr::Response request = api_data_.DataCityCodes();
    if (request.status_code != 200) {
        std::cerr << "Request error! Please try again." << std::endl;
        return;
    }

    nlohmann::json *json_file = cash_info_.GetFile(kNameCodes_);
    auto result_parsing = ParseCityCodes(request, json_file);
    if (!result_parsing.has_value()) {
        std::cerr << result_parsing.error() << std::endl;
        return;
    }
    cash_info_.SetFile(*json_file, kNameCodes_);
    std::cout << "data updated successfully" << std::endl;
}

std::expected<void, std::string> DataProcessing::ParseCityCodes(const cpr::Response &request, nlohmann::json *json_file) {
    try {
        nlohmann::json json_file_city_info = nlohmann::json::parse(request.text);

        for (const auto& country : json_file_city_info["couАntries"]) {
            for (const auto& region : country["regions"]) {
                for (const auto& settlement : region["settlements"]) {
                    if (settlement.contains("codes") && settlement["codes"].contains("yandex_code")) {
                        (*json_file)[settlement["title"]] = settlement["codes"]["yandex_code"];
                    }
                }
            }
        }
        return {};
    } catch (nlohmann::json::parse_error& e) {
        return std::unexpected(std::string(e.what()));
    }
}

std::string DataProcessing::GetDataInNormalFormat(const std::string &date) {
    const int date_size = 10;
    const int time_size = 8;
    const int date_pos = 0;
    const int time_pos = 11;
    return date.substr(date_pos, date_size) + ' ' + date.substr(time_pos, time_size);
}

std::expected<void, std::string> DataProcessing::ParseRequest(const cpr::Response &request, nlohmann::json *json_file) {
    try {
        nlohmann::json file_name = nlohmann::json::parse(request.text);
        
        for (const auto &segment : file_name["segments"]) {
            if (!segment["has_transfers"]) {
                (*json_file)["routes"].push_back( {
                    {"has_transfers", false},
                    {"from", segment["from"]["title"]},
                    {"departure_time", GetDataInNormalFormat(std::string(segment["departure"]))},
                    {"to", segment["to"]["title"]},
                    {"arrival_time", GetDataInNormalFormat(std::string(segment["arrival"]))},
                    {"transport_type", segment["thread"]["transport_type"]}
                }
                );
            } else if (segment["transfers"].size() == 1) {
                (*json_file)["routes"].push_back( {
                    {"has_transfers", true},
                    {"from", segment["details"][0]["from"]["title"]},
                    {"departure_time", GetDataInNormalFormat(std::string(segment["details"][0]["departure"]))},
                    {"transfer_city", segment["details"][0]["to"]["title"]},
                    {"arrival_time_to_transfer_city", GetDataInNormalFormat(std::string(segment["details"][0]["arrival"]))},
                    {"transport_type_to_transfer_city", segment["transport_types"][0]},
                    {"transport_type_from_transfer_city", segment["transport_types"][1]},
                    {"departure_time_from_transfer_city", GetDataInNormalFormat(std::string(segment["details"][2]["departure"]))},
                    {"to", segment["details"][2]["to"]["title"]},
                    {"arrival_time", GetDataInNormalFormat(std::string(segment["details"][2]["arrival"]))}

                }
                );
            }
        }
        return {};
    } catch (nlohmann::json::parse_error& e) {
        return std::unexpected(std::string(e.what()));
    }
}

void DataProcessing::OutputResult() {
    nlohmann::json *file_info = cash_info_.GetFile(GenerateNameFile());
    if (file_info->empty()) {
        if (!cash_info_.IsFileExist(kNameCodes_)) {
            UpdateCityData();
        }
        nlohmann::json *codes = cash_info_.GetFile(kNameCodes_);
        cpr::Response request = api_data_.RequestRoute(date_, (*codes)[first_city_], (*codes)[second_city_]);
        if (request.status_code != 200) {
            std::cerr << "Request error! Please try again." << std::endl;
            return;
        }

        auto result_parsing = ParseRequest(request, file_info);
        if (!result_parsing.has_value()) {
            std::cerr << result_parsing.error() << std::endl;
            return;
        }

        cash_info_.SetFile(*file_info, GenerateNameFile());
    }
    DisplayData(*file_info);
}


void DataProcessing::DisplayData(const nlohmann::json &file_info) {
    std::cout << std::endl << "|||Routes " << first_city_ << 
        '-' << second_city_ << "|||" << std::endl << std::endl;

    if (file_info.is_null()) {
        std::cout << "Route data not found, please check your input is correct." << std::endl;
        std::filesystem::remove(cash_info_.GetPathName() + GenerateNameFile());
        return;
    }

    for (const auto &info : file_info["routes"]) {
        if (info["has_transfers"]) {
            std::cout << "Route: " << info["from"] << '-' << info["transfer_city"] << std::endl;
            std::cout << "Departure time: " << info["departure_time"] << std::endl;
            std::cout << "Arrival time: " << info["arrival_time_to_transfer_city"] << std::endl;
            std::cout << "Transport type: " << info["transport_type_to_transfer_city"] << std::endl << std::endl;

            std::cout << "Route: " << info["transfer_city"] << '-' << info["to"] << std::endl;
            std::cout << "Departure time: " << info["departure_time_from_transfer_city"] << std::endl;
            std::cout << "Arrival time: " << info["arrival_time"] << std::endl;
            std::cout << "Transport type: " << info["transport_type_from_transfer_city"] << std::endl;
            std::cout << "----------------------" << std::endl;
        } else {
            std::cout << "Route: " << info["from"] << '-' << info["to"] << std::endl;
            std::cout << "Departure time: " << info["departure_time"] << std::endl;
            std::cout << "Arrival time: " << info["arrival_time"] << std::endl;
            std::cout << "Transport type: " << info["transport_type"] << std::endl;
            std::cout << "----------------------" << std::endl;
        }
    }
}

std::string DataProcessing::GenerateNameFile() {
    return date_ + ", " + first_city_ + " to " + second_city_ + ".json";
}
