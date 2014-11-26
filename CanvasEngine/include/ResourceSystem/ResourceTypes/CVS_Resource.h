#pragma once
// Todo: create serializable
struct CVS_Resource
{
	enum class eType
	{
		Unknown,
		Mesh,
		Skeleton,
		Animation
	};

	CVS_Resource() : m_name("DefaultName"), m_type(CVS_Resource::eType::Unknown) {}
	CVS_Resource(std::string _name, CVS_Resource::eType _type) : m_name(_name), m_type(_type)
	{}
	std::string m_name;
	eType m_type;
};

struct CVS_InternalResource
{
	enum class eType
	{
		Unknown,
		Bone,
		AnimCurveNode
	};

	CVS_InternalResource() : m_name("DefaultName"), m_type(eType::Unknown) {}
	CVS_InternalResource(std::string _name, eType _type) : m_name(_name), m_type(_type)
	{}
	std::string m_name;
	eType m_type;
};