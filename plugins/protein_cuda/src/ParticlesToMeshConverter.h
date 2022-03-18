/*
 * QuickSurfRaycaster.h
 *
 * Copyright (C) 2016 by Universitaet Stuttgart (VISUS).
 * All rights reserved
 */

#ifndef MMPROTEINCUDAPLUGIN_PARTICLESTOMESHCONVERTER_H_INCLUDED
#define MMPROTEINCUDAPLUGIN_PARTICLESTOMESHCONVERTER_H_INCLUDED

#include "mmcore/CallerSlot.h"
#include "mmcore/param/ParamSlot.h"
#include "mmcore/view/CallRender3D.h"
#include "mmcore/view/Renderer3DModule.h"
#include "vislib_gl/graphics/gl/GLSLShader.h"

#include "geometry_calls/MultiParticleDataCall.h"
#include "geometry_calls/VolumetricDataCall.h"
#include "geometry_calls/VolumetricDataCallTypes.h"
#include "protein_calls/MolecularDataCall.h"

#include "CUDAMarchingCubes.h"
#include "CUDAQuickSurfAlternative.h"

#include <fstream>

#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime_api.h>
#include <driver_functions.h>
#include <helper_cuda.h>
#include <vector_functions.h>
#include <vislib_gl/graphics/gl/IncludeAllGL.h>

extern "C" void setTextureFilterMode(bool bLinearFilter);
extern "C" void initCudaDevice(void* h_volume, cudaExtent volumeSize);
extern "C" void freeCudaBuffers();
//extern "C" void render_kernel(dim3 gridSize, dim3 blockSize, unsigned int *d_output, unsigned int imageW, unsigned int imageH, float fovx, float fovy, float3 camPos, float3 camDir, float3 camUp, float3 camRight, float zNear, float density, float brightness, float transferOffset, float transferScale, const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f), const float3 boxMax = make_float3(1.0, 1.0, 1.0), cudaExtent volSize = make_cudaExtent(1, 1, 1), const float3 lightDir = make_float3(1.0f, 1.0f, 1.0f), const float4 lightParams = make_float4(0.3f, 0.5f, 0.4f, 10.0f));
//extern "C" void renderArray_kernel(cudaArray* renderArray, dim3 gridSize, dim3 blockSize, unsigned int *d_output, unsigned int imageW, unsigned int imageH, float fovx, float fovy, float3 camPos, float3 camDir, float3 camUp, float3 camRight, float zNear, float density, float brightness, float transferOffset, float transferScale, const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f), const float3 boxMax = make_float3(1.0f, 1.0f, 1.0f), cudaExtent volSize = make_cudaExtent(1, 1, 1), const float3 lightDir = make_float3(1.0f, 1.0f, 1.0f), const float4 lightParams = make_float4(0.3f, 0.5f, 0.4f, 10.0f));
//extern "C" void copyLUT(float4* myLUT, int lutSize = 256);
extern "C" void transferIsoValues(float4 h_isoVals, int h_numIsos);
extern "C" void transferNewVolume(void* h_volume, cudaExtent volumeSize);
extern "C" void transferVolumeDirect(void* h_volume, cudaExtent volumeSize, float myMin, float myMax);

#include "vislib/math/Matrix.h"
#include "vislib/math/Vector.h"
typedef vislib::math::Matrix<float, 3, vislib::math::COLUMN_MAJOR> Mat3f;
typedef vislib::math::Matrix<float, 4, vislib::math::COLUMN_MAJOR> Mat4f;

namespace megamol {
namespace protein_cuda {

class ParticlesToMeshConverter : public megamol::core::Module {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName(void) {
        return "ParticlesToMeshConverter";
    }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description(void) {
        return "Converts particles into a density-based meshed isosurface via QuickSurf";
    }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable(void) {
        return true;
    }

    /** Ctor. */
    ParticlesToMeshConverter(void);

    /** Dtor. */
    virtual ~ParticlesToMeshConverter(void);

protected:
    /**
     * Implementation of 'Create'.
     *
     * @return 'true' on success, 'false' otherwise.
     */
    virtual bool create(void);

    /**
     * Implementation of 'release'.
     */
    virtual void release(void);

private:
    /*
     * Struct representing a vertex with a position and a color.
     */
    struct Vertex {
        // The vertex position.
        GLfloat x, y, z, w;

        // The vertex color.
        GLfloat r, g, b, a;

        Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
                : x(x)
                , y(y)
                , z(z)
                , w(1.0f)
                , r(r)
                , g(g)
                , b(b)
                , a(a) {}

        Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
                : x(x)
                , y(y)
                , z(z)
                , w(w)
                , r(r)
                , g(g)
                , b(b)
                , a(a) {}
    };

    // This function returns the best GPU (with maximum GFLOPS)
    VISLIB_FORCEINLINE int cudaUtilGetMaxGflopsDeviceId() const {
        int device_count = 0;
        cudaGetDeviceCount(&device_count);

        cudaDeviceProp device_properties;
        int max_gflops_device = 0;
        int max_gflops = 0;

        int current_device = 0;
        cudaGetDeviceProperties(&device_properties, current_device);
        max_gflops = device_properties.multiProcessorCount * device_properties.clockRate;
        ++current_device;

        while (current_device < device_count) {
            cudaGetDeviceProperties(&device_properties, current_device);
            int gflops = device_properties.multiProcessorCount * device_properties.clockRate;
            if (gflops > max_gflops) {
                max_gflops = gflops;
                max_gflops_device = current_device;
            }
            ++current_device;
        }

        return max_gflops_device;
    }

    VISLIB_FORCEINLINE int iDivUp(int a, int b) {
        return (a % b != 0) ? (a / b + 1) : (a / b);
    }

    VISLIB_FORCEINLINE float3 norm(float3 a) {
        float scale = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
        return make_float3(a.x / scale, a.y / scale, a.z / scale);
    }

    /**
     * The get extents callback. The module should set the members of
     * 'call' to tell the caller the extents of its data (bounding boxes
     * and times).
     *
     * @param call The calling call.
     *
     * @return True on success, false otherwise.
     */
    virtual bool GetExtents(megamol::core::Call& call);

    /**
     * The get data callback. The module should set the members of
     * 'call' to tell the caller the data
     *
     * @param call The calling call.
     *
     * @return True on success, false otherwise.
     */
    virtual bool GetData(megamol::core::Call& call);


    /**
     * Calculates the volume data used for ray casting from the given call and other values
     *
     * @param bbMin The min values of the object space bounding box
     * @param bbMax The max values of the object space bounding box
     * @param position Array containing all particle positions followed by the radii.
     * @param quality
     * @param radscale
     * @param gridspacing
     * @param isoval
     * @param minConcentration Minimal concentration over all particles
     * @param maxConcentration Maximal concentration over all particles
     * @param useCol
     * @param timestep The current timestep index
     * @return True on success, false otherwise
     */
    bool calcVolume(float3 bbMin, float3 bbMax, float* positions, int quality, float radscale, float gridspacing,
        float isoval, float minConcentration, float maxConcentration, bool useCol, int timestep = 0);

    /// <summary>
    /// Converts a given volume to a mesh and writes it to disk.
    /// </summary>
    /// <param name="volumeData">Pointer to the voxel data.</param>
    /// <param name="volSize">Dimensions of the volume data.</param>
    /// <param name="isoValue">The isovalue for which the conversion should happen.</param>
    void makeMesh(float* volumeData, cudaExtent volSize, float3 bbMin, float3 bbMax, float isoValue = 0.1,
        float concMin = 0.0f, float concMax = 1.0f);

    /** caller slot */
    megamol::core::CallerSlot particleDataSlot;

    megamol::core::CalleeSlot meshDataSlot;


    //! Pointer to the CUDAQuickSurf object if it exists
    void* cudaqsurf;

    void* cudaMarching;

    /** Has the CUDA device to be set? */
    bool setCUDAGLDevice;

    /** The total number of rendered particles*/
    UINT64 numParticles;

    /** The number of particles after filtering*/
    UINT64 particleCnt;

    /** The array containing the rendered volume */
    cudaArray* volumeArray;

    /** The vertex array for texture drawing*/
    GLuint textureVAO;

    /** The vertex buffer containing the vertices for texture drawing*/
    GLuint textureVBO;

    /** The texture handle */
    GLuint texHandle;

    /** The shader program for texture drawing */
    vislib_gl::graphics::gl::GLSLShader textureShader;

    /** The viewport dimensions of the last frame */
    vislib::math::Dimension<int, 2U> lastViewport;

    /** Pointer to the read particle data */
    float* particles;

    /** Pointer to the read particle colors */
    float* particleColors;

    /** Number of particles in the particles array */
    UINT64 particlesSize;

    /** The color table for all particles */
    std::vector<float> colorTable;

    megamol::core::param::ParamSlot qualityParam;

    megamol::core::param::ParamSlot radscaleParam;

    megamol::core::param::ParamSlot gridspacingParam;

    megamol::core::param::ParamSlot isovalParam;

    megamol::core::param::ParamSlot scalingFactor;

    megamol::core::param::ParamSlot concFactorParam;

    megamol::core::param::ParamSlot maxRadius;

    std::vector<float> isoVals;

    cudaArray* tmpCudaArray;

    bool firstTransfer;

    cudaExtent volumeExtent;

    int curTime;

    cudaExtent volumeExtentSmall;

    bool recomputeVolume;

    int lastTimeVal;

    float* map;

    float3* mcVertices_d;
    float3* mcNormals_d;
    float3* mcColors_d;

    SIZE_T lastMPDCHash;
    SIZE_T lastMDCHash;
    SIZE_T lastVDCHash;
    unsigned int lastFrameID;
};

} /* end namespace protein_cuda */
} /* end namespace megamol */

#endif // MMPROTEINCUDAPLUGIN_PARTICLESTOMESHCONVERTER_H_INCLUDED
