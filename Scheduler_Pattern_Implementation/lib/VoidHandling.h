#pragma once


namespace VoidHandling {

    template<typename T>
    struct is_void {
        static const bool value = false;
    };

    template<>
    struct is_void<void> {
        static const bool value = true;
    };

    template<typename T>
    constexpr bool is_void_v = is_void<T>::value;

}
