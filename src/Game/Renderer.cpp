#include "Renderer.h"
#include "../ContestAPI/app.h"
#include <algorithm>

struct Face
{
	Vector3 position[3];
	Vector4 clip[3];
	Vector3 ndc[3];
	Vector3 normal;
};

struct Rectangle
{
	int x_min;
	int x_max;
	int y_min;
	int y_max;
};

inline void DrawTriangle(Vector4 v0, Vector4 v1, Vector4 v2, Vector3 c0, Vector3 c1, Vector3 c2, bool wireframe)
{
	App::DrawTriangle(v0.x, v0.y, v0.z, v0.w, v1.x, v1.y, v1.z, v1.w, v2.x, v2.y, v2.z, v2.w, c0.x, c0.y, c0.z, c1.x, c1.y, c1.z, c2.x, c2.y, c2.z, wireframe);
}

void DrawMesh(const Mesh& mesh, const UniformData& data, FragmentShader shader, bool wireframe)
{
	Matrix normal_matrix = MatrixNormal(data.world);

	std::vector<Face> faces;
	faces.resize(mesh.face_count);
	for (size_t f = 0; f < mesh.face_count; f++)
	{
		size_t v = f * 3;
		for (size_t i = 0; i < 3; i++)
		{
			Vector3 position_local = mesh.positions[v + i];
			faces[f].position[i] = position_local * data.world;
			faces[f].clip[i] = Vector4{ position_local.x, position_local.y, position_local.z, 1.0f } * data.mvp;
			faces[f].ndc[i] = MatrixPerspectiveDivide(data.mvp, position_local);
		}
		faces[f].normal = Vector3Normalize(mesh.normals[f] * normal_matrix);
	}

	for (const Face& face : faces)
	{
		// Backface culling (API enables depth-test but not face-culling)...
		{
			Vector3 face_normal = Vector3Normalize(Vector3CrossProduct(Vector3Normalize(face.ndc[1] - face.ndc[0]), Vector3Normalize(face.ndc[2] - face.ndc[0])));
			if (Vector3DotProduct(face_normal, Vector3UnitZ) < 0.0f) continue;
		}

		// (Not worth it to interpolate 3 colors from 3 shaders just to get the same colour that you can get from a single shader)
		Fragment frag;
		frag.p = (face.position[0] + face.position[1] + face.position[2]) / 3.0f;
		frag.n = face.normal;
		Vector3 color = shader(data, frag);
		::DrawTriangle(face.clip[0], face.clip[1], face.clip[2], color, color, color, wireframe);

		// New API produces a near-identical result but with 3x the fragment shader evaluations...
		//Vector3 colors[3];
		//for (int i = 0; i < 3; i++)
		//{
		//	Fragment frag;
		//	frag.p = face.position[i];
		//	frag.n = face.normal;
		//	colors[i] = shader(data, frag);
		//}
		//::DrawTriangle(face.clip[0], face.clip[1], face.clip[2], colors[0], colors[1], colors[2], wireframe);
	}
}
