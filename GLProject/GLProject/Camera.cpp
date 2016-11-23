#include "Utility.h"
#include "Camera.h"

CCamera::CCamera()
{
	//do nothing
}

CCamera::CCamera(GLfloat _screenWidth, GLfloat _screenHeight, glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up)
:m_screenWidth(_screenWidth),
m_screenHeight(_screenHeight),
m_fov(FOV),
m_position(0.0f, 0.0f, 0.0f),
m_faceTo(0.0f, 0.0f, 0.0f),
m_right(0.0f, 0.0f, 0.0f),
m_up(0.0f, 0.0f, 0.0f),
m_worldUp(0.0f, 0.0f, 0.0f),
m_yaw(0.0f),
m_pitch(0.0f),
m_moveSpeed(SPEED)
{
	m_position = _pos;
	m_faceTo = glm::normalize(_lookAt - _pos);
	m_worldUp = glm::normalize(_up);
	m_right = glm::normalize(glm::cross(m_worldUp, -m_faceTo));
	m_up = glm::cross(-m_faceTo, m_right);
	m_matProjection = glm::perspective(FOV, (float)m_screenWidth / m_screenHeight, 0.1f, 100.0f);
	UpdateCameraVectors();
}

CCamera::~CCamera()
{
}

const glm::mat4 * CCamera::GetViewMat()
{
	return &m_matView;
}

const glm::mat4 * CCamera::GetProjectionMat()
{
	return &m_matProjection;
}

void CCamera::UpdateCameraVectors()
{
	this->CalcLookAtMat();
	//m_matView = glm::lookAt(m_position, m_position + m_faceTo, m_up);
}

void CCamera::MoveCamera(CameraMoveDir _cmd, GLfloat _dt)
{
	auto _speed = m_moveSpeed * _dt;
	switch (_cmd)
	{
	case CMD_FORWARD:
		m_position -= m_faceTo * _speed;
		break;
	case CMD_BACKWARD:
		m_position += m_faceTo * _speed;
		break;
	case CMD_LEFT:
		m_position -= m_right * _speed;
		break;
	case CMD_RIGHT:
		m_position += m_right * _speed;
		break;
	default:
		break;
	}
	this->UpdateCameraVectors();
}

void CCamera::RotateCamera(GLfloat _yaw, GLfloat _pitch)
{
	printf_s("%.2f, %.2f\n", _yaw, _pitch);
	m_yaw += _yaw * SENSITIVITY;
	m_pitch += _pitch * SENSITIVITY;
	m_pitch = m_pitch > 89 ? 89 : m_pitch;
	m_pitch = m_pitch < -89 ? -89 : m_pitch;
	m_faceTo.x = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	m_faceTo.y = sin(glm::radians(m_pitch));
	m_faceTo.z = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	this->UpdateCameraVectors();
}

void CCamera::CameraZoom(GLfloat _zoom)
{
	m_fov += _zoom * 0.1f;
	m_fov = m_fov > FOV ? FOV : m_fov;
	m_fov = m_fov < 25 ? 25 : m_fov;
	printf_s("%.2f\n", m_fov);
	m_matProjection = glm::perspective(m_fov, (float)m_screenWidth / m_screenHeight, 0.1f, 100.0f);
}

void CCamera::CalcLookAtMat()
{
	m_faceTo = glm::normalize(m_faceTo);
	m_right = glm::normalize(glm::cross(m_worldUp, m_faceTo));
	m_up = glm::normalize(glm::cross(m_faceTo, m_right));
	auto _translation = glm::mat4();
	_translation[3][0] = -m_position.x;
	_translation[3][1] = -m_position.y;
	_translation[3][2] = -m_position.z;
	auto _rotation = glm::mat4();
	_rotation[0][0] = m_right.x;
	_rotation[1][0] = m_right.y;
	_rotation[2][0] = m_right.z;

	_rotation[0][1] = m_up.x;
	_rotation[1][1] = m_up.y;
	_rotation[2][1] = m_up.z;

	_rotation[0][2] = m_faceTo.x;
	_rotation[1][2] = m_faceTo.y;
	_rotation[2][2] = m_faceTo.z;

	m_matView = _rotation * _translation;
	int a = 1;
}