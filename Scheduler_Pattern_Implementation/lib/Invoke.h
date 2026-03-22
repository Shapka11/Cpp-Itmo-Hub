#pragma once


namespace InvokeFunction {

    template<typename Func, typename... Args>
    auto Invoke(Func&& func, Args&&... args)
            -> decltype(std::forward<Func>(func)(std::forward<Args>(args)...)) {
        return std::forward<Func>(func)(std::forward<Args>(args)...);
    }

    template<typename Method, typename Obj, typename... Args>
    auto Invoke(Method&& method, Obj&& obj, Args&&... args)
            -> decltype((std::forward<Obj>(obj).*std::forward<Method>(method))(std::forward<Args>(args)...)) {
        return (std::forward<Obj>(obj).*std::forward<Method>(method))(std::forward<Args>(args)...);
    }

    template<typename Method, typename ObjPtr, typename... Args>
    auto Invoke(Method&& method, ObjPtr* ptr, Args&&... args)
            -> decltype(((*ptr).*std::forward<Method>(method))(std::forward<Args>(args)...)) {
        return ((*ptr).*std::forward<Method>(method))(std::forward<Args>(args)...);
    }

    template<typename Member, typename Obj>
    auto Invoke(Member&& member, Obj&& obj) -> decltype(std::forward<Obj>(obj).*std::forward<Member>(member)) {
        return std::forward<Obj>(obj).*std::forward<Member>(member);
    }

    template<typename Member, typename ObjPtr>
    auto Invoke(Member&& member, ObjPtr* ptr) -> decltype((*ptr).*std::forward<Member>(member)){
        return (*ptr).*std::forward<Member>(member);
    }

}
