#include "AmberGL/Geometry/Plane.h"

void AmberGL::Geometry::Plane::ClipTriangle(Polygon& p_polygon) const
{
	if (p_polygon.VerticesCount == 0)
		return;

	glm::vec4 insideVertices[MAX_POLY_COUNT];
	glm::vec2 insideTexCoords[MAX_POLY_COUNT];
	glm::vec3 insideNormals[MAX_POLY_COUNT];
	float insideVaryings[MAX_POLY_COUNT][16];

	uint8_t insideVerticesCount = 0;

	glm::vec4 previousVertex = p_polygon.Vertices[p_polygon.VerticesCount - 1];
	glm::vec2 previousTexCoords = p_polygon.TexCoords[p_polygon.VerticesCount - 1];
	glm::vec3 previousNormal = p_polygon.Normals[p_polygon.VerticesCount - 1];
	float* previousVarying = p_polygon.Varyings[p_polygon.VerticesCount - 1];

	float previousPlaneDistance = SignedDistance(previousVertex);

	for (int i = 0; i < p_polygon.VerticesCount; i++)
	{
		float currentPlaneDistance = SignedDistance(p_polygon.Vertices[i]);

		if (previousPlaneDistance * currentPlaneDistance < 0.0f)
		{
			float t = previousPlaneDistance / (previousPlaneDistance - currentPlaneDistance);

			insideVertices[insideVerticesCount] = {
				glm::lerp(previousVertex.x, p_polygon.Vertices[i].x, t),
				glm::lerp(previousVertex.y, p_polygon.Vertices[i].y, t),
				glm::lerp(previousVertex.z, p_polygon.Vertices[i].z, t),
				glm::lerp(previousVertex.w, p_polygon.Vertices[i].w, t)
			};

			insideTexCoords[insideVerticesCount] = {
				glm::lerp(previousTexCoords.x, p_polygon.TexCoords[i].x, t),
				glm::lerp(previousTexCoords.y, p_polygon.TexCoords[i].y, t)
			};

			insideNormals[insideVerticesCount] = {
				glm::lerp(previousNormal.x, p_polygon.Normals[i].x, t),
				glm::lerp(previousNormal.y, p_polygon.Normals[i].y, t),
				glm::lerp(previousNormal.z, p_polygon.Normals[i].z, t)
			};

			for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
			{
				insideVaryings[insideVerticesCount][j] = glm::lerp(previousVarying[j], p_polygon.Varyings[i][j], t);
			}

			insideVerticesCount++;
		}

		if (IsInFront(p_polygon.Vertices[i]))
		{
			insideVertices[insideVerticesCount] = p_polygon.Vertices[i];
			insideTexCoords[insideVerticesCount] = p_polygon.TexCoords[i];
			insideNormals[insideVerticesCount] = p_polygon.Normals[i];

			for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
			{
				insideVaryings[insideVerticesCount][j] = p_polygon.Varyings[i][j];
			}

			insideVerticesCount++;
		}

		previousVertex = p_polygon.Vertices[i];
		previousTexCoords = p_polygon.TexCoords[i];
		previousNormal = p_polygon.Normals[i];
		previousVarying = p_polygon.Varyings[i];
		previousPlaneDistance = currentPlaneDistance;
	}

	p_polygon.VerticesCount = insideVerticesCount;

	for (int i = 0; i < insideVerticesCount; i++)
	{
		p_polygon.Vertices[i] = insideVertices[i];
		p_polygon.TexCoords[i] = insideTexCoords[i];
		p_polygon.Normals[i] = insideNormals[i];

		for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
		{
			p_polygon.Varyings[i][j] = insideVaryings[i][j];
		}
	}
}

void AmberGL::Geometry::Plane::ClipLine(Polygon& p_polygon) const
{
	if (p_polygon.VerticesCount != 2)
		return;

	float startPointPlaneDistance = SignedDistance(p_polygon.Vertices[0]);
	float endPointPlaneDistance = SignedDistance(p_polygon.Vertices[1]);

	if (startPointPlaneDistance < 0.0f && endPointPlaneDistance < 0.0f)
	{
		p_polygon.VerticesCount = 0;
		return;
	}

	if (startPointPlaneDistance >= 0.0f && endPointPlaneDistance >= 0.0f)
	{
		return;
	}

	float t = startPointPlaneDistance / (startPointPlaneDistance - endPointPlaneDistance);

	glm::vec4 intersectionPoint = 
	{
		glm::lerp(p_polygon.Vertices[0].x, p_polygon.Vertices[1].x, t),
		glm::lerp(p_polygon.Vertices[0].y, p_polygon.Vertices[1].y, t),
		glm::lerp(p_polygon.Vertices[0].z, p_polygon.Vertices[1].z, t),
		glm::lerp(p_polygon.Vertices[0].w, p_polygon.Vertices[1].w, t)
	};

	glm::vec2 intersectionTexCoord = 
	{
		glm::lerp(p_polygon.TexCoords[0].x, p_polygon.TexCoords[1].x, t),
		glm::lerp(p_polygon.TexCoords[0].y, p_polygon.TexCoords[1].y, t)
	};

	glm::vec3 intersectionNormal = 
	{
		glm::lerp(p_polygon.Normals[0].x, p_polygon.Normals[1].x, t),
		glm::lerp(p_polygon.Normals[0].y, p_polygon.Normals[1].y, t),
		glm::lerp(p_polygon.Normals[0].z, p_polygon.Normals[1].z, t)
	};

	float intersectionVaryings[16];
	for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
	{
		intersectionVaryings[j] = glm::lerp(p_polygon.Varyings[0][j], p_polygon.Varyings[1][j], t);
	}

	if (startPointPlaneDistance < 0.0f)
	{
		p_polygon.Vertices[0] = intersectionPoint;
		p_polygon.TexCoords[0] = intersectionTexCoord;
		p_polygon.Normals[0] = intersectionNormal;

		for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
		{
			p_polygon.Varyings[0][j] = intersectionVaryings[j];
		}
	}
	else
	{
		p_polygon.Vertices[1] = intersectionPoint;
		p_polygon.TexCoords[1] = intersectionTexCoord;
		p_polygon.Normals[1] = intersectionNormal;

		for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
		{
			p_polygon.Varyings[1][j] = intersectionVaryings[j];
		}
	}
}

void AmberGL::Geometry::Plane::ClipPoint(Polygon& p_polygon) const
{
	if (p_polygon.VerticesCount != 1)
		return;

	if (SignedDistance(p_polygon.Vertices[0]) < 0.0f)
	{
		p_polygon.VerticesCount = 0;
	}
}

float AmberGL::Geometry::Plane::SignedDistance(const glm::vec4& p_point) const
{
	return glm::dot(p_point, glm::vec4(Normal.x, Normal.y, Normal.z, Distance));
}

bool AmberGL::Geometry::Plane::IsInFront(const glm::vec4& p_point) const
{
	return SignedDistance(p_point) >= 0.0f;
}
