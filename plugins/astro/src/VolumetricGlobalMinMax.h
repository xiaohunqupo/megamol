/*
 * VolumetricGlobalMinMax.h
 *
 * Copyright (C) 2019 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOL_ASTRO_VOLUMETRICGLOBALMINMAX_H_INCLUDED
#define MEGAMOL_ASTRO_VOLUMETRICGLOBALMINMAX_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include <array>

#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/Module.h"

#include "geometry_calls/VolumetricDataCall.h"


namespace megamol::astro {

/// <summary>
/// Gets min/max values on a <see cref="VolumetricDataCall" />.
/// </summary>
class VolumetricGlobalMinMax : public core::Module {

public:
    static inline const char* ClassName() {
        return "VolumetricGlobalMinMax";
    }

    static inline const char* Description() {
        return "Gets min/max values over all frames of a VolumetricDataCall";
    }

    static bool IsAvailable() {
        return true;
    }

    /** Ctor. */
    VolumetricGlobalMinMax();

    /** Dtor. */
    ~VolumetricGlobalMinMax() override;

protected:
    bool create() override;

    void release() override;

    bool onGetData(core::Call& call);

    bool onGetExtents(core::Call& call);

    bool onGetMetadata(core::Call& call);

    bool onUnsupportedCallback(core::Call& call);

    bool pipeVolumetricDataCall(core::Call& call, unsigned int funcIdx);

private:
    core::CallerSlot slotVolumetricDataIn;
    core::CalleeSlot slotVolumetricDataOut;
    size_t hash;
    std::vector<double> minValues;
    std::vector<double> maxValues;
};

} // namespace megamol::astro

#endif /* MEGAMOL_ASTRO_VOLUMETRICGLOBALMINMAX_H_INCLUDED */
