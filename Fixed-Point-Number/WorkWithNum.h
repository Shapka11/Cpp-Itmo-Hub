#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>

#include "Parsing.h"
#include "Rounding.h"


void PrintNum(const uint32_t &num, const int &a, const int &b, const int &type_round);
void AddZero(RoundBase10 &out_num, const uint32_t &frac_part, std::string &frac_ans);
void CheckZeroToPrint(std::string &str);


class Operation {
public:
    uint64_t GetResult(Data *data);
    bool GetSignResult();
    bool IsDivByZero();

private:
    RoundBase2 round_;

    void Sum();
    void Minus();
    void Multiplication();
    void Division();

    bool DetermineSign(Data *data, const uint32_t &num);

    Data *data_;
    uint64_t result_;
    bool sign_res_;
    bool is_div_by_0_ = false;
};
