#include <map>
#include <cassert>
#include <iostream>
#include "numeric_limits_specialization.h"

template<typename K, typename V>
class interval_map {
    friend void IntervalMapTest();

    V m_valBegin;
    std::map<K, V> m_map;
public:
    interval_map(V const &val)
            : m_valBegin(val) {}

    void assign(K const &keyBegin, K const &keyEnd, V const &val) {
        if (!(keyBegin < keyEnd)) {
            return;
        }
        auto [shouldReplaceBegin, beginInsertionPoint] = find_insertion_point(keyBegin);
        auto [shouldReplaceEnd, endInsertionPoint] = find_insertion_point(keyEnd);

        V valBeforeBegin = m_valBegin;
        auto trimStart = insert_begin(keyBegin,
                                      val,
                                      beginInsertionPoint,
                                      shouldReplaceBegin,
                                      &valBeforeBegin);
        auto trimEnd = insert_end(keyEnd,
                                  val,
                                  endInsertionPoint,
                                  shouldReplaceEnd,
                                  &valBeforeBegin);

        trim(trimStart, trimEnd);
    }

    std::pair<bool, typename std::map<K, V>::iterator>
    find_insertion_point(K const &keyToInsert) {
        auto lowerBound = m_map.lower_bound(keyToInsert);
        auto upperBound = m_map.upper_bound(keyToInsert);
        bool keyExists = !(lowerBound == upperBound);
        bool shouldReplace = keyExists;
        return {shouldReplace, lowerBound};
    }

    typename std::map<K, V>::iterator
    insert_begin(K const &key, V const &val,
                 typename std::map<K, V>::iterator insertionPoint,
                 bool shouldReplace,
                 V *valBeforeBegin) {
        if (shouldReplace) {
            return replace_begin(key, val, insertionPoint, valBeforeBegin);
        }
        return insert_begin_before(key, val, insertionPoint, valBeforeBegin);
    }

    /*
     * Replaces the begin key if not in an existing interval with the same value
     * Returns the pointer to the first key that needs to be trimmed after the insertions have been made
     * to maintain the map canonical.
    */
    typename std::map<K, V>::iterator
    replace_begin(K const &key, V const &val,
                  typename std::map<K, V>::iterator atPoint,
                  V *valBeforeBegin) {
        bool intervalAlreadyExists = !(atPoint == m_map.begin()) && std::prev(atPoint)->second == val;
        if (intervalAlreadyExists) {
            *valBeforeBegin = val;
            return atPoint;
        }

        *valBeforeBegin = atPoint->second;
        m_map.insert_or_assign(key, val);
        return std::next(atPoint);
    }

    /*
     * Inserts the begin key if not in an existing interval with the same value or before the beginning of the map.
     * Returns the pointer to the first key that needs to be trimmed after the insertions have been made
     * to maintain the map canonical.
    */
    typename std::map<K, V>::iterator
    insert_begin_before(K const &key, V const &val, typename std::map<K, V>::iterator beforePoint, V *valBeforeBegin) {
        if (beforePoint == m_map.begin()) {
            *valBeforeBegin = m_valBegin;
            auto pos = m_map.insert(beforePoint, std::pair{key, val});
            return std::next(pos);
        }

        auto prev = std::prev(beforePoint);
        bool intervalAlreadyExists = prev->second == val;
        if (!intervalAlreadyExists) {
            *valBeforeBegin = prev->second;
            m_map.insert_or_assign(key, val);
            return beforePoint;
        }

        *valBeforeBegin = val;
        return beforePoint;
    }

    typename std::map<K, V>::iterator insert_end(K const &key, V const &val,
                                                 typename std::map<K, V>::iterator insertionPoint,
                                                 bool shouldReplace,
                                                 V *valBeforeBegin) {
        if (shouldReplace) {
            return replace_end(val, insertionPoint);
        }
        return insert_end_before(key, val, valBeforeBegin, insertionPoint);
    }

    /*
     * Returns a pointer to the last key after the one that needs to be trimmed to maintain the map canonic.
     * If the interval already exists, then the previous key should be trimmed.
     * Else, then the one pointed to by the insertion point will be trimmed.
     */
    typename std::map<K, V>::iterator
    replace_end(V const &val, typename std::map<K, V>::iterator endInsertionPoint) {
        bool intervalAlreadyExists = !(endInsertionPoint == m_map.end()) && endInsertionPoint->second == val;
        if (intervalAlreadyExists) {
            return std::next(endInsertionPoint);
        }
        return endInsertionPoint;
    }

    /*
     * Closes the inserted interval, either by continuing an existing interval or setting
     * the value that existed before the begin insertion point.
     * Returns a pointer to the last key after the one that needs to be trimmed to maintain the map canonical.
     */
    typename std::map<K, V>::iterator
    insert_end_before(K const &keyEnd, V const &val,
                      V *valBeforeBegin,
                      typename std::map<K, V>::iterator beforePoint) {
        if (beforePoint == m_map.begin()) {
            throw std::range_error("Attempting to insert end before lower bound");
        }

        auto prev = std::prev(beforePoint);
        bool intervalAlreadyExists = prev->second == val;
        if (!intervalAlreadyExists) {
            return m_map.insert(beforePoint, std::pair{keyEnd, prev->second});
        } else if (!(*valBeforeBegin == val)) {
            return m_map.insert(beforePoint, std::pair{keyEnd, *valBeforeBegin});
        }
        // This means that the insertion had no effect because it was fully enclosed in an existing interval
        return m_map.begin();
    }

    /*
     * Trims the map to keep it canonical, using the open interval [trimStart, trimEnd).
     */
    void trim(typename std::map<K, V>::iterator trimStart, typename std::map<K, V>::iterator trimEnd) {
        if (trimStart == trimEnd
            || trimStart == m_map.end()
            || !(trimEnd == m_map.end()) && trimEnd->first < trimStart->first
            || trimStart->first < m_map.begin()->first) {
            return;
        }
        m_map.erase(trimStart, trimEnd);
    }

    V const &operator[](K const &key) const {
        auto it = m_map.upper_bound(key);
        if (it == m_map.begin()) {
            return m_valBegin;
        } else {
            return (--it)->second;
        }
    }

    typename std::map<K, V>::iterator begin() {
        return m_map.begin();
    }

    typename std::map<K, V>::iterator end() {
        return m_map.end();
    }

    typename std::map<K, V>::iterator find(K const &key) {
        return m_map.find(key);
    }

    void print() {
        for (const auto &pair: m_map) {
            std::cout << pair.first << ": " << pair.second << '\n';
        }
    }
};

class IntervalMapTest {
public:
    explicit IntervalMapTest() = default;

    static int run() {
        testInitialization();
        testSingleAssignment();
        testReAssignment();
        testNullAssignment();
        testOverlapAssignment();
        testIntervalCollapseAssignment();

        std::cout << "All tests passed!" << std::endl;
        return 0;
    }

private:
    static void testInitialization() {
        interval_map<LimitedInt, LimitedChar> testMap('A');
        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'A');
        assert(testMap[1] == 'A');
    }

    static void testSingleAssignment() {
        interval_map<LimitedInt, LimitedChar> testMap('A');

        // One assignment with large gap
        testMap.assign(3, 5, 'B');

        assert(testMap[-1] == 'A');
        assert(testMap[3] == 'B');
        assert(testMap[5] == 'A');
        assert(testMap.find(6) == testMap.end());

        // One assignment with short gap
        testMap = interval_map<LimitedInt, LimitedChar>('A');
        testMap.assign(3, 4, 'B');

        assert(testMap[-2] == 'A');
        assert(testMap[-1] == 'A');
        assert(testMap[3] == 'B');
        assert(testMap[4] == 'A');
        assert(testMap.find(5) == testMap.end());

        // One assignment with no gap
        testMap = interval_map<LimitedInt, LimitedChar>('A');
        testMap.assign(3, 3, 'B');

        assert(testMap[-2] == 'A');
        assert(testMap[-1] == 'A');
        assert(testMap.find(3) == testMap.end());

        // Invalid assignment
        testMap = interval_map<LimitedInt, LimitedChar>('A');
        testMap.assign(4, 3, 'B');

        assert(testMap[-2] == 'A');
        assert(testMap[-1] == 'A');
        assert(testMap.find(3) == testMap.end());
        assert(testMap.find(4) == testMap.end());

        // One assignment to lowest
        testMap = interval_map<LimitedInt, LimitedChar>('A');
        testMap.assign(-1, 1, 'B');

        assert(testMap[-2] == 'A');
        assert(testMap[-1] == 'B');
        assert(testMap[1] == 'A');
        assert(testMap.find(2) == testMap.end());

        // Assignment before previous
        testMap = interval_map<LimitedInt, LimitedChar>('A');
        testMap.assign(1, 2, 'C');
        testMap.assign(0, 1, 'B');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'C');
        assert(testMap[2] == 'A');
    }

    static void testReAssignment() {
        interval_map<LimitedInt, LimitedChar> testMap('A');

        testMap.assign(0, 1, 'B');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'A');

        testMap.assign(0, 1, 'C');
        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'C');
        assert(testMap[1] == 'A');
        assert(testMap.find(2) == testMap.end());

        testMap.assign(0, 1, 'A');
        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'A');
        assert(testMap[1] == 'A');
        assert(testMap[2] == 'A');

        testMap.assign(0, 1, 'B');
        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'A');
        assert(testMap.find(2) == testMap.end());

        testMap.assign(1, 2, 'C');
        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'C');
        assert(testMap[2] == 'A');
        assert(testMap.find(3) == testMap.end());

        testMap.assign(2, 3, 'D');
        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'C');
        assert(testMap[2] == 'D');
        assert(testMap[3] == 'A');
        assert(testMap.find(4) == testMap.end());
    }

    static void testNullAssignment() {
        interval_map<LimitedInt, LimitedChar> testMap('A');

        testMap.assign(0, 1, 'A');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'A');
        assert(testMap[1] == 'A');
        assert(testMap[2] == 'A');
        assert(testMap[3] == 'A');

        testMap.assign(10, 100, 'A');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'A');
        assert(testMap.find(10) == testMap.end());
        assert(testMap[11] == 'A');
        assert(testMap.find(100) == testMap.end());
        assert(testMap[101] == 'A');

        testMap.assign(0, 1, 'B');
        testMap.assign(2, 4, 'A');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'A');
        assert(testMap.find(2) == testMap.end());
        assert(testMap.find(4) == testMap.end());
    }

    static void testOverlapAssignment() {
        // Simple overlap
        interval_map<LimitedInt, LimitedChar> testMap('A');

        testMap.assign(0, 2, 'B');
        testMap.assign(1, 3, 'C');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'B');
        assert(testMap[1] == 'C');
        assert(testMap.find(2) == testMap.end());
        assert(testMap[3] == 'A');
        assert(testMap.find(4) == testMap.end());

        // Overlap from the left
        testMap = interval_map<LimitedInt, LimitedChar>('A');

        testMap.assign(2, 4, 'B');
        testMap.assign(0, 3, 'C');

        assert(testMap[-1] == 'A');
        assert(testMap[0] == 'C');
        assert(testMap.find(2) == testMap.end());
        assert(testMap[3] == 'B');
        assert(testMap[4] == 'A');
        assert(testMap.find(5) == testMap.end());

        // Single overlap from the left
        testMap = interval_map<LimitedInt, LimitedChar>('A');

        testMap.assign(-1, 2, 'B');
        testMap.assign(-1, 1, 'C');

        assert(testMap[-1] == 'C');
        assert(testMap[1] == 'B');
        assert(testMap[2] == 'A');
        assert(testMap.find(3) == testMap.end());
    }

    static void testIntervalCollapseAssignment() {
        interval_map<LimitedInt, LimitedChar> testMap('A');

        testMap.assign(1, 2, 'B');
        testMap.assign(3, 4, 'B');
        testMap.assign(5, 6, 'B');

        assert(testMap[-1] == 'A');
        assert(testMap[1] == 'B');
        assert(testMap[2] == 'A');
        assert(testMap[3] == 'B');
        assert(testMap[4] == 'A');
        assert(testMap[5] == 'B');
        assert(testMap[6] == 'A');

        testMap.assign(1, 6, 'B');
        assert(testMap[-1] == 'A');
        assert(testMap[1] == 'B');
        assert(testMap.find(2) == testMap.end());
        assert(testMap.find(3) == testMap.end());
        assert(testMap.find(4) == testMap.end());
        assert(testMap.find(5) == testMap.end());
        assert(testMap[6] == 'A');
    }
};