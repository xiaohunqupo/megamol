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
#include <owl/common/math/vec.h>
#include <owl/owl_device.h>

#include "particle.h"

namespace megamol {
namespace optix_owl {
namespace device {
using namespace owl::common;

inline __device__ bool intersectSphere(const vec3f pos, const float particleRadius, const owl::Ray& ray, float& hit_t) {
    // Raytracing Gems Intersection Code (Chapter 7)
    const vec3f oc = ray.origin - pos;
    const float sqrRad = particleRadius * particleRadius;

    // const float  a = dot(ray.direction, ray.direction);
    const float b = dot(-oc, ray.direction);
    const vec3f temp = oc + b * ray.direction;
    const float delta = sqrRad - dot(temp, temp);

    if (delta < 0.0f)
        return false;

    const float c = dot(oc, oc) - sqrRad;
    const float sign = signbit(b) ? 1.0f : -1.0f;
    const float q = b + sign * sqrtf(delta);

    {
        float temp = fminf(c / q, q);
        if (temp < hit_t && temp > ray.tmin) {
            hit_t = temp;
            return true;
        }
    }

    return false;
}

inline __device__ bool intersectSphere(
    const Particle particle, const float particleRadius, const owl::Ray& ray, float& hit_t) {
    return intersectSphere(particle.pos, particleRadius, ray, hit_t);
}

inline __device__ bool clipToBounds(const owl::Ray& ray, const box3f& bounds, float& t0, float& t1) {
    vec3f t_lower = (bounds.lower - ray.origin) / ray.direction;
    vec3f t_upper = (bounds.upper - ray.origin) / ray.direction;

    vec3f t_min3 = min(t_lower, t_upper);
    vec3f t_max3 = max(t_lower, t_upper);

    t0 = fmaxf(ray.tmin, reduce_max(t_min3));
    t1 = fminf(ray.tmax, reduce_min(t_max3));
    return t0 < t1;
}
} // namespace device
} // namespace optix_owl
} // namespace megamol
