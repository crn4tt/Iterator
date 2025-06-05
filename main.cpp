#include <iostream>
#include <string>
#include <iterator>

template <typename T>
class ForwardList {
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& v) : data(v), next(nullptr) {}
        explicit Node(T&& v)      : data(std::move(v)), next(nullptr) {}
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;

public:
    ForwardList() = default;
    ~ForwardList() { clear(); }

    ForwardList(const ForwardList& other) { for (const auto& v : other) push_back(v); }
    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) { clear(); for (const auto& v : other) push_back(v); }
        return *this;
    }
    ForwardList(ForwardList&& other) noexcept : head_(other.head_), tail_(other.tail_) { other.head_ = other.tail_ = nullptr; }
    ForwardList& operator=(ForwardList&& other) noexcept {
        if (this != &other) { clear(); head_ = other.head_; tail_ = other.tail_; other.head_ = other.tail_ = nullptr; }
        return *this;
    }

    void push_back(const T& v)  { append(new Node(v)); }
    void push_back(T&& v)       { append(new Node(std::move(v))); }

    template <typename V, typename P, typename R>
    class basic_iterator {
        Node* node_ = nullptr;
        explicit basic_iterator(Node* n) : node_(n) {}
        friend ForwardList;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = V;
        using difference_type   = std::ptrdiff_t;
        using pointer           = P;
        using reference         = R;

        basic_iterator() = default;
        reference operator*()  const { return node_->data; }
        pointer   operator->() const { return &node_->data; }
        basic_iterator& operator++() { node_ = node_ ? node_->next : nullptr; return *this; }
        basic_iterator  operator++(int) { auto tmp = *this; ++(*this); return tmp; }
        friend bool operator==(basic_iterator a, basic_iterator b) { return a.node_ == b.node_; }
        friend bool operator!=(basic_iterator a, basic_iterator b) { return a.node_ != b.node_; }
    };

    using iterator       = basic_iterator<T,       T*,       T&>;
    using const_iterator = basic_iterator<const T, const T*, const T&>;

    iterator begin() noexcept { return iterator(head_); }
    iterator end()   noexcept { return iterator(nullptr); }
    const_iterator begin()  const noexcept { return const_iterator(head_); }
    const_iterator end()    const noexcept { return const_iterator(nullptr); }

    bool remove(const T& value) {
        bool erased = false;
        while (head_ && head_->data == value) { Node* t = head_; head_ = head_->next; delete t; erased = true; }
        if (!head_) { tail_ = nullptr; return erased; }
        Node* prev = head_;
        Node* cur  = head_->next;
        while (cur) {
            if (cur->data == value) {
                prev->next = cur->next;
                if (cur == tail_) tail_ = prev;
                delete cur;
                cur = prev->next;
                erased = true;
            } else {
                prev = cur;
                cur  = cur->next;
            }
        }
        return erased;
    }

    bool remove(iterator pos) { return pos == end() ? false : remove(*pos); }

    void clear() {
        while (head_) { Node* t = head_; head_ = head_->next; delete t; }
        tail_ = nullptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const ForwardList& fl) {
        for (const auto& v : fl) os << v << ' ';
        return os;
    }

private:
    void append(Node* n) {
        if (!head_) head_ = tail_ = n;
        else { tail_->next = n; tail_ = n; }
    }
};

struct Item {
    int id{};
    std::string name{};
    bool operator==(const Item& o) const { return id == o.id && name == o.name; }
};

std::ostream& operator<<(std::ostream& os, const Item& it) { return os << '{' << it.id << ", " << it.name << '}'; }

int main() {
    ForwardList<Item> list;
    list.push_back({1, "Alice"});
    list.push_back({2, "Bob"});
    list.push_back({1, "Alice"});
    list.push_back({3, "Carol"});
    list.push_back({2, "Bob"});

    std::cout << "Start:  " << list << '\n';

    auto it = list.begin();
    list.remove(it);

    std::cout << "After:  " << list << '\n';
}