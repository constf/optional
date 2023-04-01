#include <optional>
#include <utility>
#include <string>

class BadOptionalAccess : public std::bad_optional_access {
public:
    BadOptionalAccess() : std::bad_optional_access() {
    }
};


template <typename T>
class Optional : public std::optional<T> {
public:
    Optional() = default;

    explicit Optional(const T& val) : std::optional<T>(val) {
    }
    explicit Optional(T&& val) : std::optional<T>(std::move(val)) {
    }

    template<typename... Types>
    void Emplace(Types&&... vals) {
        this->emplace(std::forward<Types>(vals)...);

//        if (HasValue()) {
//            Reset();
//        }
//        T new_val(std::forward<Types>(vals)...);
//        Optional<T> new_opt(new_val);
//        this->swap(new_opt);
    }

    bool HasValue() const {
        return this->has_value();
    }

    const T& Value() const {
        if (!this->HasValue()){
            throw BadOptionalAccess();
        }
        return this->value();
    }

    void Reset() {
        this->reset();
    }

    Optional<T>& operator=(const T& other) {
//        Optional<T> new_value(other);
//        if (HasValue()) {
//            Reset();
//        }
        this->emplace(other);

        return *this;
    }

    Optional<T>& operator=(T&& other) {
//        if (HasValue()) {
//            Reset();
//        }
        this->emplace(std::move(other));

        return *this;
    }
};

