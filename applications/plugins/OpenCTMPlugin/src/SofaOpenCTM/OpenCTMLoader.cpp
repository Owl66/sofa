/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/ObjectFactory.h>
#include <SofaOpenCTM/OpenCTMLoader.h>
#include <fstream>
#include <openctm.h>

namespace sofa::component::loader
{

using namespace sofa::defaulttype;

int OpenCTMLoaderClass = core::RegisterObject("Specific mesh loader for STL file format.")
        .add< OpenCTMLoader >()
        ;


OpenCTMLoader::OpenCTMLoader()
    : MeshLoader()
    , texCoords(initData(&texCoords,"texcoords","Texture coordinates of all faces, to be used as the parent data of a VisualModel texcoords data"))
{
}


bool OpenCTMLoader::load()
{
    // -- Loading file
    const char* filename = m_filename.getFullPath().c_str();
    std::ifstream file(filename);

    if (!file.good())
    {
        serr << "Error: OpenCTMLoader: Cannot read file '" << m_filename << "'." << sendl;
        return false;
    }
    file.close();

    return this->readOpenCTM(filename);

}

bool OpenCTMLoader::readOpenCTM(const char *filename)
{
    // Load the file using the OpenCTM API
    CTMimporter ctm;

    // Load the OpenCTM file
    ctm.Load(filename);

    // Access the mesh vertices
    CTMuint vertCount = ctm.GetInteger(CTM_VERTEX_COUNT);
    const CTMfloat * ctmVertices = ctm.GetFloatArray(CTM_VERTICES);
    // Access the mesh triangles
    CTMuint triCount = ctm.GetInteger(CTM_TRIANGLE_COUNT);
    const CTMuint  * indices = ctm.GetIntegerArray(CTM_INDICES);

    // Filling vertices buffer
    type::vector<sofa::type::Vector3>& my_positions = *(d_positions.beginEdit());
    my_positions.fastResize(vertCount);
    for (unsigned int i=0; i<vertCount; ++i)
    {
        my_positions[i][0] = ctmVertices[i*3];
        my_positions[i][1] = ctmVertices[i*3 + 1];
        my_positions[i][2] = ctmVertices[i*3 + 2];
    }
    d_positions.endEdit();

    // Filling triangles buffer
    type::vector<Triangle>& my_triangles = *(d_triangles.beginEdit());
    my_triangles.fastResize(triCount);
    for (unsigned int i=0; i<triCount; ++i)
    {
        my_triangles[i][0] = indices[i*3];
        my_triangles[i][1] = indices[i*3 + 1];
        my_triangles[i][2] = indices[i*3 + 2];
    }
    d_triangles.endEdit();

    // Checking if mesh containes normals, otherwise fill empty buffer (NB seems mendatory for mecaObj)
    if (ctm.GetInteger(CTM_HAS_NORMALS) == CTM_TRUE)
    {
        type::vector<sofa::type::Vec<3,SReal> >& my_normals   = *(d_normals.beginEdit());
        my_normals.fastResize(vertCount);

        // Access the mesh normals        
        const CTMfloat * ctmNormals = ctm.GetFloatArray(CTM_NORMALS);
        for (unsigned int i=0; i<vertCount; ++i)
        {
            my_normals[i][0] = ctmNormals[i*3];
            my_normals[i][1] = ctmNormals[i*3 + 1];
            my_normals[i][2] = ctmNormals[i*3 + 2];
        }
        d_normals.endEdit();
    }

    // Checking if mesh containes texture coordinates. Only one set of UV is handled in SOFA
    if(ctm.GetInteger(CTM_UV_MAP_COUNT) > 0)
    {
        const CTMfloat * ctmTexCoords = ctm.GetFloatArray(CTM_UV_MAP_1);
        type::vector<sofa::type::Vector2>& my_texCoords = *texCoords.beginEdit();
        my_texCoords.fastResize(vertCount);
        for (unsigned int i=0; i<vertCount; ++i)
        {
            my_texCoords[i][0] = ctmTexCoords[i*3];
            my_texCoords[i][1] = ctmTexCoords[i*3 + 1];
        }
        texCoords.endEdit();
    }


    return true;
}

} // namespace sofa::component::loader
