#include "AudioSource.h"
#include "GameObject.h" // For accessing GameObject and its transform
#include <iostream>     // For logging

using namespace ScrapGameEngine;

AudioSource::AudioSource(GameObject* owner)
    : BaseComponent(owner), _soundEngine(nullptr), _currentSound(nullptr),
    _looping(false), _volume(1.0f)
{
    // Initialize the IrrKlang sound engine
    _soundEngine = irrklang::createIrrKlangDevice();
    if (!_soundEngine)
    {
        std::cerr << "[AUDIOSOURCE] Error: Could not initialize IrrKlang sound engine." << std::endl;
    }
}

AudioSource::~AudioSource()
{
    cleanup();
}

void AudioSource::load(const std::string& filePath, bool loop)
{
    _filePath = filePath;
    _looping = loop;
}

void AudioSource::play()
{
    if (!_soundEngine)
    {
        _soundEngine = irrklang::createIrrKlangDevice();
        return;
    }

    if (_filePath.empty())
    {
		std::cerr << "[AUDIOSOURCE] Error: No audio file loaded. Path is empty." << std::endl;
		return;
    }

    if (_currentSound)
    {
        _currentSound->stop();
        _currentSound->drop();
    }

    _currentSound = _soundEngine->play2D(_filePath.c_str(), _looping, false, true);
    if (_currentSound)
    {
        _currentSound->setVolume(_volume);
    }
    else
    {
        std::cerr << "[AUDIOSOURCE] Error: Failed to play audio file: " << _filePath << std::endl;
    }
}

void AudioSource::pause()
{
    if (_currentSound)
    {
        _currentSound->setIsPaused(true);
    }
}

void AudioSource::stop()
{
    if (_currentSound)
    {
        _currentSound->stop();
        _currentSound->drop();
        _currentSound = nullptr;
    }
}

void AudioSource::setVolume(float volume)
{
    _volume = std::clamp(volume, 0.0f, 1.0f);
    if (_currentSound)
    {
        _currentSound->setVolume(_volume);
    }
}

float AudioSource::getVolume() const
{
    return _volume;
}

void AudioSource::setLooping(bool loop)
{
    _looping = loop;
    if (_currentSound)
    {
        _currentSound->setIsLooped(loop);
    }
}

bool AudioSource::isLooping() const
{
    return _looping;
}

bool ScrapGameEngine::AudioSource::isPlaying() const
{
    return _currentSound && !_currentSound->isFinished();
}

void ScrapGameEngine::AudioSource::update(float deltaTime)
{
    /*   if (!gameObject || !gameObject->transform) return;

       auto pos = gameObject->transform->getPosition();
       if (_currentSound)
       {
           _currentSound->setPosition(irrklang::vec3df(pos.x, pos.y, 0));
       }*/
}

void ScrapGameEngine::AudioSource::cleanup()
{
    // Clean up resources
    if (_currentSound)
    {
        _currentSound->drop();
    }
    if (_soundEngine)
    {
        _soundEngine->drop();
    }
}
