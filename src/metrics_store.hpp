#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

class MetricsStore {
public:
    void set(const std::string& name, double value);
    void add(const std::string& name, double value);
    void remove(const std::string& name);
    void clear();
    std::string render() const;

private:
    static bool is_valid_name(const std::string& name);

    mutable std::mutex mutex_;
    std::unordered_map<std::string, double> values_;
};

