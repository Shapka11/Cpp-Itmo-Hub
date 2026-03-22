#include "APIInfo.h"


void QueryData::SetApiKey(const std::string &api_key) {
    api_key_ = api_key;
}

cpr::Response QueryData::RequestRoute(
    const std::string &date,
    const std::string &first_code,
    const std::string &second_code
) {
    cpr::Response request = cpr::Get(cpr::Url{"https://api.rasp.yandex.net/v3.0/search/"},
        cpr::Parameters{{"apikey", api_key_},
                        {"format", "json"},
                        {"from", first_code},
                        {"to", second_code},
                        {"lang", "ru_RU"},
                        {"date", date},
                        {"transfers", "true"}});
    return request;
}

cpr::Response QueryData::DataCityCodes() {
    cpr::Response request = cpr::Get(cpr::Url{"https://api.rasp.yandex.net/v3.0/stations_list/"},
        cpr::Parameters{{"apikey", api_key_},
                        {"lang", "ru_RU"},
                        {"format", "json"}});
    
    return request;
}
