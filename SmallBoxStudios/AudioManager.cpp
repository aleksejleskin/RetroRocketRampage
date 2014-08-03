#include "AudioManager.h"

XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[30] =
{
	XAUDIO2FX_I3DL2_PRESET_FOREST,
	XAUDIO2FX_I3DL2_PRESET_DEFAULT,
	XAUDIO2FX_I3DL2_PRESET_GENERIC,
	XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
	XAUDIO2FX_I3DL2_PRESET_ROOM,
	XAUDIO2FX_I3DL2_PRESET_BATHROOM,
	XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
	XAUDIO2FX_I3DL2_PRESET_STONEROOM,
	XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
	XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
	XAUDIO2FX_I3DL2_PRESET_CAVE,
	XAUDIO2FX_I3DL2_PRESET_ARENA,
	XAUDIO2FX_I3DL2_PRESET_HANGAR,
	XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
	XAUDIO2FX_I3DL2_PRESET_HALLWAY,
	XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
	XAUDIO2FX_I3DL2_PRESET_ALLEY,
	XAUDIO2FX_I3DL2_PRESET_CITY,
	XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
	XAUDIO2FX_I3DL2_PRESET_QUARRY,
	XAUDIO2FX_I3DL2_PRESET_PLAIN,
	XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
	XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
	XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
	XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
	XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
	XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
	XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
	XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
	XAUDIO2FX_I3DL2_PRESET_PLATE,
};

struct ImplData
{
	ImplData::ImplData() : g_sourceVoice(nullptr)
	{

	}

	~ImplData()
	{
		if (g_sourceVoice)
		{
			g_sourceVoice->DestroyVoice();
			delete g_sourceVoice;
			g_sourceVoice = nullptr;
		}
	}

	Wave g_audioData;
	IXAudio2SourceVoice* g_sourceVoice;
};

AudioManager::AudioManager() : m_audioEngine(0), m_masteringVoice(0), m_musicSubmixes(0),
	m_sfxChannelCount(0), m_musicChannelCount(0)
{
	nFrameToApply3DAudio = 0;
}
AudioManager::~AudioManager()
{
}

bool AudioManager::LoadContent( int sfxChannelCount, int musicChannelCount )
{
	//ZeroMemory(&m_3dSounds, sizeof(Audio3dInfo));
	m_sfxChannelCount = sfxChannelCount;
	m_musicChannelCount = musicChannelCount;

	

	if( !LoadSubmixes() )
	{
		return false;
	}


	//details of the current audio device, hardware 
	XAUDIO2_DEVICE_DETAILS details;
	m_audioEngine->GetDeviceDetails(0, &details);
	m_sound3D.nChannels = details.OutputFormat.Format.nChannels;
	m_sound3D.dwChannelMask = details.OutputFormat.dwChannelMask;

	//initialize 3dsound Handler
	float SPEEDOFSOUND = 343.5f;
	X3DAudioInitialize(details.OutputFormat.dwChannelMask, SPEEDOFSOUND, m_audio3dHandle);




	//SETUP LISTENER ///initial values
	m_sound3D.Emitter.push_back(Audio3dEmitter());
	m_sound3D.Listener.Position = floatToa3dfloat(XMFLOAT3(0, 0, 0));
	m_sound3D.Listener.OrientFront = floatToa3dfloat(XMFLOAT3(0, 0, 1));
	m_sound3D.Listener.OrientTop = floatToa3dfloat(XMFLOAT3(0, 1, 0));
	//Cone values
	m_sound3D.Listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

	//EMITTER
	m_sound3D.Emitter[0].m_emitter.pCone = &m_sound3D.emitterCone;
	m_sound3D.Emitter[0].m_emitter.pCone->InnerAngle = 0.0f;
	// Setting the inner cone angles to X3DAUDIO_2PI and
	// outer cone other than 0 causes
	// the emitter to act like a point emitter using the
	// INNER cone settings only.
	m_sound3D.Emitter[0].m_emitter.pCone->OuterAngle = 0.0f;
	// Setting the outer cone angles to zero causes
	// the emitter to act like a point emitter using the
	// OUTER cone settings only.
	m_sound3D.Emitter[0].m_emitter.pCone->InnerVolume = 0.0f;
	m_sound3D.Emitter[0].m_emitter.pCone->OuterVolume = 1.0f;
	m_sound3D.Emitter[0].m_emitter.pCone->InnerLPF = 0.0f;
	m_sound3D.Emitter[0].m_emitter.pCone->OuterLPF = 1.0f;
	m_sound3D.Emitter[0].m_emitter.pCone->InnerReverb = 0.0f;
	m_sound3D.Emitter[0].m_emitter.pCone->OuterReverb = 1.0f;

	m_sound3D.Emitter[0].m_emitter.Position = floatToa3dfloat(XMFLOAT3(0, 0, 5));
	m_sound3D.Emitter[0].m_emitter.OrientFront = floatToa3dfloat(XMFLOAT3(0, 0, 1));
	m_sound3D.Emitter[0].m_emitter.OrientTop = floatToa3dfloat(XMFLOAT3(0, 1, 0));
	m_sound3D.Emitter[0].m_emitter.ChannelCount = INPUTCHANNELS;
	m_sound3D.Emitter[0].m_emitter.ChannelRadius = 1.0f;
	m_sound3D.Emitter[0].m_emitter.pChannelAzimuths = m_sound3D.emitterAzimuths;

	// Use of Inner radius allows for smoother transitions as
	// a sound travels directly through, above, or below the listener.
	// It also may be used to give elevation cues.
	m_sound3D.Emitter[0].m_emitter.InnerRadius = 2.0f;
	m_sound3D.Emitter[0].m_emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;;

	m_sound3D.Emitter[0].m_emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
	m_sound3D.Emitter[0].m_emitter.pLFECurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
	m_sound3D.Emitter[0].m_emitter.pLPFDirectCurve = NULL; // use default curve
	m_sound3D.Emitter[0].m_emitter.pLPFReverbCurve = NULL; // use default curve
	m_sound3D.Emitter[0].m_emitter.pReverbCurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
	m_sound3D.Emitter[0].m_emitter.CurveDistanceScaler = 14.0f;
	m_sound3D.Emitter[0].m_emitter.DopplerScaler = 1.0f;

	//DPS
	m_sound3D.Emitter[0].dspSettings.SrcChannelCount = INPUTCHANNELS;
	m_sound3D.Emitter[0].dspSettings.DstChannelCount = m_sound3D.nChannels;
	m_sound3D.Emitter[0].dspSettings.pMatrixCoefficients = m_sound3D.matrixCoefficients;









	if (!LoadAudio())
	{
		return false;
	}



	////LOOP SOUND
	//XAUDIO2_BUFFER playerBuffer = { 0 };
	//shared_ptr<ImplData> soundData = m_3dTrackList[sound3dIndexMap["3ds_Pboost.wav"]];
	//playerBuffer.AudioBytes = soundData->g_audioData.xaBuffer()->AudioBytes;
	//playerBuffer.pAudioData = soundData->g_audioData.xaBuffer()->pAudioData;
	//playerBuffer.Flags = XAUDIO2_END_OF_STREAM;
	//playerBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	////Get rid of what ever was queued to play
	////soundData->g_sourceVoice->FlushSourceBuffers();

	//soundData->g_sourceVoice->SubmitSourceBuffer(&playerBuffer);

	//soundData->g_sourceVoice->Start(0);


	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	return true;
}

bool AudioManager::LoadAudio()
{
	//Create all the layers of sfx for the different players
	for(int i = 0; i < m_sfxChannelCount; i++)
	{
		vector<shared_ptr<ImplData>> sfxVec;
		m_sfxTrackList.push_back(sfxVec);
	}
	for(int m = 0; m < m_musicChannelCount; m++)
	{
		vector<shared_ptr<ImplData>> musicVev;
		m_musicTrackList.push_back(musicVev);
	}

	string path = "Data\\Audio\\";
	string searchPattern = "*.wav";
	string fullSearchPath = path + searchPattern;
	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	hFind = FindFirstFile(fullSearchPath.c_str(), &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		errorOutput << "Unable to find directory: " << path;
		string o = errorOutput.str();
		MessageBoxA(NULL, o.c_str(), "ERROR", MB_OK | MB_ICONERROR);
	}

	do
	{
		//convert to sent the path
		string filePath = path + FindData.cFileName;
		char *cstr = new char[filePath.length() + 1];
		strcpy(cstr, filePath.c_str());
		
		ifstream in(filePath.c_str());
		if (in)
		{
			string filecheck = cstr;

			//Check to see if the file is of type music and add it
			if(filecheck.find("music_") != string::npos)
			{
				musicIndexMap.insert(std::map<string, int >::value_type(FindData.cFileName, musicIndexMap.size()));
				AddMusic(cstr);
			}
			//Check to see if the file is of type sfx and add it
			if(filecheck.find("sfx_") != string::npos)
			{
				sfxIndexMap.insert(std::map<string, int >::value_type(FindData.cFileName, sfxIndexMap.size()));
				AddSFX(cstr);
			}

			//Check to see if the file is of type sfx and add it
			if (filecheck.find("3ds_") != string::npos)
			{
				sound3dIndexMap.insert(std::map<string, int >::value_type(FindData.cFileName, sound3dIndexMap.size()));
				Add3dSound(cstr);
			}
		}
		else
		{
			errorOutput << "Problem opening file: " << FindData.cFileName;
			string o = errorOutput.str();
			MessageBoxA(NULL, o.c_str(), "ERROR", MB_OK | MB_ICONERROR);
		}


	} while (FindNextFile(hFind, &FindData) > 0);


	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		//   cout << "Something went wrong during searching\n";
	}
	return true;;
}

void AudioManager::AddMusic(char* audioFilename)
{
	for(int p = 0; p < m_musicChannelCount; p++)
	{
		//assign it to its effect
		XAUDIO2_SEND_DESCRIPTOR sendMusic[2];
		sendMusic[0].Flags = XAUDIO2_SEND_USEFILTER;
		sendMusic[0].pOutputVoice = m_masteringVoice;
		sendMusic[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
		sendMusic[1].pOutputVoice = m_musicSubmixes[p];

		XAUDIO2_VOICE_SENDS sendMusicList = { 2, sendMusic };

		//Load the WAV file into the implData struct
		shared_ptr<ImplData> musicTrack(new ImplData());
		musicTrack->g_audioData.load(audioFilename);

		//Create the source voice for the implData struct
		m_audioEngine->CreateSourceVoice(&musicTrack->g_sourceVoice,
			musicTrack->g_audioData.wf(),
			0,
			XAUDIO2_DEFAULT_FREQ_RATIO,
			nullptr,
			&sendMusicList,
			nullptr);

		m_musicTrackList[p].push_back(musicTrack);
	}
}

void AudioManager::Add3dSound(char* audioFilename)
{
		//assign it to its effect
		XAUDIO2_SEND_DESCRIPTOR sendMusic[2];
		sendMusic[0].Flags = XAUDIO2_SEND_USEFILTER;
		sendMusic[0].pOutputVoice = m_masteringVoice;
		sendMusic[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
		sendMusic[1].pOutputVoice = m_3dSubmix;

		XAUDIO2_VOICE_SENDS sendMusicList = { 2, sendMusic };

		//Load the WAV file into the implData struct
		shared_ptr<ImplData> musicTrack(new ImplData());
		musicTrack->g_audioData.load(audioFilename);

		//Create the source voice for the implData struct
		m_audioEngine->CreateSourceVoice(&musicTrack->g_sourceVoice,
			musicTrack->g_audioData.wf(),
			0,
			XAUDIO2_DEFAULT_FREQ_RATIO,
			nullptr,
			&sendMusicList,
			nullptr);

		m_3dTrackList.push_back(musicTrack);
}

void AudioManager::AddSFX(char* audioFilename)
{
	for(int p = 0; p < m_sfxChannelCount; p++)
	{
		//assign it to its effect
		XAUDIO2_SEND_DESCRIPTOR sendSFX[2];
		sendSFX[0].Flags = XAUDIO2_SEND_USEFILTER;
		sendSFX[0].pOutputVoice = m_masteringVoice;
		sendSFX[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
		sendSFX[1].pOutputVoice = m_effectSubmixes[p];

		XAUDIO2_VOICE_SENDS sendSFXList = { 2, sendSFX };

		//Load the WAV file into the implData struct
		shared_ptr<ImplData> sfxTrack(new ImplData());
		sfxTrack->g_audioData.load(audioFilename);

		//Create the source voice for the implData struct
		m_audioEngine->CreateSourceVoice(&sfxTrack->g_sourceVoice,
			sfxTrack->g_audioData.wf(),
			0,
			2.0f,
			NULL,
			&sendSFXList);

		m_sfxTrackList[p].push_back(sfxTrack);
	}
}

bool AudioManager::PlayMusic(char* audioFilename, int effectIndex, float volume)
{
	if(effectIndex > m_musicChannelCount)
	{
		return false;
	}

	//Create the audio buffer locally
	XAUDIO2_BUFFER playerBuffer = { 0 };
	shared_ptr<ImplData> soundData = m_musicTrackList[effectIndex][musicIndexMap[audioFilename]];
	playerBuffer.AudioBytes = soundData->g_audioData.xaBuffer()->AudioBytes;
	playerBuffer.pAudioData = soundData->g_audioData.xaBuffer()->pAudioData;
	playerBuffer.Flags = XAUDIO2_END_OF_STREAM;
	//Get rid of what ever was queued to play
	soundData->g_sourceVoice->FlushSourceBuffers();

	HRESULT hr = soundData->g_sourceVoice->SubmitSourceBuffer(&playerBuffer);
	if(FAILED(hr))
	{
		return false;
	}
	if(SUCCEEDED(hr))
	{
		hr = soundData->g_sourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
		//m_musicSubmixes[effectIndex]->SetVolume( volume, XAUDIO2_COMMIT_NOW );
		return true;
	}

	return true;
}

bool AudioManager::PlaySFX(char* audioFilename, int effectIndex, float volume)
{
	if(effectIndex > m_sfxChannelCount)
	{
		return false;
	}

	//Create the audio buffer locally
	XAUDIO2_BUFFER playerBuffer = { 0 };
	shared_ptr<ImplData> soundData = m_sfxTrackList[effectIndex][sfxIndexMap[audioFilename]];
	playerBuffer.AudioBytes = soundData->g_audioData.xaBuffer()->AudioBytes;
	playerBuffer.pAudioData = soundData->g_audioData.xaBuffer()->pAudioData;
	playerBuffer.Flags = XAUDIO2_END_OF_STREAM;

	//Get rid of what ever was queued to play
	soundData->g_sourceVoice->FlushSourceBuffers();

	HRESULT hr = soundData->g_sourceVoice->SubmitSourceBuffer(&playerBuffer);
	if(FAILED(hr))
	{
		return false;
	}
	if(SUCCEEDED(hr))
	{
		hr = soundData->g_sourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
		m_effectSubmixes[effectIndex]->SetVolume( volume, XAUDIO2_COMMIT_NOW );
		return true;
	}

	return true;
}

bool AudioManager::StopMusic(char* audioFilename, int effectIndex)
{
	if(effectIndex > m_musicChannelCount)
	{
		return false;
	}

	//get the correct sound data and stop it
	shared_ptr<ImplData> soundData = m_musicTrackList[effectIndex][musicIndexMap[audioFilename]];

	HRESULT hr = soundData->g_sourceVoice->Stop();
	if(FAILED(hr))
	{
		return false;
	}

	soundData->g_sourceVoice->FlushSourceBuffers();

	return true;
}

bool AudioManager::StopSFX(char* audioFilename, int effectIndex)
{
	shared_ptr<ImplData> soundData = m_sfxTrackList[effectIndex][sfxIndexMap[audioFilename]];

	HRESULT hr = soundData->g_sourceVoice->Stop();
	if(FAILED(hr))
	{
		return false;
	}

	soundData->g_sourceVoice->FlushSourceBuffers();

	return true;
}

bool AudioManager::IsSFXPlaying()
{
	for(int p = 0; p < m_sfxChannelCount; p++)
	{
		for each(shared_ptr<ImplData> Data in m_sfxTrackList[p])
		{
			XAUDIO2_VOICE_STATE sfxState;
			Data->g_sourceVoice->GetState(&sfxState);

			if(sfxState.BuffersQueued > 0)
			{
				return true;
			}
		}
	}

	return false;
}

bool AudioManager::IsMusicPlaying()
{
	for(int m = 0; m < m_musicChannelCount; m++)
	{
		for each(shared_ptr<ImplData> Data in m_musicTrackList[m])
		{
			XAUDIO2_VOICE_STATE sfxState;
			Data->g_sourceVoice->GetState(&sfxState);

			if(sfxState.BuffersQueued > 0)
			{
				return true;
			}
		}
	}

	return true;
}

bool AudioManager::StopAllSFX()
{
	for(int s = 0; s < m_sfxChannelCount; s++)
	{
		for each(shared_ptr<ImplData> sfx in m_sfxTrackList[s])
		{

			HRESULT hr = sfx->g_sourceVoice->Stop();
			if(FAILED(hr))
			{
				return false;
			}

			sfx->g_sourceVoice->FlushSourceBuffers();
		}
	}

	return true;
}

bool AudioManager::StopAllMusic( )
{
	for(int m = 0; m < m_musicChannelCount; m++)
	{
		for each(shared_ptr<ImplData> music in m_musicTrackList[m])
		{
			HRESULT hr = music->g_sourceVoice->Stop();
			if(FAILED(hr))
			{
				return false;
			}

			music->g_sourceVoice->FlushSourceBuffers();
		}
	}

	return true;
}

void AudioManager::UnloadAudio()
{
	//Stop all current playing sounds and flush the buffers
	StopAllMusic();
	StopAllSFX();

	for(unsigned int e = 0; e < (unsigned)m_musicChannelCount; e++)
	{
		//Deallocate the effect submixes per player
		if (m_musicSubmixes[e] != nullptr)
		{
			m_musicSubmixes[e]->DestroyVoice();
			m_musicSubmixes[e] = nullptr;
		}
	}

	//Deallocate the remaning submixes
	for(unsigned int e = 0; e < (unsigned)m_sfxChannelCount; e++)
	{
		//Deallocate the effect submixes per player
		if (m_effectSubmixes[e] != nullptr)
		{
			m_effectSubmixes[e]->DestroyVoice();
			m_effectSubmixes[e] = nullptr;
		}
	}

	if (m_masteringVoice != nullptr)
	{
		m_masteringVoice->DestroyVoice();
		m_masteringVoice = nullptr;
	}


	//Deallocate the source data
	for(unsigned int m = 0; m < (unsigned)m_musicChannelCount; m++)
	{
		//Deallocte the source data from both music and sfx track lists
		for each(shared_ptr<ImplData> Data in m_musicTrackList[m])
		{
			Data->g_audioData.~Wave();
			//Data->~ImplData();
		}
	}

	for(unsigned int p = 0; p < (unsigned)m_sfxChannelCount; p++)
	{
		for each(shared_ptr<ImplData> Data in m_sfxTrackList[p])
		{
			Data->g_audioData.~Wave();
			//Data->~ImplData();
		}
	}

	if (m_audioEngine != nullptr)
	{
		m_audioEngine->Release();
//		m_audioEngine = nullptr;
	}
	CoUninitialize();

}

bool AudioManager::LoadSubmixes()
{
	//Load the submixes and XAudio2
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	const UINT32 SOUND_SAMPLE_RATE = 48000;
	UINT32 flags = 0;

	// Create the XAudio2 Engine
	if (FAILED(XAudio2Create(&m_audioEngine, flags)))
	{
		MessageBoxA(NULL, "Failed to create AudioEngine",ERROR, MB_OK | MB_ICONERROR);
		m_audioEngine->Release();
		CoUninitialize();
		return false;
	}

	// Create the mastering voice
	if (FAILED(
		m_audioEngine->CreateMasteringVoice(
		&m_masteringVoice)))
	{
		MessageBoxA(NULL, "Failed to create MasteringVoice", ERROR, MB_OK | MB_ICONERROR);
		CoUninitialize();
		return false;
	}

	//Initialize Submixes
	for(unsigned int e = 0; e < (unsigned)m_sfxChannelCount; e++)
	{
		IXAudio2SubmixVoice* newEffectSubmix;
		m_audioEngine->CreateSubmixVoice(&newEffectSubmix, 1, 44100, 0, 0, 0, 0);
		m_effectSubmixes.push_back(newEffectSubmix);
	}

	for(unsigned int m = 0; m < (unsigned)m_musicChannelCount; m++)
	{
		IXAudio2SubmixVoice* newMusicSubmix;
		m_audioEngine->CreateSubmixVoice(&newMusicSubmix, 1, 44100, 0, 0, 0, 0);
		m_musicSubmixes.push_back(newMusicSubmix);
	}

	//details of the current audio device, hardware 
	XAUDIO2_DEVICE_DETAILS details;
	m_audioEngine->GetDeviceDetails(0, &details);
	m_sound3D.nChannels = details.OutputFormat.Format.nChannels;
	m_sound3D.dwChannelMask = details.OutputFormat.dwChannelMask;


	//REVERB 
	XAudio2CreateReverb(&m_sound3D.pReverbEffect, 0);
	XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { m_sound3D.pReverbEffect, TRUE, 1 } };
	XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

	m_audioEngine->CreateSubmixVoice(&m_3dSubmix, 1,
		details.OutputFormat.Format.nSamplesPerSec, 0, 0,
		NULL, &effectChain);


	// Set default FX params
	XAUDIO2FX_REVERB_PARAMETERS native;
	ReverbConvertI3DL2ToNative(&g_PRESET_PARAMS[1], &native);
	m_3dSubmix->SetEffectParameters(0, &native, sizeof(native));

	return true;
}

///GET SET
IXAudio2MasteringVoice& AudioManager::GetMasteringVoice()
{
	return *m_masteringVoice;
}

void AudioManager::GetEffectSubmix(int effectIndex, IXAudio2SubmixVoice** effectVoice )
{
	if(effectIndex < m_sfxChannelCount)
	{
		*effectVoice = m_effectSubmixes[effectIndex];
	}

}

void AudioManager::GetMusicSubmix(int effectIndex, IXAudio2SubmixVoice** effectVoice )
{
	if (effectIndex < m_sfxChannelCount)
	{
		*effectVoice = m_musicSubmixes[effectIndex];
	}
}

std::vector<std::vector<shared_ptr<ImplData>>>* AudioManager::GetSFXDataVector()
{
	return &m_sfxTrackList;
}

std::vector<std::vector<shared_ptr<ImplData>>>* AudioManager::GetMusicDataVector()
{
	return &m_musicTrackList;
}

void AudioManager::Update(float dt, Input* pInput)
{

	if (pInput->GetDirectInput()->GetKeyboardState(DIK_R))
	{
		m_sound3D.Emitter[0].m_emitter.Position.x += 2 * dt;
	}

	if (pInput->GetDirectInput()->GetKeyboardState(DIK_T))
	{
		m_sound3D.Emitter[0].m_emitter.Position.x -= 2 * dt;
	}

	if (pInput->GetDirectInput()->GetKeyboardState(DIK_D))
	{
		m_sound3D.Emitter[0].m_emitter.Position.z += 2 * dt;
	}

	if (pInput->GetDirectInput()->GetKeyboardState(DIK_F))
	{
		m_sound3D.Emitter[0].m_emitter.Position.z -= 2 * dt;
	}

	if (nFrameToApply3DAudio == 0)
	{
		cout << "E" << m_sound3D.Emitter[0].m_emitter.Position.x << " " << m_sound3D.Emitter[0].m_emitter.Position.y << " " << m_sound3D.Emitter[0].m_emitter.Position.z << " " << endl;
		cout << "L" << m_sound3D.Listener.Position.x << " " << m_sound3D.Listener.Position.y << " " << m_sound3D.Listener.Position.z << " " << endl;


		m_sound3D.Listener.pCone = NULL;

		m_sound3D.Emitter[0].m_emitter.InnerRadius = 0.0f;
		m_sound3D.Emitter[0].m_emitter.InnerRadiusAngle = 0.0f;
		m_sound3D.Emitter[0].m_emitter.Velocity = floatToa3dfloat(XMFLOAT3(0, 0, 0));
		m_sound3D.Listener.Velocity = floatToa3dfloat(XMFLOAT3(0, 0, 0));



		DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
			| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
			| X3DAUDIO_CALCULATE_REVERB;
		//if (g_audioState.fUseRedirectToLFE)
		//{
		//	// On devices with an LFE channel, allow the mono source data
		//	// to be routed to the LFE destination channel.
		//	dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
		//}

		X3DAudioCalculate(m_audio3dHandle, &m_sound3D.Listener, &m_sound3D.Emitter[0].m_emitter, dwCalcFlags, &m_sound3D.Emitter[0].dspSettings);

		IXAudio2SourceVoice* voice = m_3dTrackList[sound3dIndexMap["3ds_Pboost.wav"]]->g_sourceVoice;
		if (voice)
		{

			voice->SetFrequencyRatio(m_sound3D.Emitter[0].dspSettings.DopplerFactor);
			voice->SetOutputMatrix(m_masteringVoice, INPUTCHANNELS, m_sound3D.nChannels, m_sound3D.matrixCoefficients);


			voice->SetOutputMatrix(m_3dSubmix, 1, 1, &m_sound3D.Emitter[0].dspSettings.ReverbLevel);

			XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * m_sound3D.Emitter[0].dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
			voice->SetOutputFilterParameters(m_masteringVoice, &FilterParametersDirect);
			XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * m_sound3D.Emitter[0].dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
				voice->SetOutputFilterParameters(m_3dSubmix, &FilterParametersReverb);
		}
	}

	nFrameToApply3DAudio++;
	nFrameToApply3DAudio &= 1;

	
}

X3DAUDIO_VECTOR AudioManager::floatToa3dfloat(XMFLOAT3 _value)
{
	X3DAUDIO_VECTOR value;
	value.x = _value.x;
	value.y = _value.y;
	value.z = _value.z;

	return value;
}

XMFLOAT3 AudioManager::a3dfloatToflaot(X3DAUDIO_VECTOR _value)
{
	XMFLOAT3 value;
	value.x = _value.x;
	value.y = _value.y;
	value.z = _value.z;

	return value;
}