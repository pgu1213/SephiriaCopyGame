#pragma once
#include "Engine/Scene/Scene.h"

class GameScene : public Scene
{
public:
    void Init() override
    {
        // �⺻ ������Ÿ�� ����
        GameObjectFactory::CreateDefaultPrototypes();

        // �÷��̾� ����
        auto player = GameObjectFactory::CreateFromPrototype("Player");
        player->GetComponent<Transform>()->SetPosition(Vector2(100, 100));
        player->GetComponent<SpriteRenderer>()->SetTexture(
            ResourceManager::GetInstance()->LoadTexture("player.png")
        );
        AddGameObject(std::move(player));

        // ���� �� ����
        for (int i = 0; i < 5; ++i)
        {
            auto enemy = GameObjectFactory::CreateFromPrototype("Enemy");
            enemy->GetComponent<Transform>()->SetPosition(Vector2(200 + i * 50, 200));
            enemy->GetComponent<SpriteRenderer>()->SetTexture(
                ResourceManager::GetInstance()->LoadTexture("enemy.png")
            );
            AddGameObject(std::move(enemy));
        }
    }

    void SpawnProjectile(const Vector2& position, const Vector2& direction)
    {
        auto projectile = GameObjectFactory::CreateFromPrototype("Projectile");
        projectile->GetComponent<Transform>()->SetPosition(position);
        projectile->GetComponent<RigidBody>()->SetVelocity(direction * 300.0f);
        projectile->GetComponent<SpriteRenderer>()->SetTexture(
            ResourceManager::GetInstance()->LoadTexture("bullet.png")
        );
        AddGameObject(std::move(projectile));
    }
};