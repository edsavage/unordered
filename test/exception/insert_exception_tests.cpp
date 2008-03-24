
// Copyright 2006-2007 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "./containers.hpp"
#include <string>
#include "../helpers/random_values.hpp"
#include "../helpers/invariants.hpp"
#include "../helpers/strong.hpp"
#include "../helpers/input_iterator.hpp"
#include <boost/utility.hpp>
#include <cmath>

test::seed_t seed(747373);

template <class T>
struct insert_test_base : public test::exception_base
{
    test::random_values<T> values;
    insert_test_base(unsigned int count = 5) : values(count) {}

    typedef T data_type;
    typedef test::strong<T> strong_type;

    data_type init() const {
        return T();
    }

    void check(T const& x, strong_type const& strong) const {
        std::string scope(test::scope);

        if(scope.find("hash::operator()") == std::string::npos)
            strong.test(x);
        test::check_equivalent_keys(x);
    }
};

template <class T>
struct insert_test1 : public insert_test_base<T>
{
    typedef BOOST_DEDUCED_TYPENAME insert_test_base<T>::strong_type strong_type;

    void run(T& x, strong_type& strong) const {
        for(BOOST_DEDUCED_TYPENAME test::random_values<T>::const_iterator
                it = this->values.begin(), end = this->values.end(); it != end; ++it)
        {
            strong.store(x);
            x.insert(*it);
        }
    }
};

template <class T>
struct insert_test2 : public insert_test_base<T>
{
    typedef BOOST_DEDUCED_TYPENAME insert_test_base<T>::strong_type strong_type;

    void run(T& x, strong_type& strong) const {
        for(BOOST_DEDUCED_TYPENAME test::random_values<T>::const_iterator
                it = this->values.begin(), end = this->values.end(); it != end; ++it)
        {
            strong.store(x);
            x.insert(x.begin(), *it);
        }
    }
};

template <class T>
struct insert_test3 : public insert_test_base<T>
{
    void run(T& x) const {
        x.insert(this->values.begin(), this->values.end());
    }

    void check(T const& x) const {
        test::check_equivalent_keys(x);
    }
};

template <class T>
struct insert_test4 : public insert_test_base<T>
{
    typedef BOOST_DEDUCED_TYPENAME insert_test_base<T>::strong_type strong_type;

    void run(T& x, strong_type& strong) const {
        for(BOOST_DEDUCED_TYPENAME test::random_values<T>::const_iterator
                it = this->values.begin(), end = this->values.end(); it != end; ++it)
        {
            strong.store(x);
            x.insert(it, boost::next(it));
        }
    }
};

template <class T>
struct insert_test_rehash1 : public insert_test_base<T>
{
    typedef BOOST_DEDUCED_TYPENAME insert_test_base<T>::strong_type strong_type;

    insert_test_rehash1() : insert_test_base<T>(1000) {}

    T init() const {
        using namespace std;
        typedef BOOST_DEDUCED_TYPENAME T::size_type size_type;

        T x;
        x.max_load_factor(0.25);
        size_type bucket_count = x.bucket_count();
        size_type initial_elements = static_cast<size_type>(
            ceil(bucket_count * (double) x.max_load_factor()) - 1);
        UNORDERED_REQUIRE(initial_elements < this->values.size());
        x.insert(this->values.begin(),
                boost::next(this->values.begin(), initial_elements));
        UNORDERED_REQUIRE(bucket_count == x.bucket_count());
        return x;
    }

    void run(T& x, strong_type& strong) const {
        BOOST_DEDUCED_TYPENAME T::size_type bucket_count = x.bucket_count();
        int count = 0;
        BOOST_DEDUCED_TYPENAME T::const_iterator pos = x.cbegin();

        for(BOOST_DEDUCED_TYPENAME test::random_values<T>::const_iterator
            it = boost::next(this->values.begin(), x.size()), end = this->values.end();
            it != end && count < 10; ++it, ++count)
        {
            strong.store(x);
            pos = x.insert(pos, *it);
        }

        // This isn't actually a failure, but it means the test isn't doing its
        // job.
        UNORDERED_REQUIRE(x.bucket_count() != bucket_count);
    }
};

template <class T>
struct insert_test_rehash2 : public insert_test_rehash1<T>
{
    typedef BOOST_DEDUCED_TYPENAME insert_test_base<T>::strong_type strong_type;

    void run(T& x, strong_type& strong) const {
        BOOST_DEDUCED_TYPENAME T::size_type bucket_count = x.bucket_count();
        int count = 0;

        for(BOOST_DEDUCED_TYPENAME test::random_values<T>::const_iterator
            it = boost::next(this->values.begin(), x.size()), end = this->values.end();
            it != end && count < 10; ++it, ++count)
        {
            strong.store(x);
            x.insert(*it);
        }

        // This isn't actually a failure, but it means the test isn't doing its
        // job.
        UNORDERED_REQUIRE(x.bucket_count() != bucket_count);
    }
};

template <class T>
struct insert_test_rehash3 : public insert_test_base<T>
{
    BOOST_DEDUCED_TYPENAME T::size_type mutable rehash_bucket_count, original_bucket_count;

    insert_test_rehash3() : insert_test_base<T>(1000) {}

    T init() const {
        using namespace std;
        typedef BOOST_DEDUCED_TYPENAME T::size_type size_type;

        T x;
        x.max_load_factor(0.25);

        original_bucket_count = x.bucket_count();
        rehash_bucket_count = static_cast<size_type>(
            ceil(original_bucket_count * (double) x.max_load_factor())) - 1;

        size_type initial_elements = rehash_bucket_count - 5;

        UNORDERED_REQUIRE(initial_elements < this->values.size());
        x.insert(this->values.begin(),
                boost::next(this->values.begin(), initial_elements));
        UNORDERED_REQUIRE(original_bucket_count == x.bucket_count());
        return x;
    }

    void run(T& x) const {
        BOOST_DEDUCED_TYPENAME T::size_type bucket_count = x.bucket_count();

        x.insert(boost::next(this->values.begin(), x.size()),
                boost::next(this->values.begin(), x.size() + 20));

        // This isn't actually a failure, but it means the test isn't doing its
        // job.
        UNORDERED_REQUIRE(x.bucket_count() != bucket_count);
    }

    void check(T const& x) const {
        if(x.size() < rehash_bucket_count) {
            //BOOST_CHECK(x.bucket_count() == original_bucket_count);
        }
        test::check_equivalent_keys(x);
    }
};

RUN_EXCEPTION_TESTS(
    (insert_test1)(insert_test2)(insert_test3)(insert_test4)
    (insert_test_rehash1)(insert_test_rehash2)(insert_test_rehash3),
    CONTAINER_SEQ)