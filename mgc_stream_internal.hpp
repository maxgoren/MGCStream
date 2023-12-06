#ifndef mgc_stream_internal_hpp
#define mgc_stream_internal_hpp
#include <iostream>
using namespace std;

template <class T>
struct __boxed_val {
    T m_val;
    __boxed_val *next;
    __boxed_val(T val, __boxed_val* nextval) : m_val(val), next(nextval) { }
    __boxed_val(T val) : m_val(val), next(nullptr) { }
    __boxed_val() {
        next = nullptr;
    }
    bool operator<(const __boxed_val& o) const {
        return m_val < o.m_val;
    }
    bool operator==(const __boxed_val& o) const {
        return m_val = o.m_val;
    }
    bool operator!=(const __boxed_val& o) const {
        return !(*this==o);
    }
};

template <class T>
class __boxed_val_itr {
    private:
        __boxed_val<T>* m_curr;
    public:
        __boxed_val_itr(__boxed_val<T>* c) : m_curr(c) { }
        T& operator*() {
            return m_curr->m_val;
        }
        __boxed_val_itr& operator++() {
            m_curr = m_curr->next;
            return *this;
        }
        __boxed_val_itr& operator++(int) {
            __boxed_val_itr tmp = *this;
            ++*this;
            return tmp;
        }
        bool operator==(const __boxed_val_itr& o) {
            return m_curr == o.m_curr;
        }
        bool operator!=(const __boxed_val_itr& o) {
            return !(*this==o);
        }
};

template <class T>
class __internal_stream {
    private:
        using val_node = __boxed_val<T>*;
        using iterator = __boxed_val_itr<T>;
        val_node m_head, m_tail;
        int m_node_count;
        val_node m_merge(val_node a, val_node b, bool (*cmp)(T a, T b)) {
            __boxed_val<T> d; val_node c = &d;
            while (a && b) {
                if (cmp(a->m_val, b->m_val)) {
                    c->next = a; a = a->next; c = c->next;
                } else {
                    c->next = b; b = b->next; c = c->next;
                }
            }
            c->next = (a == nullptr) ? b:a;
            return d.next;
        }
        val_node m_sort(val_node a, bool (*cmp)(T a, T b)) {
            if (a == nullptr || a->next == nullptr)
                return a;
            val_node fast = a->next, slow = a;
            while (fast && fast->next) {
                slow = slow->next;
                fast = fast->next->next;
            }
            val_node back = slow->next;
            slow->next = nullptr;
            return m_merge(m_sort(a, cmp), m_sort(back, cmp), cmp);
        }
        T m_minVal;
        T m_maxVal;
    public:
        __internal_stream() {
            m_head = nullptr;
            m_tail = nullptr;
            m_node_count = 0;
        }
        __internal_stream(const __internal_stream& o) {
            m_head = nullptr;
            m_tail = nullptr;
            m_node_count = 0;
            for (T val : o) {
                add(val);
            }
        }
        ~__internal_stream() {
            while (m_head) {
                val_node tmp = m_head;
                m_head = m_head->next;
                tmp->next = nullptr;
                delete tmp;
            }
        }
        int size() {
            return m_node_count;
        }
        void add(T val) {
            val_node t = new __boxed_val<T>(val);
            if (size() == 0) {
                m_maxVal = val;
                m_minVal = val;
                m_head = t;
                m_tail = t;
            } else {
                m_tail->next = t;
                m_tail = t;
                if (val < m_minVal) m_minVal = val;
                if (val > m_maxVal) m_maxVal = val;
            }
            m_node_count = m_node_count + 1;
        }
        void make_unique() {
            m_head = m_sort(m_head, [](T a, T b) { return b > a; });
            val_node it = m_head;
            while (it) {
                if (it->next && it->m_val == it->next->m_val) {
                    val_node t = it->next;
                    it->next = t->next;
                    t->next = nullptr; delete t;
                    m_node_count = m_node_count - 1;
                } else {
                    it = it->next;
                }
            }
        }
        T min() {
            return m_minVal;
        }
        T max() {
            return m_maxVal;
        }
        void sort() {
            m_head = m_sort(m_head, [](T a, T b) { return b > a; });
        }
        void sort(bool (*cmp)(T a, T b)) {
            m_head = m_sort(m_head, cmp);
        }
        iterator begin() {
            return __boxed_val_itr(m_head);
        }
        iterator end() {
            return __boxed_val_itr<T>(nullptr);
        }
        const iterator begin() const {
            return __boxed_val_itr(m_head);
        }
        const iterator end() const {
            return __boxed_val_itr<T>(nullptr);
        }
        __internal_stream& operator=(const __internal_stream& o) {
            m_head = nullptr;
            m_tail = nullptr;
            m_node_count = 0;
            for (T val : o) {
                add(val);
            }
            return *this;
        }
};

#endif