#include "ThrownDevice.h"
#include "PresetMan.h"

namespace RTE {

	ConcreteClassInfo(ThrownDevice, HeldDevice, 50);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ThrownDevice::Clear() {
		m_ActivationSound.Reset();
		m_StartThrowOffset.Reset();
		m_EndThrowOffset.Reset();
		m_MinThrowVel = 0;
		m_MaxThrowVel = 0;
		m_TriggerDelay = 0;
		m_ActivatesWhenReleased = false;
		m_StrikerLever = nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ThrownDevice::Create() {
		if (HeldDevice::Create() < 0) {
			return -1;
		}
		m_MOType = MovableObject::TypeThrownDevice;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ThrownDevice::Create(const ThrownDevice &reference) {
		HeldDevice::Create(reference);

		m_MOType = MovableObject::TypeThrownDevice;

		m_ActivationSound = reference.m_ActivationSound;

		m_StartThrowOffset = reference.m_StartThrowOffset;
		m_EndThrowOffset = reference.m_EndThrowOffset;
		m_MinThrowVel = reference.m_MinThrowVel;
		m_MaxThrowVel = reference.m_MaxThrowVel;
		m_TriggerDelay = reference.m_TriggerDelay;
		m_ActivatesWhenReleased = reference.m_ActivatesWhenReleased;
		m_StrikerLever = reference.m_StrikerLever;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ThrownDevice::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "ActivationSound") {
			reader >> m_ActivationSound;
		} else if (propName == "StartThrowOffset") {
			reader >> m_StartThrowOffset;
		} else if (propName == "EndThrowOffset") {
			reader >> m_EndThrowOffset;
		} else if (propName == "MinThrowVel") {
			reader >> m_MinThrowVel;
		} else if (propName == "MaxThrowVel") {
			reader >> m_MaxThrowVel;
		} else if (propName == "TriggerDelay") {
			reader >> m_TriggerDelay;
		} else if (propName == "ActivatesWhenReleased") {
			reader >> m_ActivatesWhenReleased;
		} else if (propName == "StrikerLever") {
			m_StrikerLever = dynamic_cast<const MovableObject *>(g_PresetMan.GetEntityPreset(reader));
		} else {
			return HeldDevice::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ThrownDevice::Save(Writer &writer) const {
		HeldDevice::Save(writer);

		writer.NewProperty("ActivationSound");
		writer << m_ActivationSound;
		writer.NewProperty("StartThrowOffset");
		writer << m_StartThrowOffset;
		writer.NewProperty("EndThrowOffset");
		writer << m_EndThrowOffset;
		writer.NewProperty("MinThrowVel");
		writer << m_MinThrowVel;
		writer.NewProperty("MaxThrowVel");
		writer << m_MaxThrowVel;
		writer.NewProperty("TriggerDelay");
		writer << m_TriggerDelay;
		writer.NewProperty("ActivatesWhenReleased");
		writer << m_ActivatesWhenReleased;
		writer.NewProperty("StrikerLever");
		writer << m_StrikerLever;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ThrownDevice::ResetAllTimers() {
		double elapsedTime = m_Activated ? m_ActivationTimer.GetElapsedSimTimeMS() : 0;
		HeldDevice::ResetAllTimers();
		if (m_Activated) { m_ActivationTimer.SetElapsedSimTimeMS(elapsedTime); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ThrownDevice::Activate() {
		if (!m_Activated) {
			m_ActivationTimer.Reset();
			m_ActivationSound.Play(m_Pos);
			if (MovableObject *strikerLever = m_StrikerLever ? dynamic_cast<MovableObject *>(m_StrikerLever->Clone()) : nullptr) {
				Vector randomVel(m_Vel.GetMagnitude() * 0.25F + 1.0F, 0);

				strikerLever->SetVel(m_Vel * 0.5F + randomVel.RadRotate(c_PI * RandomNormalNum()));
				strikerLever->SetPos(m_Pos);
				strikerLever->SetRotAngle(m_Rotation.GetRadAngle());
				strikerLever->SetAngularVel(m_AngularVel + strikerLever->GetAngularVel() * RandomNormalNum());
				strikerLever->SetHFlipped(m_HFlipped);
				strikerLever->SetTeam(m_Team);
				strikerLever->SetIgnoresTeamHits(true);
				g_MovableMan.AddParticle(strikerLever);
			}
			m_Activated = true;
		}
	}
}