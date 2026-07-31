#include "Body.h"
#include "Model.h"

void Body::SetModel(Model* model)
{
	m_Model = model;

	// Dropped rather than kept: the old animator was built on the old model's
	// rig, and its bone matrices mean nothing on a different skeleton.
	m_Animator.reset();

	if (m_Model)
	{
		if (const Rig* rig = m_Model->GetRig())
		{
			m_Animator = std::make_unique<Animator>(*rig);
		}
	}
}

void Body::Update(float deltaTime)
{
	if (m_Animator)
	{
		m_Animator->Update(deltaTime);
	}
}

bool Body::PlayAnimation(const std::string& name, bool loop)
{
	return m_Animator ? m_Animator->Play(name, loop) : false;
}

void Body::SetAnimationTime(float seconds)
{
	if (m_Animator)
	{
		m_Animator->SetTime(seconds);
	}
}
