#pragma once

class CObj;

template<typename T>
class CAbstractFactory
{
public:
	static CObj* CreateObj()
	{
		CObj* pObj = new T;
		pObj->Init();

		return pObj;
	}

	static CObj* CreateObj(float fX, float fY)
	{
		CObj* pObj = new T;
		pObj->Init();
		pObj->SetPos(fX, fY);

		return pObj;
	}

	static CObj* CreateObj(float fX, float fY, float fAngle)
	{
		CObj* pObj = new T;
		pObj->Init();
		pObj->SetPos(fX, fY);
		pObj->SetAngle(fAngle);

		return pObj;
	}

	static CObj* CreateObj(CObj* pTarget)
	{
		CObj* pObj = new T;
		pObj->Init();
		pObj->SetTarget(pTarget);

		return pObj;
	}
};