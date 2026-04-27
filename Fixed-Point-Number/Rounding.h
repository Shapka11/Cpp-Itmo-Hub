#pragma once

#include <cstdint>
#include <iostream>


class Rounding {
public:
    virtual ~Rounding() {};

    void SetNum(const uint64_t &num);
    void SetAAndB(const size_t &a, const size_t &b);
    void SetSign(const bool &sign);

protected:
    virtual void TowardZero() = 0;
    virtual void TowardNearestEven() = 0;
    virtual void TowardPosInfinity() = 0;
    virtual void TowardNegInfinity() = 0;

    uint64_t num_;
    size_t a_;
    size_t b_;
    bool sign_;

};


class RoundBase2 : public Rounding {
public:
    uint64_t GetResult(const int &type);
    void SetNumberDigitsAfterB(const uint64_t &number_digits_after_b);

private:
    void TowardZero() override;
    void TowardNearestEven() override;
    void TowardPosInfinity() override;
    void TowardNegInfinity() override;

    uint64_t number_digits_after_b_;

};


class RoundBase10 : public Rounding {
public:
    void SetFracPart(const uint32_t &frac_part);
    
    uint32_t GetResult(const int &type_round);
    size_t GetCntInsignificantZero();

private:
    virtual void TowardZero() override;
    virtual void TowardNearestEven() override;
    virtual void TowardPosInfinity() override;
    virtual void TowardNegInfinity() override;

    void ConvertToBase10();

    uint32_t frac_part_base_2_;
    uint64_t frac_part_base_10_;
    size_t cnt_round_digit_;
    size_t size_num_;
    bool discarded_part_ = 0;
    bool disc_part_to_nearest_even_ = 0;

    const size_t kMaxSizeNumBase10_ = 19;

};

uint64_t PowNum(const int &degree, const int &num);
size_t SizeNum(uint64_t num);
