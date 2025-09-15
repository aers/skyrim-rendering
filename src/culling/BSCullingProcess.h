#pragma once
#include "RE/B/BSCullingProcess.h"

namespace Culling
{
    class BSCullingProcess final : public RE::BSCullingProcess
    {
    public:
        void AppendVirtual(RE::BSGeometry& a_visible, std::int32_t a_arg2) override;
        [[nodiscard]] bool TestBaseVisibility3(const RE::NiBound& a_bound) const override;

        void hk_Process1(RE::NiAVObject* a_object, std::int32_t a_alphaGroupIndex);

        static void InstallHooks() {
            REL::Relocation _Process1 { RELOCATION_ID(0, 76553) };
            _Process1.replace_func(VAR_NUM(0, 0x2EB), SKSE::stl::unrestricted_cast<std::uintptr_t>(&BSCullingProcess::hk_Process1));
        }
    };
}