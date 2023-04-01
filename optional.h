#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;
    explicit Optional(const T& value);
    explicit Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other) noexcept ;

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs);
    Optional& operator=(const Optional& rhs);
    Optional& operator=(Optional&& rhs) noexcept;

    template<typename... Types>
    void Emplace(Types&&... vals) {
        if (HasValue()) {
            Reset();
        }
        value_ = new (&data_) T(std::forward<Types>(vals)...);
        is_initialized_ = true;
    }

    ~Optional();

    bool HasValue() const;

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value();
    const T& Value() const;

    void Reset();

private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
    T* value_ = nullptr;
};

template<typename T>
T& Optional<T>::Value() {
    if (!is_initialized_) {
        throw BadOptionalAccess();
    }

    return *value_;
}

template<typename T>
const T &Optional<T>::Value() const {
    if (!is_initialized_) {
        throw BadOptionalAccess();
    }

    return *value_;
}

template<typename T>
void Optional<T>::Reset() {
    if (is_initialized_) {
        value_->~T();
        value_ = nullptr;
        is_initialized_ = false;
    }
}

template<typename T>
Optional<T>::Optional(const T &value) {
    value_ = new (&data_) T(value);
    is_initialized_ = true;
}

template<typename T>
Optional<T>::Optional(T &&value) {
    value_ = new (&data_) T(std::move(value));
    is_initialized_ = true;
}

template<typename T>
bool Optional<T>::HasValue() const {
    return is_initialized_;
}

template<typename T>
Optional<T>::~Optional() {
    if (is_initialized_) {
        value_->~T();
    }
}

template<typename T>
Optional<T>::Optional(const Optional& other) {
    is_initialized_ = other.HasValue();
    if (is_initialized_) {
        value_ = new(&data_) T(other.Value());
    } else {
        value_ = nullptr;
    }
}

template<typename T>
Optional<T>::Optional(Optional&& other) noexcept {
    is_initialized_ = std::move(other.is_initialized_);
    value_ = is_initialized_? new(&data_) T(std::move(*other.value_)) : nullptr;
}

template<typename T>
Optional<T>& Optional<T>::operator=(const T& value) {
    if (!is_initialized_) {
        value_ = new (&data_) T(value);
        is_initialized_ = true;
    } else {
//        value_->~T();
        *value_ = value;
    }

    return *this;
}

template<typename T>
Optional<T>& Optional<T>::operator=(T&& rhs) {
    if (!is_initialized_) {
        value_ = new (&data_) T(std::move(rhs));
        is_initialized_ = true;
    } else {
//        value_->~T();
        *value_ = std::move(rhs);
    }

    return *this;
}

template<typename T>
Optional<T>& Optional<T>::operator=(const Optional &rhs) {
    if (this == &rhs) return *this;

    if (rhs.HasValue()) {
        *this = *rhs;
    } else {
        Reset();
    }

    return *this;
}

template<typename T>
Optional<T>& Optional<T>::operator=(Optional &&rhs) noexcept {
    if (this == &rhs) return *this;

    if (rhs.HasValue()) {
        *this = std::move(*rhs);
    } else {
        Reset();
    }

    return *this;
}

template<typename T>
T& Optional<T>::operator*() {
    return *value_;
}

template<typename T>
const T &Optional<T>::operator*() const {
    return *value_;
}

template<typename T>
T* Optional<T>::operator->() {
    return value_;
}

template<typename T>
const T* Optional<T>::operator->() const {
    return value_;
}
