// forward_list_demo.cpp
// Переработанная односвязная коллекция с корректными итераторами.
// Требует стандарт C++17.

#include <iostream>
#include <memory>
#include <iterator>
#include <utility>

template <typename T>
class ForwardList
{
    struct Node
    {
        T data;
        std::unique_ptr<Node> next{nullptr};

        explicit Node(const T& value) : data(value) {}
        explicit Node(T&& value)      : data(std::move(value)) {}
    };

    std::unique_ptr<Node> head_{nullptr};
    Node* tail_{nullptr};

public:
    ForwardList() = default;
    ~ForwardList() = default;

    ForwardList(const ForwardList& other)
    {
        for (const auto& v : other)
            push_back(v);
    }
    ForwardList& operator=(const ForwardList& other)
    {
        if (this != &other)
        {
            clear();
            for (const auto& v : other)
                push_back(v);
        }
        return *this;
    }
    ForwardList(ForwardList&&) noexcept = default;
    ForwardList& operator=(ForwardList&&) noexcept = default;


    void push_back(const T& value) { append_node(std::make_unique<Node>(value)); }
    void push_back(T&& value)      { append_node(std::make_unique<Node>(std::move(value))); }

    bool remove(const T& value)
    {
        Node* prev = nullptr;
        Node* curr = head_.get();

        while (curr)
        {
            if (curr->data == value)
            {
                if (prev)            
                {
                    prev->next = std::move(curr->next);
                    if (!prev->next) tail_ = prev;
                }
                else                 
                {
                    head_ = std::move(curr->next);
                    if (!head_) tail_ = nullptr;
                }
                return true;
            }
            prev = curr;
            curr = curr->next.get();
        }
        return false;
    }

    bool contains(const T& value) const
    {
        for (const auto& v : *this)
            if (v == value)
                return true;
        return false;
    }

    void clear() { head_.reset(); tail_ = nullptr; }


    template <typename Value, typename Pointer, typename Reference>
    class basic_iterator
    {
        friend ForwardList;
        typename ForwardList<T>::Node* node_{nullptr};

        explicit basic_iterator(Node* n) noexcept : node_(n) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Value;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pointer;
        using reference         = Reference;

        basic_iterator() = default;

        reference operator*()  const { return node_->data; }
        pointer   operator->() const { return &node_->data; }

        basic_iterator& operator++()
        {
            node_ = node_->next.get();
            return *this;
        }
        basic_iterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(basic_iterator a, basic_iterator b) { return a.node_ == b.node_; }
        friend bool operator!=(basic_iterator a, basic_iterator b) { return !(a == b); }
    };

    using iterator       = basic_iterator<T,       T*,       T&>;
    using const_iterator = basic_iterator<const T, const T*, const T&>;

    iterator       begin() noexcept { return iterator(head_.get()); }
    iterator       end()   noexcept { return iterator(nullptr); }

    const_iterator begin()  const noexcept { return const_iterator(head_.get()); }
    const_iterator end()    const noexcept { return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend()   const noexcept { return end(); }


    friend std::ostream& operator<<(std::ostream& os, const ForwardList& fl)
    {
        for (const auto& v : fl) os << v << ' ';
        return os;
    }

private:
    void append_node(std::unique_ptr<Node> n)
    {
        Node* raw = n.get();
        if (!head_)
        {
            head_ = std::move(n);
            tail_ = head_.get();
        }
        else
        {
            tail_->next = std::move(n);
            tail_ = raw;
        }
    }
};


int main()
{
    ForwardList<int> list;
    for (int i = 0; i < 7; ++i) list.push_back(i);

    std::cout << "Initial : " << list << '\n';

    list.remove(3);
    std::cout << "Remove 3: " << list << '\n';

    list.remove(6);
    std::cout << "Remove 6: " << list << '\n';

    list.push_back(42);
    std::cout << "Push 42 : " << list << '\n';

    std::cout << "Contains 6 " << list.contains(6) << '\n';
}
