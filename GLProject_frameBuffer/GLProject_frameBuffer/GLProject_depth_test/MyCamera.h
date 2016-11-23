#pragma once

enum CameraMoveDir{
	CMD_FORWARD,
	CMD_BACKWARD,
	CMD_LEFT,
	CMD_RIGHT,
	CMD_UP,
	CMD_DOWN
};

class CMyCamera
{
private:
	CMyCamera();
public:
	CMyCamera(GLfloat _fov, GLfloat _viewWidth, GLfloat _viewHeight, glm::vec3 _pos, glm::vec3 _lookAt);
	~CMyCamera();

public:
	const glm::mat4 * GetView(){ return &m_matView; }
	const glm::mat4 * GetProjection(){ return &m_matProjection; }
	void Update(GLfloat _dt);
	void MoveCamera(CameraMoveDir _dir, bool _move);
	void CheckMoveStatus(GLfloat _dt);
	void RotateCamera(GLfloat _offsetX, GLfloat _offsetY);
	const glm::vec3 * GetPosition(){ return & m_position; }
	const glm::vec3 * GetFaceTo(){ return &m_faceTo; }
	void TurnFace();

private:
	bool m_dirty;
	GLfloat m_fov;
	GLfloat m_moveSpeed;
	GLfloat m_sensitive;
	glm::vec3 m_position;
	glm::vec3 m_faceTo;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_worldUp;
	glm::mat4 m_matView;
	glm::mat4 m_matProjection;

	//Camera move status
	GLint m_curMoveStatus;
};

