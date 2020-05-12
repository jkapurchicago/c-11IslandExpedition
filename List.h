#ifndef LIST_H
#define LIST_H

#include <utility>
#include <stdexcept>
#include <iostream>

struct IllegalOperationOnCurrentState : std::logic_error {
    using std::logic_error::logic_error;
};

template<typename T>
class List {
    struct Node {
        T data;
        Node* next, * prev;

        template<typename... Args>
        explicit Node(Args&&... args)
        : data(std::forward<Args>(args)...), next(nullptr), prev(nullptr) {}
    };


    template <class U>
    class ListIterator {
    public:
        typedef U value_type;
        typedef U& reference;
        typedef U* pointer;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef void difference_type;

        explicit ListIterator(Node* p = nullptr): ptr(p) {}

        ListIterator& operator=(const ListIterator& it) {
            ptr = it.ptr;
            return *this;
        }
        bool operator==(const ListIterator& it) const {
            return ptr == it.ptr;
        }
        bool operator!=(const ListIterator& it) const {
            return ptr != it.ptr;
        }
        reference operator*() const {
            return ptr->data;
        }
        pointer operator->() const { return &(ptr->data);}

    protected:
        Node* ptr;
    };


    Node* head, *tail;
    size_t count;

    void append(Node* n) {
        if (head == nullptr)
            head = n;
        else {
            n->prev = tail;
            tail->next = n;
        }
        tail = n;
        ++count;
    }

    void prepend(Node* n) {
        if (head == nullptr)
            tail = n;
        else {
            n->next = head;
            head->prev = n;
        }
        head = n;
        ++count;
    }

    T& get(size_t idx) const {
        if (idx >= count)
            throw std::out_of_range("In List::operator[]");
        Node* temp;
        if (idx < count/2) {
            temp = head;
            for (size_t j = 0; j < idx; ++j)
                temp = temp->next;
        } else{
            temp = tail;
            for (size_t j = count - 1; j > idx; --j)
                temp = temp->prev;
        }
        return temp->data;
    }

    void copy(const List& other) {
        Node* temp = other.head;
        Node* temp2 = head = new Node(temp->data);

        while ((temp = temp->next)) {
            Node* n = new Node(temp->data);
            temp2->next = n;
            n->prev = temp2;
            temp2 = n;
        }
        tail = temp2;
        count = other.count;
    }

    void move(List&& other) {
        head = other.head; other.head = nullptr;
        tail = other.tail; other.tail = nullptr;
        count = other.count; other.count = 0;
    }

    Node* search(const T& val) const {
        Node* cur = head;
        while (cur) {
            if (cur->data == val)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    void remove(Node* n) {
        if (head) {
            if (head == n)
                head = head->next;
            if (tail == n)
                tail = tail->prev;
            if (n->next != nullptr)
                n->next->prev = n->prev;
            if (n->prev != nullptr)
                n->prev->next = n->next;
            delete n;
            --count;
        }
    }


public:
    using value_type = T;
    using size_type = size_t;


    class iterator : public ListIterator<T> {
    public:
        explicit iterator(Node* p = nullptr): ListIterator<T>(p) {}
        iterator& operator++() {this->ptr = this->ptr->next; return *this;}
        iterator operator++(int) {iterator it(this->ptr); ++*this; return it;}
        iterator& operator--() {this->ptr = this->ptr->prev; return *this;}
        iterator operator--(int) {iterator it(this->ptr); --*this; return it;}
    };
    class const_iterator : public ListIterator<const T>{
    public:
        explicit const_iterator(Node* p = nullptr): ListIterator<const T>(p) {}
        const_iterator(const iterator& it): ListIterator<const T>(it.ptr) {}
        const_iterator& operator++() {this->ptr = this->ptr->next; return *this;}
        const_iterator operator++(int) {const_iterator it(this->ptr); ++*this; return it;}
        const_iterator& operator--() {this->ptr = this->ptr->prev; return *this;}
        const_iterator operator--(int) {const_iterator it(this->ptr); --*this; return it;}
    };
    class reverse_iterator : public ListIterator<T> {
    public:
        explicit reverse_iterator(Node* p = nullptr): ListIterator<T>(p) {}
        reverse_iterator& operator++() {this->ptr = this->ptr->prev; return *this;}
        reverse_iterator operator++(int) {reverse_iterator it(this->ptr); ++*this; return it;}
        reverse_iterator& operator--() {this->ptr = this->ptr->next; return *this;}
        reverse_iterator operator--(int) {reverse_iterator it(this->ptr); --*this; return it;}
    };
    class const_reverse_iterator : public ListIterator<const T> {
    public:
        explicit const_reverse_iterator(Node* p = nullptr): ListIterator<const T>(p) {}
        const_reverse_iterator& operator++() {this->ptr = this->ptr->prev; return *this;}
        const_reverse_iterator operator++(int) {const_reverse_iterator it(this->ptr); ++*this; return it;}
        const_reverse_iterator& operator--() {this->ptr = this->ptr->next; return *this;}
        const_reverse_iterator operator--(int) {const_reverse_iterator it(this->ptr); --*this; return it;}
    };
    

    List() noexcept : head(nullptr), tail(nullptr), count(0) {}

    List(const List& other) {
        copy(other);
    }

    List(List&& other) noexcept {
        move(std::forward<List>(other));
    }

    List& operator=(const List& other) {
        clear();
        copy(other);
    }

    List& operator=(List&& other) noexcept {
        clear();
        move(std::forward<List>(other));
    }

    ~List() { clear(); }

    void push_back(const T& val) {
        append(new Node(val));
    }

    void push_back(T&& val) {
        append(new Node(std::forward<T>(val)));
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        append(new Node(std::forward<Args>(args)...));
    }

    void push_front(const T& val) {
        prepend(new Node(val));
    }

    void push_front(T&& val) {
        prepend(new Node(std::forward<T>(val)));
    }

    void pop_back() {
        remove(tail);
    }

    void pop_front(){
        remove(head);
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        prepend(new Node(std::forward<Args>(args)...));
    }

    [[nodiscard]] bool empty() const { return count == 0;}

    T& front() {
        if (head)
            return head->data;
        throw IllegalOperationOnCurrentState("In List::front. List is empty");
    }

    const T& front() const {
        if (head)
            return head->data;
        throw IllegalOperationOnCurrentState("In List::front. List is empty");
    }

    T& back() {
        if (tail)
            return tail->data;
        throw IllegalOperationOnCurrentState("In List::tail. List is empty");
    }

    const T& back() const {
        if (head)
            return head->data;
        throw IllegalOperationOnCurrentState("In List::tail. List is empty");
    }

    [[nodiscard]] size_t size() const { return count; }

    [[nodiscard]] T& operator[](size_t idx) { return get(idx); }

    [[nodiscard]] const T& operator[](size_t idx) const { return get(idx); }

    bool remove(const T& val) {
        Node* n = search(val);
        if (n) {
            remove(n);
            return true;
        }
        return false;
    }

    [[nodiscard]] bool contains(const T& val) const {
        return search(val) != nullptr;
    }

    void clear() {
        while (head) {
            Node* next = head->next;
            delete head;
            head = next;
        }
        tail = nullptr;
        count = 0;
    }

    iterator begin() {
        return iterator(head);
    }

    const_iterator begin() const {
        return const_iterator(head);
    }
    iterator end() {
        return iterator();
    }

    const_iterator end() const {
        return const_iterator();
    }

    const_iterator cbegin() const {
        return const_iterator(head);
    }

    const_iterator cend() const {
        return const_iterator();
    }
    reverse_iterator rbegin() {
        return reverse_iterator(tail);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(tail);
    }

    reverse_iterator rend() {
        return reverse_iterator();
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator();
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(tail);
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator();
    }

    template<typename Ch, typename Tr>
    friend std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const List& list) {
        os << '[';
        if (list.head) {
            os << list.head->data;

            Node* cur = list.head->next;
            while (cur) {
                os << ", " << cur->data;
                cur = cur->next;
            }
        }
        return os << ']';
    }
};



#endif //LIST_H
