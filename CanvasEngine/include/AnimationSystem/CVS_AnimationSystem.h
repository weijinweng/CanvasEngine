#pragma once
#include "CVS_SubSystem.h"
#include "CVS_AnimationComponent.h"

class Timer;
class CVS_Animation;

struct CVS_AnimationBlendSpace
{

};

struct CVS_AnimationSample
{
	virtual ~CVS_AnimationSample()
	{

	}

	//CVS_Skeleton* m_pSkeleton;
	std::vector<CVS_BonePose> m_aLocalPose;
};

struct CVS_AnimationClip
{
	CVS_Animation* m_pAnimation;
	float m_dt;
	bool m_isLooping;

	CVS_AnimationClip() {}

	CVS_AnimationClip(CVS_Animation* _pAnimation, float _startTime, bool _isLooping) :
		m_pAnimation(_pAnimation), m_dt(_startTime), m_isLooping(_isLooping)
	{}

};

struct CVS_AnimationLayerSample : CVS_AnimationSample
{
	float m_weight;
};

struct CVS_AnimationSystem : public CVS_SubSystem
{
	virtual bool Initialize();

	virtual bool Update();

	virtual bool End();

	// Incurs dynamic allocation, caller needs to delete the pointer after using the frame
	CVS_AnimationSample* getFrame(CVS_Animation* _pAnimation, int _frameIndex);

	CVS_AnimationSample* evaluate(CVS_Animation* _pAnimation, float _dt);

	std::map<CVS_AnimationComponent*, std::vector<CVS_AnimationClip*>> m_registeredClips;
	Timer* m_pTimer;
};

