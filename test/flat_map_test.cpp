/*
 * Copyright (c) 2016 Vladimir Golubev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cppgear/container/flat_map.h>

#include <random>

#include <gtest/gtest.h>

namespace cppgear {

    template < typename >
    struct Generator;

    template < >
    struct Generator<std::string> {
        using Result = std::string;

        Result operator()() const {
            using Char = Result::value_type;
            Result result;

            std::mt19937 engine((std::random_device()()));

            std::uniform_int_distribution<Char> generator(std::numeric_limits<Char>::min(), std::numeric_limits<Char>::max());

            const size_t size = std::uniform_int_distribution<size_t>(10, 50)(engine);
            for (size_t i = 0; i < size; ++i) {
                result.push_back(generator(engine));
            }

            return result;
        }
    };

    template < typename Key_, typename Value_ >
    struct Generator<std::map<Key_, Value_>> {
        using Result = std::map<Key_, Value_>;

        Result operator()() const {
            Result result;

            std::mt19937 engine((std::random_device()()));
            const size_t size = std::uniform_int_distribution<size_t>(1000, 10000)(engine);
            for (size_t i = 0; i < size; ++i) {
                result.emplace(Generator<Key_>()(), Generator<Value_>()());
            }

            return result;
        }
    };

    template < typename Key_, typename Value_ >
    struct Generator<std::vector<std::pair<Key_, Value_>>> {
        using Result = std::vector<std::pair<Key_, Value_>>;

        Result operator()() const {
            Result result;

            std::mt19937 engine((std::random_device()()));
            const size_t size = std::uniform_int_distribution<size_t>(1000, 10000)(engine);
            for (size_t i = 0; i < size; ++i) {
                result.emplace_back(Generator<Key_>()(), Generator<Value_>()());
            }

            return result;
        }
    };

    template < typename Key_, typename Value_ >
    struct Generator<flat_map<Key_, Value_>> {
        using Result = flat_map<Key_, Value_>;

        Result operator()() const {
            Result result;

            std::mt19937 engine((std::random_device()()));
            const size_t size = std::uniform_int_distribution<size_t>(1000, 10000)(engine);
            for (size_t i = 0; i < size; ++i) {
                result.emplace(Generator<Key_>()(), Generator<Value_>()());
            }

            return result;
        }
    };

    struct PairComparator {
        template < typename Lhs_, typename Rhs_ >
        bool operator()(Lhs_ const& lhs, Rhs_ const& rhs) const {
            return lhs.first == rhs.first && lhs.second == lhs.second;
        }
    };

    TEST(FlatMapTest, Construction) {
        using Testee = flat_map<std::string, std::string>;

        {
            Testee testee;
            EXPECT_TRUE(testee.empty());
        }
        {
            Testee testee((Testee::allocator_type(), Testee::key_compare()));
            EXPECT_TRUE(testee.empty());
        }
        {
            Testee testee((Testee::allocator_type()));
            EXPECT_TRUE(testee.empty());
        }
        {
            Testee testee((Testee::key_compare()));
            EXPECT_TRUE(testee.empty());
        }
        {
            Testee testee{{"one", "jaws"}, {"two", "bite"}, {"three", "claws"}, {"four", "catch"}};
            EXPECT_EQ(testee.size(), 4);
            EXPECT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));

            testee = {{"five", "jaws"}, {"six", "bite"}, {"seven", "claws"}, {"eight", "catch"}};
            EXPECT_EQ(testee.size(), 4);
            EXPECT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));
        }
        {
            using Sample = std::map<std::string, std::string>;

            auto sample = Generator<Sample>()();
            Testee testee(sample.begin(), sample.end(), Testee::allocator_type());
            EXPECT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));
            EXPECT_TRUE(std::equal(sample.begin(), sample.end(), testee.begin(), testee.end(), PairComparator()));
        }
        {
            using Sample = std::vector<std::pair<std::string, std::string>>;

            auto sample = Generator<Sample>()();
            Testee testee(sample.begin(), sample.end(), Testee::allocator_type());
            EXPECT_EQ(sample.size(), testee.size());
            EXPECT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));

            Testee testee2(testee);
            EXPECT_EQ(testee2, testee);

            Testee testee3(std::move(testee2));
            EXPECT_EQ(testee3, testee);

            Testee testee4(testee, Testee::allocator_type());
            EXPECT_EQ(testee4, testee);

            Testee testee5(std::move(testee4), Testee::allocator_type());
            EXPECT_EQ(testee5, testee);

            Testee testee6;
            testee6 = testee;
            EXPECT_EQ(testee6, testee);

            Testee testee7;
            testee7 = std::move(testee6);
            EXPECT_EQ(testee7, testee);
        }
    }

    TEST(FlatMapTest, Lookup) {
        using Unordered = std::vector<std::pair<std::string, std::string>>;
        using Sample = std::map<std::string, std::string>;
        using Testee = flat_map<std::string, std::string>;

        auto unordered = Generator<Unordered>()();

        Sample sample(unordered.begin(), unordered.end());
        Testee testee(unordered.begin(), unordered.end());

        EXPECT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));
        EXPECT_TRUE(std::equal(sample.begin(), sample.end(), testee.begin(), testee.end(), PairComparator()));

        for (auto sample_iter = sample.begin(); sample_iter != sample.end(); ++sample_iter) {
            auto testee_iter = testee.find(sample_iter->first);
            ASSERT_NE(testee_iter, testee.end());
            EXPECT_TRUE(PairComparator()(*testee_iter, *sample_iter));
        }
        for (auto testee_iter = testee.begin(); testee_iter != testee.end(); ++testee_iter) {
            auto sample_iter = sample.find(testee_iter->first);
            ASSERT_NE(sample_iter, sample.end());
            EXPECT_TRUE(PairComparator()(*sample_iter, *testee_iter));
        }

        for (auto sample_iter = sample.rbegin(); sample_iter != sample.rend(); ++sample_iter) {
            auto testee_iter = testee.find(sample_iter->first);
            ASSERT_NE(testee_iter, testee.end());
            EXPECT_TRUE(PairComparator()(*testee_iter, *sample_iter));
        }
        for (auto testee_iter = testee.rbegin(); testee_iter != testee.rend(); ++testee_iter) {
            auto sample_iter = sample.find(testee_iter->first);
            ASSERT_NE(sample_iter, sample.end());
            EXPECT_TRUE(PairComparator()(*sample_iter, *testee_iter));
        }

        for (auto sample_iter = sample.cbegin(); sample_iter != sample.cend(); ++sample_iter) {
            auto testee_iter = testee.find(sample_iter->first);
            ASSERT_NE(testee_iter, testee.end());
            EXPECT_TRUE(PairComparator()(*testee_iter, *sample_iter));
        }
        for (auto testee_iter = testee.cbegin(); testee_iter != testee.cend(); ++testee_iter) {
            auto sample_iter = sample.find(testee_iter->first);
            ASSERT_NE(sample_iter, sample.end());
            EXPECT_TRUE(PairComparator()(*sample_iter, *testee_iter));
        }

        for (auto sample_iter = sample.crbegin(); sample_iter != sample.crend(); ++sample_iter) {
            auto testee_iter = testee.find(sample_iter->first);
            ASSERT_NE(testee_iter, testee.end());
            EXPECT_TRUE(PairComparator()(*testee_iter, *sample_iter));
        }
        for (auto testee_iter = testee.crbegin(); testee_iter != testee.crend(); ++testee_iter) {
            auto sample_iter = sample.find(testee_iter->first);
            ASSERT_NE(sample_iter, sample.end());
            EXPECT_TRUE(PairComparator()(*sample_iter, *testee_iter));
        }

        for (auto& kv : sample) {
            Testee::mapped_type* value = nullptr;
            ASSERT_NO_THROW(value = &testee.at(kv.first));
            EXPECT_EQ(*value, kv.second);
        }
        for (auto& kv : testee) {
            Sample::mapped_type* value = nullptr;
            ASSERT_NO_THROW(value = &sample.at(kv.first));
            EXPECT_EQ(*value, kv.second);
        }

        for (auto& kv : sample) {
            EXPECT_TRUE(testee.count(kv.first));
            auto& value = testee[kv.first];
            EXPECT_EQ(value, kv.second);
        }
        for (auto& kv : testee) {
            EXPECT_TRUE(sample.count(kv.first));
            auto& value = sample[kv.first];
            EXPECT_EQ(value, kv.second);
        }
    }
}
