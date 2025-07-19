#pragma once
#include <Engine/Object/Component/Component.h>

class SpriteRenderer;

struct AnimationFrame
{
    std::wstring spriteName;  // ��������Ʈ �̸�
    RECT sourceRect;          // ��������Ʈ ��Ʈ������ ���� (���û���)
    float duration;           // �� �������� ���� �ð�

    AnimationFrame(const std::wstring& name, float dur)
        : spriteName(name), duration(dur)
    {
        sourceRect = { 0, 0, 0, 0 }; // ��ü �̹��� ���
    }

    AnimationFrame(const std::wstring& name, const RECT& rect, float dur)
        : spriteName(name), sourceRect(rect), duration(dur) {
    }
};

struct AnimationClip
{
    std::string name;
    std::vector<AnimationFrame> frames;
    bool loop;

    // �⺻ ������ �߰� (std::map���� �ʿ�)
    AnimationClip() : name(""), loop(true) {}

    AnimationClip(const std::string& clipName, bool isLoop = true)
        : name(clipName), loop(isLoop) {
    }
};

class Animation : public Component
{
public:
    Animation(Object* owner);
    Animation(const Animation& other); // ���� ������
    void OnDestroy() override;
protected:
    Animation* CloneImpl() const override;
public:
    void CopyFrom(const IPrototypeable* source) override;
    void Update(float deltaTime) override;

public:
    // �ִϸ��̼� ��� ����
    void AddClip(const AnimationClip& clip);
    void PlayClip(const std::string& clipName);
    void Stop();
    void Pause();
    void Resume();

    // �ִϸ��̼� ����
    bool IsPlaying() const { return m_IsPlaying; }
    bool IsPaused() const { return m_IsPaused; }
    std::string GetCurrentClipName() const { return m_CurrentClipName; }

    // �ִϸ��̼� �ӵ� ����
    void SetSpeed(float speed) { m_Speed = speed; }
    float GetSpeed() const { return m_Speed; }

private:
    void UpdateCurrentFrame();
    SpriteRenderer* GetSpriteRenderer();

private:
    std::map<std::string, AnimationClip> m_Clips;
    SpriteRenderer* m_pSpriteRenderer;

    std::string m_CurrentClipName;
    int m_CurrentFrameIndex;
    float m_CurrentFrameTime;
    float m_Speed;
    bool m_IsPlaying;
    bool m_IsPaused;
};