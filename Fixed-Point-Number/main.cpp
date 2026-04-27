#include <iostream>

#include "Parsing.h"
#include "WorkWithNum.h"


int main(int argv, char **argc) {
    Data *data = new Data();
    Parser parser(data);

    if (argv == 4) {
        bool is_correct_pars = parser.ParseFirstVariant(argc);
        if (!is_correct_pars) {
            return 1;
        }
        PrintNum(data->num1, data->a, data->b, data->type_rounding);
    } else if (argv == 6) {
        bool is_correct_pars = parser.ParseSecondVariant(argc);
        if (!is_correct_pars) {
            return 1;
        }
        Operation op;
        uint32_t result = op.GetResult(data);
        if (op.IsDivByZero()) {
            std::cout << "div_by_zero" << std::endl;
            return 0;
        }
        PrintNum(result, data->a, data->b, data->type_rounding);
    } else {
        std::cerr << "Error, not enough arguments" << std::endl;
        return 1;
    }

    delete data;
    return 0;
}

