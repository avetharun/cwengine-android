#include "../include/camera.h"
#include "../imgui/imgui_extras.h"
#include "../ndk-helper/JNIHelper.h"
#include "../utils.hpp"

void Transform::recalcScale(ImVec2* base_sc, ImVec2 scale) {
	base_sc->x *= scale.x;
	base_sc->y *= scale.y;
}

ImVec2 VecUtils::parse_angle(float angle) {
	ImVec2 i(
		(1 * cos(angle * M_PI / 180)),
		(1 * sin(angle * M_PI / 180))
	);
	return i;
}

float VecUtils::parse_angle_vec(ImVec2 v) {
	return atan2f(v.y, v.x) * 180 / M_PI;
}
static Camera* s_Instance = nullptr;
Camera* Camera::GetInstance() { return s_Instance = (s_Instance != nullptr) ? s_Instance : new Camera(); }
bool Camera::isPortrait() { return m_VirtualViewport.x < m_VirtualViewport.y; }
ImVec2 Camera::getViewport() { return m_VirtualViewport; }

void Camera::SetTarget(ImVec2* targ) {
	m_target = targ;
}

void Camera::Update() {}

void Camera::lu_SetTarget(ImVec2* target) {
	GetInstance()->m_target = target;
}

float Camera::lu_GetScale() {
	return GetInstance()->scale;
}

void Camera::lu_SetScale(float _target) {
	GetInstance()->scale = _target;
}

ImVec2 Camera::lu_GetPosition() {
	return *GetInstance()->m_target;
}

void Camera::lu_SetPosition(ImVec2* _target) {
	GetInstance()->m_target = _target;
}

ImVec2 Camera::ImVec2ToCameraSpace(ImVec2 v) {
	return ((v + *GetInstance()->m_target) + GetInstance()->m_Offset) * GetInstance()->scale;
}

ImVec2 screenCenter(Camera* c) {
	if (!c->m_GlobalViewport.x && !c->m_GlobalViewport.y) {
		LOGF("[FTH] Camera Screen viewport was of size zero!");
		IM_ASSERT(c->m_GlobalViewport.x != 0 && c->m_GlobalViewport.y != 0);
	}
	return { c->m_GlobalViewport.x / 2, c->m_GlobalViewport.y / 2 }; };
ImVec2 virtualViewportCenter(Camera *c) {
	if (!c->m_VirtualViewport.x && !c->m_VirtualViewport.y) {
		LOGF("[FTH] Camera World viewport was of size zero!");
		IM_ASSERT(c->m_VirtualViewport.x != 0 && c->m_VirtualViewport.y != 0);
	}
	return { c->m_VirtualViewport.x / 2, c->m_VirtualViewport.y / 2 }; };

ImVec2 Camera::pointToWorldViewport(ImVec2 pos) {
	ImVec2 translatedPos;
	translatedPos.x = (pos.x / m_GlobalViewport.x) * m_VirtualViewport.x;
	translatedPos.y = (pos.y / m_GlobalViewport.y) * m_VirtualViewport.y;

	return translatedPos;
}
float Camera::getScaleFactor() {
	// Determine the minimum dimension of the physical viewport
	float minDimension = alib_min(m_GlobalViewport.x, m_GlobalViewport.y);
	// Landscape mode
	if (m_VirtualViewport.x > m_VirtualViewport.y) {
		return minDimension / m_VirtualViewport.y;
	}
	// Portrait mode
	else {
		return minDimension / m_VirtualViewport.x;
	}
}

ImVec2 Camera::pointToScreenViewport(ImVec2 pos) {
	ImVec2 translatedPos;

	// Calculate the scale factor
	float scaleFactor = getScaleFactor();

	// Calculate the scaled virtual viewport size
	ImVec2 scaledVirtualViewport;
	scaledVirtualViewport.x = m_VirtualViewport.x * scaleFactor;
	scaledVirtualViewport.y = m_VirtualViewport.y * scaleFactor;

	// Calculate the translation offset to center the viewport
	ImVec2 translationOffset;
	translationOffset.x = (m_GlobalViewport.x - scaledVirtualViewport.x) / 2.0f;
	translationOffset.y = (m_GlobalViewport.y - scaledVirtualViewport.y) / 2.0f;

	// Apply the translation and scale
	translatedPos.x = ((pos.x / m_VirtualViewport.x) * scaledVirtualViewport.x) + translationOffset.x;
	translatedPos.y = ((pos.y / m_VirtualViewport.y) * scaledVirtualViewport.y) + translationOffset.y;

	return translatedPos;
}

Camera::Camera() {}

Camera::Camera(ImVec2* targ) : m_target(targ){}
