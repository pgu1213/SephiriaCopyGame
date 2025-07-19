#include "pch.h"
#include "InputManager.h"

InputManager::InputManager()
{
	ZeroMemory(m_PrevKeyStates, sizeof(m_PrevKeyStates));
	ZeroMemory(m_KeyStates, sizeof(m_KeyStates));

	ZeroMemory(&m_MousePos, sizeof(POINT));
}

InputManager::~InputManager()
{
	
}

void InputManager::Update()
{
	memcpy(m_PrevKeyStates, m_KeyStates, sizeof(m_KeyStates));

	for (int i = 0; i < 256; ++i)
	{
		m_KeyStates[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
	}

	GetCursorPos(&m_MousePos);

	ScreenToClient(g_hWnd, &m_MousePos);
}

bool InputManager::IsKeyPressed(int iKey)
{
	return m_KeyStates[iKey];
}

bool InputManager::IsKeyDown(int iKey)
{
	return !m_PrevKeyStates[iKey] && m_KeyStates[iKey];
}

bool InputManager::IsKeyUp(int iKey)
{
	return m_PrevKeyStates[iKey] && !m_KeyStates[iKey];
}

POINT InputManager::GetMousePosition()
{
	return m_MousePos;
}
