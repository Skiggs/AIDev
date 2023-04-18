#pragma once

class GameO
{

protected:
	unsigned int m_id;
	std::string m_name;

	bool m_active;

	bool m_visible;

	Vector3 m_position;
	Vector3 m_scale;

	double m_rotation;
	Vector3 m_rotation_axis;

	Vector2 m_origin;
	Vector3 m_size;

	Vector4 m_color;

	std::vector< std::shared_ptr<CoreHandler> > m_handlers;

	CoreObject* m_parent;
	std::vector< std::shared_ptr<CoreObject>> m_children;

public:
	CoreObject();
	virtual ~CoreObject();

	unsigned int GetID() { return m_id; }
	std::string GetObjectName() { return m_name; }
	void SetObjectName(std::string name) { m_name = name; }

	bool IsActive() { return m_active; }
	virtual void SetActive(bool b) { m_active = b; }

	bool GetVisible() { return m_visible; }
	void SetVisible(bool b) { m_visible = b; }

	virtual void SetPosition(Vector3 pos) { m_position = pos; }
	Vector3 GetPosition() { return m_position; }

	virtual void SetScale(Vector3 scale) { m_scale = scale; }
	Vector3 GetScale() { return m_scale; }

	virtual void SetRotationAngle(double rot) { m_rotation = rot; }
	double GetRotationAngle() { return m_rotation; }