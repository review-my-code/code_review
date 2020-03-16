#include <iostream>
#include <vector>
#include <string>
#include <tuple>


template <class...>
class Zip_implementation;

template <size_t... Indices, class... Containers>
class Zip_implementation<std::index_sequence<Indices...>, Containers...> {
    std::tuple<Containers&...> refs;

    template <class... Args>
    class Iterator {
    protected:
        template <class T>
        using Proper_iterator = std::conditional_t<std::is_const_v<T>, typename T::const_iterator, typename T::iterator>;

        std::tuple<Proper_iterator<Args>...> iterators;
    public:
        Iterator(std::tuple<Proper_iterator<Args>...>&& iterators_)
            :iterators{ std::move(iterators_) } {}

        std::tuple<decltype(*get<Indices>(iterators))&...> operator*() noexcept {
            return { *get<Indices>(iterators)... };
        }
        std::tuple<const typename Args::value_type&...> operator*() const noexcept {
            return { *get<Indices>(iterators)... };
        }

        Iterator& operator++() {
            (++get<Indices>(iterators), ...);
            return *this;
        }
        Iterator& operator--() {
            (--get<Indices>(iterators), ...);
            return *this;
        }

        Iterator operator+(const size_t offset) {
            return std::tuple{ (get<Indices>(iterators) + offset)... };
        }
        Iterator operator-(const size_t offset) {
            return std::tuple{ (get<Indices>(iterators) - offset)... };
        }

        Iterator& operator+=(const size_t offset) {
            return *this = *this + offset;
        }
        Iterator& operator-=(const size_t offset) {
            return *this = *this - offset;
        }

        bool operator==(const Iterator& other) const noexcept {
            return ((get<Indices>(iterators) == get<Indices>(other.iterators)) && ...);
        }
        bool operator!=(const Iterator& other) const noexcept {
            return ((get<Indices>(iterators) != get<Indices>(other.iterators)) && ...);
        }
    };

    template <class... Args>
    class Reverse_iterator : public Iterator<Args...> {
    public:
        using Iterator<Args...>::Iterator, Iterator<Args...>::iterators;

        Reverse_iterator& operator++() {
            (--get<Indices>(iterators), ...);
            return *this;
        }
        Reverse_iterator& operator--() {
            (++get<Indices>(iterators), ...);
            return *this;
        }

        Reverse_iterator operator+(const size_t offset) {
            return std::tuple{ (get<Indices>(iterators) - offset)... };
        }
        Reverse_iterator operator-(const size_t offset) {
            return std::tuple{ (get<Indices>(iterators) + offset)... };
        }

        Reverse_iterator& operator+=(const size_t offset) {
            return *this = *this - offset;
        }
        Reverse_iterator& operator-=(const size_t offset) {
            return *this = *this + offset;
        }
    };
public:
    using               iterator =         Iterator<      Containers...>;
    using         const_iterator =         Iterator<const Containers...>;

    using       reverse_iterator = Reverse_iterator<      Containers...>;
    using const_reverse_iterator = Reverse_iterator<const Containers...>;

    Zip_implementation(Containers&... args) noexcept
        :refs{ args... } {}

#define RTI(method, op) return std::tuple{ (get<Indices>(refs).method() op)... }
#pragma warning(push)
#pragma warning(disable : 4003) // Intentionally missing macro arguments

    iterator begin() noexcept {
        RTI(begin);
    }
    iterator end() noexcept {
        RTI(end);
    }

    const_iterator begin() const noexcept {
        RTI(begin);
    }
    const_iterator end() const noexcept {
        RTI(end);
    }

    const_iterator cbegin() const noexcept {
        RTI(cbegin);
    }
    const_iterator cend() const noexcept {
        RTI(cend);
    }

    reverse_iterator rbegin() noexcept {
        RTI(rbegin, - 1);
    }
    reverse_iterator rend() noexcept {
        RTI(rend, - 1);
    }

    const_reverse_iterator crbegin() const noexcept {
        RTI(crbegin, -1);
    }
    const_reverse_iterator crend() const noexcept {
        RTI(crend, - 1);
    }

#pragma warning(pop)
#undef RTI
};

template <class... Containers>
auto zip(Containers&... args) noexcept {
    return Zip_implementation<std::index_sequence_for<Containers...>, Containers...>{ args... };
}


int main() {
    std::vector<int> x{ 1, 50, 100 };
    std::vector<std::string> y{ "hello", "world", "!" };
    
    for (auto [i, j] : zip(x, y))
        std::cout << i << ", " << j << '\n';
    
    /*
    
    OUTPUT:
    
    1, hello
    50, world
    100, !
    
    */
}
