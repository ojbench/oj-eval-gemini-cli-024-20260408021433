#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>

namespace sjtu {

template<class T>
class deque {
private:
    static const size_t BLOCK_SIZE = 512;
    T** map;
    size_t map_size;
    size_t start_block;
    size_t start_pos;
    size_t num_elements;

public:
    class const_iterator;
    class iterator {
        friend class deque;
        friend class const_iterator;
    private:
        deque* dq;
        size_t index;
    public:
        iterator(deque* dq = nullptr, size_t index = 0) : dq(dq), index(index) {}
        
        iterator operator+(const int &n) const {
            return iterator(dq, index + n);
        }
        iterator operator-(const int &n) const {
            return iterator(dq, index - n);
        }
        int operator-(const iterator &rhs) const {
            if (dq != rhs.dq) throw invalid_iterator();
            return (int)index - (int)rhs.index;
        }
        iterator operator+=(const int &n) {
            index += n;
            return *this;
        }
        iterator operator-=(const int &n) {
            index -= n;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            index++;
            return tmp;
        }
        iterator& operator++() {
            index++;
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            index--;
            return tmp;
        }
        iterator& operator--() {
            index--;
            return *this;
        }
        T& operator*() const {
            if (!dq || index >= dq->num_elements) throw invalid_iterator();
            size_t idx = dq->start_pos + index;
            size_t block = (dq->start_block + idx / BLOCK_SIZE) % dq->map_size;
            size_t p = idx % BLOCK_SIZE;
            return dq->map[block][p];
        }
        T* operator->() const noexcept {
            size_t idx = dq->start_pos + index;
            size_t block = (dq->start_block + idx / BLOCK_SIZE) % dq->map_size;
            size_t p = idx % BLOCK_SIZE;
            return &dq->map[block][p];
        }
        bool operator==(const iterator &rhs) const {
            return dq == rhs.dq && index == rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return dq == rhs.dq && index == rhs.index;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    class const_iterator {
        friend class deque;
        friend class iterator;
    private:
        const deque* dq;
        size_t index;
    public:
        const_iterator(const deque* dq = nullptr, size_t index = 0) : dq(dq), index(index) {}
        const_iterator(const iterator &other) : dq(other.dq), index(other.index) {}
        
        const_iterator operator+(const int &n) const {
            return const_iterator(dq, index + n);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(dq, index - n);
        }
        int operator-(const const_iterator &rhs) const {
            if (dq != rhs.dq) throw invalid_iterator();
            return (int)index - (int)rhs.index;
        }
        const_iterator operator+=(const int &n) {
            index += n;
            return *this;
        }
        const_iterator operator-=(const int &n) {
            index -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            index++;
            return tmp;
        }
        const_iterator& operator++() {
            index++;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            index--;
            return tmp;
        }
        const_iterator& operator--() {
            index--;
            return *this;
        }
        const T& operator*() const {
            if (!dq || index >= dq->num_elements) throw invalid_iterator();
            size_t idx = dq->start_pos + index;
            size_t block = (dq->start_block + idx / BLOCK_SIZE) % dq->map_size;
            size_t p = idx % BLOCK_SIZE;
            return dq->map[block][p];
        }
        const T* operator->() const noexcept {
            size_t idx = dq->start_pos + index;
            size_t block = (dq->start_block + idx / BLOCK_SIZE) % dq->map_size;
            size_t p = idx % BLOCK_SIZE;
            return &dq->map[block][p];
        }
        bool operator==(const iterator &rhs) const {
            return dq == rhs.dq && index == rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return dq == rhs.dq && index == rhs.index;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    deque() : map(nullptr), map_size(0), start_block(0), start_pos(0), num_elements(0) {}
    
    deque(const deque &other) : map(nullptr), map_size(0), start_block(0), start_pos(0), num_elements(0) {
        if (other.num_elements > 0) {
            map_size = 8;
            while (map_size * BLOCK_SIZE < other.num_elements) map_size *= 2;
            map = new T*[map_size]();
            for (size_t i = 0; i < other.num_elements; ++i) {
                push_back(other[i]);
            }
        }
    }
    
    ~deque() {
        clear();
        delete[] map;
    }
    
    deque &operator=(const deque &other) {
        if (this == &other) return *this;
        clear();
        for (size_t i = 0; i < other.num_elements; ++i) {
            push_back(other[i]);
        }
        return *this;
    }
    
    T & at(const size_t &pos) {
        if (pos >= num_elements) throw index_out_of_bound();
        size_t idx = start_pos + pos;
        size_t block = (start_block + idx / BLOCK_SIZE) % map_size;
        size_t p = idx % BLOCK_SIZE;
        return map[block][p];
    }
    const T & at(const size_t &pos) const {
        if (pos >= num_elements) throw index_out_of_bound();
        size_t idx = start_pos + pos;
        size_t block = (start_block + idx / BLOCK_SIZE) % map_size;
        size_t p = idx % BLOCK_SIZE;
        return map[block][p];
    }
    T & operator[](const size_t &pos) {
        if (pos >= num_elements) throw index_out_of_bound();
        size_t idx = start_pos + pos;
        size_t block = (start_block + idx / BLOCK_SIZE) % map_size;
        size_t p = idx % BLOCK_SIZE;
        return map[block][p];
    }
    const T & operator[](const size_t &pos) const {
        if (pos >= num_elements) throw index_out_of_bound();
        size_t idx = start_pos + pos;
        size_t block = (start_block + idx / BLOCK_SIZE) % map_size;
        size_t p = idx % BLOCK_SIZE;
        return map[block][p];
    }
    
    const T & front() const {
        if (num_elements == 0) throw container_is_empty();
        return map[start_block][start_pos];
    }
    const T & back() const {
        if (num_elements == 0) throw container_is_empty();
        size_t end_idx = start_pos + num_elements - 1;
        size_t end_block = (start_block + end_idx / BLOCK_SIZE) % map_size;
        size_t end_pos = end_idx % BLOCK_SIZE;
        return map[end_block][end_pos];
    }
    
    iterator begin() { return iterator(this, 0); }
    const_iterator cbegin() const { return const_iterator(this, 0); }
    iterator end() { return iterator(this, num_elements); }
    const_iterator cend() const { return const_iterator(this, num_elements); }
    
    bool empty() const { return num_elements == 0; }
    size_t size() const { return num_elements; }
    
    void clear() {
        if (num_elements == 0) return;
        for (size_t i = 0; i < num_elements; ++i) {
            size_t idx = start_pos + i;
            size_t block = (start_block + idx / BLOCK_SIZE) % map_size;
            size_t p = idx % BLOCK_SIZE;
            map[block][p].~T();
        }
        for (size_t i = 0; i < map_size; ++i) {
            if (map[i]) {
                operator delete(map[i]);
                map[i] = nullptr;
            }
        }
        num_elements = 0;
        start_block = 0;
        start_pos = 0;
    }
    
    iterator insert(iterator pos, const T &value) {
        if (pos.dq != this || pos.index > num_elements) throw invalid_iterator();
        size_t idx = pos.index;
        if (idx == 0) {
            push_front(value);
        } else if (idx == num_elements) {
            push_back(value);
        } else if (idx < num_elements / 2) {
            push_front(front());
            for (size_t i = 0; i < idx; ++i) {
                (*this)[i] = (*this)[i + 1];
            }
            (*this)[idx] = value;
        } else {
            push_back(back());
            for (size_t i = num_elements - 1; i > idx; --i) {
                (*this)[i] = (*this)[i - 1];
            }
            (*this)[idx] = value;
        }
        return iterator(this, idx);
    }
    
    iterator erase(iterator pos) {
        if (pos.dq != this || pos.index >= num_elements) throw invalid_iterator();
        size_t idx = pos.index;
        if (idx == 0) {
            pop_front();
        } else if (idx == num_elements - 1) {
            pop_back();
        } else if (idx < num_elements / 2) {
            for (size_t i = idx; i > 0; --i) {
                (*this)[i] = (*this)[i - 1];
            }
            pop_front();
        } else {
            for (size_t i = idx; i < num_elements - 1; ++i) {
                (*this)[i] = (*this)[i + 1];
            }
            pop_back();
        }
        return iterator(this, idx);
    }
    
    void push_back(const T &value) {
        if (num_elements == 0) {
            if (map_size == 0) {
                map_size = 8;
                map = new T*[map_size]();
            }
            start_block = 0;
            start_pos = 0;
            if (!map[start_block]) map[start_block] = (T*)operator new(BLOCK_SIZE * sizeof(T));
            new (&map[start_block][start_pos]) T(value);
            num_elements++;
            return;
        }
        size_t end_idx = start_pos + num_elements;
        size_t end_block = (start_block + end_idx / BLOCK_SIZE) % map_size;
        size_t end_pos = end_idx % BLOCK_SIZE;

        size_t active_blocks = (start_pos + num_elements + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (end_pos == 0 && active_blocks == map_size) {
            size_t new_map_size = map_size * 2;
            T** new_map = new T*[new_map_size]();
            for (size_t i = 0; i < active_blocks; ++i) {
                new_map[i] = map[(start_block + i) % map_size];
            }
            delete[] map;
            map = new_map;
            start_block = 0;
            map_size = new_map_size;
            end_block = active_blocks;
        }

        if (!map[end_block]) {
            map[end_block] = (T*)operator new(BLOCK_SIZE * sizeof(T));
        }
        new (&map[end_block][end_pos]) T(value);
        num_elements++;
    }
    
    void pop_back() {
        if (num_elements == 0) throw container_is_empty();
        size_t end_idx = start_pos + num_elements - 1;
        size_t end_block = (start_block + end_idx / BLOCK_SIZE) % map_size;
        size_t end_pos = end_idx % BLOCK_SIZE;
        map[end_block][end_pos].~T();
        num_elements--;
        if (num_elements == 0) {
            operator delete(map[start_block]);
            map[start_block] = nullptr;
            start_pos = 0;
        } else {
            if (end_pos == 0) {
                operator delete(map[end_block]);
                map[end_block] = nullptr;
            }
        }
    }
    
    void push_front(const T &value) {
        if (num_elements == 0) {
            push_back(value);
            return;
        }
        if (start_pos == 0) {
            size_t active_blocks = (start_pos + num_elements + BLOCK_SIZE - 1) / BLOCK_SIZE;
            if (active_blocks == map_size) {
                size_t new_map_size = map_size * 2;
                T** new_map = new T*[new_map_size]();
                for (size_t i = 0; i < active_blocks; ++i) {
                    new_map[i] = map[(start_block + i) % map_size];
                }
                delete[] map;
                map = new_map;
                start_block = 0;
                map_size = new_map_size;
            }
            start_block = (start_block + map_size - 1) % map_size;
            start_pos = BLOCK_SIZE - 1;
        } else {
            start_pos--;
        }
        if (!map[start_block]) {
            map[start_block] = (T*)operator new(BLOCK_SIZE * sizeof(T));
        }
        new (&map[start_block][start_pos]) T(value);
        num_elements++;
    }
    
    void pop_front() {
        if (num_elements == 0) throw container_is_empty();
        map[start_block][start_pos].~T();
        num_elements--;
        if (num_elements == 0) {
            operator delete(map[start_block]);
            map[start_block] = nullptr;
            start_pos = 0;
        } else {
            start_pos++;
            if (start_pos == BLOCK_SIZE) {
                operator delete(map[start_block]);
                map[start_block] = nullptr;
                start_block = (start_block + 1) % map_size;
                start_pos = 0;
            }
        }
    }
};

}

#endif