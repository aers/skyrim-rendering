#include "culling/NiCullingProcess.h"

namespace Culling
{
    void NiCullingProcess::hk_DoCulling(RE::NiAVObject* a_object, std::int32_t a_alphaGroupIndex)
    {
        if (a_object->IsVisualObjectI()) {
            RE::NiFrustumPlanes::ActivePlane currentActivePlanes = planes.activePlanes.get();
            bool                             visible = true;
            for (int i = 0; i < RE::NiFrustumPlanes::Planes::kTotal; i++) {
                if (planes.activePlanes.all(static_cast<RE::NiFrustumPlanes::ActivePlane>(1 << i))) {
                    const auto& activePlane = planes.cullingPlanes[i];
                    const auto& worldBound = a_object->worldBound;

                    const float radius = activePlane.normal.x * worldBound.center.x + activePlane.normal.y * worldBound.center.y + activePlane.normal.z * worldBound.center.z - activePlane.constant;

                    if (radius <= -worldBound.radius) {
                        visible = false;
                        break;
                    }

                    if (radius >= worldBound.radius && enablePlaneOptimization)
                        planes.activePlanes.set(false, static_cast<RE::NiFrustumPlanes::ActivePlane>(1 << i));
                }
            }
            if (visible)
                a_object->OnVisible(*this, a_alphaGroupIndex);
            if (updateAccumulateFlag)
                a_object->flags.set(visible, RE::NiAVObject::Flag::kAccumulated);
            planes.activePlanes.set(currentActivePlanes);
        }
    }
}