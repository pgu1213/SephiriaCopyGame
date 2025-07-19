#include <pch.h>
#include "Animation.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Managers/ResourceManager/ResourceManager.h>

Animation::Animation(Object* owner)
    : Component(owner)
    , m_CurrentClipName(L"")
    , m_CurrentFrameIndex(0)
    , m_CurrentFrameTime(0.0f)
    , m_PlaybackSpeed(1.0f)
    , m_IsPlaying(false)
    , m_IsPaused(false)
    , m_IsFinished(false)
    , m_OnAnimationFinished(nullptr)
    , m_OnFrameChanged(nullptr)
{
}

Animation::Animation(const Animation& other)
    : Component(other.m_pOwner)
    , m_AnimationClips(other.m_AnimationClips)
    , m_CurrentClipName(other.m_CurrentClipName)
    , m_CurrentFrameIndex(other.m_CurrentFrameIndex)
    , m_CurrentFrameTime(other.m_CurrentFrameTime)
    , m_PlaybackSpeed(other.m_PlaybackSpeed)
    , m_IsPlaying(other.m_IsPlaying)
    , m_IsPaused(other.m_IsPaused)
    , m_IsFinished(other.m_IsFinished)
    , m_OnAnimationFinished(other.m_OnAnimationFinished)
    , m_OnFrameChanged(other.m_OnFrameChanged)
{
}

Animation::~Animation()
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
        m_AnimationClips = other->m_AnimationClips;
        m_CurrentClipName = other->m_CurrentClipName;
        m_CurrentFrameIndex = other->m_CurrentFrameIndex;
        m_CurrentFrameTime = other->m_CurrentFrameTime;
        m_PlaybackSpeed = other->m_PlaybackSpeed;
        m_IsPlaying = other->m_IsPlaying;
        m_IsPaused = other->m_IsPaused;
        m_IsFinished = other->m_IsFinished;
        m_OnAnimationFinished = other->m_OnAnimationFinished;
        m_OnFrameChanged = other->m_OnFrameChanged;
    }
}

void Animation::Update(float deltaTime)
{
    if (m_IsPlaying && !m_IsPaused)
    {
        UpdateAnimation(deltaTime);
    }
}

void Animation::Render(HDC hdc)
{
    if (!m_IsPlaying || m_CurrentClipName.empty())
        return;

    AnimationClip* currentClip = GetClip(m_CurrentClipName);
    if (!currentClip || currentClip->frames.empty() ||
        m_CurrentFrameIndex >= static_cast<int>(currentClip->frames.size()))
        return;

    // ���� �������� ��������Ʈ ��������
    const AnimationFrame& currentFrame = currentClip->frames[m_CurrentFrameIndex];
    Bitmap* sprite = ResourceManager::GetInstance()->GetSprite(currentFrame.spriteName);

    if (!sprite)
        return;

    // ������Ʈ�� Transform ���� ��������
    Transform transform = m_pOwner->GetTransform();

    // ��������Ʈ ������
    Graphics graphics(hdc);
    graphics.DrawImage(sprite,
        static_cast<int>(transform.position.x),
        static_cast<int>(transform.position.y),
        static_cast<int>(sprite->GetWidth() * transform.scale.x),
        static_cast<int>(sprite->GetHeight() * transform.scale.y));
}

void Animation::AddAnimationClip(const AnimationClip& clip)
{
    m_AnimationClips[clip.clipName] = clip;
}

void Animation::AddAnimationClip(const wstring& clipName, const vector<wstring>& spriteNames,
    float frameDuration, bool isLoop)
{
    AnimationClip clip(clipName, isLoop);

    for (const auto& spriteName : spriteNames)
    {
        clip.frames.emplace_back(spriteName, frameDuration);
    }

    m_AnimationClips[clipName] = clip;
}

void Animation::RemoveAnimationClip(const wstring& clipName)
{
    auto it = m_AnimationClips.find(clipName);
    if (it != m_AnimationClips.end())
    {
        // ���� ��� ���� Ŭ���̸� ����
        if (m_CurrentClipName == clipName)
        {
            StopAnimation();
        }
        m_AnimationClips.erase(it);
    }
}

void Animation::PlayAnimation(const wstring& clipName, bool forceRestart)
{
    AnimationClip* clip = GetClip(clipName);
    if (!clip || clip->frames.empty())
        return;

    // �̹� ���� �ִϸ��̼��� ��� ���̰� ���� ������� �ƴϸ� ����
    if (m_CurrentClipName == clipName && m_IsPlaying && !forceRestart)
        return;

    m_CurrentClipName = clipName;
    m_CurrentFrameIndex = 0;
    m_CurrentFrameTime = 0.0f;
    m_IsPlaying = true;
    m_IsPaused = false;
    m_IsFinished = false;

    // ù ��° ���������� ����
    ChangeFrame(0);
}

void Animation::StopAnimation()
{
    m_IsPlaying = false;
    m_IsPaused = false;
    m_IsFinished = false;
    m_CurrentFrameIndex = 0;
    m_CurrentFrameTime = 0.0f;
}

void Animation::PauseAnimation()
{
    if (m_IsPlaying)
    {
        m_IsPaused = true;
    }
}

void Animation::ResumeAnimation()
{
    if (m_IsPlaying && m_IsPaused)
    {
        m_IsPaused = false;
    }
}

void Animation::UpdateAnimation(float deltaTime)
{
    AnimationClip* currentClip = GetClip(m_CurrentClipName);
    if (!currentClip || currentClip->frames.empty())
        return;

    m_CurrentFrameTime += deltaTime * m_PlaybackSpeed;

    // ���� �������� ���� �ð� Ȯ��
    if (m_CurrentFrameIndex < static_cast<int>(currentClip->frames.size()))
    {
        const AnimationFrame& currentFrame = currentClip->frames[m_CurrentFrameIndex];

        if (m_CurrentFrameTime >= currentFrame.duration)
        {
            m_CurrentFrameTime = 0.0f;
            int nextFrame = m_CurrentFrameIndex + 1;

            // ������ �������� ���
            if (nextFrame >= static_cast<int>(currentClip->frames.size()))
            {
                if (currentClip->isLoop)
                {
                    // ���� �ִϸ��̼��̸� ó������
                    ChangeFrame(0);
                }
                else
                {
                    // ������ �ƴϸ� �ִϸ��̼� �Ϸ�
                    m_IsFinished = true;
                    m_IsPlaying = false;

                    if (m_OnAnimationFinished)
                    {
                        m_OnAnimationFinished();
                    }
                }
            }
            else
            {
                // ���� ����������
                ChangeFrame(nextFrame);
            }
        }
    }
}

void Animation::ChangeFrame(int frameIndex)
{
    AnimationClip* currentClip = GetClip(m_CurrentClipName);
    if (!currentClip || frameIndex < 0 || frameIndex >= static_cast<int>(currentClip->frames.size()))
        return;

    int previousFrame = m_CurrentFrameIndex;
    m_CurrentFrameIndex = frameIndex;

    // �������� ������ ����Ǿ����� �ݹ� ȣ��
    if (previousFrame != frameIndex && m_OnFrameChanged)
    {
        m_OnFrameChanged(frameIndex);
    }
}

AnimationClip* Animation::GetClip(const wstring& clipName)
{
    auto it = m_AnimationClips.find(clipName);
    return (it != m_AnimationClips.end()) ? &it->second : nullptr;
}