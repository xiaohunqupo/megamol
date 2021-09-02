#include "mmcore/CallProfiling.h"
#include "mmcore/Call.h"

using namespace megamol;
using namespace core;

std::string CallProfiling::err_oob = "out of bounds";
PerformanceQueryManager* CallProfiling::qm = nullptr;

CallProfiling::CallProfiling() {
    
}

CallProfiling::~CallProfiling() {
    if (parent_call->UsesGL()) {
        qm->RemoveCall(parent_call);
    }
}

uint32_t CallProfiling::GetSampleHistoryLength() {
    return PerformanceHistory::buffer_length;
}

double CallProfiling::GetLastCPUTime(uint32_t func) const {
    if (func < callback_names.size())
        return cpu_history[func].last_value();
    else
        return -1.0;
}

double CallProfiling::GetAverageCPUTime(uint32_t func) const {
    if (func < callback_names.size())
        return cpu_history[func].average();
    else
        return -1.0;
}

uint32_t CallProfiling::GetNumCPUSamples(uint32_t func) const {
    if (func < callback_names.size())
        return cpu_history[func].samples();
    else
        return 0;
}

std::array<double, PerformanceHistory::buffer_length> CallProfiling::GetCPUHistory(
    uint32_t func) const {
    if (func < callback_names.size())
        return cpu_history[func].copyHistory();
    else
        return std::array<double, PerformanceHistory::buffer_length>{};
}

double CallProfiling::GetLastGPUTime(uint32_t func) const {
    if (func < callback_names.size())
        return gpu_history[func].last_value();
    else
        return -1.0;
}

double CallProfiling::GetAverageGPUTime(uint32_t func) const {
    if (func < callback_names.size())
        return gpu_history[func].average();
    else
        return -1.0;
}

uint32_t CallProfiling::GetNumGPUSamples(uint32_t func) const {
    if (func < callback_names.size())
        return gpu_history[func].samples();
    else
        return 0;
}

std::array<double, PerformanceHistory::buffer_length> CallProfiling::GetGPUHistory(
    uint32_t func) const {
    if (func < callback_names.size())
        return gpu_history[func].copyHistory();
    else
        return std::array<double, PerformanceHistory::buffer_length>{};
}

uint32_t CallProfiling::GetFuncCount() const {
    /// XXX assert(last_cpu_time.size() == avg_cpu_time.size() == num_cpu_time_samples.size() == last_gpu_time.size() ==
            ///       avg_gpu_time.size() == num_gpu_time_samples.size());
    return static_cast<uint32_t>(callback_names.size());
}

const std::string& CallProfiling::GetFuncName(uint32_t i) const {
    if (i < callback_names.size()) {
        return callback_names[i];
    } else {
        return err_oob;
    }
}

void CallProfiling::InitializeQueryManager() {
    if (qm == nullptr) {
        qm = new PerformanceQueryManager();
    }
}

void CallProfiling::CollectGPUPerformance() {
    if (qm != nullptr) {
        qm->Collect();
    }
}

void CallProfiling::setProfilingInfo(std::vector<std::string> names, Call* parent) {
    callback_names = std::move(names);
    cpu_history.resize(callback_names.size());
    gpu_history.resize(callback_names.size());
    parent_call = parent;
    if (parent_call->UsesGL()) {
        InitializeQueryManager();
        qm->AddCall(parent_call);
    }
}
