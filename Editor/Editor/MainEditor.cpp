#include "../pch.h"
#include "MainEditor.h"
#include "../Managers/InputManager/InputManager.h"
#include "../Managers/ResourceManager/ResourceManager.h"
#include "../Managers/ConfigManager/ConfigManager.h"

MainEditor::MainEditor()
{
}

MainEditor::~MainEditor()
{
}

void MainEditor::Initialize()
{
	// �Ŵ��� Init
	// Ÿ�� �� ������ Init ��
	if (!ResourceManager::GetInstance()->Init())
	{
		return;
	}
}

void MainEditor::Update()
{
	InputManager::GetInstance()->Update();
}

void MainEditor::Render()
{
	
}

void MainEditor::Release()
{
	if (m_pTileMapEditor)
	{
		delete m_pTileMapEditor;
		m_pTileMapEditor = nullptr;
	}
}