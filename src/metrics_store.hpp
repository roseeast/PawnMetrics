#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>

struct MetricSample {
    std::string name;
    std::string labels;
    double value{};
};

struct HistogramData {
    std::string name;
    std::string labels;
    std::vector<double> buckets;
    std::vector<unsigned long long> counts;
    unsigned long long count{};
    double sum{};
};

struct SummaryData {
    std::string name;
    std::string labels;
    unsigned long long count{};
    double sum{};
};

class MetricsStore {
public:
    void set(const std::string& name, const std::string& labels, double value);
    void add(const std::string& name, const std::string& labels, double value);
    void observe_histogram(const std::string& name, const std::string& labels, double value);
    void observe_summary(const std::string& name, const std::string& labels, double value);
    void remove(const std::string& name, const std::string& labels);
    void clear();
    std::string render() const;

private:
    static bool is_valid_name(const std::string& name);
    static bool is_valid_labels(const std::string& labels);
    static std::string key_for(const std::string& name, const std::string& labels);
    static std::string labels_with_extra(const std::string& labels, const std::string& extra);

    mutable std::mutex mutex_;
    std::map<std::string, MetricSample> values_;
    std::map<std::string, HistogramData> histograms_;
    std::map<std::string, SummaryData> summaries_;
};
