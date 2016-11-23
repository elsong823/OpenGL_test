#include "Utility.h"
#include "MyCamera.h"

CMyCamera::CMyCamera()
{
}

CMyCamera::~CMyCamera()
{
}

CMyCamera::CMyCamera(GLfloat _fov, GLfloat _viewWidth, GLfloat _viewHeight, glm::vec3 _pos, glm::vec3 _lookAt)
:m_position(_pos),
m_worldUp(0.0f, 1.0f, 0.0f),
m_fov(_fov),
m_moveSpeed(10.0f),
m_dirty(true),
m_curMoveStatus(0x00000000),
m_sensitive(0.03f)
{
	m_faceTo = glm::normalize(_lookAt - _pos);
	m_right = glm::normalize(glm::cross(m_faceTo, m_worldUp));
	m_up = glm::normalize(glm::cross(-m_faceTo, m_right));
	m_matProjection = glm::perspective(m_fov, (GLfloat)_viewWidth/_viewHeight, 0.1f, 100.0f);
	this->Update(0.0f);
}

void CMyCamera::Update(GLfloat _dt)
{
	this->CheckMoveStatus(_dt);
	if (m_dirty)
	{
		m_matView = glm::lookAt(m_position, m_position + m_faceTo, m_worldUp);
		m_dirty = false;
	}
}

void CMyCamera::CheckMoveStatus(GLfloat _dt)
{
	glm::vec3 moveDir = glm::vec3();
	//判断相机移动状态
	if (m_curMoveStatus & (1 << CameraMoveDir::CMD_FORWARD))
	{
		//按了W
		moveDir += m_faceTo * _dt * m_moveSpeed;
	}
	if (m_curMoveStatus & (1 << CameraMoveDir::CMD_BACKWARD))
	{
		//按了S
		moveDir -= m_faceTo * _dt * m_moveSpeed;
	}
	if (m_curMoveStatus & (1 << CameraMoveDir::CMD_LEFT))
	{
		moveDir -= m_right * _dt * m_moveSpeed;
	}
	if (m_curMoveStatus & (1 << CameraMoveDir::CMD_RIGHT))
	{
		moveDir += m_right * _dt * m_moveSpeed;
	}
	if (m_curMoveStatus & (1 << CameraMoveDir::CMD_UP))
	{
		moveDir += m_moveSpeed * _dt * m_up;
	}
	if (m_curMoveStatus & (1 << CameraMoveDir::CMD_DOWN))
	{
		moveDir -= m_moveSpeed * _dt * m_up;
	}
	moveDir *= _dt * m_moveSpeed;
	if (glm::length(moveDir) > 0.001f)
	{
		m_position += moveDir;
		m_dirty = true;
	}
}

void CMyCamera::MoveCamera(CameraMoveDir _dir, bool _move)
{
	if (_move)
	{
		m_curMoveStatus |= (1 << _dir);
	}
	else if (m_curMoveStatus & (1 << _dir))
	{
		m_curMoveStatus -= 1 << _dir;
	}
}

void CMyCamera::RotateCamera(GLfloat _offsetX, GLfloat _offsetY)
{
	if (abs(_offsetX) <= 0.01f && abs(_offsetY) <= 0.01f)
	{
		return;
	}
	static glm::mat4 matRotate;
	matRotate = glm::mat4();
	matRotate = glm::rotate(matRotate, -glm::radians(_offsetX) * m_sensitive, m_up);
	matRotate = glm::rotate(matRotate, -glm::radians(_offsetY) * m_sensitive, m_right);
	m_faceTo = glm::vec3(matRotate * glm::vec4(m_faceTo, 0.0f));
	m_faceTo = glm::normalize(m_faceTo);
	m_right = glm::normalize(glm::cross(m_faceTo, m_worldUp));
	m_up = glm::normalize(glm::cross(-m_faceTo, m_right));
	m_dirty = true;
}