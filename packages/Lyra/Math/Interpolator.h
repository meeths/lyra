
#pragma once

// Created on 2019-02-26 by fmacias
#include <Math/Easings.h>
#include <Math/Functions.h>

namespace lyra
{
namespace Math
{
template<typename T>
class Interpolator
{
public:
	Interpolator() = default;
	void SetOrigin(const T& _origin) { m_Origin = _origin; };
	void SetTarget(const T& _target)
	{
		m_Target = _target;
	}
	T GetInterpolatedValue(float _factor, Math::Easings::Function _function = Math::Easings::Function::Linear) const
	{
		_factor = Math::Easings::Interpolate(_factor, _function);
		return m_Origin * (1.0f - _factor) + m_Target * _factor;
	}
private:
	T m_Origin;
	T m_Target;
};

template<typename T>
class StateSavingInterpolator : public Interpolator<T>
{
public:
	StateSavingInterpolator() = default;
	void IncrementFactor(float _delta, bool _saturate = true)
	{
		SetFactor(m_CurrentFactor + _delta, _saturate);
	}
	
	void SetFactor(float _factor, bool _saturate = true)
	{
		m_CurrentFactor = _factor;
		if (_saturate)
			m_CurrentFactor = Math::Min(1.0f, Math::Max(0.0f, m_CurrentFactor));
	}
	void SetTarget(const T& _target, bool _updateOrigin = true)
	{
		if (_updateOrigin)
		{
			SetOrigin(GetCurrentInterpolatedValue());
		}
		Interpolator<T>::SetTarget(_target);
		m_CurrentFactor = 0;
	}
	
	T GetCurrentInterpolatedValue() const { return Interpolator<T>::GetInterpolatedValue(m_CurrentFactor); }
private:
	float m_CurrentFactor = 0;
};
}
}

