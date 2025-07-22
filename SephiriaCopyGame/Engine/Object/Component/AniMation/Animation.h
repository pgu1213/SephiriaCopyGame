#pragma once
#include <Engine/Object/Component/Component.h>

class SpriteRenderer;

struct AnimationFrame
{
    wstring spriteName;  // ��������Ʈ �̸�
    RECT sourceRect;          // ��������Ʈ ��Ʈ������ ���� (���û���)
    float duration;           // �� �������� ���� �ð�

    AnimationFrame(const wstring& name, float dur)
        : spriteName(name), duration(dur)
    {
        sourceRect = { 0, 0, 0, 0 }; // ��ü �̹��� ���
    }

    AnimationFrame(const wstring& name, const RECT& rect, float dur)
        : spriteName(name), sourceRect(rect), duration(dur) {
    }
};

struct AnimationClip
{
    string name;
    vector<AnimationFrame> frames;
    bool loop;

    AnimationClip() : name(""), loop(true) {}

    AnimationClip(const string& clipName, bool isLoop = true)
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
    void PlayClip(const string& clipName);
    void Stop();
    void Pause();
    void Resume();

    // �ִϸ��̼� ����
    bool IsPlaying() const { return m_IsPlaying; }
    bool IsPaused() const { return m_IsPaused; }
    string GetCurrentClipName() const { return m_CurrentClipName; }

    // �ִϸ��̼� �ӵ� ����
    void SetSpeed(float speed) { m_Speed = speed; }
    float GetSpeed() const { return m_Speed; }

private:
    void UpdateCurrentFrame();
    SpriteRenderer* GetSpriteRenderer();

private:
    map<string, AnimationClip> m_Clips;
    SpriteRenderer* m_pSpriteRenderer;

    string m_CurrentClipName;
    int m_CurrentFrameIndex;
    float m_CurrentFrameTime;
    float m_Speed;
    bool m_IsPlaying;
    bool m_IsPaused;
};