#include <iostream>

#include "lib/ConsoleInterface.h"
#include "lib/Processing.h"


int main() {
    DataProcessing data_handler;
    Interface interface(data_handler);
    bool is_app_work = true;

    while (is_app_work) {
        interface.CommandsList();
        interface.Parse();     
    }

    return 0;
}
