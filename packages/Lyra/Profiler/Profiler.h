
#pragma once
#include <Tracy/Tracy.hpp>

#define ProfileFrameMark FrameMark

#define ProfileScoped ZoneScoped
#define ProfileScopedN ZoneScopedN
#define ProfileAddText(text, size) ZoneText(text, size)
