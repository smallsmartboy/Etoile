/**
* Copyright(C) 2009-2012                
* @author Jing HUANG
* @file GLVBOTriMesh.h
* @brief VBO in OpenGL vertex buffer object
* @date 1/2/2011
*/

#pragma once
#ifndef GLVBO_TRI_MESH_H
#define GLVBO_TRI_MESH_H
#include "geometry/TriMesh.h"
#include "VBO.h"
#include "IBO.h"

namespace Etoile
{
	//using dynamic_draw using separate data structure for performance
	struct DynamicGLVBOSharedTriMesh : public SharedTriMesh
	{
	public:
		void initVBO();
		void drawElements();
		void drawCustumElements();
		std::vector<Vec3f> _vdata_buffer;
		std::vector<Vec3f> _ndata_buffer;
		std::vector<Vec2f> _tdata_buffer;
		VBOVec3f _vertex_vbo;
		VBOVec3f _normal_vbo;
		VBOVec2f _texcoord_vbo;
		std::vector<IBO> _ibos;
	};

	//using static_draw thus using interleaved data structure for performance
	struct StaticGLVBOSharedTriMesh : public SharedTriMesh
	{
		struct VertexData
		{
			Vec3f _v;
			Vec3f _n;
			Vec2f _t;
		};
	public:
		void initVBO();
		void drawElements();
		void drawCustumElements();
		std::vector<VertexData> _interleaveddata;
		VBO<VertexData> _interleavedvbo;
		std::vector<IBO> _ibos;
	};


	struct DynamicGLVBOSeparateTriMesh : public SeparateTriMesh
	{
	public:
		void initVBO();
		void drawElements();
		void drawCustumElements();
		std::vector<std::vector<Vec3f>> _vdata_buffer;
		std::vector<std::vector<Vec3f>> _ndata_buffer;
		std::vector<std::vector<Vec2f>> _tdata_buffer;
		std::vector<VBOVec3f> _vertex_vbo;
		std::vector<VBOVec3f> _normal_vbo;
		std::vector<VBOVec2f> _texcoord_vbo;
		std::vector<IBO> _ibos;
	};

	//using static_draw thus using interleaved data structure for performance
	struct StaticGLVBOSeparateTriMesh : public SeparateTriMesh
	{
		struct VertexData
		{
			Vec3f _v;
			Vec3f _n;
			Vec2f _t;
		};
	public:
		void initVBO();
		void drawElements();
		void drawCustumElements();
		std::vector<std::vector<VertexData>> _interleaveddata;
		std::vector<VBO<VertexData>> _interleavedvbo;
		std::vector<IBO> _ibos;
	};

}

#endif //GLVBO_TRI_MESH_H