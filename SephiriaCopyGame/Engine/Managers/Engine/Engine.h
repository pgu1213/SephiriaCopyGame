#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>

class Engine : public SingleTon<Engine>
{
	friend class SingleTon<Engine>;
private:
	explicit Engine();
public:
	virtual ~Engine();
public:
	void Init();
	void LateInit();
	void Update();
	void Render(HDC hdc);
};