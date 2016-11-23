#pragma once

const GLfloat YAW = 0.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat SPEED = 5.0f;
const GLfloat FOV = 45.0f;

enum CameraMoveDir{
	CMD_FORWARD,
	CMD_BACKWARD,
	CMD_LEFT,
	CMD_RIGHT,
};

class CCamera
{
public:
	CCamera(GLfloat _screenWidth, GLfloat _screenHeight, glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up);
	~CCamera();
private:
	CCamera();


public:
	const glm::mat4 * GetViewMat();
	const glm::mat4 * GetProjectionMat();
	void MoveCamera(CameraMoveDir _cmd, GLfloat _dt);
	void RotateCamera(GLfloat _yaw, GLfloat _pitch);
	void CameraZoom(GLfloat _zoom);

private:
	void UpdateCameraVectors();
	void CalcLookAtMat();


private:
	GLfloat m_screenWidth;
	GLfloat m_screenHeight;
	GLfloat m_fov;
	glm::vec3 m_position;
	glm::vec3 m_faceTo;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_worldUp;
	glm::mat4 m_matView;
	glm::mat4 m_matProjection;
	GLfloat m_moveSpeed;
	GLfloat m_yaw;
	GLfloat m_pitch;
};
