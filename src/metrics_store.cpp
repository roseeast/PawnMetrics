#include "metrics_store.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace {

const std::vector<double> kDefaultBuckets = {
    0.005, 0.01, 0.025, 0.05, 0.1,
    0.25, 0.5, 1.0, 2.5, 5.0, 10.0
};

}

void MetricsStore::set(const std::string& name, const std::string& labels, double value)
{
    if (!is_valid_name(name) || !is_valid_labels(labels)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    values_[key_for(name, labels)] = MetricSample{name, labels, value};
}

void MetricsStore::add(const std::string& name, const std::string& labels, double value)
{
    if (!is_valid_name(name) || !is_valid_labels(labels)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto& metric = values_[key_for(name, labels)];
    metric.name = name;
    metric.labels = labels;
    metric.value += value;
}

void MetricsStore::observe_histogram(const std::string& name, const std::string& labels, double value)
{
    if (!is_valid_name(name) || !is_valid_labels(labels)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto& histogram = histograms_[key_for(name, labels)];

    if (histogram.buckets.empty()) {
        histogram.name = name;
        histogram.labels = labels;
        histogram.buckets = kDefaultBuckets;
        histogram.counts.assign(histogram.buckets.size(), 0);
    }

    for (std::size_t index = 0; index < histogram.buckets.size(); ++index) {
        if (value <= histogram.buckets[index]) {
            histogram.counts[index]++;
        }
    }

    histogram.count++;
    histogram.sum += value;
}

void MetricsStore::observe_summary(const std::string& name, const std::string& labels, double value)
{
    if (!is_valid_name(name) || !is_valid_labels(labels)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto& summary = summaries_[key_for(name, labels)];
    summary.name = name;
    summary.labels = labels;
    summary.count++;
    summary.sum += value;
}

void MetricsStore::remove(const std::string& name, const std::string& labels)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const std::string key = key_for(name, labels);
    values_.erase(key);
    histograms_.erase(key);
    summaries_.erase(key);
}

void MetricsStore::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    values_.clear();
    histograms_.clear();
    summaries_.clear();
}

std::string MetricsStore::render() const
{
    std::map<std::string, MetricSample> values;
    std::map<std::string, HistogramData> histograms;
    std::map<std::string, SummaryData> summaries;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        values = values_;
        histograms = histograms_;
        summaries = summaries_;
    }

    std::ostringstream output;
    output << "# HELP pawn_metrics_info Pawn metrics plugin scrape info\n";
    output << "# TYPE pawn_metrics_info gauge\n";
    output << "pawn_metrics_info 1\n";

    for (const auto& entry : values) {
        const auto& metric = entry.second;
        output << "# TYPE " << metric.name << " gauge\n";
        output << metric.name;

        if (!metric.labels.empty()) {
            output << '{' << metric.labels << '}';
        }

        output << ' ' << std::setprecision(15) << metric.value << '\n';
    }

    for (const auto& entry : histograms) {
        const auto& histogram = entry.second;
        output << "# TYPE " << histogram.name << " histogram\n";

        for (std::size_t index = 0; index < histogram.buckets.size(); ++index) {
            std::ostringstream le;
            le << "le=\"" << std::setprecision(15) << histogram.buckets[index] << "\"";
            output << histogram.name << "_bucket{" << labels_with_extra(histogram.labels, le.str()) << "} "
                   << histogram.counts[index] << '\n';
        }

        output << histogram.name << "_bucket{" << labels_with_extra(histogram.labels, "le=\"+Inf\"") << "} "
               << histogram.count << '\n';
        output << histogram.name << "_sum";
        if (!histogram.labels.empty()) {
            output << '{' << histogram.labels << '}';
        }
        output << ' ' << std::setprecision(15) << histogram.sum << '\n';
        output << histogram.name << "_count";
        if (!histogram.labels.empty()) {
            output << '{' << histogram.labels << '}';
        }
        output << ' ' << histogram.count << '\n';
    }

    for (const auto& entry : summaries) {
        const auto& summary = entry.second;
        output << "# TYPE " << summary.name << " summary\n";
        output << summary.name << "_sum";
        if (!summary.labels.empty()) {
            output << '{' << summary.labels << '}';
        }
        output << ' ' << std::setprecision(15) << summary.sum << '\n';
        output << summary.name << "_count";
        if (!summary.labels.empty()) {
            output << '{' << summary.labels << '}';
        }
        output << ' ' << summary.count << '\n';
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

bool MetricsStore::is_valid_labels(const std::string& labels)
{
    bool quoted = false;
    bool escaped = false;

    for (char ch : labels) {
        if (ch == '\r' || ch == '\n' || ch == '{' || ch == '}') {
            return false;
        }

        if (escaped) {
            escaped = false;
            continue;
        }

        if (ch == '\\') {
            escaped = true;
            continue;
        }

        if (ch == '"') {
            quoted = !quoted;
        }
    }

    return !quoted && !escaped;
}

std::string MetricsStore::key_for(const std::string& name, const std::string& labels)
{
    return name + '\n' + labels;
}

std::string MetricsStore::labels_with_extra(const std::string& labels, const std::string& extra)
{
    if (labels.empty()) {
        return extra;
    }

    return labels + ',' + extra;
}
