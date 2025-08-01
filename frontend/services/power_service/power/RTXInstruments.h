#pragma once

#ifdef MEGAMOL_USE_POWER

#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <power_overwhelming/rtx_instrument.h>
#include <power_overwhelming/rtx_instrument_configuration.h>

#include "MetaData.h"
#include "Trigger.h"
#include "Utility.h"

#include <sol/sol.hpp>

namespace megamol::power {

/**
 * @brief Class representing the interface to the external R&S oscilloscopes.
 */
class RTXInstruments {
public:
    /**
     * @brief Ctor. Searches for all attached scopes.
     * @param trigger Trigger object that will trigger the scopes.
     * @throws std::runtime_error No scopes are detected.
     */
    RTXInstruments(std::shared_ptr<Trigger> trigger);

    /**
     * @brief Loads the scope configs in @c config_folder.
     * Expecting the scope name as file name with "rtxcfg" as extension.
     * The configs will not be applied immediately.
     * @param config_folder Path to the configs.
     * @param points Number of samples to acquire per measurment segment.
     * @param count Number of segments.
     * @param range Time range of measurement in milliseconds.
     * @param timeout Timeout for communication with the scopes in milliseconds.
     */
    void UpdateConfigs(std::filesystem::path const& config_folder, int points, int count,
        std::chrono::milliseconds range, std::chrono::milliseconds timeout);

    /**
     * @brief Apply the configs previously read with UpdateConfigs.
     * @param meta Pointer to MetaData object to insert the applied configs.
     */
    void ApplyConfigs(MetaData* meta = nullptr);

    /**
     * @brief Starts the measurement with the attached and configures scopes.
     * @param output_folder Path to folder for storing the acquired samples.
     * @param writer_funcs Writer functions to push the acquired samples.
     * @param meta MetaData object to embed in the output files.
     * @param signal Will be set to one while the measurement is running.
     */
    void StartMeasurement(std::filesystem::path const& output_folder,
        std::vector<power::writer_func_t> const& writer_funcs, power::MetaData const* meta, char& signal);

    /**
     * @brief Toggles alternative software trigger according to @c set.
     * @param set Flag to set the software trigger.
     */
    void SetSoftwareTrigger(bool set) {
        enforce_software_trigger_ = set;
        trigger_->RegisterSubTrigger("RTXInstruments", std::bind(&RTXInstruments::soft_trg, this));
    }

private:
    void soft_trg() {
        /*for (auto& [name, i] : rtx_instr_) {
            i.trigger_manually();
        }*/
        main_instr_->trigger_manually();
    }

    bool waiting_on_trigger() const;

    std::unordered_map<std::string, visus::power_overwhelming::rtx_instrument> rtx_instr_;

    visus::power_overwhelming::rtx_instrument* main_instr_ = nullptr;

    std::unordered_map<std::string, visus::power_overwhelming::rtx_instrument_configuration> rtx_config_;

    sol::state sol_state_;

    std::chrono::milliseconds config_range_;

    bool enforce_software_trigger_ = false;

    std::shared_ptr<Trigger> trigger_ = nullptr;
};

} // namespace megamol::power

#endif
