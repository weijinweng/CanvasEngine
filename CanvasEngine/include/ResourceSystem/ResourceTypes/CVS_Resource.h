#pragma once
// Todo: create serializable
struct CVS_Resource
{
	enum class EType
	{
		Unknown,
		Mesh,
		Skeleton,
		Animation
	};

	CVS_Resource() : m_name("Resource"), m_type(CVS_Resource::EType::Unknown) {}
	CVS_Resource(std::string _name, CVS_Resource::EType _type) : m_name(_name), m_type(_type)
	{}
	std::string m_name;
	EType m_type;
};

struct CVS_InternalResource
{
	enum class EType
	{
		Unknown,
		Bone,
		AnimationLayer,
		AnimationNode,
		AnimationCurve
	};

	CVS_InternalResource() : m_name("InternalResource"), m_type(EType::Unknown) {}
	CVS_InternalResource(std::string _name, EType _type) : m_name(_name), m_type(_type)
	{}
	std::string m_name;
	EType m_type;
};