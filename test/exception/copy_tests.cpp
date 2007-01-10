
// Copyright 2006 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "./containers.hpp"

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/exception_safety.hpp>
#include "../helpers/random_values.hpp"

template <class T>
struct copy_test1 : public test::exception_base
{
    T x;

    void run() const {
        T y(x);
    }
};

template <class T>
struct copy_test2 : public test::exception_base
{
    test::random_values<T> values;
    T x;

    copy_test2() : values(5), x(values.begin(), values.end()) {}

    void run() const {
        T y(x);
    }
};

template <class T>
struct copy_test3 : public test::exception_base
{
    test::random_values<T> values;
    T x;

    copy_test3() : values(100), x(values.begin(), values.end()) {}

    void run() const {
        T y(x);
    }
};

RUN_EXCEPTION_TESTS(
    (copy_test1)(copy_test2)(copy_test3),
    CONTAINER_SEQ)