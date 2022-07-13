#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#include <string>
#include <memory>

#include "gbcpu.h"
#include "gbinput.h"
#include "gbgpu.h"
#include "gbtimer.h"
#include "SDLWindow.h"
#include "gb.h"

Rom* read_rom(const char* file) {
	HANDLE hRom = CreateFileA(
		file,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hRom == INVALID_HANDLE_VALUE) {
		// Retrieve the system error message for the last-error code

		LPTSTR lpMsgBuf;
		LPTSTR lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		// Display the error message and exit the process

		lpDisplayBuf = (LPTSTR)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf) + 40 * sizeof(TCHAR)));

		if (lpDisplayBuf == nullptr) {
			return nullptr;
		}

		StringCchPrintf((LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("failed with error %d: %s"),
			dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);

		return nullptr;
	}

	DWORD fileSize = GetFileSize(hRom, NULL);
	UINT8* romBuffer = new UINT8[fileSize];
	DWORD bytesRead;

	if (!ReadFile(
		hRom,
		romBuffer,
		fileSize,
		&bytesRead,
		nullptr
	))
	{
		OutputDebugString(_T("Failed to read rom\n"));
		CloseHandle(hRom);
		return nullptr;
	}

	Rom* rom = createRom(romBuffer, fileSize);
	CloseHandle(hRom);
	return rom;
}

void printHeader(Rom* rom) {
	const CartHeader* hdr = rom->getHeader();
	char str[17]{ 0 }; // 16 char max + \0

	// TODO: Smarter title
	strncpy_s(str, hdr->title, 17);

	debugPrint("RomName: %s\nRomType: %s\n",
		str,
		rom->getType().c_str()
	);
}

void handleSDLkey(SDL_KeyboardEvent& event, bool down) {
	gbInputType input;
	switch (event.keysym.sym) {
	case SDLK_w:
	case SDLK_UP:
		input = gbInputType::UP;
		break;
	case SDLK_s:
	case SDLK_DOWN:
		input = gbInputType::DOWN;
		break;
	case SDLK_a:
	case SDLK_LEFT:
		input = gbInputType::LEFT;
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		input = gbInputType::RIGHT;
		break;
	case SDLK_z:
		input = gbInputType::START;
		break;
	case SDLK_x:
		input = gbInputType::SELECT;
		break;
	case SDLK_q:
		input = gbInputType::A;
		break;
	case SDLK_e:
		input = gbInputType::B;
		break;
	default: return;
	}

	g_gb->input->setKey(input, down);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	SDL_Event event;
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
	SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
		debugPrint("Could not init SDL: %\n", SDL_GetError());
		return 1;
	}

	int res = SDL_GetDisplayMode(0, 0, &mode);
	if (res) return 1;

	debugPrint("%d FPS - %d cycle per frame\n", mode.refresh_rate, GB_HZ / mode.refresh_rate);
	int cyclesPerFrame = GB_HZ / mode.refresh_rate;

	Rom* rom = read_rom("Pokemon Blue.gb");
	//Rom* rom = read_rom("07-jr,jp,call,ret,rst.gb");
	//Rom* rom = read_rom("cpu_instrs.gb");
	//Rom* rom = read_rom("01-special.gb");
	//Rom* rom = read_rom("tetris.gb");
	if (rom == nullptr) return 1;

	printHeader(rom);

	g_gb = new GameboyEmu(rom);
	sdlWindow mgr(rom->getHeader()->title);

	UINT64 lastUpdateTime = SDL_GetPerformanceCounter();
	UINT64 deltaCounter;
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) 
				running = false;
			if (event.type == SDL_KEYDOWN) 
				handleSDLkey(event.key, true);
			if (event.type == SDL_KEYUP)
				handleSDLkey(event.key, false);
		}

		UINT64 currentTime = SDL_GetPerformanceCounter();
		deltaCounter = currentTime - lastUpdateTime;
		float elapsed = deltaCounter / (float)SDL_GetPerformanceFrequency();

		int cyclesLeft = (int) (GB_HZ * elapsed);
		while (cyclesLeft > 0) {
			cyclesLeft -= 4;
			g_gb->step();
		}

		mgr.present();
		lastUpdateTime = currentTime;
	}

	return 0;
}