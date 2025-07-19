#pragma once
#include "../../pch.h"

template<typename T>
class SingleTon
{
protected:
	explicit SingleTon() = default;
	virtual ~SingleTon() = default;
public:
	static T* GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T;
		}

		return m_pInstance;
	}
	static BOOL DestoryInstance()
	{
		if (m_pInstance)
		{
			SafeDelete(m_pInstance);
		}

		return TRUE;
	}

private:
	static T* m_pInstance;
};

template<typename T>
T* SingleTon<T>::m_pInstance = nullptr;


