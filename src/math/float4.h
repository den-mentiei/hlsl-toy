#pragma once

#include "math/types.h"

namespace toy {

Float4 float4(const float x, const float y, const float z, const float w) {
	Float4 v = { x, y, z, w };
	return v;
}

} // namespace toy