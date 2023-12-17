#pragma once

// Created on 2023-12-17 by sisco
#include <cstddef>

void* operator new(std::size_t count);

void operator delete(void* ptr) noexcept;