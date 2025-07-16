#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>

class MainManager : public SingleTon<MainManager>
{
	friend class SingleTon<MainManager>;
private:
	explicit MainManager();
public:
	virtual ~MainManager();
public:
	void Init();
	void LateInit();
	void Update(float DeltaTime);
	void Render(HDC hdc);
};

