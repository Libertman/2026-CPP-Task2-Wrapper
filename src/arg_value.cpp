#include "arg_value.h"
#include <stdexcept>
#include <utility>

ArgValue::ArgValue() = default;

ArgValue::ArgValue(int val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(long val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(double val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(float val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(bool val) : value_(val), str_repr_(val ? "true" : "false") {}

ArgValue::ArgValue(const char* val)
    : value_(val != nullptr ? std::string(val) : ""),
      str_repr_(val != nullptr ? val : "") {}

ArgValue::ArgValue(const std::string& val) : value_(val), str_repr_(val) {}

ArgValue::ArgValue(std::string&& val) noexcept
    : value_(val), str_repr_(std::move(val)) {}

const std::string& ArgValue::to_string() const noexcept {
    return str_repr_;
}

bool ArgValue::has_value() const noexcept {
    return value_.has_value() || !str_repr_.empty();
}

ArgsMap::ArgsMap(std::initializer_list<std::pair<std::string, ArgValue>> init)
    : entries_(init) {}

ArgsMap::ArgsMap(const std::vector<std::pair<std::string, ArgValue>>& entries)
    : entries_(entries) {}

ArgsMap::ArgsMap(const std::map<std::string, ArgValue>& map) {
    entries_.reserve(map.size());
    for (const auto& kv : map) {
        entries_.emplace_back(kv.first, kv.second);
    }
}

ArgsMap::ArgsMap(const std::map<std::string, std::string>& map) {
    entries_.reserve(map.size());
    for (const auto& kv : map) {
        entries_.emplace_back(kv.first, ArgValue(kv.second));
    }
}

ArgsMap::ArgsMap(const std::unordered_map<std::string, ArgValue>& map) {
    entries_.reserve(map.size());
    for (const auto& kv : map) {
        entries_.emplace_back(kv.first, kv.second);
    }
}

ArgsMap::ArgsMap(const std::unordered_map<std::string, std::string>& map) {
    entries_.reserve(map.size());
    for (const auto& kv : map) {
        entries_.emplace_back(kv.first, ArgValue(kv.second));
    }
}

const ArgValue* ArgsMap::find(const std::string& name) const noexcept {
    for (const auto& kv : entries_) {
        if (kv.first == name) {
            return &kv.second;
        }
    }
    return nullptr;
}

bool ArgsMap::has(const std::string& name) const noexcept {
    return find(name) != nullptr;
}

const ArgValue& ArgsMap::at(const std::string& name) const {
    const ArgValue* val = find(name);
    if (val == nullptr) {
        throw std::out_of_range("Argument not found: " + name);
    }
    return *val;
}

std::size_t ArgsMap::size() const noexcept {
    return entries_.size();
}

bool ArgsMap::empty() const noexcept {
    return entries_.empty();
}

const std::vector<std::pair<std::string, ArgValue>>& ArgsMap::entries() const noexcept {
    return entries_;
}
