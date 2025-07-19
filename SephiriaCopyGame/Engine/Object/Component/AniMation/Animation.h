#pragma once
#include <Engine/Object/Component/Component.h>

struct AnimationFrame
{
    wstring spriteName;  // ���ҽ� �Ŵ������� �ε�� ��������Ʈ �̸�
    float duration;      // �ش� �������� ���� �ð� (��)

    AnimationFrame(const wstring& name = L"", float dur = 0.1f)
        : spriteName(name), duration(dur) {
    }
};

struct AnimationClip
{
    wstring clipName;                    // �ִϸ��̼� Ŭ�� �̸�
    vector<AnimationFrame> frames;       // �����ӵ�
    bool isLoop;                         // �ݺ� ��� ����

    AnimationClip(const wstring& name = L"", bool loop = true)
        : clipName(name), isLoop(loop) {
    }
};

class Animation : public Component
{
public:
    Animation(Object* owner);
    Animation(const Animation& other); // ���� ������
    virtual ~Animation();

protected:
    Animation* CloneImpl() const override;

public:
    void CopyFrom(const IPrototypeable* source) override;
    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

public:
    // �ִϸ��̼� Ŭ�� ����
    void AddAnimationClip(const AnimationClip& clip);
    void AddAnimationClip(const wstring& clipName, const vector<wstring>& spriteNames,
        float frameDuration = 0.1f, bool isLoop = true);
    void RemoveAnimationClip(const wstring& clipName);

    // �ִϸ��̼� ��� ����
    void PlayAnimation(const wstring& clipName, bool forceRestart = false);
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();

    // �ִϸ��̼� ���� Ȯ��
    bool IsPlaying() const { return m_IsPlaying && !m_IsPaused; }
    bool IsPaused() const { return m_IsPaused; }
    bool IsAnimationFinished() const { return m_IsFinished; }
    wstring GetCurrentClipName() const { return m_CurrentClipName; }
    int GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

    // �ִϸ��̼� ����
    void SetPlaybackSpeed(float speed) { m_PlaybackSpeed = speed; }
    float GetPlaybackSpeed() const { return m_PlaybackSpeed; }

    // �̺�Ʈ �ݹ� ����
    void SetOnAnimationFinished(function<void()> callback) { m_OnAnimationFinished = callback; }
    void SetOnFrameChanged(function<void(int)> callback) { m_OnFrameChanged = callback; }

private:
    void UpdateAnimation(float deltaTime);
    void ChangeFrame(int frameIndex);
    AnimationClip* GetClip(const wstring& clipName);

private:
    map<wstring, AnimationClip> m_AnimationClips;  // �ִϸ��̼� Ŭ����
    wstring m_CurrentClipName;                     // ���� ��� ���� Ŭ�� �̸�

    int m_CurrentFrameIndex;                       // ���� ������ �ε���
    float m_CurrentFrameTime;                      // ���� �������� ��� �ð�
    float m_PlaybackSpeed;                         // ��� �ӵ� ����

    bool m_IsPlaying;                              // ��� ������ ����
    bool m_IsPaused;                               // �Ͻ����� �������� ����
    bool m_IsFinished;                             // �ִϸ��̼��� �������� ����

    // �̺�Ʈ �ݹ�
    function<void()> m_OnAnimationFinished;        // �ִϸ��̼� �Ϸ� �� ȣ��
    function<void(int)> m_OnFrameChanged;          // ������ ���� �� ȣ��
};