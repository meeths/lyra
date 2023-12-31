
#pragma once

// Created on 2020-06-22 by sisco
#include <Core/Types.h>

namespace lyra
{
namespace Math
{
template<typename T, size_t N>
class NumericalIntegrator
{
public:

	template<typename ...Ts>
	NumericalIntegrator(Ts... args) : m_Values{args...}
	{
		static_assert(sizeof...(args) <= N, "Numerical integrator initialized to incorrect number of values");
	};

	void Evaluate(float _delta)
	{
		for (auto it = m_Values.rbegin(); it != (m_Values.rend() - 1); ++it)
		{
			T& integral = *(it + 1);
			integral = integral + (*(it) * _delta);
		}
	};

	T GetDerivative(int _index) const { return m_Values[_index]; }
private:
	Array<T, N> m_Values;
};
}

}

