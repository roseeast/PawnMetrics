#include "metrics_store.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <vector>

void MetricsStore::set(const std::string& name, double value)
{
    if (!is_valid_name(name)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    values_[name] = value;
}

void MetricsStore::add(const std::string& name, double value)
{
    if (!is_valid_name(name)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    values_[name] += value;
}

void MetricsStore::remove(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    values_.erase(name);
}

void MetricsStore::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    values_.clear();
}

std::string MetricsStore::render() const
{
    std::vector<std::pair<std::string, double>> metrics;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics.reserve(values_.size());

        for (const auto& entry : values_) {
            metrics.push_back(entry);
        }
    }

    std::sort(metrics.begin(), metrics.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });

    std::ostringstream output;
    output << "# HELP pawn_metrics_info Pawn metrics plugin scrape info\n";
    output << "# TYPE pawn_metrics_info gauge\n";
    output << "pawn_metrics_info 1\n";

    for (const auto& metric : metrics) {
        output << "# TYPE " << metric.first << " gauge\n";
        output << metric.first << ' ' << std::setprecision(15) << metric.second << '\n';
    }

    return output.str();
}

bool MetricsStore::is_valid_name(const std::string& name)
{
    if (name.empty()) {
        return false;
    }

    const unsigned char first = static_cast<unsigned char>(name[0]);
    if (!(std::isalpha(first) || name[0] == '_' || name[0] == ':')) {
        return false;
    }

    for (char ch : name) {
        const unsigned char value = static_cast<unsigned char>(ch);
        if (!(std::isalnum(value) || ch == '_' || ch == ':')) {
            return false;
        }
    }

    return true;
}

