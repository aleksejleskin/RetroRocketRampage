//AudioManager.h

#ifndef INCLUDE_GUARD_AUDIO_MANAGER_H
#define INCLUDE_GUARD_AUDIO_MANAGER_H

#include "wavFileReader.h"
#include <XAudio2.h>
#include <xaudio2fx.h>
#include <X3DAudio.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include "DxGraphics.h"
#include "Input.h"


#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample
// Specify sound cone to add directionality to listener for artistic effect:
// Emitters behind the listener are defined here to be more attenuated,
// have a lower LPF cutoff frequency,
// yet have a slightly higher reverb send level.
static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI*5.0f / 6.0f, X3DAUDIO_PI*11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

// Specify LFE level distance curve such that it rolls off much sooner than
// all non-LFE channels, making use of the subwoofer more dramatic.
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

// Specify reverb send level distance curve such that reverb send increases
// slightly with distance before rolling off to silence.
// With the direct channels being increasingly attenuated with distance,
// this has the effect of increasing the reverb-to-direct sound ratio,
// reinforcing the perception of distance.
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };


using namespace std;

struct ImplData;

struct Audio3dEmitter
{
	X3DAUDIO_EMITTER		m_emitter;
	X3DAUDIO_DSP_SETTINGS	m_dps;
	X3DAUDIO_DSP_SETTINGS dspSettings;
};

struct Sound3D
{
	vector<Audio3dEmitter> Emitter;
	X3DAUDIO_LISTENER Listener;

	DWORD        dwChannelMask;          /* number of channels (i.e. mono, stereo...) */
	UINT32 nChannels;
	IUnknown* pReverbEffect;
	FLOAT32 emitterAzimuths[INPUTCHANNELS];
	X3DAUDIO_CONE emitterCone;
	FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
	float fListenerAngle;
};

class AudioManager
{
public:

	AudioManager();
	~AudioManager();

	bool LoadContent(int sfxChannelCount, int musicChannelCount);

	bool PlayMusic(char* audioFileName, int effectIndex, float volume);
	bool PlaySFX(char* audioFileName, int effectIndex, float volume);
	bool StopMusic(char* audioFilename, int effectIndex);
	bool StopSFX(char* audioFilename, int effectIndex);

	bool StopAllSFX();
	bool StopAllMusic();

	bool IsSFXPlaying();
	bool IsMusicPlaying();

	void AddSound(char* audioFileName);
	void Add3dSound(char* audioFilename);
	void UnloadAudio();

	IXAudio2MasteringVoice& GetMasteringVoice();
	void GetEffectSubmix( int effectIndex, IXAudio2SubmixVoice** effectVoice);
	void GetMusicSubmix( int effectIndex,  IXAudio2SubmixVoice** musicVoice);
	void Update(float dt, Input* pInput);
	std::vector<std::vector<shared_ptr<ImplData>>>* GetSFXDataVector();
	std::vector<std::vector<shared_ptr<ImplData>>>* GetMusicDataVector();

private:
	//Loads all the submixes
	bool LoadSubmixes();

	//Loads all the audio files
	bool LoadAudio();

	//Loads music
	void AddMusic(char* audioFileName);
	void AddSFX(char* audioFilenName);

	//Loads sfx
	int m_sfxChannelCount,
		m_musicChannelCount;

	std::stringstream errorOutput;

	//XAUDIO 2 engine
	IXAudio2* m_audioEngine;
	X3DAUDIO_HANDLE m_audio3dHandle;

	//Voices
	IXAudio2MasteringVoice* m_masteringVoice;
	vector<IXAudio2SubmixVoice*> m_effectSubmixes;
	vector<IXAudio2SubmixVoice*> m_musicSubmixes;

	IXAudio2SubmixVoice* m_3dSubmix;
	//Holds the sources for music and sfx
	std::vector<std::vector<shared_ptr<ImplData>>> m_musicTrackList;
	std::vector<std::vector<shared_ptr<ImplData>>> m_sfxTrackList;
	std::vector<shared_ptr<ImplData>> m_3dTrackList;

	//map to get to the index of vector through filename
	std::map<string,int > nameIndexMap;
	std::map<string,int > musicIndexMap;
	std::map<string,int > sfxIndexMap;
	std::map<string, int > sound3dIndexMap;

	//X3DAUDIO_LISTENER Listener;
	//X3DAUDIO_EMITTER Emitter;

	Sound3D m_sound3D;

	int nFrameToApply3DAudio;

	X3DAUDIO_VECTOR floatToa3dfloat(XMFLOAT3 _value);
	XMFLOAT3 a3dfloatToflaot(X3DAUDIO_VECTOR);

};



//initialize listener
//initialize emitter.

//xmfloatTo3dvector
//3dvectorToxmfloat
#endif