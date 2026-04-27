#include "WorkWithNum.h"


bool Operation::GetSignResult() {
    return sign_res_;
}

uint64_t Operation::GetResult(Data *data) {
    data_ = data;
    if (data->operation == '+') {
        Sum();
    } else if (data->operation == '-') {
        Minus();
    } else if (data->operation == '*') {
        Multiplication();
    } else {
        Division();
    }
    return result_;
}

bool Operation::DetermineSign(Data *data, const uint32_t &num) {
    return (num >> (data->a + data->b - 1));
}

bool Operation::IsDivByZero() {
    return is_div_by_0_;
}


void Operation::Sum() {
    result_ = data_->num1 + data_->num2;
    result_ = result_ << (64 - data_->a - data_->b) >> (64 - data_->a - data_->b);
}

void Operation::Minus() {
    result_ = data_->num1 - data_->num2;
    result_ = result_ << (64 - data_->a - data_->b) >> (64 - data_->a - data_->b);
}

void Operation::Multiplication() {
    bool sign_num1 = DetermineSign(data_, data_->num1);
    bool sign_num2 = DetermineSign(data_, data_->num2);

    sign_res_ = sign_num1 ^ sign_num2;
    if (sign_num1) {
        data_->num1 = -data_->num1;
        data_->num1 <<= (32 - data_->a - data_->b);
        data_->num1 >>= (32 - data_->a - data_->b);
    }
    if (sign_num2) {
        data_->num2 = -data_->num2;
        data_->num2 <<= (32 - data_->a - data_->b);
        data_->num2 >>= (32 - data_->a - data_->b);
    }
    result_ = (uint64_t)data_->num1 * data_->num2;

    round_.SetAAndB(data_->a, data_->b);
    round_.SetNum(result_);
    round_.SetNumberDigitsAfterB(data_->b);
    round_.SetSign(sign_res_);
    result_ = round_.GetResult(data_->type_rounding);

    if (sign_res_) {
        result_ = -result_;
    }

    result_ = result_ << (64 - data_->a - data_->b) >> (64 - data_->a - data_->b);
}

void Operation::Division() {
    bool sign_num1 = DetermineSign(data_, data_->num1);
    bool sign_num2 = DetermineSign(data_, data_->num2);

    sign_res_ = sign_num1 ^ sign_num2;
    if (sign_num1) {
        data_->num1 = -data_->num1;
        data_->num1 <<= (32 - data_->a - data_->b);
        data_->num1 >>= (32 - data_->a - data_->b);
    }
    if (sign_num2) {
        data_->num2 = -data_->num2;
        data_->num2 <<= (32 - data_->a - data_->b);
        data_->num2 >>= (32 - data_->a - data_->b);
    }

    if (data_->num2 == 0) {
        is_div_by_0_ = true;
        return;
    }

    uint64_t dividend = data_->num1;
    dividend <<= (64 - data_->a - data_->b);
    result_ = (uint64_t)dividend / data_->num2;

    round_.SetAAndB(data_->a, data_->b);
    round_.SetNum(result_);
    round_.SetNumberDigitsAfterB(64 - data_->a - 2 * data_->b);
    round_.SetSign(sign_res_);
    result_ = round_.GetResult(data_->type_rounding);

    if (sign_res_) {
        result_ = -result_;
    }

    result_ = result_ << (64 - data_->a - data_->b) >> (64 - data_->a - data_->b);
}



void PrintNum(const uint32_t &num, const int &a, const int &b, const int &type_round) {
    std::string ans = "";
    std::string frac_ans = "";
    bool sign = num >> (a + b - 1);
    RoundBase10 out_num;
    out_num.SetAAndB(a, b);
    out_num.SetSign(sign);

    if (sign == 1) {
        uint32_t int_part = (-num) << (32 - a - b) >> (32 - a);
        uint32_t frac_part = b == 0 ? 0 : (-num) << (32 - b) >> (32 - b);

        out_num.SetFracPart(frac_part);
        frac_part = out_num.GetResult(type_round) % PowNum(out_num.GetCntInsignificantZero(), 10);

        if (SizeNum(frac_part) == 4) {
            frac_part = 0;
            ++int_part;
        }

        AddZero(out_num, frac_part, frac_ans);
        ans += '-' + std::to_string(int_part) + "." + frac_ans;
        CheckZeroToPrint(ans);
    } else {
        uint32_t int_part = num >> b;
        uint32_t frac_part = b == 0 ? 0 : num << (32 - b) >> (32 - b);

        out_num.SetFracPart(frac_part);
        frac_part = out_num.GetResult(type_round) % PowNum(out_num.GetCntInsignificantZero(), 10);
        if (SizeNum(frac_part) == 4) {
            frac_part = 0;
            ++int_part;
        }

        AddZero(out_num, frac_part, frac_ans);
        ans +=  std::to_string(int_part) + "." + frac_ans;

    }
    std::cout << ans << std::endl;

}

void AddZero(RoundBase10 &out_num, const uint32_t &frac_part, std::string &frac_ans) {
    switch (out_num.GetCntInsignificantZero()) {
    case 1:
        frac_ans += "00" + std::to_string(frac_part);
        break;
    case 2:
        frac_ans += "0" + std::to_string(frac_part);
        break;
    default:
        frac_ans += std::to_string(frac_part);
        break;
    }

    switch (frac_ans.size()) {
    case 0:
        frac_ans += "000";
        break;
    case 1:
        frac_ans += "00";
        break;
    case 2:
        frac_ans += "0";
        break;
    default:
        break;
    }
}

void CheckZeroToPrint(std::string &str) {
    bool is_null = true;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] >= '1' && str[i] <= '9') {
            is_null = false;
        }
    }
    if (is_null) {
        str = str.substr(1, str.size() - 1);
    }
}
