#pragma once

#include "RE/M/Main.h"

namespace Draw::DrawWorld {
    void hk_BuildSceneLists([[maybe_unused]] RE::Main* a_self);

    inline void InstallHooks() {
        REL::Relocation buildSceneLists { REL::ID(36643) };
        buildSceneLists.replace_func(0x96C, hk_BuildSceneLists);
    }
}