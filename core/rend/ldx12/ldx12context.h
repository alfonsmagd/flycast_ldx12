#pragma once
#include "build.h"

#ifdef LIBRETRO
#include "ldx12context_lr.h"
#elif defined(_WIN32)
#include "wsi/context.h"

namespace ldx12
{
	class DeviceManager;
	class RenderDevice;
}

class LDX12Context : public GraphicsContext
{
public:
	static LDX12Context *Instance()
	{
		return static_cast<LDX12Context *>(GraphicsContext::Instance());
	}

	static void Create(void *window, void *display = nullptr);
	~LDX12Context() override;

	ldx12::DeviceManager& getDeviceManager() const;
	ldx12::RenderDevice& getDevice() const;
	void Present();
	void resize() override;
	std::string getDriverName() override;
	std::string getDriverVersion() override;
	bool isAMD() override;

	void setSwapInterval(int interval) override
	{
		gameSwapInterval = interval;
	}

	void setFrameRendered()
	{
		frameRendered = true;
	}

private:
	LDX12Context(void *window, void *display);
	bool init(bool keepCurrentWindow = false);
	void term();

	bool initialized = false;
	std::string adapterDesc;
	std::string adapterVersion;
	u32 vendorId = 0;
	bool frameRendered = false;
	int gameSwapInterval = 1;
};
#endif
