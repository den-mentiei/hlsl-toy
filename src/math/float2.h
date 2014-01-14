#pragma once

#include "math/types.h"

namespace toy {

Float2 float2(const float x, const float y) {
	Float2 v = { x, y };
	return v;
}

} // namespace toy