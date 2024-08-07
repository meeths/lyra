
#pragma once
#include "tracy/Tracy.hpp"

#define ProfileFrameMark FrameMark

#define ProfileScoped ZoneScoped
#define ProfileScopedN ZoneScopedN
#define ProfileScopedNC ZoneScopedNC
#define ProfileScopedC ZoneScopedC

#define ProfileScopeName(text, size) ZoneName(text, size)
#define ProfileAddText(text, size) ZoneText(text, size)

#define ProfileSetThreadName(x) tracy::SetThreadName(x)
