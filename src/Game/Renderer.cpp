#include "Renderer.h"
#include "../ContestAPI/app.h"
#include <algorithm>

struct Face
{
	Vector3 position[3]; // world-space
	Vector3 normal[3];   // world-space

	Vector4 clip[3];
	Vector3 ndc[3];
};

inline void DrawTriangle(Vector4 v0, Vector4 v1, Vector4 v2, Vector3 c0, Vector3 c1, Vector3 c2, bool wireframe)
{
	App::DrawTriangle(v0.x, v0.y, v0.z, v0.w, v1.x, v1.y, v1.z, v1.w, v2.x, v2.y, v2.z, v2.w, c0.x, c0.y, c0.z, c1.x, c1.y, c1.z, c2.x, c2.y, c2.z, wireframe);
}

void DrawMesh(const Mesh& mesh, const UniformData& data, FragmentShader shader, bool wireframe)
{
	Matrix normal_matrix = MatrixNormal(data.world);

	std::vector<Face> faces(mesh.face_count);
	for (size_t f = 0; f < mesh.face_count; f++)
	{
		size_t v = f * 3;
		for (size_t i = 0; i < 3; i++)
		{
			Vector3 p = mesh.positions[v + i];
			Vector3 n = mesh.normals[v + i];

			faces[f].position[i] = p * data.world;
			faces[f].normal[i] = Vector3Normalize(n * normal_matrix);

			faces[f].clip[i] = Vector4{ p.x, p.y, p.z, 1.0f } * data.mvp;
			faces[f].ndc[i] = MatrixPerspectiveDivide(data.mvp, p);
		}
	}

	for (const Face& face : faces)
	{
		// Backface culling
		{
			Vector3 normal = Vector3Normalize(Vector3CrossProduct(Vector3Normalize(face.ndc[1] - face.ndc[0]), Vector3Normalize(face.ndc[2] - face.ndc[0])));
			if (Vector3DotProduct(normal, Vector3UnitZ) < 0.0f) continue;
		}

		Vector3 colors[3];
		for (int i = 0; i < 3; i++)
		{
			Fragment frag;
			frag.p = face.position[i];
			frag.n = face.normal[i];
			colors[i] = shader(data, frag);
		}

		::DrawTriangle(face.clip[0], face.clip[1], face.clip[2], colors[0], colors[1], colors[2], wireframe);
	}
}
