#include "Mesh.h"
#include <fstream>

void MeshImport(Mesh* mesh, const char* filename)
{
	std::ifstream in;
	in.open(filename, std::ios::binary);

	size_t position_count = 0;
	size_t normal_count = 0;
	size_t index_count = 0;
	in.read((char*)&position_count, sizeof(position_count));
	in.read((char*)&normal_count, sizeof(normal_count));
	in.read((char*)&index_count, sizeof(index_count));

	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<uint16_t> indices;
	positions.resize(position_count);
	normals.resize(normal_count);
	indices.resize(index_count);

	in.read((char*)positions.data(), sizeof(Vector3) * position_count);
	in.read((char*)normals.data(), sizeof(Vector3) * normal_count);
	in.read((char*)indices.data(), sizeof(Vector3) * index_count);
	in.close();

	MeshTriangulate(mesh, positions, normals, indices);
}

void MeshTriangulate(Mesh* mesh, const std::vector<Vector3>& positions, const std::vector<Vector3>& normals, const std::vector<uint16_t>& indices)
{
	mesh->face_count = indices.size() / 3;
	mesh->positions.resize(indices.size());
	mesh->normals.resize(indices.size());

	for (size_t f = 0; f < mesh->face_count; f++)
	{
		size_t v = f * 3;
		mesh->positions[v + 0] = positions[indices[v + 0]];
		mesh->positions[v + 1] = positions[indices[v + 1]];
		mesh->positions[v + 2] = positions[indices[v + 2]];

		mesh->normals[v + 0] = normals[indices[v + 0]];
		mesh->normals[v + 1] = normals[indices[v + 1]];
		mesh->normals[v + 2] = normals[indices[v + 2]];
	}
}

void MeshUnload(Mesh* mesh)
{
	mesh->positions.resize(0);
	mesh->normals.resize(0);
	mesh->face_count = 0;
}
