#pragma once

#include <array>
#include <limits>

namespace megamol {
namespace core {

class PerformanceHistory {
public:
    static constexpr uint32_t buffer_length = 100;

    PerformanceHistory();

    void push_value(double val);

    void reset();

    double operator[](int index) const;

    double last_value() const {
        // do we want to treat the special case?
        //if (num_samples > 0) {
        //    return time_buffer[last_index];
        //} else {
        //    return std::numeric_limits<double>::infinity();
        //}

        // or just like this...
        //return time_buffer[last_index];

        // or even better
        return time_buffer[offset(next_index, buffer_length - 1)];
    }

    double average() const {
        return avg_time;
    }

    double buffer_average() const {
        return window_avg;
    }

    uint32_t samples() const {
        return num_samples;
    }

private:
    static int offset(const int index, const int offset);

    static int next_wrap(const int index) {
        return offset(index, 1);
    }
    static int prev_wrap(const int index) {
        return offset(index, -1);
    }

    std::array<double, buffer_length> time_buffer{};
    int next_index = 0;
    //int last_index = 0;
    double avg_time = 0;
    double window_total = 0;
    double window_avg = 0;
    uint32_t num_samples = 0;
};

}
}
