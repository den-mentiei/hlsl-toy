#pragma once

#include "math/types.h"

namespace toy {

Float3 float3(const float x, const float y, const float z) {
	Float3 v = { x, y, z };
	return v;
}

} // namespace toy