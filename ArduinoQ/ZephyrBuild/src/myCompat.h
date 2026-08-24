#include <new>
#include <cstddef>
#include <cstdint>

void* operator new(std::size_t, void* p) noexcept { return p; }
void* operator new[](std::size_t, void* p) noexcept { return p; }

void operator delete(void*, void*) noexcept {}
void operator delete[](void*, void*) noexcept {}

#include <Arduino.h>
#include <Arduino_RouterBridge.h>
BridgeClass Bridge(Serial);
BridgeMonitor<> Monitor(Bridge);
