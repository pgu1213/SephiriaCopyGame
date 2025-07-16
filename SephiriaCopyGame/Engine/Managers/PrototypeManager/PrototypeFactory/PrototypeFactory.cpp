#include <pch.h>
#include "PrototypeFactory.h"
#include <Engine/Managers/PrototypeManager/PrototypeManager.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>

unique_ptr<Object> PrototypeFactory::CreateFromPrototype(const string& prototypeName)
{
    return PrototypeManager::GetInstance()->CreateFromPrototype(prototypeName);
}

void PrototypeFactory::CreateDefaultPrototypes()
{
    auto player = make_unique<Object>("Player");
    player->AddComponent<SpriteRenderer>();

    PrototypeManager::GetInstance()->RegisterPrototype("Player", move(player));
}
