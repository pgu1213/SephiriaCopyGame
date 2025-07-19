#pragma once
#include <Engine/Object/Component/Component.h>

struct AnimationFrame
{
    wstring spriteName;  // 리소스 매니저에서 로드된 스프라이트 이름
    float duration;      // 해당 프레임의 지속 시간 (초)

    AnimationFrame(const wstring& name = L"", float dur = 0.1f)
        : spriteName(name), duration(dur) {
    }
};

struct AnimationClip
{
    wstring clipName;                    // 애니메이션 클립 이름
    vector<AnimationFrame> frames;       // 프레임들
    bool isLoop;                         // 반복 재생 여부

    AnimationClip(const wstring& name = L"", bool loop = true)
        : clipName(name), isLoop(loop) {
    }
};

class Animation : public Component
{
public:
    Animation(Object* owner);
    Animation(const Animation& other); // 복사 생성자
    virtual ~Animation();

protected:
    Animation* CloneImpl() const override;

public:
    void CopyFrom(const IPrototypeable* source) override;
    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

public:
    // 애니메이션 클립 관리
    void AddAnimationClip(const AnimationClip& clip);
    void AddAnimationClip(const wstring& clipName, const vector<wstring>& spriteNames,
        float frameDuration = 0.1f, bool isLoop = true);
    void RemoveAnimationClip(const wstring& clipName);

    // 애니메이션 재생 제어
    void PlayAnimation(const wstring& clipName, bool forceRestart = false);
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();

    // 애니메이션 상태 확인
    bool IsPlaying() const { return m_IsPlaying && !m_IsPaused; }
    bool IsPaused() const { return m_IsPaused; }
    bool IsAnimationFinished() const { return m_IsFinished; }
    wstring GetCurrentClipName() const { return m_CurrentClipName; }
    int GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

    // 애니메이션 설정
    void SetPlaybackSpeed(float speed) { m_PlaybackSpeed = speed; }
    float GetPlaybackSpeed() const { return m_PlaybackSpeed; }

    // 이벤트 콜백 설정
    void SetOnAnimationFinished(function<void()> callback) { m_OnAnimationFinished = callback; }
    void SetOnFrameChanged(function<void(int)> callback) { m_OnFrameChanged = callback; }

private:
    void UpdateAnimation(float deltaTime);
    void ChangeFrame(int frameIndex);
    AnimationClip* GetClip(const wstring& clipName);

private:
    map<wstring, AnimationClip> m_AnimationClips;  // 애니메이션 클립들
    wstring m_CurrentClipName;                     // 현재 재생 중인 클립 이름

    int m_CurrentFrameIndex;                       // 현재 프레임 인덱스
    float m_CurrentFrameTime;                      // 현재 프레임의 경과 시간
    float m_PlaybackSpeed;                         // 재생 속도 배율

    bool m_IsPlaying;                              // 재생 중인지 여부
    bool m_IsPaused;                               // 일시정지 상태인지 여부
    bool m_IsFinished;                             // 애니메이션이 끝났는지 여부

    // 이벤트 콜백
    function<void()> m_OnAnimationFinished;        // 애니메이션 완료 시 호출
    function<void(int)> m_OnFrameChanged;          // 프레임 변경 시 호출
};