// forward_list_demo.cpp
// Односвязный список с корректными итераторами.
// Требует стандарт C++17.

#include <iostream>
#include <memory>
#include <iterator>
#include <utility>
#include <string>

// -------------------------- Пример пользовательского типа -------------------
struct Item
{
    int         id{};
    std::string name{};

    // Переопределяем оператор равенства — ForwardList им пользуется
    bool operator==(const Item& other) const noexcept
    {
        return id == other.id && name == other.name;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Item& item)
{
    return os << '{' << item.id << ", " << item.name << '}';
}
// ----------------------------------------------------------------------------

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
    ForwardList()  = default;
    ~ForwardList() = default;

    ForwardList(const ForwardList& other) { for (const auto& v : other) push_back(v); }
    ForwardList& operator=(const ForwardList& other)
    {
        if (this != &other)
        {
            clear();
            for (const auto& v : other) push_back(v);
        }
        return *this;
    }
    ForwardList(ForwardList&&)            noexcept = default;
    ForwardList& operator=(ForwardList&&) noexcept = default;

    // ----------------------------- Базовые операции --------------------------
    void push_back(const T& value) { append_node(std::make_unique<Node>(value)); }
    void push_back(T&& value)      { append_node(std::make_unique<Node>(std::move(value))); }

    /**
     * Удаляет все элементы, равные value.
     * Возвращает true, если хотя бы один элемент был стёрт.
     */
    bool remove(const T& value)
    {
        bool erased = false;

        // 1. Чистим возможную «голову»-цепочку совпадений
        while (head_ && head_->data == value)
        {
            head_ = std::move(head_->next);
            erased = true;
        }
        if (!head_)                        // список мог опустеть
        {
            tail_ = nullptr;
            return erased;
        }

        // 2. Проходим остаток
        Node* prev = head_.get();
        Node* curr = prev->next.get();
        while (curr)
        {
            if (curr->data == value)
            {
                prev->next = std::move(curr->next);
                erased = true;
                curr = prev->next.get();   // перескакиваем не меняя prev
            }
            else
            {
                prev = curr;
                curr = curr->next.get();
            }
        }
        tail_ = prev;                      // prev указывает на последний узел
        return erased;
    }

    /** Удаляет все элементы, равные тому, на который указывает pos. */
    bool remove(typename ForwardList<T>::basic_iterator<T, T*, T&> pos)
    {
        if (pos == end()) return false;
        return remove(*pos);
    }

    bool contains(const T& value) const
    {
        for (const auto& v : *this)
            if (v == value) return true;
        return false;
    }

    void clear() { head_.reset(); tail_ = nullptr; }

    // ----------------------------- Итераторы ---------------------------------
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

        basic_iterator& operator++()        { node_ = node_->next.get(); return *this; }
        basic_iterator  operator++(int)     { auto tmp = *this; ++(*this); return tmp; }

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

    // ----------------------------- Отладочный вывод --------------------------
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

// ----------------------------- Демонстрация ----------------------------------
int main()
{
    ForwardList<Item> list;
    list.push_back({1, "Alice"});
    list.push_back({2, "Bob"});
    list.push_back({1, "Alice"});
    list.push_back({3, "Carol"});
    list.push_back({2, "Bob"});

    std::cout << "Initial          : " << list << '\n';

    // пример 1 — передаём объект напрямую
    list.remove(Item{2, "Bob"});
    std::cout << "After remove(2)  : " << list << '\n';

    // пример 2 — берём образец через итератор
    auto it = list.begin();          // указывает на {1,"Alice"}
    list.remove(it);                 // удалит все {1,"Alice"}
    std::cout << "After remove(*it): " << list << '\n';
}