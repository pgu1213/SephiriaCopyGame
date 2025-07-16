#pragma once

template<typename T>
void SafeDelete(T& obj)
{
	if (obj)
	{
		delete obj;
		obj = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T& obj)
{
	if (obj)
	{
		delete[] obj;
		obj = nullptr;
	}
}