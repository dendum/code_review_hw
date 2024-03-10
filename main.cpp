#ifndef CODEREVIEWTASK_MYVECTOR_HPP
#define CODEREVIEWTASK_MYVECTOR_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

/*
 * MyVector stores a collection of objects with their names.
 *
 * For each object T, MyVector stores T`s name as std::string.
 * Several objects can have similar name.
 * operator[](const std::string& name) should return the first object
 * with the given name.
 *
 * Your task is to find as many mistakes and drawbacks in this code
 * (according to the presentation) as you can.
 * Annotate these mistakes with comments.
 *
 * Once you have found all the mistakes, rewrite the code
 * so it would not change its original purpose
 * and it would contain no mistakes.
 * Try to make the code more efficient without premature optimization.
 *
 * You can change MyVector interface completely, but there are several rules:
 * 1) you should correctly and fully implement copy-on-write idiom.
 * 2) std::pair<const T&, const std::string&> operator[](int index) const must take constant time at worst.
 * 3) const T& operator[](const std::string& name) const should be present.
 * 4) both operator[] should have non-const version.
 * 5) your implementation should provide all the member types of std::vector.
 * 6) your implementation should provide the following functions:
 *    1) begin(), cbegin(), end(), cend()
 *    2) empty(), size()
 *    3) reserve(), clear()
 */

// Major code issues
// Inherit from std::vector<T>. Inheriting from standard library containers is not recommended.
// These classes are not designed for inheritance and have no virtual destructors.
// Actions in the copy_names() function can cause a memory leak.
// new std::out_of_range(); new std::invalid_argument(); throws pointers to exceptions, but should be thrown by value.
// Implementation of push_back(). It must ensure that the object is the only one referencing the data, otherwise a new copy of the data must be created.

template<typename T>
class MyVector {
public:
    using iter_type = typename std::vector<T>::iterator;
    using const_iter_type = typename std::vector<T>::const_iterator;

    MyVector() : m_ref_count(new size_t(1)), m_values(new std::vector<T>), m_names(new std::vector<std::string>) {}

    MyVector(const MyVector &other)
            : m_ref_count(other.m_ref_count), m_values(other.m_values), m_names(other.m_names) {
        (*m_ref_count)++;
    }

    ~MyVector() {
        if (--(*m_ref_count) == 0) {
            delete m_ref_count;
            delete m_values;
            delete m_names;
        }
    }

    void push_back(const T &value, const std::string &name) {
        detach();
        m_values->push_back(value);
        m_names->push_back(name);
    }

    std::pair<T &, std::string &> operator[](size_t index) {
        if (index >= m_values->size()) {
            throw std::out_of_range("Index is out of range");
        }
        detach();
        return {*(*m_values)[index], (*m_names)[index]};
    }

    std::pair<const T &, const std::string &> operator[](size_t index) const {
        if (index >= m_values->size()) {
            throw std::out_of_range("Index is out of range");
        }
        return {(*m_values)[index], (*m_names)[index]};
    }

    T &operator[](const std::string &name) {
        return const_cast<T &>(static_cast<const MyVector *>(this)->operator[](name));
    }

    const T &operator[](const std::string &name) const {
        auto iter = std::find(m_names->cbegin(), m_names->cend(), name);
        if (iter == m_names->cend()) {
            throw std::invalid_argument(name + " is not found in the MyVector");
        }
        return (*m_values)[std::distance(m_names->cbegin(), iter)];
    }

    iter_type begin() {
        detach();
        return m_values->begin();
    }

    const_iter_type cbegin() const { return m_values->cbegin(); }

    iter_type end() {
        detach();
        return m_values->end();
    }

    const_iter_type cend() const { return m_values->cend(); }

    bool empty() const { return m_values->empty(); }

    std::size_t size() const { return m_values->size(); }

    void reserve(std::size_t n) {
        m_values->reserve(n);
        m_names->reserve(n);
    }

    void clear() {
        detach();
        m_values->clear();
        m_names->clear();
    }

private:
    void detach() {
        if (*m_ref_count > 1) {
            --(*m_ref_count);
            m_ref_count = new size_t(1);
            m_values = new std::vector<T>(*m_values);
            m_names = new std::vector<std::string>(*m_names);
        }
    }

    std::vector<T> *m_values;
    std::vector<std::string> *m_names;
    size_t *m_ref_count;
};


#endif //CODEREVIEWTASK_MYVECTOR_HPP


