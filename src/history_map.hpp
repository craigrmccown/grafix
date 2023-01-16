#pragma once

#include <array>
#include <map>

/**
 * Key-value store that keeps the previous version of each value. Every key is
 * treated as if it exists, but is initialized by the default value supplied to
 * the constructor.
 */
template <class K, class V>
class HistoryMap
{
    public:
    HistoryMap(V defaultVal) : defaultVal(defaultVal) {}

    void set(K key, V val)
    {
        if (has(key)) m[key] = {m[key][1], val};
        else m[key] = {defaultVal, val};
    }

    V get(K key)
    {
        return getKeyIdx(key, 1);
    }

    V getPrev(K key)
    {
        return getKeyIdx(key, 0);
    }

    private:
    std::map<K, std::array<V, 2>> m;
    V defaultVal;

    bool has(K key)
    {
        return m.count(key);
    }

    V getKeyIdx(K key, int idx)
    {
        if (has(key)) return m[key][idx];
        else return defaultVal;
    }
};
