#include "culling/BSCullingProcess.h"
#include "culling/NiCullingProcess.h"

#include <stacktrace>

namespace Culling
{
    void BSCullingProcess::hk_Process1(RE::NiAVObject* a_object, std::int32_t a_alphaGroupIndex)
    {
        const bool alwaysDraw = a_object->flags.all(RE::NiAVObject::Flag::kAlwaysDraw);
        bool       culled;

        if (!a_object->IsVisualObjectI() && !alwaysDraw)
            return;

        if (cullMode.get() == BSCPCullingType::kAllPass) {
            culled = false;
        } else if (cullMode.get() == BSCPCullingType::kAllFail) {
            culled = true;
        } else if (a_object->flags.all(RE::NiAVObject::Flag::kPreProcessedNode) && !ignorePreprocess && cullMode.get() != BSCPCullingType::kForceMultiBoundsNoUpdate) {
            const bool isNotVisible = a_object->flags.all(RE::NiAVObject::Flag::kNotVisible);
            if (!recurseToGeometry) {
                if (isNotVisible)
                    return;

                culled = false;
            } else if (isNotVisible) {
                culled = true;
            } else {
                culled = false;
            }
        } else if (compoundFrustum && compoundFrustum->freeOp && cullMode.get() != BSCPCullingType::kIgnoreMultiBounds) {
            if (!alwaysDraw) {
                RE::NiFrustumPlanes::ActivePlane planesActivePlanes = planes.activePlanes.get();
                std::uint32_t                    compoundFrustumActivePlanes[256];
                compoundFrustum->GetActivePlaneState(compoundFrustumActivePlanes);

                bool isVisible = (compoundFrustum->skipViewFrustum || reinterpret_cast<RE::BSCullingProcess*>(this)->TestBaseVisibility3(a_object->worldBound)) && compoundFrustum->Process(a_object);

                culled = !isVisible;

                compoundFrustum->SetActivePlaneState(compoundFrustumActivePlanes);
                planes.activePlanes.set(planesActivePlanes);
            } else {
                culled = false;
            }
        } else {
            if (recurseToGeometry) {
                if (!alwaysDraw && planes.activePlanes.underlying()) {
                    reinterpret_cast<Culling::NiCullingProcess*>(this)->hk_DoCulling(a_object, a_alphaGroupIndex);
                    return;
                } else {
                    culled = false;
                }
            } else if (alwaysDraw || !planes.activePlanes.underlying() || reinterpret_cast<RE::BSCullingProcess*>(this)->TestBaseVisibility3(a_object->worldBound)) {
                culled = false;
            } else
                return;
        }

        if (recurseToGeometry) {
            if (culled && updateAccumulateFlag)
                a_object->flags.set(false, RE::NiAVObject::Flag::kAccumulated);
            else if (!culled) {
                a_object->OnVisible(*this, a_alphaGroupIndex);
                if (updateAccumulateFlag)
                    a_object->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
            }
        } else if (!culled) {
            reinterpret_cast<RE::BSCullingProcess*>(this)->AppendVirtual(*static_cast<RE::BSGeometry*>(a_object), a_alphaGroupIndex);
        }
    }

    void BSCullingProcess::hk_Process2(const RE::NiCamera* a_camera, RE::NiAVObject* a_scene, RE::NiVisibleArray* a_visibleSet) {
        if (a_camera && a_scene) {
            camera = a_camera;
            SetFrustum(&camera->viewFrustum);
            if (doCustomCullPlanes)
                planes = customCullPlanes;
            RE::NiVisibleArray* currentArray = nullptr;
            if (a_visibleSet) {
                currentArray = visibleSet;
                visibleSet = a_visibleSet;
            }
            if (recurseToGeometry)
                a_scene->Cull(this, -1);
            else
                reinterpret_cast<RE::BSCullingProcess*>(this)->Process1(a_scene, -1);
            if (a_visibleSet) {
                visibleSet = currentArray;
            }
        }
    }

    void BSCullingProcess::hk_AppendVirtual(RE::BSGeometry& a_visible, std::int32_t a_alphaGroupIndex) {
        if (!isGroupingAlphas && a_alphaGroupIndex == -1)
            return reinterpret_cast<RE::BSCullingProcess*>(this)->AppendNonAccum(a_visible, alphaGroupIndex);

        RE::BSCullingProcess::Data* data = cullQueue.free.Next();
        data->geometry = &a_visible;
        data->alphaGroupStartIndex = a_alphaGroupIndex;
        data->alphaGroupStopIndex = alphaGroupIndex;
        cullQueue.queued.Add(data);
    }

    void BSCullingProcess::hk_AppendNonAccum(RE::NiAVObject& a_visible, [[maybe_unused]] std::int32_t a_alphaGroupIndex) {
        objectArray.emplace_back(&a_visible);
    }

    bool BSCullingProcess::hk_TestBaseVisibility1(const RE::BSMultiBound& a_bound) const {
        return a_bound.data->WithinFrustum(planes);
    }

    bool BSCullingProcess::hk_TestBaseVisibility2(RE::BSOcclusionPlane& a_occlusionPlane) const {
        return a_occlusionPlane.WithinFrustumDistFirst(&planes, &camera->world.translate);
    }

    bool BSCullingProcess::hk_TestBaseVisibility3(const RE::NiBound& a_bound)  {
        for (int i = 0; i < RE::NiFrustumPlanes::Planes::kTotal; i++) {
            if (planes.activePlanes.all(static_cast<RE::NiFrustumPlanes::ActivePlane>(1 << i))) {
                const auto& activePlane = planes.cullingPlanes[i];

                const float radius = activePlane.normal.x * a_bound.center.x + activePlane.normal.y * a_bound.center.y + activePlane.normal.z * a_bound.center.z - activePlane.constant;

                if (radius <= -a_bound.radius) {
                    return false;
                }

                if (radius >= a_bound.radius && enablePlaneOptimization)
                    planes.activePlanes.set(false, static_cast<RE::NiFrustumPlanes::ActivePlane>(1 << i));
            }
        }
        return true;
    }
}