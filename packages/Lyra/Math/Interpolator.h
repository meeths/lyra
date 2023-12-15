
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
	void SetOrigin(const T& _origin) { mOrigin = _origin; };
	void SetTarget(const T& _target)
	{
		mTarget = _target;
	}
	T GetInterpolatedValue(float _factor, Math::Easings::Function _function = Math::Easings::Function::Linear) const
	{
		_factor = Math::Easings::Interpolate(_factor, _function);
		return mOrigin * (1.0f - _factor) + mTarget * _factor;
	}
private:
	T mOrigin;
	T mTarget;
};

template<typename T>
class StateSavingInterpolator : public Interpolator<T>
{
public:
	StateSavingInterpolator() = default;
	void IncrementFactor(float _delta, bool _saturate = true)
	{
		SetFactor(mCurrentFactor + _delta, _saturate);
	}
	
	void SetFactor(float _factor, bool _saturate = true)
	{
		mCurrentFactor = _factor;
		if (_saturate)
			mCurrentFactor = Math::Min(1.0f, Math::Max(0.0f, mCurrentFactor));
	}
	void SetTarget(const T& _target, bool _updateOrigin = true)
	{
		if (_updateOrigin)
		{
			SetOrigin(GetCurrentInterpolatedValue());
		}
		Interpolator<T>::SetTarget(_target);
		mCurrentFactor = 0;
	}
	
	T GetCurrentInterpolatedValue() const { return Interpolator<T>::GetInterpolatedValue(mCurrentFactor); }
private:
	float mCurrentFactor = 0;
};
}
}

