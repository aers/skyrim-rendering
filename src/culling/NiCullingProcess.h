#pragma once
#include "RE/N/NiCullingProcess.h"

namespace Culling {
    class NiCullingProcess final : public RE::NiCullingProcess {
    public:
        void hk_DoCulling(RE::NiAVObject *a_object, std::int32_t a_alphaGroupIndex);

        static void InstallHooks() {
            REL::Relocation _DoCulling { RELOCATION_ID(0, 71083) };
            _DoCulling.replace_func(VAR_NUM(0, 0x111), SKSE::stl::unrestricted_cast<std::uintptr_t>(&NiCullingProcess::hk_DoCulling));
        }
    };
}
