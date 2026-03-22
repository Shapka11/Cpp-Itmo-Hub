#pragma once

#include <cpr/cpr.h>
#include <iostream>
#include <string>


class QueryData {
public:
    cpr::Response RequestRoute(
        const std::string &date,
        const std::string &first_code,
        const std::string &second_code
    );

    cpr::Response DataCityCodes();
    void SetApiKey(const std::string &api_key);

private:
    std::string api_key_;

};
