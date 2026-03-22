#include "CashInfo.h"


std::string GetCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d");
    return oss.str();
}

const std::string &Cash::GetPathName() {
    return kPathName_;
}

void Cash::CleanCash() {
    std::filesystem::path directory_path = kPathName_;
    std::string curr_date = GetCurrentDate();
    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        std::string name_file = entry.path().filename();
        const int date_size = 10;
        if (name_file.substr(0, date_size) < curr_date) {
            std::filesystem::remove(entry);
        }
    }
}

void Cash::CleanAllCash() {
    std::filesystem::path directory_path = kPathName_;
    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        std::filesystem::remove(entry);
    }
}

bool Cash::IsFileExist(const std::string &filename) {
    return std::filesystem::exists(kPathName_ + filename);
}

nlohmann::json *Cash::GetFile(const std::string &filename) {
    if (map_data_.find(filename) != map_data_.end()) {
        return map_data_[filename];
    }

    if (!IsFileExist(filename)) {
        std::ofstream lib_file(kPathName_ + filename);
        lib_file.close();
    }

    std::ifstream lib_file(kPathName_ + filename);
    if (!lib_file.is_open()) {
        std::cerr << "Error opening data file" << std::endl;
        return nullptr;
    }

    nlohmann::json *json_file = new nlohmann::json();
    if (lib_file.peek() != std::ifstream::traits_type::eof()) {
        *json_file = nlohmann::json::parse(lib_file);
    }
    lib_file.close();

    map_data_[filename] = json_file;
    return map_data_[filename];
}

void Cash::SetFile(const nlohmann::json &json_file, const std::string &filename) {
    std::ofstream lib_file(kPathName_ + filename);
    lib_file << json_file.dump(4);
    lib_file.close();
}
