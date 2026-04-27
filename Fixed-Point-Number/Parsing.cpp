#include "Parsing.h"


Parser::Parser(Data *data) : data_(data) {
}

void Parser::ParseRounding(const std::string &argc) {
    if (argc == "0" || argc == "1" || argc == "2" || argc == "3") {    
        try {
            data_->type_rounding = std::stoi(argc);
        } catch (std::out_of_range) {
            std::cerr << "Error, too many type rounding" << std::endl;
            is_correct_args_ = false;
        }
    } else {
        std::cerr << "Error, incorrect rounding type" << std::endl;
        is_correct_args_ = false;
    }
}

void Parser::ParseOperation(const std::string &argc) {
    if (argc == "/" || argc == "-" || argc == "+" || argc == "*") {
        data_->operation = argc[0];
    } else {
        std::cerr << "Error, incorrect operation" << std::endl;
        is_correct_args_ = false;
    }
}


void Parser::CheckCorrectNumber(const std::string &argc) {
    if (argc.substr(0, 2) != "0x") {
        std::cerr << "Error, incorrect notation" << std::endl;
        is_correct_args_ = false;
    }

    for (int i = 2; i < argc.size(); ++i) {
        if (!((argc[i] >= 'a' && argc[i] <= 'f') || (argc[i] >= 'A' && argc[i] <= 'F') || (argc[i] >= '0' && argc[i] <= '9'))) {
            std::cerr << "Error, incorrect number" << std::endl;
            is_correct_args_ = false;
        }
    }
}

void Parser::ParseNum(const std::string &argc, uint32_t &num) {
    try {
        num = std::stoul(argc, nullptr, 16);
        size_t size_num = CountBitsNum(num);
        if (size_num > data_->a + data_->b) {
            int shift = 32 - (data_->a + data_->b);
            num <<= shift;
            num >>= shift;
        }
    } catch (std::out_of_range) {
        std::cerr << "Error, too many number" << std::endl;
        is_correct_args_ = false;
    }
}

int Parser::CountBitsNum(uint32_t num) {
    int size_num = 0;
    while (num != 0) {
        num = num >> 1;
        ++size_num;
    }
    return size_num;
}

bool Parser::IsNum(const std::string &num) {
    for (int i = 0; i < num.size(); ++i) {
        if (!(num[i] >= '0' && num[i] <= '9')) {
            return false;
        }
    }
    return true;
}


void Parser::ParseFormat(const char * argc) {
    char *int_part = new char[100] {"33"};
    char *frac_part = new char[100] {"33"};
    try {
        sscanf(argc, "%[^.].%s", int_part, frac_part);
        data_->a = std::stoi(int_part);
        data_->b = std::stoi(frac_part);

        if (data_->a + data_->b > 32) {
            std::cerr << "Error, A+B > 32" << std::endl;
            is_correct_args_ = false;
        }
    } catch (std::invalid_argument) {
        std::cerr << "Error, incorrect format" << std::endl;
        is_correct_args_ = false;
    } catch (std::out_of_range) {
        std::cerr << "Error, too big number" << std::endl;
        is_correct_args_ = false;
    }
    delete int_part;
    delete frac_part;

}

bool Parser::ParseFirstVariant(char **argc) {
    ParseFormat(argc[1]);
    ParseRounding(argc[2]);

    CheckCorrectNumber(argc[3]);
    ParseNum(argc[3], data_->num1);

    return is_correct_args_;
}

bool Parser::ParseSecondVariant(char **argc) {
    ParseFormat(argc[1]);
    ParseRounding(argc[2]);
    ParseOperation(argc[3]);

    CheckCorrectNumber(argc[4]);
    ParseNum(argc[4], data_->num1);

    CheckCorrectNumber(argc[5]);
    ParseNum(argc[5], data_->num2);

    return is_correct_args_;
}

