/*
 * Copyright (c) Vladimir Golubev
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

#include <cppgear/container/FlatMap.h>
#include <cppgear/Maybe.h>
#include <cppgear/Optional.h>
#include <cppgear/Range.h>

#include <functional>
#include <random>

#include <gtest/gtest.h>

namespace cppgear {

    using namespace std::placeholders;

    template < typename Value_ >
    class Generator {
    public:
        using Result = Value_;

        Generator(Result min = std::numeric_limits<Result>::min(), Result max = std::numeric_limits<Result>::max()) :
            m_dist(min, max),
            m_engine(std::random_device()()) { }

        Result operator()() {
            return m_dist(m_engine);
        }

    private:
        std::uniform_int_distribution<Result> m_dist;
        std::mt19937 m_engine;
    };

    template < >
    class Generator<bool> {
        using Result = bool;

    public:
        Generator(double probability = 0.5) :
            m_dist(probability),
            m_engine(std::random_device()()) { }

        Result operator()() {
            return m_dist(m_engine);
        }

    private:
        std::bernoulli_distribution m_dist;
        std::mt19937 m_engine;
    };

    template < >
    struct Generator<std::string> {
        using Result = std::string;

        Result operator()() const {
            using Char = Result::value_type;
            Result result;

            Generator<Char> generator;

            size_t const size = Generator<size_t>(0, 20)();
            for (size_t i = 0; i < size; ++i) {
                result.push_back(generator());
            }

            return result;
        }
    };

    template < typename Key_, typename Value_ >
    struct Generator<std::pair<Key_, Value_>> {
        using Result = std::pair<Key_, Value_>;

        Result operator()() const {
            return { Generator<Key_>()(), Generator<Value_>()() };
        }
    };

    template < typename Value_ >
    struct Generator<std::vector<Value_>> {
        using Result = std::vector<Value_>;

        Generator(size_t min, size_t max) :
            m_size(Generator<size_t>(min, max)()) { }

        Result operator()() const {
            Result result;

            for (size_t i = 0; i < m_size; ++i) {
                result.push_back(Generator<Value_>()());
            }

            return result;
        }

    private:
        size_t m_size;
    };

    template < typename Key_, typename Value_ >
    struct Generator<std::map<Key_, Value_>> {
        using Result = std::map<Key_, Value_>;

        Result operator()() const {
            Result result;

            const size_t size = Generator<size_t>(1000, 10000)();
            for (size_t i = 0; i < size; ++i) {
                result.emplace(Generator<Key_>()(), Generator<Value_>()());
            }

            return result;
        }
    };

    template < typename Key_, typename Value_ >
    struct Generator<FlatMap<Key_, Value_>> {
        using Result = FlatMap<Key_, Value_>;

        Result operator()() const {
            Result result;

            const size_t size = Generator<size_t>(1000, 10000)();
            for (size_t i = 0; i < size; ++i) {
                result.emplace(Generator<Key_>()(), Generator<Value_>()());
            }

            return result;
        }
    };

    template < typename >
    struct RandomOrDefaultTag;

    template < typename Value_ >
    class Generator<RandomOrDefaultTag<Value_>> {
        using Result = Value_;

    public:
        Generator(Result const& value) :
            m_value(value) { }

        Result operator()() const {
            return Generator<bool>()() ? Generator<Result>()() : m_value;
        }

    private:
        Result m_value;
    };

    struct PairComparator {
        template < typename Lhs_, typename Rhs_ >
        bool operator()(Lhs_ const& lhs, Rhs_ const& rhs) const {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };

    TEST(FlatMapTest, Construction) {
        using Testee = FlatMap<std::string, std::string>;

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
            Testee testee{{"one", "jaws"}, {"two", "bite"}, {"three", "claws"}, {"four", "catch"}, {"four", "dup"}, {"three", "dup"}};
            EXPECT_EQ(testee.size(), 4);
            EXPECT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));

            testee = {{"five", "jaws"}, {"six", "bite"}, {"seven", "claws"}, {"eight", "catch"}, {"five", "dup"}, {"six", "dup"}};
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

            auto sample = Generator<Sample>(1000, 10000)();
            Testee testee(sample.begin(), sample.end(), Testee::allocator_type());
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
        using Testee = FlatMap<std::string, std::string>;
        using Sample = std::map<std::string, std::string>;

        auto unordered = Generator<Unordered>(1000, 10000)();

        Testee testee;
        Sample sample;

        testee.reserve(unordered.size());
        for (auto const& kv : unordered) {
            testee.insert(kv);
        }
        for (auto const& kv : unordered) {
            sample.insert(kv);
        }

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
            ASSERT_NO_THROW(EXPECT_EQ(testee.at(kv.first), kv.second));
        }
        for (auto& kv : testee) {
            ASSERT_NO_THROW(EXPECT_EQ(sample.at(kv.first), kv.second));
        }

        for (auto& kv : sample) {
            ASSERT_TRUE(testee.count(kv.first));
            EXPECT_EQ(testee[kv.first], kv.second);
        }
        for (auto& kv : testee) {
            ASSERT_TRUE(sample.count(kv.first));
            EXPECT_EQ(sample[kv.first], kv.second);
        }
    }

    enum class MapOperation {
        Insert,
        InsertRange,
        Remove
    };

    template < typename, MapOperation >
    struct MapOperationTag;

    template < typename Map_ >
    class Generator<MapOperationTag<Map_, MapOperation::Insert>> {
        using Value = typename Map_::value_type;
        using Iterator = typename Map_::iterator;
        using Result = std::function<Optional<Value&>(Map_&, Value const&)>;

    public:
        Result operator()() const {
            std::array<Result, 5> arr = {{
                &SelfType::_insert,
                &SelfType::_insert_hint,
                &SelfType::_emplace,
                &SelfType::_emplace_hint,
                &SelfType::_operator_subscript,
            }};
            return arr[Generator<size_t>(0, arr.size() - 1)()];
        }

    private:
        static Optional<Value&> _optional_from_pair(std::pair<Iterator, bool> pair) {
            if (pair.second) {
                return nullptr;
            }
            return make_optional<Value&>(*pair.first);
        }

        static Optional<Value&> _find(Map_& map, Value const& value) {
            auto iter = map.find(value.first);
            if (iter == map.end()) {
                return nullptr;
            }
            return make_optional<Value&>(*iter);
        }

        static Iterator _iterator_at(Map_& map, size_t pos) {
            for (auto iter = map.begin(); ; ++iter) {
                if (!pos) {
                    return iter;
                }
                if (iter == map.end()) {
                    break;
                }
                --pos;
            }
            throw std::logic_error("Cound not find iterator");
        }

        static Optional<Value&> _insert(Map_& map, Value const& value) {
            return _optional_from_pair(map.insert(value));
        }

        static Optional<Value&> _insert_hint(Map_& map, Value const& value) {
            return maybe(_find(map, value))
                .or_bind([&] {
                    auto const pos = Generator<size_t>(0, map.size())();
                    map.insert(_iterator_at(map, pos), value);
                })
                .take();
        }

        static Optional<Value&> _emplace(Map_& map, Value const& value) {
            return _optional_from_pair(map.emplace(value.first, value.second));
        }

        static Optional<Value&> _emplace_hint(Map_& map, Value const& value) {
            return maybe(_find(map, value))
                .or_bind([&] {
                    auto const pos = Generator<size_t>(0, map.size())();
                    map.emplace_hint(_iterator_at(map, pos), value.first, value.second);
                })
                .take();
        }

        static Optional<Value&> _operator_subscript(Map_& map, Value const& value) {
            return maybe(_find(map, value))
                .or_bind([&] { map[value.first] = value.second; })
                .take();
        }
    };

    TEST(FlatMapTest, Insertion) {
        using Unordered = std::vector<std::pair<std::string, std::string>>;
        using Testee = FlatMap<std::string, std::string>;
        using Sample = std::map<std::string, std::string>;

        using TesteeOpGenerator = Generator<MapOperationTag<Testee, MapOperation::Insert>>;
        using SampleOpGenerator = Generator<MapOperationTag<Sample, MapOperation::Insert>>;

        Testee testee;
        Sample sample;

        auto unordered = Generator<Unordered>(1000, 10000)();
        for (auto const& kv : unordered) {
            auto testee_result = TesteeOpGenerator()()(testee, kv);
            auto sample_result = SampleOpGenerator()()(sample, kv);

            EXPECT_EQ((bool)testee_result, (bool)sample_result);
            maybe(testee_result)
                .and_(sample_result)
                .and_bind([&](auto) { EXPECT_TRUE(PairComparator()(*testee_result, *sample_result)); });

            ASSERT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));
            ASSERT_TRUE(std::equal(sample.begin(), sample.end(), testee.begin(), testee.end(), PairComparator()));
        }
    }

    template < typename Map_ >
    class Generator<MapOperationTag<Map_, MapOperation::InsertRange>> {
    public:
        using Value = typename Map_::value_type;
        using Range = std::vector<Value>;
        using Iterator = typename Map_::iterator;
        using Result = std::function<void(Map_&, Range const&)>;

    public:
        Result operator()() const {
            std::array<Result, 3> arr = {{
                &SelfType::_medley_insert,
                &SelfType::_iterator_insert,
                &SelfType::_initializer_list_insert,
            }};
            return arr[Generator<size_t>(0, arr.size() - 1)()];
        }

    private:
        static void _medley_insert(Map_& map, Range const& range) {
            for (auto const& kv : range) {
                Generator<MapOperationTag<Map_, MapOperation::Insert>>()()(map, kv);
            }
        }

        static void _iterator_insert(Map_& map, Range const& range) {
            map.insert(range.begin(), range.end());
        }

        static void _initializer_list_insert(Map_& map, Range const& range) {
            if (0 == range.size() % 5) {
                for (size_t i = 0; i < range.size(); i += 5)
                    map.insert({ range[i], range[i+1], range[i+2], range[i+3], range[i+4] });
            } else if (0 == range.size() % 4) {
                for (size_t i = 0; i < range.size(); i += 4)
                    map.insert({ range[i], range[i+1], range[i+2], range[i+3] });
            } else if (0 == range.size() % 3) {
                for (size_t i = 0; i < range.size(); i += 3)
                    map.insert({ range[i], range[i+1], range[i+2]});
            } else if (0 == range.size() % 2) {
                for (size_t i = 0; i < range.size(); i += 2)
                    map.insert({ range[i], range[i+1]});
            } else {
                for (size_t i = 0; i < range.size(); ++i)
                    map.insert({ range[i] });
            }
        }
    };

    TEST(FlatMapTest, RangeInsertion) {
        using Testee = FlatMap<std::string, std::string>;
        using OpGenerator = Generator<MapOperationTag<Testee, MapOperation::InsertRange>>;

        Testee testee;
        for (auto i : range(Generator<size_t>(150, 200)())) {
            (void)i;

            OpGenerator()()(testee, Generator<OpGenerator::Range>(10, 50)());

            ASSERT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));
            ASSERT_EQ(std::unique(testee.begin(), testee.end(), testee.get_equality_predicate()), testee.end());
        }
    }

    template < typename Map_ >
    class Generator<MapOperationTag<Map_, MapOperation::Remove>> {
    public:
        using Key = typename Map_::key_type;
        using Value = typename Map_::mapped_type;
        using Result = std::function<Optional<Value>(Map_&, Key const&)>;

    public:
        Result operator()() const {
            std::array<Result, 2> arr = {{
                &SelfType::_erase,
                &SelfType::_iterator_erase,
            }};
            return arr[Generator<size_t>(0, arr.size() - 1)()];
        }

    private:
        static Optional<Value&> _find(Map_& map, Key const& key) {
            auto iter = map.find(key);
            if (iter == map.end()) {
                return nullptr;
            }
            return make_optional<Value&>(iter->second);
        }

        static Optional<Value> _erase(Map_& map, Key const& key) {
            return maybe(_find(map, key))
                .and_bind([&](auto){ map.erase(key); })
                .and_bind(OptionalBuilder<Value>())
                .take();
        }

        static Optional<Value> _iterator_erase(Map_& map, Key const& key) {
            auto iter = map.find(key);
            if (iter == map.end()) {
                return nullptr;
            }
            auto result = iter->second;
            map.erase(iter);
            return make_optional<Value>(result);
        }
    };

    TEST(FlatMapTest, Removal) {
        using Unordered = std::vector<std::pair<std::string, std::string>>;
        using Testee = FlatMap<std::string, std::string>;
        using OpGenerator = Generator<MapOperationTag<Testee, MapOperation::Remove>>;

        auto unordered = Generator<Unordered>(1000, 10000)();
        Testee testee(unordered.begin(), unordered.end());

        while (unordered.size()) {
            Unordered::value_type next = unordered.back();
            auto key = Generator<RandomOrDefaultTag<Testee::key_type>>(next.first)();
            if (next.first == key) {
                unordered.pop_back();
            }

            OpGenerator()()(testee, key);

            ASSERT_TRUE(std::is_sorted(testee.begin(), testee.end(), testee.value_comp()));
            ASSERT_EQ(std::unique(testee.begin(), testee.end(), testee.get_equality_predicate()), testee.end());
        }
    }
}
