#include "culling/BSCullingProcess.h"
#include "culling/NiCullingProcess.h"

namespace Culling
{
    void BSCullingProcess::hk_Process1(RE::NiAVObject* a_object, std::int32_t a_alphaGroupIndex)
    {
        const bool alwaysDraw = a_object->flags.all(RE::NiAVObject::Flag::kAlwaysDraw);
        bool       culled = false;

        if (!a_object->IsVisualObjectI() && !alwaysDraw)
            return;

        if (cullMode.get() == BSCPCullingType::kAllPass) {
            culled = false;
            // if (!recurseToGeometry) {
            //     AppendVirtual(*static_cast<RE::BSGeometry*>(a_object), a_alphaGroupIndex);
            //     return;
            // } else {
            //     a_object->OnVisible(*this, a_alphaGroupIndex);
            //     if (updateAccumulateFlag)
            //         a_object->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
            //     return;
            // }
        } else if (cullMode.get() == BSCPCullingType::kAllFail) {
            culled = true;
            // if (recurseToGeometry && updateAccumulateFlag) {
            //     a_object->flags.set(false, RE::NiAVObject::Flag::kAccumulated);
            // }
            // return;
        } else if (a_object->flags.all(RE::NiAVObject::Flag::kPreProcessedNode) && !ignorePreprocess && cullMode.get() != BSCPCullingType::kForceMultiBoundsNoUpdate) {
            const bool isNotVisible = a_object->flags.all(RE::NiAVObject::Flag::kNotVisible);
            if (!recurseToGeometry) {
                if (isNotVisible)
                    return;

                culled = false;
            } else if (isNotVisible) {
                culled = true;
            }
            culled = false;
            // if (!recurseToGeometry) {
            //     if (a_object->flags.all(RE::NiAVObject::Flag::kNotVisible)) {
            //         return;
            //     }
            //     // else {
            //     //     AppendVirtual(*static_cast<RE::BSGeometry*>(a_object), a_alphaGroupIndex);
            //     //     return;
            //     // }
            //
            // } else if (a_object->flags.all(RE::NiAVObject::Flag::kNotVisible)) {
            //     if (updateAccumulateFlag) {
            //         a_object->flags.set(false, RE::NiAVObject::Flag::kAccumulated);
            //     }
            //     return;
            // }
            // culled = false;
            // a_object->OnVisible(*this, a_alphaGroupIndex);
            // if (updateAccumulateFlag)
            //     a_object->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
            // return;
        } else if (compoundFrustum && compoundFrustum->freeOp && cullMode.get() != BSCPCullingType::kIgnoreMultiBounds) {
            if (!alwaysDraw) {
                RE::NiFrustumPlanes::ActivePlane planesActivePlanes = planes.activePlanes.get();
                std::uint32_t                    compoundFrustumActivePlanes[256];
                compoundFrustum->GetActivePlaneState(compoundFrustumActivePlanes);

                bool isVisible = (compoundFrustum->skipViewFrustum || TestBaseVisibility3(a_object->worldBound)) && compoundFrustum->Process(a_object);

                culled = !isVisible;
                // if (recurseToGeometry) {
                //     if (isVisible) {
                //         culled = false;
                //         // a_object->OnVisible(*this, a_alphaGroupIndex);
                //         // if (updateAccumulateFlag) {
                //         //     a_object->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
                //         // }
                //     } else if (updateAccumulateFlag) {
                //         culled = true;
                //         // a_object->flags.set(false, RE::NiAVObject::Flag::kAccumulated);
                //     }
                // } else if (isVisible) {
                //     AppendVirtual(*static_cast<RE::BSGeometry*>(a_object), a_alphaGroupIndex);
                // }

                compoundFrustum->SetActivePlaneState(compoundFrustumActivePlanes);
                planes.activePlanes.set(planesActivePlanes);
            } else {
                culled = false;
                // if (!recurseToGeometry) {
                //     AppendVirtual(*static_cast<RE::BSGeometry*>(a_object), a_alphaGroupIndex);
                //     return;
                // } else {
                //     a_object->OnVisible(*this, a_alphaGroupIndex);
                //     if (updateAccumulateFlag)
                //         a_object->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
                //     return;
                // }
            }
        }

        if (recurseToGeometry) {
            if (!alwaysDraw && planes.activePlanes.underlying()) {
                reinterpret_cast<Culling::NiCullingProcess*>(this)->hk_DoCulling(a_object, a_alphaGroupIndex);
            } else if (!culled) {
                a_object->OnVisible(*this, a_alphaGroupIndex);
                if (updateAccumulateFlag)
                    a_object->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
            } else if (updateAccumulateFlag) {
                a_object->flags.set(false, RE::NiAVObject::Flag::kAccumulated);
            }
        } else if (!culled || alwaysDraw || !planes.activePlanes.underlying() || TestBaseVisibility3(a_object->worldBound)) {
            AppendVirtual(*static_cast<RE::BSGeometry*>(a_object), a_alphaGroupIndex);
        }
    }

    void BSCullingProcess::AppendVirtual(RE::BSGeometry& a_visible, std::int32_t a_arg2)
    {
        using func_t = decltype(&BSCullingProcess::AppendVirtual);
        static REL::Relocation<func_t> func{ RELOCATION_ID(0, 76556) };
        func(this, a_visible, a_arg2);
    }

    bool BSCullingProcess::TestBaseVisibility3(const RE::NiBound& a_bound) const
    {
        using func_t = decltype(&BSCullingProcess::TestBaseVisibility3);
        static REL::Relocation<func_t> func{ RELOCATION_ID(0, 76564) };
        return func(this, a_bound);
    }
}