#include "ConsoleInterface.h"


Interface::Interface(DataProcessing handler) : handler_(handler) {
}

void Interface::ParseDate() {
    std::cout << "Enter date in format XXXX-XX-XX: ";
    std::string date;
    std::cin >> date;
    handler_.SetDate(date);
}

void Interface::ParseFirstCity() {
    std::cout << "Enter first city: ";
    std::string name_city;
    std::cin >> name_city;
    handler_.SetFirstCity(name_city);
}

void Interface::ParseSecondCity() {
    std::cout << "Enter second city: ";
    std::string name_city;
    std::cin >> name_city;
    handler_.SetSecondCity(name_city);
}

void Interface::ParseApiKey() {
    std::cout << "Enter API key: ";
    std::string api_key;
    std::cin >> api_key;
    handler_.SetApiKey(api_key);
}


void Interface::CommandsList()
{
    std::cout << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "List commands:" << std::endl;
    std::cout << "0 Input API key(necessarily)." << std::endl;
    std::cout << "1 Make a request." << std::endl;
    std::cout << "2 Update city data." << std::endl;
    std::cout << "3 Clear all cash" << std::endl;
    std::cout << "4 Exit." << std::endl;
    std::cout << "=================================" << std::endl;
}

void Interface::Parse() {
    std::cout << "Enter command number: ";
    std::string number;
    std::cin >> number;

    int num;
    if(number[0] >= '0' && number[0] <= '9') {
        num = number[0] - '0';
    } else {
        std::cerr << "Error, incorrect command" << std::endl;
        return;
    }

    switch (num) {
    case 0:
        ParseApiKey();
        is_key_install_ = true;
        break;
    case 1:
        if (!is_key_install_) {
            std::cerr << "Error, api key not set" << std::endl;
            return;
        }
        ParseDate();
        ParseFirstCity();
        ParseSecondCity();
        handler_.OutputResult();
        handler_.SwapCity();
        handler_.OutputResult();
        break;
    case 2:
        if (!is_key_install_) {
            std::cerr << "Error, api key not set" << std::endl;
            return;
        }
        handler_.UpdateCityData();
        break;
    case 3:
        cash_.CleanAllCash();
        std::cout << "cache cleared successfully" << std::endl;
        break;
    case 4:
        cash_.CleanCash();
        exit(EXIT_SUCCESS);
    default:
        std::cerr << "Error, such a command does not exist" << std::endl;
        return;
    }
}
