#pragma once
#include <Engine/Object/Component/Component.h>

class SpriteRenderer;

struct AnimationFrame
{
    std::wstring spriteName;  // 스프라이트 이름
    RECT sourceRect;          // 스프라이트 시트에서의 영역 (선택사항)
    float duration;           // 이 프레임의 지속 시간

    AnimationFrame(const std::wstring& name, float dur)
        : spriteName(name), duration(dur)
    {
        sourceRect = { 0, 0, 0, 0 }; // 전체 이미지 사용
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

    // 기본 생성자 추가 (std::map에서 필요)
    AnimationClip() : name(""), loop(true) {}

    AnimationClip(const std::string& clipName, bool isLoop = true)
        : name(clipName), loop(isLoop) {
    }
};

class Animation : public Component
{
public:
    Animation(Object* owner);
    Animation(const Animation& other); // 복사 생성자
    void OnDestroy() override;
protected:
    Animation* CloneImpl() const override;
public:
    void CopyFrom(const IPrototypeable* source) override;
    void Update(float deltaTime) override;

public:
    // 애니메이션 재생 제어
    void AddClip(const AnimationClip& clip);
    void PlayClip(const std::string& clipName);
    void Stop();
    void Pause();
    void Resume();

    // 애니메이션 상태
    bool IsPlaying() const { return m_IsPlaying; }
    bool IsPaused() const { return m_IsPaused; }
    std::string GetCurrentClipName() const { return m_CurrentClipName; }

    // 애니메이션 속도 조절
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