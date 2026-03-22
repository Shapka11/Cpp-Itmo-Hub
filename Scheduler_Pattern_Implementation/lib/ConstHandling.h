#pragma once


namespace ConstHandling {

    template<typename T>
    struct remove_const {
        using type = T;
    };

    template<typename T>
    struct remove_const<const T> {
        using type = T;
    };

    template<typename T>
    using remove_const_t = typename remove_const<T>::type;

}
