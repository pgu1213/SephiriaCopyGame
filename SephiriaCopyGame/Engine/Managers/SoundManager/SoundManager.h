#pragma once
#include <Engine/Managers/SingletonManager/SingletonManager.h>

class SoundManager : public SingleTon<SoundManager>
{
	friend class SingleTon<SoundManager>;
private:
	explicit SoundManager();
public:
	virtual ~SoundManager();
public:
	enum ChannelID { BGM, PLAYER, MONSTER, EFFECT, END };

public:
	void Init();
	void Update();
	void Release();
public:
	void LoadSoundFile();
	void PlaySound(const TCHAR* pSoundKey, ChannelID eID);
	void PlayBGM(const TCHAR* pSoundKey);
	void StopSound(ChannelID eID);
	void StopAll();

public:
	//fmod�� �ý����� �Ѱ�
	FMOD_SYSTEM* m_pSystem;
	//���� ����� ���� ä��
	FMOD_CHANNEL* m_pChannelArr[END];
	map<const TCHAR*, FMOD_SOUND*>  m_MapSound;
};

