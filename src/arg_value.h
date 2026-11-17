#ifndef ARG_VALUE_H
#define ARG_VALUE_H

#include <any>
#include <initializer_list>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

class ArgValue {
private:
    std::any value_;
    std::string str_repr_;

public:
    ArgValue();
    ArgValue(int val);                     // NOLINT(google-explicit-constructor)
    ArgValue(long val);                    // NOLINT(google-explicit-constructor)
    ArgValue(double val);                  // NOLINT(google-explicit-constructor)
    ArgValue(float val);                   // NOLINT(google-explicit-constructor)
    ArgValue(bool val);                    // NOLINT(google-explicit-constructor)
    ArgValue(const char* val);             // NOLINT(google-explicit-constructor)
    ArgValue(const std::string& val);      // NOLINT(google-explicit-constructor)
    ArgValue(std::string&& val) noexcept;  // NOLINT(google-explicit-constructor)

    template <typename T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<T>, ArgValue> &&
        !std::is_same_v<std::decay_t<T>, const char*> &&
        !std::is_same_v<std::decay_t<T>, std::string>>>
    ArgValue(T&& val) : value_(std::forward<T>(val)) { // NOLINT(google-explicit-constructor)
        if constexpr (std::is_arithmetic_v<std::decay_t<T>>) {
            str_repr_ = std::to_string(val);
        }
    }

    template <typename T>
    T as() const {
        using Clean = std::decay_t<T>;
        if constexpr (std::is_same_v<Clean, std::string>) {
            return str_repr_;
        } else if constexpr (std::is_same_v<Clean, const char*>) {
            return str_repr_.c_str();
        } else if constexpr (std::is_same_v<Clean, int>) {
            if (value_.type() == typeid(int)) {
                return std::any_cast<int>(value_);
            }
            if (value_.type() == typeid(long)) {
                return static_cast<int>(std::any_cast<long>(value_));
            }
            if (value_.type() == typeid(double)) {
                return static_cast<int>(std::any_cast<double>(value_));
            }
            if (value_.type() == typeid(bool)) {
                return std::any_cast<bool>(value_) ? 1 : 0;
            }
            return !str_repr_.empty() ? std::stoi(str_repr_) : 0;
        } else if constexpr (std::is_same_v<Clean, long>) {
            if (value_.type() == typeid(long)) {
                return std::any_cast<long>(value_);
            }
            if (value_.type() == typeid(int)) {
                return static_cast<long>(std::any_cast<int>(value_));
            }
            return !str_repr_.empty() ? std::stol(str_repr_) : 0L;
        } else if constexpr (std::is_same_v<Clean, double>) {
            if (value_.type() == typeid(double)) {
                return std::any_cast<double>(value_);
            }
            if (value_.type() == typeid(float)) {
                return static_cast<double>(std::any_cast<float>(value_));
            }
            if (value_.type() == typeid(int)) {
                return static_cast<double>(std::any_cast<int>(value_));
            }
            return !str_repr_.empty() ? std::stod(str_repr_) : 0.0;
        } else if constexpr (std::is_same_v<Clean, float>) {
            if (value_.type() == typeid(float)) {
                return std::any_cast<float>(value_);
            }
            if (value_.type() == typeid(double)) {
                return static_cast<float>(std::any_cast<double>(value_));
            }
            if (value_.type() == typeid(int)) {
                return static_cast<float>(std::any_cast<int>(value_));
            }
            return !str_repr_.empty() ? std::stof(str_repr_) : 0.0F;
        } else if constexpr (std::is_same_v<Clean, bool>) {
            if (value_.type() == typeid(bool)) {
                return std::any_cast<bool>(value_);
            }
            if (value_.type() == typeid(int)) {
                return std::any_cast<int>(value_) != 0;
            }
            return str_repr_ == "true" || str_repr_ == "1" ||
                   str_repr_ == "найдено" || str_repr_ == "yes";
        } else {
            return std::any_cast<Clean>(value_);
        }
    }

    const std::string& to_string() const noexcept;
    bool has_value() const noexcept;
};

class ArgsMap {
private:
    std::vector<std::pair<std::string, ArgValue>> entries_;

public:
    ArgsMap() = default;
    ArgsMap(std::initializer_list<std::pair<std::string, ArgValue>> init); // NOLINT(google-explicit-constructor)
    ArgsMap(const std::vector<std::pair<std::string, ArgValue>>& entries); // NOLINT(google-explicit-constructor)
    ArgsMap(const std::map<std::string, ArgValue>& map);                  // NOLINT(google-explicit-constructor)
    ArgsMap(const std::map<std::string, std::string>& map);               // NOLINT(google-explicit-constructor)
    ArgsMap(const std::unordered_map<std::string, ArgValue>& map);        // NOLINT(google-explicit-constructor)
    ArgsMap(const std::unordered_map<std::string, std::string>& map);     // NOLINT(google-explicit-constructor)

    const ArgValue* find(const std::string& name) const noexcept;
    bool has(const std::string& name) const noexcept;
    const ArgValue& at(const std::string& name) const;
    std::size_t size() const noexcept;
    bool empty() const noexcept;
    const std::vector<std::pair<std::string, ArgValue>>& entries() const noexcept;
};

#endif // ARG_VALUE_H
