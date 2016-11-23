#pragma once

enum CameraMoveDir
{
	CMD_FORWARD,
	CMD_BACKWARD,
	CMD_LEFT,
	CMD_RIGHT,
	CMD_UPWARD,
	CMD_DOWNWARD,
};

class CMyCamera
{
	//constructor
private:
	CMyCamera();
public:
	CMyCamera(GLfloat _fov, GLint _width, GLint _height, glm::vec3 _pos, glm::vec3 _lookAt);
	~CMyCamera();

public:
	void Update(GLfloat _dt);
	const glm::mat4 * GetView(){ return &m_matView; }
	const glm::mat4 * GetProjection(){ return &m_matProjection; }
	void MoveCamera(bool _move, CameraMoveDir _dir);
	void SetYaw(double _yaw){ m_yaw = (float)_yaw; m_dirty = true; }
	void SetPitch(double _pitch){ m_pitch = (float)_pitch; m_dirty = true; }
	void ZoomCamera(double _zoom);
	glm::vec3 GetPosition(){ return m_position; };
	glm::vec3 GetFaceTo(){ return m_faceTo; };

private:
	void UpdatePosition(GLfloat _dt);
	void UpdateRotation();
	void UpdateView();
	void UpdateProjection();

private:
	GLboolean m_dirty;
	GLfloat m_sensitivity;
	GLubyte m_moveStatus;
	GLfloat m_moveSpeed;
	GLfloat m_yaw;
	GLfloat m_pitch;
	GLint m_viewWidth;
	GLint m_viewHeight;
	GLfloat m_fov;
	glm::vec3 m_position;
	glm::vec3 m_lookAt;
	glm::vec3 m_faceTo;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::mat4 m_matView;
	glm::mat4 m_matProjection;
};

