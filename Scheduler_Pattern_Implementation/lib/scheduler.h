#pragma once

#include <memory>
#include <vector>

#include "Invoke.h"
#include "UniversalHolder.h"
#include "VoidHandling.h"


class TTaskScheduler {
public:

    template <typename Pred>
    auto add(Pred&& pred) {
        auto task_ptr = std::make_shared<ZeroArgWrapper<Pred>>(std::forward<Pred>(pred));
        AddData(task_ptr);
        return index_vertex - 1;
    }

    template <typename Pred, typename Arg>
    auto add(Pred&& pred, Arg&& arg) {
        auto task_ptr = std::make_shared<OneArgWrapper<Pred, Arg>>(
            std::forward<Pred>(pred),
            std::forward<Arg>(arg)
        );
        AddData(task_ptr);
        return index_vertex - 1;
    }

    template <typename Pred, typename FirstArg, typename SecondArg>
    auto add(Pred&& pred, FirstArg&& first_arg, SecondArg&& second_arg) {
        auto task_ptr = std::make_shared<TwoArgWrapper<Pred, FirstArg, SecondArg>>(
            std::forward<Pred>(pred),
            std::forward<FirstArg>(first_arg),
            std::forward<SecondArg>(second_arg)
        );
        AddData(task_ptr);
        return index_vertex - 1;
    }


    template<typename Result>
    Result getResult(const size_t& num_vertex) {
        if (num_vertex >= index_vertex) {
            throw std::out_of_range("Incorrect id");
        }
        DFS(num_vertex);

        if constexpr (VoidHandling::is_void_v<Result>) {
            ptr_base_wrapper_[num_vertex].get()->GetResult();
            return;
        } else {
            return ptr_base_wrapper_[num_vertex].get()->GetResult().Cast<Result>();
        }

    }

    void executeAll() {
        for (size_t i = 0; i < ptr_base_wrapper_.size(); ++i) {
            ptr_base_wrapper_[i].get()->GetResult();
        }
    }

    template<typename Result>
    auto getFutureResult(const size_t& num_vertex) {
        static_assert(!VoidHandling::is_void_v<Result>, "Can't create FutureResult for void type");

        if (num_vertex >= index_vertex) {
            throw std::out_of_range("Incorrect id");
        }

        while (graph.size() != ptr_base_wrapper_.size() + 1) {
            graph.emplace_back();
        }
        graph[index_vertex].emplace_back(num_vertex);

        return FutureResult<Result>(ptr_base_wrapper_[num_vertex]);
    }

private:

    class BaseWrapper {
    public:
        virtual ~BaseWrapper() = default;
        virtual UniversalHolder& GetResult() = 0;

    };

    template<typename Result>
    class FutureResult {
    public:
        explicit FutureResult(std::weak_ptr<BaseWrapper> wrapper) 
                : wrapper_(std::move(wrapper)) {}

        operator Result() {
            auto ptr = wrapper_.lock();
            return ptr->GetResult().Cast<Result>();
        }

    private:
        std::weak_ptr<BaseWrapper> wrapper_;
    };


    template<typename Pred>
    class ZeroArgWrapper : public BaseWrapper {
    public:
        explicit ZeroArgWrapper(Pred&& pred) : pred_(std::forward<Pred>(pred)) {}

        UniversalHolder& GetResult() override {
            if (is_done_) {
                return result_;
            }

            using ReturnType = decltype(InvokeFunction::Invoke(std::forward<Pred>(pred_)));

            if constexpr (VoidHandling::is_void_v<ReturnType>) {
                InvokeFunction::Invoke(std::forward<Pred>(pred_));
                result_ = UniversalHolder{};
            } else {
                result_ = InvokeFunction::Invoke(std::forward<Pred>(pred_));
            }

            is_done_ = true;
            return result_;
        }

    private:
        Pred pred_;

        bool is_done_ = false;
        UniversalHolder result_;

    };


    template<typename Pred, typename Arg>
    class OneArgWrapper : public BaseWrapper {
    public:
        explicit OneArgWrapper(Pred&& pred, Arg&& arg) 
                : pred_(std::forward<Pred>(pred)), arg_(std::forward<Arg>(arg)) {}

        UniversalHolder& GetResult() override {
            if (is_done_) {
                return result_;
            }

            using ReturnType = decltype(InvokeFunction::Invoke(std::forward<Pred>(pred_), 
                std::forward<Arg>(arg_)));

            if constexpr (VoidHandling::is_void_v<ReturnType>) {
                InvokeFunction::Invoke(std::forward<Pred>(pred_), 
                    std::forward<Arg>(arg_));
                result_ = UniversalHolder{};
            } else {
                result_ =  InvokeFunction::Invoke(std::forward<Pred>(pred_), 
                    std::forward<Arg>(arg_));
            }

            is_done_ = true;
            return result_;
        }

    private:
        Pred pred_;
        Arg arg_;

        bool is_done_ = false;
        UniversalHolder result_;

    };


    template<typename Pred, typename FirstArg, typename SecondArg>
    class TwoArgWrapper : public BaseWrapper {
    public:
        explicit TwoArgWrapper(Pred&& pred, FirstArg&& first_arg, SecondArg&& second_arg)
                : pred_(std::forward<Pred>(pred))
                , first_arg_(std::forward<FirstArg>(first_arg))
                , second_arg_(std::forward<SecondArg>(second_arg)) {}

        UniversalHolder& GetResult() override {
            if (is_done_) {
                return result_;
            }

            using ReturnType = decltype(std::forward<Pred>(pred_), 
                std::forward<FirstArg>(first_arg_), 
                std::forward<SecondArg>(second_arg_));
    


            if constexpr (VoidHandling::is_void_v<ReturnType>) {
                InvokeFunction::Invoke(std::forward<Pred>(pred_), 
                    std::forward<FirstArg>(first_arg_), 
                    std::forward<SecondArg>(second_arg_));

                result_ = UniversalHolder{};
            } else {
                result_ = InvokeFunction::Invoke(std::forward<Pred>(pred_), 
                    std::forward<FirstArg>(first_arg_), 
                    std::forward<SecondArg>(second_arg_));
            }
            
            is_done_ = true;
            return result_;
        }

    private:
        Pred pred_;
        FirstArg first_arg_;
        SecondArg second_arg_;

        bool is_done_ = false;
        UniversalHolder result_;

    };


    void AddData(const std::shared_ptr<BaseWrapper>& ptr) {
        ptr_base_wrapper_.emplace_back(ptr);
        ++index_vertex;
    }


    void DFSRecursive(const size_t& index, std::vector<bool>& visited) {
        visited[index] = true;
    
        for (size_t vertex : graph[index]) {
            if (!visited[vertex]) {
                DFSRecursive(vertex, visited);
            }
        }

        ptr_base_wrapper_[index].get()->GetResult();
    }
    
    void DFS(const size_t& index) {
        if (graph.size() == 0) { return; }
        std::vector<bool> visited(index + 1);
        DFSRecursive(index, visited);
    }

    size_t index_vertex = 0;
    std::vector<std::shared_ptr<BaseWrapper>> ptr_base_wrapper_;
    std::vector<std::vector<size_t>> graph;

};
