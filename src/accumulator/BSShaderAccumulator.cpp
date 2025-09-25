#include "BSShaderAccumulator.h"

#include "RE/B/BSShaderAccumulator.h"

#include <cstdint>

namespace Accumulator {
    void BSShaderAccumulator::hk_StartAccumulating(const RE::NiCamera* a_camera) {
        camera = a_camera;
    }
}
