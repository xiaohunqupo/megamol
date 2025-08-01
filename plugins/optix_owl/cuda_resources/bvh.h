// code originally from https://github.com/UniStuttgart-VISUS/rtxpkd_ldav2020
// modified for MegaMol

// ======================================================================== //
// Copyright 2018-2019 Ingo Wald                                            //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

// ======================================================================== //
// Modified 2019-2025 VISUS - University of Stuttgart                       //
// ======================================================================== //

#pragma once

#include <owl/common/math/box.h>

#include "particle.h"

namespace megamol {
namespace optix_owl {
namespace device {
using namespace owl::common;
struct BVHGeomData {
    Particle* particleBuffer;
    float particleRadius;
};
} // namespace device
} // namespace optix_owl
} // namespace megamol
