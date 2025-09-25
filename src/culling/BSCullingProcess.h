#pragma once
#include "RE/B/BSCullingProcess.h"

namespace Culling
{
    class BSCullingProcess final : public RE::BSCullingProcess
    {
    public:
        void hk_Process1(RE::NiAVObject* a_object, std::int32_t a_alphaGroupIndex);
        void hk_Process2(const RE::NiCamera* a_camera, RE::NiAVObject* a_scene, RE::NiVisibleArray* a_visibleSet);
        void hk_AppendVirtual(RE::BSGeometry& a_visible, std::int32_t a_alphaGroupIndex);
        void hk_AppendNonAccum(RE::NiAVObject& a_visible, std::int32_t a_alphaGroupIndex);
        bool hk_TestBaseVisibility1(const RE::BSMultiBound& a_bound) const;
        bool hk_TestBaseVisibility2(RE::BSOcclusionPlane& a_occlusionPlane) const;
        bool hk_TestBaseVisibility3(const RE::NiBound& a_bound);

        static void InstallHooks() {
            REL::Relocation _Process1 { RELOCATION_ID(0, 76553) };
            _Process1.replace_func(VAR_NUM(0, 0x2EB), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_Process1));

            REL::Relocation _Process2 { RELOCATION_ID(0, 76554) };
            _Process2.replace_func(VAR_NUM(0, 0x1CF), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_Process2));

            REL::Relocation _AppendVirtual { RELOCATION_ID(0, 76556) };
            _AppendVirtual.replace_func(VAR_NUM(0, 0x80), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_AppendVirtual));

            REL::Relocation _AppendNonAccum { RELOCATION_ID(0, 76555) };
            _AppendNonAccum.replace_func(VAR_NUM(0, 0x6A), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_AppendNonAccum));

            REL::Relocation _TestBaseVisibility1 { RELOCATION_ID(0, 76566) };
            _TestBaseVisibility1.replace_func(VAR_NUM(0, 0x15), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_TestBaseVisibility1));

            REL::Relocation _TestBaseVisibility2 { RELOCATION_ID(0,76565)};
            _TestBaseVisibility2.replace_func(VAR_NUM(0, 0x1A), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_TestBaseVisibility2));

            REL::Relocation _TestBaseVisibility3 { RELOCATION_ID(0, 76564)};
            _TestBaseVisibility3.replace_func(VAR_NUM(0, 0xC6), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_TestBaseVisibility3));
        }
    };
}