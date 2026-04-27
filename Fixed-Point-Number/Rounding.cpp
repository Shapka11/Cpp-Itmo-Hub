#include "Rounding.h"


void Rounding::SetNum(const uint64_t &num) {
    num_ = num;
}

void Rounding::SetAAndB(const size_t &a, const size_t &b) {
    a_ = a;
    b_ = b;
}

void Rounding::SetSign(const bool &sign) {
    sign_ = sign;
}



void RoundBase2::SetNumberDigitsAfterB(const uint64_t &number_digits_after_b) {
    number_digits_after_b_ = number_digits_after_b;
}

uint64_t RoundBase2::GetResult(const int &type) {
    if (number_digits_after_b_ == 0) {
        return num_;
    }
    switch (type) {
    case 0:
        TowardZero();
        break;
    case 1:
        TowardNearestEven();
        break;
    case 2:
        TowardPosInfinity();
        break;
    case 3:
        TowardNegInfinity();
        break;
    default:
        break;
    }
    return num_;
}


void RoundBase2::TowardZero() {
    num_ >>= number_digits_after_b_;
}

void RoundBase2::TowardNearestEven() {
    uint64_t discarded_part = num_ << (64 - number_digits_after_b_ + 1);

    if (num_ << (64 - number_digits_after_b_) == 0) {
        num_ >>= number_digits_after_b_;
        return;
    }

    bool last_bit_b =  num_ << (64 - number_digits_after_b_ - 1) >> 63;

    if (number_digits_after_b_ == 1) {
        discarded_part = 0;
    }

    bool fisrt_bit_after_b = number_digits_after_b_ == 1 ? num_ & 1 : num_ << (64 - number_digits_after_b_) >> 63;
    if (fisrt_bit_after_b == 0) {
        num_ >>= number_digits_after_b_;
    } else {

        if (discarded_part == 0) {
            num_ >>= number_digits_after_b_;
            if (last_bit_b == 1) {
                ++num_;
            }
        } else {
            num_ >>= number_digits_after_b_;
            ++num_;
        }
    }
}

void RoundBase2::TowardPosInfinity() {
    uint64_t discarded_part = num_ << (64 - number_digits_after_b_);
    num_ >>= number_digits_after_b_;
    if (discarded_part != 0 && !sign_) {
        ++num_;
    }
}

void RoundBase2::TowardNegInfinity() {
    uint64_t discarded_part = num_ << (64 - number_digits_after_b_);
    num_ >>= number_digits_after_b_;
    if (discarded_part != 0 && sign_) {
        ++num_;
    }
}



void RoundBase10::SetFracPart(const uint32_t &frac_part) {
    frac_part_base_2_ = frac_part;
}

void RoundBase10::ConvertToBase10() {
    if (b_ <= kMaxSizeNumBase10_) {
        frac_part_base_10_ = frac_part_base_2_ * PowNum(b_, 5);
    } else {
        frac_part_base_10_ = (frac_part_base_2_ >> (b_ - kMaxSizeNumBase10_)) * PowNum(kMaxSizeNumBase10_, 5);
        discarded_part_ = frac_part_base_2_ << (32  - (b_ - kMaxSizeNumBase10_));
    }
}


uint64_t PowNum(const int &degree, const int &num) {
    uint64_t res = 1;
    for (int i = 0; i < degree; ++i) {
        res *= num;
    }
    return res;
}

size_t SizeNum(uint64_t num) {
    size_t res = 0;
    while (num > 0) {
        ++res;
        num /= 10;
    }
    return res;
}



uint32_t RoundBase10::GetResult(const int &type_round) {
    ConvertToBase10();
    size_num_ = SizeNum(frac_part_base_10_);
    cnt_round_digit_ = 3 - std::min(b_, kMaxSizeNumBase10_) + size_num_;
    disc_part_to_nearest_even_ = discarded_part_ + frac_part_base_10_ % PowNum(size_num_ - cnt_round_digit_ - 1, 10);
    discarded_part_ += frac_part_base_10_ % PowNum(size_num_ - cnt_round_digit_, 10);
    if (cnt_round_digit_ <= 0 && discarded_part_ == 0) {
        return 0;
    }

    switch (type_round) {
    case 0:
        TowardZero();
        break;
    case 1:
        TowardNearestEven();
        break;
    case 2:
        TowardPosInfinity();
        break;
    case 3:
        TowardNegInfinity();
        break;
    default:
        break;
    }
    return frac_part_base_10_;
}

size_t RoundBase10::GetCntInsignificantZero() {
    return cnt_round_digit_;
}


void RoundBase10::TowardZero() {
    frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
}

void RoundBase10::TowardNearestEven() {
    if (size_num_ <= cnt_round_digit_) {
        return;
    }

    int last_sign_num = frac_part_base_10_ / PowNum(size_num_ - cnt_round_digit_, 10) % 10;
    int first_disc_digit = frac_part_base_10_ / PowNum(size_num_- cnt_round_digit_ - 1, 10) % 10;
    size_t size_before_round;
    if (first_disc_digit == 5 && !disc_part_to_nearest_even_) {

        if (last_sign_num % 2 == 0) {
            frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
            size_before_round = SizeNum(frac_part_base_10_);
        } else {
            frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
            size_before_round = SizeNum(frac_part_base_10_);
            ++frac_part_base_10_;
        }

    } else if (first_disc_digit > 5 || (first_disc_digit == 5 && disc_part_to_nearest_even_)) {
        frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
        size_before_round = SizeNum(frac_part_base_10_);
        ++frac_part_base_10_;
    } else {
        frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
        size_before_round = SizeNum(frac_part_base_10_);
    }

    if (size_before_round < SizeNum(frac_part_base_10_)) {
        ++cnt_round_digit_;
    }
}

void RoundBase10::TowardPosInfinity() {
    frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
    size_t size_before_round = SizeNum(frac_part_base_10_);
    if (discarded_part_ && sign_ == 0) {
        ++frac_part_base_10_;
    }
    if (size_before_round < SizeNum(frac_part_base_10_)) {
        ++cnt_round_digit_;
    }
}

void RoundBase10::TowardNegInfinity() {
    frac_part_base_10_ /= PowNum(size_num_ - cnt_round_digit_, 10);
    size_t size_before_round = SizeNum(frac_part_base_10_);
    if (discarded_part_ && sign_) {
        ++frac_part_base_10_;
    }
    if (size_before_round < SizeNum(frac_part_base_10_)) {
        ++cnt_round_digit_;
    }
}

