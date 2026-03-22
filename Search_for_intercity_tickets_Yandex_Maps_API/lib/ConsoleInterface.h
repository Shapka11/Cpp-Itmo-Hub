#pragma once

#include <iostream>
#include <string>

#include "CashInfo.h"
#include "Processing.h"


class Interface {
public:
    Interface(DataProcessing handler);
    void CommandsList();
    void Parse();

private:
    DataProcessing handler_;
    Cash cash_;
    bool is_key_install_ = false;

    void ParseApiKey();
    void ParseFirstCity();
    void ParseSecondCity();
    void ParseDate();

};
