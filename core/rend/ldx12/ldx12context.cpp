#include "ldx12context.h"

#if defined(_WIN32) && !defined(LIBRETRO)
#include "Ldx12/Ldx12.hpp"
#include "Ldx12/Ldx12Native.hpp"
#include "cfg/option.h"
#include <algorithm>
#include <exception>
#ifdef USE_SDL
#include "sdl/sdl.h"
HWND getNativeHwnd();
#endif
void LDX12Context::Create(void *window, void *display)
{
	new LDX12Context(window, display);
}

LDX12Context::LDX12Context(void *window, void *display)
	: GraphicsContext(window, display)
{
	if (!init())
		throw FlycastException("LDX12 initialization failed");
}

LDX12Context::~LDX12Context()
{
	term();
}

bool LDX12Context::init(bool keepCurrentWindow)
{
	NOTICE_LOG(RENDERER, "LDX12 Context initializing");
	if (initialized)
		return true;

#ifdef USE_SDL
	if (!keepCurrentWindow && !sdl_recreate_window(0))
		return false;
	setWindow(getNativeHwnd(), display);
#else
	(void)keepCurrentWindow;
#endif

	HWND hwnd = static_cast<HWND>(window);
	RECT clientRect{};
	if (!IsWindow(hwnd) || !GetClientRect(hwnd, &clientRect))
		return false;

	ldx12::ContextDesc contextDesc{};
	contextDesc.enableDebugLayer = true;
	contextDesc.preferHighPerformanceAdapter = true;
	contextDesc.allowTearing = true;
	contextDesc.swapchainBufferCount = 3;
	contextDesc.swapchainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ldx12::SwapchainDesc swapchainDesc{};
	swapchainDesc.window = ldx12::MakeWin32WindowHandle(hwnd);
	swapchainDesc.width = static_cast<u32>(std::max<LONG>(1, clientRect.right - clientRect.left));
	swapchainDesc.height = static_cast<u32>(std::max<LONG>(1, clientRect.bottom - clientRect.top));
	swapchainDesc.vsync = config::VSync && !settings.input.fastForwardMode;

	ldx12::DeviceManager::Initialize(contextDesc, swapchainDesc);
	// TODO: GetVendor id and version driver.
	initialized = true;
	return true;
}

void LDX12Context::term()
{
	if (initialized)
	{
		ldx12::DeviceManager::ShutdownSingleton();
		initialized = false;
	}
	frameRendered = false;
	adapterDesc.clear();
	adapterVersion.clear();
	vendorId = 0;
}

void LDX12Context::resize()
{
	if (!initialized)
		return;
	RECT clientRect{};
	if (!GetClientRect(static_cast<HWND>(window), &clientRect))
		return;
	const LONG width = clientRect.right - clientRect.left;
	const LONG height = clientRect.bottom - clientRect.top;
	if (width <= 0 || height <= 0)
		return;
	try
	{
		ldx12::DeviceManager& manager = getDeviceManager();
		if (manager.GetWidth() != static_cast<u32>(width) || manager.GetHeight() != static_cast<u32>(height))
			manager.Resize(static_cast<u32>(width), static_cast<u32>(height));
		settings.display.width = manager.GetWidth();
		settings.display.height = manager.GetHeight();
		frameRendered = false;
	}
	catch (const std::exception& e)
	{
		WARN_LOG(RENDERER, "LDX12 resize failed: %s", e.what());
	}
}

std::string LDX12Context::getDriverName()
{
	return adapterDesc;
}

std::string LDX12Context::getDriverVersion()
{
	return adapterVersion;
}

bool LDX12Context::isAMD()
{
	return vendorId == 0x1002 || vendorId == 0x1022;
}

ldx12::DeviceManager& LDX12Context::getDeviceManager() const
{
	return ldx12::DeviceManager::Get();
}

ldx12::RenderDevice& LDX12Context::getDevice() const
{
	return *getDeviceManager().GetRenderDevice();
}
#endif
