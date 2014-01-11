#pragma once

namespace toy {

class Application {
public:
	void init();
	void shutdown();
	bool work();
};

} // namespace toy