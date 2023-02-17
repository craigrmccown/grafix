#pragma once

/**
 * Elements are written to a contiguous block of memory circularly, where the
 * oldest element is always replaced. However, elements are read from a fixed
 * index (the 0-index of the underlying storage). This means that accessing
 * every element by index after a write will produce a stable value, except for
 * the value that was written.
 */
template <class T>
class CircularWriteBuffer
{
    public:
    CircularWriteBuffer(int cap) : cap(cap), size(0), head(0)
    {
        buf = new T[cap];
    }

    ~CircularWriteBuffer()
    {
        delete[] buf;
    }

    const T &operator[](int i)
    {
        if (i < 0 || i >= size)
        {
            throw std::out_of_range(
                "Index out of bounds, must be between 0 and " +
                std::to_string(size)
            );
        }
        return buf[i];
    }

    int getSize()
    {
        return size;
    }

    void write(T t)
    {
        head = (head + 1) % cap;
        buf[head] = t;
        if (size < cap) size ++;
    }

    void fill(T t)
    {
        for (int i = 0; i < cap; i++)
        {
            buf[i] = t;
        }
        size = cap;
    }

    private:
    int cap, size, head;
    T *buf;
};
