/**
 * MegaMol
 * Copyright (c) 2022, MegaMol Dev Team
 * All rights reserved.
 */

#pragma once

#include <tuple>

#include "SampleSphere.h"
#include "mmcore/BoundingBoxes_2.h"

namespace megamol::core::utility {
std::tuple<std::vector<glm::vec3>, std::vector<glm::vec3>> orbital_camera_samples(
    core::BoundingBoxes_2 const& bboxes, unsigned int num_samples, float dis);
} // namespace megamol::core::utility
