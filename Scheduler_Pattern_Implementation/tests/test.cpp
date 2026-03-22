#include "scheduler.h"

#include <cmath>
#include <functional>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(SchedulerTest, ZeroArg) {
    TTaskScheduler scheduler;
    auto id1 = scheduler.add([](){ return 10; });

    ASSERT_EQ(10, scheduler.getResult<int>(id1));
}

TEST(SchedulerTest, OneArg) {
    TTaskScheduler scheduler;

    int a = 1;
    auto id1 = scheduler.add([](int a){ return 10 + a; }, a);

    ASSERT_EQ(11, scheduler.getResult<int>(id1));
}

TEST(SchedulerTest, TwoArg) {
    TTaskScheduler scheduler;

    int a = 1;
    int b = 2;
    auto id1 = scheduler.add([](int a, int b){ return 10 + a + b; }, a, b);

    ASSERT_EQ(13, scheduler.getResult<int>(id1));
}

TEST(SchedulerTest, ClassMethod) {
    TTaskScheduler scheduler;

    class Mult {
    public:
        Mult(float value) : value_(value) {}
        float GetResult(float x) const { return x * value_; }
    
    private:
        float value_;
    };

    Mult mult{2.5f};
    float value = 4.0f;
    
    auto task = scheduler.add(&Mult::GetResult, mult, value);

    ASSERT_EQ(10.0f, scheduler.getResult<float>(task));
}

TEST(SchedulerTest, ClassMember) {
    TTaskScheduler scheduler;
    struct S {
        int value = 10;
    };
    S s;

    auto id1 = scheduler.add(&S::value, s);
    auto id2 = scheduler.add(&S::value, &s);
    
    ASSERT_EQ(10, scheduler.getResult<int>(id1));
    ASSERT_EQ(10, scheduler.getResult<int>(id2));
}

TEST(SchedulerTest, QuadraticEquation) {
    TTaskScheduler scheduler;
    float a = 1, b = -5, c = 6;
    
    auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);
    auto id2 = scheduler.add([](float b, float part) { return b * b + part; }, b, scheduler.getFutureResult<float>(id1));
    auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));

    auto x1 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));
    auto x2 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));

    scheduler.executeAll();

    ASSERT_EQ(3.0f, scheduler.getResult<float>(x1));
    ASSERT_EQ(2.0f, scheduler.getResult<float>(x2));
}

TEST(SchedulerTest, TaskDependencies) {
    TTaskScheduler scheduler;
    
    auto t1 = scheduler.add([](int x) { return x * 2; }, 5);
    auto t2 = scheduler.add([](int x) { return x + 3; }, scheduler.getFutureResult<int>(t1));
    auto t3 = scheduler.add([](int a, int b) { return a + b; }, scheduler.getFutureResult<int>(t1), scheduler.getFutureResult<int>(t2));

    scheduler.executeAll();

    ASSERT_EQ(10, scheduler.getResult<int>(t1));
    ASSERT_EQ(13, scheduler.getResult<int>(t2));
    ASSERT_EQ(23, scheduler.getResult<int>(t3));
}

TEST(SchedulerTest, MultiTypesOneArg) {
    TTaskScheduler scheduler;
    
    auto id_int = scheduler.add([](int x) { return x * 2; }, 5);
    auto id_float = scheduler.add([](float x) { return x / 2; }, 10.0f);
    auto id_string = scheduler.add([](std::string s) { return s + " world"; }, "hello");

    scheduler.executeAll();

    ASSERT_EQ(10, scheduler.getResult<int>(id_int));
    ASSERT_EQ(5.0f, scheduler.getResult<float>(id_float));
    ASSERT_EQ("hello world", scheduler.getResult<std::string>(id_string));
}


TEST(SchedulerTest, LazyFibbonachi) {
    TTaskScheduler scheduler;
    int lazy_cnt = 0;

    auto id1 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 1, 1);

    auto id2 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        1, scheduler.getFutureResult<int>(id1));

    auto id3 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id1), scheduler.getFutureResult<int>(id2));

    auto id4 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id2), scheduler.getFutureResult<int>(id3));

    auto id5 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id3), scheduler.getFutureResult<int>(id4));

    auto id6 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id4), scheduler.getFutureResult<int>(id5));

    auto id7 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id5), scheduler.getFutureResult<int>(id6));

    auto id8 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id6), scheduler.getFutureResult<int>(id7));

    auto id9 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id7), scheduler.getFutureResult<int>(id8));

    auto id10 = scheduler.add([&lazy_cnt](int a, int b) {lazy_cnt++; return a + b; }, 
        scheduler.getFutureResult<int>(id8), scheduler.getFutureResult<int>(id9));

    ASSERT_EQ(144, scheduler.getResult<int>(id10));

    ASSERT_EQ(10, lazy_cnt);
}

TEST(SchedulerTest, VoidFunc) {
    TTaskScheduler scheduler;

    int value = 0;

    auto id1 = scheduler.add([](int& value) { value++; }, value );
    scheduler.getResult<void>(id1);

    ASSERT_EQ(1, value);
 
}

TEST(SchedulerTest, CastReference) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        int Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<int&>(id1));

}

TEST(SchedulerTest, CastConstReference) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        int Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<const int&>(id1));
}

TEST(SchedulerTest, ReferenceToCopy) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        int& Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<int>(id1));
}

TEST(SchedulerTest, ReferenceToRef) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        int& Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<int&>(id1));
}

TEST(SchedulerTest, ReferenceToConstRef) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        int& Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<const int&>(id1));
}

TEST(SchedulerTest, ConstRefToCopy) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        const int& Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<int>(id1));
}

TEST(SchedulerTest, ConstRefToRef) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        const int& Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<int&>(id1));
}

TEST(SchedulerTest, ConstRefToConstRef) {
    TTaskScheduler scheduler;

    class ABC {
    public:
        const int& Out() { return value_; }

    private:
        int value_ = 5;

    };
    ABC abc_class;

    auto id1 = scheduler.add(&ABC::Out, abc_class);

    ASSERT_EQ(5, scheduler.getResult<const int&>(id1));
}

TEST(SchedulerTest, UniversalRef) {
    TTaskScheduler scheduler;
    
    auto id1 = scheduler.add([](float&& a){ return a; }, 5.0f);
    
    ASSERT_EQ(scheduler.getResult<float>(id1), 5.0f);
}


TEST(SchedulerTest, Rhombus) {
    TTaskScheduler scheduler;
    struct S {
        int v = 10;
    };
    S d;
    int cnt = 0;

    auto id1 = scheduler.add([&cnt]() -> std::string { ++cnt; return "A"; });
    auto id2 = scheduler.add([&cnt](std::string arg){ ++cnt; return arg + "B"; }, scheduler.getFutureResult<std::string>(id1));
    auto id3 = scheduler.add([&cnt](std::string arg){ ++cnt; return arg + "C"; }, scheduler.getFutureResult<std::string>(id1));
    auto id4 = scheduler.add([&cnt](std::string arg1, std::string arg2){ ++cnt; return arg1 + arg2 + "D"; }, 
        scheduler.getFutureResult<std::string>(id2), scheduler.getFutureResult<std::string>(id3));

    scheduler.executeAll();

    ASSERT_EQ(4, cnt);
    ASSERT_EQ("A", scheduler.getResult<std::string>(id1));
    ASSERT_EQ("AB", scheduler.getResult<std::string>(id2));
    ASSERT_EQ("AC", scheduler.getResult<std::string>(id3));
    ASSERT_EQ("ABACD", scheduler.getResult<std::string>(id4));
}

TEST(SchedulerTest, ManyParallelTasks) {
    TTaskScheduler scheduler;
    constexpr int N = 1000;
    std::vector<int> results(N);
    
    for (int i = 0; i < N; ++i) {
        scheduler.add([i, &results]() { results[i] = i * i; });
    }
    
    scheduler.executeAll();
    
    for (int i = 0; i < N; ++i) {
        ASSERT_EQ(results[i], i * i);
    }
}

TEST(SchedulerTest, TaskExceptions) {
    TTaskScheduler scheduler;
    
    auto id1 = scheduler.add([]() { throw std::runtime_error("Error"); });
    auto id2 = scheduler.add([]() { return 42; });
    
    ASSERT_THROW(scheduler.getResult<void>(id1), std::runtime_error);
    ASSERT_NO_THROW(scheduler.getResult<int>(id2));
}

TEST(SchedulerTest, ExceptionPropagation) {
    TTaskScheduler scheduler;
    
    auto id1 = scheduler.add([]() { throw std::runtime_error("First error"); });
    auto id2 = scheduler.add([](int) { return 42; }, scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](int) { return 100; }, scheduler.getFutureResult<int>(id2));
    
    ASSERT_THROW(scheduler.getResult<int>(id3), std::runtime_error);
    ASSERT_THROW(scheduler.getResult<int>(id2), std::runtime_error);
    ASSERT_THROW(scheduler.getResult<int>(id1), std::runtime_error);
}

TEST(SchedulerTest, ExeptionOutOfRange) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a) { return a; }, 1);

    ASSERT_THROW(scheduler.getResult<int>(2), std::out_of_range);
}

TEST(SchedulerTest, CallCheck) {
    TTaskScheduler scheduler;
    int call_count = 0;

    auto id = scheduler.add([&call_count] { ++call_count; return 77; });

    ASSERT_EQ(scheduler.getResult<int>(id), 77);
    ASSERT_EQ(scheduler.getResult<int>(id), 77);
    ASSERT_EQ(call_count, 1);
}

TEST(SchedulerTest, ComplexDependencyGraph) {
    TTaskScheduler scheduler;
    
    auto A = scheduler.add([]() { return 1; });
    auto B = scheduler.add([](int a) { return a + 1; }, scheduler.getFutureResult<int>(A));
    auto C = scheduler.add([](int b) { return b * 2; }, scheduler.getFutureResult<int>(B));
    auto D = scheduler.add([](int c) { return c - 1; }, scheduler.getFutureResult<int>(C));
    auto E = scheduler.add([](int a, int b) { return a + b; }, 
        scheduler.getFutureResult<int>(A), scheduler.getFutureResult<int>(B));

    auto F = scheduler.add([](int c, int e) { return c + e; },
        scheduler.getFutureResult<int>(C), scheduler.getFutureResult<int>(E));

    auto G = scheduler.add([](int f) { return f * 10; }, scheduler.getFutureResult<int>(F));
    
    ASSERT_EQ(scheduler.getResult<int>(D), 3);
    ASSERT_EQ(scheduler.getResult<int>(G), 70);
}


int Function(int a) {
    return a * 3;
}

TEST(SchedulerTest, Function) {
    TTaskScheduler scheduler;

    auto id = scheduler.add(Function, 4);
    ASSERT_EQ(12, scheduler.getResult<int>(id));
}


struct Functor {
    int operator()(int x) const {
        return x * x;
    }
};

TEST(SchedulerTest, FunctorCallable) {
    TTaskScheduler scheduler;

    Functor f;
    auto id = scheduler.add(f, 6);

    ASSERT_EQ(36, scheduler.getResult<int>(id));
}

TEST(SchedulerTest, StdFunction) {
    TTaskScheduler scheduler;

    std::function<int(int)> f = [](int x) { return x + 10; };
    auto id = scheduler.add(f, 7);

    ASSERT_EQ(17, scheduler.getResult<int>(id));
}


int Multiply(int a, int b) {
    return a * b;
}

TEST(SchedulerTest, StdBind) {
    TTaskScheduler scheduler;

    auto bound_func = std::bind(Multiply, 2, 5);
    auto id = scheduler.add(bound_func);

    ASSERT_EQ(10, scheduler.getResult<int>(id));
}

TEST(SchedulerTest, ReferenceWrapper) {
    TTaskScheduler scheduler;

    auto lambda = [](int x) { return x + 1; };
    std::reference_wrapper<decltype(lambda)> ref(lambda);

    auto id = scheduler.add(ref, 9);

    ASSERT_EQ(10, scheduler.getResult<int>(id));
}



struct MyStruct {
    int value = 2;
    int GetVal() const { return value; }
};

TEST(SchedulerTest, ConstMemberFunctionPointer) {
    TTaskScheduler scheduler;

    MyStruct obj;
    auto id = scheduler.add(&MyStruct::GetVal, obj);

    ASSERT_EQ(2, scheduler.getResult<int>(id));
}
