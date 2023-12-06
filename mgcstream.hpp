#ifndef mgc_stream_hpp
#define mgc_stream_hpp
#include <iostream>
#include "mgc_stream_internal.hpp"
using namespace std;


template <class T>
class Stream {
    private:
        using m_stream = __internal_stream<T>;
        m_stream m_originalCopy;
        m_stream m_mutableCopy;
    public:
        Stream() {

        }
                   Stream& of(T* array, size_t length);
        //mutators
                   Stream& sort();
                   Stream& sort(bool (*cmpFunc)(T a, T b));
                   Stream& filter(bool (*filterFunc)(T item));
                   Stream& map(T (*mapFunc)(T item));
                   Stream& forEach(void (*func)(T item));
                   Stream& unique();
                   Stream& skip(int n);
                   Stream& chop(int n);
        //Terminating methods
                       int count();
                       T min();
                       T max();
        std::pair<T,T> minmax();
        std::pair<T*,int> toArray();
};

template <class T>
Stream<T>& Stream<T>::of(T* arr, size_t length) {
    for (int i = 0; i < length; i++) {
        m_originalCopy.add(arr[i]);
        m_mutableCopy.add(arr[i]);
    }
    return *this;
}

template <class T>
Stream<T>& Stream<T>::filter(bool (*filterFunc)(T item)) {
    m_stream localNext;
    for (T info : m_mutableCopy) {
        if (filterFunc(info))
            localNext.add(info);
    }
    m_mutableCopy = localNext;
    return *this;
}

template <class T>
Stream<T>& Stream<T>::sort() {
    m_mutableCopy.sort();
    return *this;
}

template <class T>
Stream<T>& Stream<T>::sort(bool (*cmpFunc)(T a, T b)) {
    m_mutableCopy.sort(cmpFunc);
    return *this;
}

template <class T>
Stream<T>& Stream<T>::unique() {
    m_mutableCopy.make_unique();
    return *this;
}

template <class T>
std::pair<T*,int> Stream<T>::toArray() {
    T* arr = new T[m_mutableCopy.size()];
    int n = 0;
    for (T item : m_mutableCopy)
        arr[n++] = item;
    return make_pair(arr, n);
}

template <class T>
Stream<T>& Stream<T>::map(T (*mapFunc)(T info)) {
    m_stream transformed;
    for (T item : m_mutableCopy) {
        transformed.add(mapFunc(item));
    }
    m_mutableCopy = transformed;
    return *this;
}

template <class T>
Stream<T>& Stream<T>::skip(int n) {
    m_stream localNext;
    int i = 0;
    for (T val : m_mutableCopy) {
        if (i > n)
            localNext.add(val);
        i++;
    }
    m_mutableCopy = localNext;
    return *this;
}

template <class T>
Stream<T>& Stream<T>::chop(int n) {
    m_stream localNext;
    int i = 0;
    for (T val : m_mutableCopy) {
        if (i < m_mutableCopy.size() - n) {
            localNext.add(val);
        } else {
            break;
        }
        i++;
    }
    m_mutableCopy = localNext;
    return *this;
}

template <class T>
Stream<T>& Stream<T>::forEach(void(*func)(T item)) {
    for (T val : m_mutableCopy)
        func(val);
    return *this;
}

template <class T>
int Stream<T>::count() {
    return m_mutableCopy.size();
}

template <class T>
T Stream<T>::min() {
    return m_mutableCopy.min();
}

template <class T>
T Stream<T>::max() {
    return m_mutableCopy.max();
}

template <class T>
std::pair<T,T> Stream<T>::minmax() {
    return make_pair(m_mutableCopy.min(), m_mutableCopy.max());
}


#endif