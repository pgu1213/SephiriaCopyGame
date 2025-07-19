#include <pch.h>
#include "Animation.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Managers/ResourceManager/ResourceManager.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>

Animation::Animation(Object* owner)
    : Component(owner)
	, m_pSpriteRenderer(nullptr)
    , m_CurrentClipName("")
    , m_CurrentFrameIndex(0)
    , m_CurrentFrameTime(0.0f)
    , m_Speed(1.0f)
    , m_IsPlaying(false)
	, m_IsPaused(false)
{
}

Animation::Animation(const Animation& other)
    : Component(other.m_pOwner)
    , m_pSpriteRenderer(other.m_pSpriteRenderer)
    , m_CurrentClipName(other.m_CurrentClipName)
    , m_CurrentFrameIndex(other.m_CurrentFrameIndex)
    , m_CurrentFrameTime(other.m_CurrentFrameTime)
    , m_Speed(other.m_Speed)
    , m_IsPlaying(other.m_IsPlaying)
    , m_IsPaused(other.m_IsPaused)
{
}

void Animation::OnDestroy()
{
}

Animation* Animation::CloneImpl() const
{
    return new Animation(*this);
}

void Animation::CopyFrom(const IPrototypeable* source)
{
    const Animation* other = dynamic_cast<const Animation*>(source);
    if (other)
    {
		m_pSpriteRenderer = other->m_pSpriteRenderer;
        m_CurrentClipName = other->m_CurrentClipName;
        m_CurrentFrameIndex = other->m_CurrentFrameIndex;
        m_CurrentFrameTime = other->m_CurrentFrameTime;
		m_Speed = other->m_Speed;
        m_IsPlaying = other->m_IsPlaying;
        m_IsPaused = other->m_IsPaused;
    }
}

void Animation::Update(float deltaTime)
{
    if (!m_IsPlaying || m_IsPaused || m_CurrentClipName.empty())
        return;

    // SpriteRenderer 참조 확인
    if (!m_pSpriteRenderer)
    {
        m_pSpriteRenderer = GetSpriteRenderer();
        if (!m_pSpriteRenderer)
            return;
    }

    auto clipIt = m_Clips.find(m_CurrentClipName);
    if (clipIt == m_Clips.end() || clipIt->second.frames.empty())
        return;

    AnimationClip& currentClip = clipIt->second;

    // 프레임 시간 업데이트
    m_CurrentFrameTime += deltaTime * m_Speed;

    // 현재 프레임의 지속 시간을 초과했는지 확인
    if (m_CurrentFrameTime >= currentClip.frames[m_CurrentFrameIndex].duration)
    {
        m_CurrentFrameTime = 0.0f;
        m_CurrentFrameIndex++;

        // 애니메이션 끝 처리
        if (m_CurrentFrameIndex >= currentClip.frames.size())
        {
            if (currentClip.loop)
            {
                m_CurrentFrameIndex = 0; // 루프
            }
            else
            {
                m_CurrentFrameIndex = currentClip.frames.size() - 1; // 마지막 프레임에 멈춤
                m_IsPlaying = false;
            }
        }

        // 새 프레임 적용
        UpdateCurrentFrame();
    }
}

void Animation::AddClip(const AnimationClip& clip)
{
    m_Clips[clip.name] = clip;
}

void Animation::PlayClip(const std::string& clipName)
{
    auto it = m_Clips.find(clipName);
    if (it == m_Clips.end())
    {
        std::cout << "Animation clip '" << clipName << "' not found!" << std::endl;
        return;
    }

    // 이미 같은 클립이 재생 중이면 리셋하지 않음
    if (m_CurrentClipName == clipName && m_IsPlaying)
        return;

    m_CurrentClipName = clipName;
    m_CurrentFrameIndex = 0;
    m_CurrentFrameTime = 0.0f;
    m_IsPlaying = true;
    m_IsPaused = false;

    // 첫 프레임 적용
    UpdateCurrentFrame();
}

void Animation::Stop()
{
    m_IsPlaying = false;
    m_IsPaused = false;
    m_CurrentFrameIndex = 0;
    m_CurrentFrameTime = 0.0f;
}

void Animation::Pause()
{
    m_IsPaused = true;
}

void Animation::Resume()
{
    m_IsPaused = false;
}

void Animation::UpdateCurrentFrame()
{
    if (!m_pSpriteRenderer)
    {
        m_pSpriteRenderer = GetSpriteRenderer();
        if (!m_pSpriteRenderer)
            return;
    }

    auto clipIt = m_Clips.find(m_CurrentClipName);
    if (clipIt == m_Clips.end() || m_CurrentFrameIndex >= clipIt->second.frames.size())
        return;

    const AnimationFrame& currentFrame = clipIt->second.frames[m_CurrentFrameIndex];

    // ResourceManager를 통해 스프라이트 가져오기
    Gdiplus::Bitmap* sprite = ResourceManager::GetInstance()->GetSprite(currentFrame.spriteName);
    if (sprite)
    {
        // SpriteRenderer에 스프라이트 설정
        m_pSpriteRenderer->SetSprite(sprite);

        // 소스 영역이 지정되어 있으면 설정
        if (currentFrame.sourceRect.right > 0 && currentFrame.sourceRect.bottom > 0)
        {
            m_pSpriteRenderer->SetSourceRect(currentFrame.sourceRect);
            m_pSpriteRenderer->SetUseSourceRect(true);
        }
        else
        {
            m_pSpriteRenderer->SetUseSourceRect(false);
        }
    }
}

SpriteRenderer* Animation::GetSpriteRenderer()
{
    if (!m_pOwner)
        return nullptr;

    return m_pOwner->GetComponent<SpriteRenderer>();
}