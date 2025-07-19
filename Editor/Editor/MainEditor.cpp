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
	// 매니저 Init
	// 타일 맵 에디터 Init 등
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