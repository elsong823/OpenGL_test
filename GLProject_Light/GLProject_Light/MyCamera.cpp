#include "Utility.h"
#include "MyCamera.h"

CMyCamera::CMyCamera()
{
}

CMyCamera::~CMyCamera()
{
}

CMyCamera::CMyCamera(GLfloat _fov, GLint _width, GLint _height, glm::vec3 _pos, glm::vec3 _lookAt)
:m_fov(_fov),
m_viewWidth(_width),
m_viewHeight(_height),
m_position(_pos),
m_lookAt(_lookAt),
m_dirty(true),
m_moveStatus(0x00000000),
m_moveSpeed(2.0f),
m_yaw(0.0f),
m_pitch(0.0f),
m_sensitivity(0.03f)
{
	m_faceTo = glm::normalize(m_lookAt - m_position);
	UpdateView();
	UpdateProjection();
}

void CMyCamera::Update(GLfloat _dt)
{
	UpdatePosition(_dt);
	if (m_dirty)
	{
		UpdateRotation();
		UpdateView();
	}
	m_dirty = false;
}

void CMyCamera::UpdatePosition(GLfloat _dt)
{
	if (m_moveStatus & 2 << CMD_FORWARD)
	{
		m_position += m_faceTo * _dt * m_moveSpeed;
	}
	if (m_moveStatus & 2 << CMD_BACKWARD)
	{
		m_position -= m_faceTo * _dt * m_moveSpeed;
	}
	if (m_moveStatus & 2 << CMD_LEFT)
	{
		m_position -= m_right * _dt * m_moveSpeed;
	}
	if (m_moveStatus & 2 << CMD_RIGHT)
	{
		m_position += m_right * _dt * m_moveSpeed;
	}
	if (m_moveStatus & 2 << CMD_UPWARD)
	{
		m_position += glm::vec3(0.0f, 1.0f, 0.0f) * _dt * m_moveSpeed;
	}
	if (m_moveStatus & 2 << CMD_DOWNWARD)
	{
		m_position -= glm::vec3(0.0f, 1.0f, 0.0f) * _dt * m_moveSpeed;
	}
	//printf_s("pos:%.2f, %.2f, %.2f\n", m_position.x, m_position.y, m_position.z);
	if (m_moveStatus != 0)
	{
		m_dirty = true;
	}
}

void CMyCamera::UpdateRotation()
{
	static glm::mat4 matRotate;
	matRotate = glm::mat4();
	matRotate = glm::rotate(matRotate, -glm::radians(m_yaw*m_sensitivity), m_up);
	matRotate = glm::rotate(matRotate, -glm::radians(m_pitch*m_sensitivity), m_right);
	glm::vec4 _newFaceTo = matRotate * glm::vec4(m_faceTo, 0.0f);
	m_faceTo = glm::vec3(_newFaceTo.x, _newFaceTo.y, _newFaceTo.z);
	m_yaw = 0.0f;
	m_pitch = 0.0f;
}

void CMyCamera::UpdateView()
{
	//Update vectors
	m_right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), -m_faceTo));
	m_up = glm::normalize(glm::cross(-m_faceTo, m_right));
	//Update view matrix
	m_matView = glm::lookAt(m_position, m_position + m_faceTo, glm::vec3(0.0f, 1.0f, 0.0f));
	int a = 1;
}

void CMyCamera::UpdateProjection()
{
	m_matProjection = glm::perspective(m_fov, (float)m_viewWidth / m_viewHeight, 0.1f, 100.0f);
}

void CMyCamera::MoveCamera(bool _move, CameraMoveDir _dir)
{
	if (_move)
	{
		m_moveStatus |= (2 << (int)_dir);
	}
	else if (m_moveStatus & (2 << (int)_dir))
	{
		m_moveStatus -= (2 << (int)_dir);
	}
	m_dirty = true;
}

void CMyCamera::ZoomCamera(double _zoom)
{
	m_fov += _zoom * 0.01f;
	m_fov = m_fov > 45.0f ? 45.0f : m_fov;
	m_fov = m_fov < 44.7f ? 44.7f : m_fov;
	UpdateProjection();
}
