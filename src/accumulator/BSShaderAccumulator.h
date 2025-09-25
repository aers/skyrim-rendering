#pragma once
#include "RE/B/BSShaderAccumulator.h"

namespace Accumulator
{
    class BSShaderAccumulator final : public RE::BSShaderAccumulator
    {
    public:
        void hk_StartAccumulating(const RE::NiCamera* a_camera);

        static void InstallHooks()
        {
        }
    };

}
