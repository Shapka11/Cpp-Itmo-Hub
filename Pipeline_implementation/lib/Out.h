#pragma once

#include <iostream>

#include "Pipeline.h"


class Out {
public:
    Out(std::ostream& os) : os_(os) {}
    
    void operator()(auto&& pipeline) {
        for (const auto& element : pipeline) {
            os_ << element << ' ';
        }
    }
    
private:
    std::ostream& os_;

};
