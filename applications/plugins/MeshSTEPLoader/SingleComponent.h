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

#ifndef SOFA_COMPONENT_ENGINE_SINGLECOMPONENT_H
#define SOFA_COMPONENT_ENGINE_SINGLECOMPONENT_H

#include <vector>
#include <sofa/core/DataEngine.h>
#include <sofa/core/objectmodel/DataFileName.h>
#include <sofa/type/Vec.h>

namespace sofa
{

namespace component
{

namespace engine
{

using namespace sofa::defaulttype;

template <class DataTypes>
class SingleComponent : public sofa::core::DataEngine
{
public:
    SOFA_CLASS(SingleComponent,sofa::core::DataEngine);

    SingleComponent();

    virtual void init() override;
    virtual void reinit() override;
    virtual void doUpdate() override;

    template <class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        return core::DataEngine::canCreate(obj, context, arg);
    }

    virtual std::string getTemplateName() const override
    {
        return templateName(this);
    }

    static std::string templateName(const SingleComponent<DataTypes>*)
    {
        return "int";
    }

protected:
    void loadMesh();

public:
    Data<type::vector<sofa::type::Vector3> > _positionsI; ///< input: vertices position of whole mesh
    Data<type::vector<sofa::type::Vector3> > _positionsO; ///< output: vertices position of the component
    Data<type::vector<type::fixed_array <unsigned int,2> > > _edgesI; ///< input: edges of whole mesh
    Data<type::vector<type::fixed_array <unsigned int,2> > > _edgesO; ///< output: edges of the component
    Data<type::vector<type::fixed_array <unsigned int,3> > > _trianglesI; ///< input: triangles of whole mesh
    Data<type::vector<type::fixed_array <unsigned int,3> > > _trianglesO; ///< output: triangles of the component
    Data<type::vector<sofa::type::Vector3> > _normalsI; ///< input: normals of the whole mesh
    Data<type::vector<sofa::type::Vector3> > _normalsO; ///< output: normals of the component
    Data<type::vector<sofa::type::Vector2> > _uvI; ///< input: UV coordinates of the whole mesh
    Data<type::vector<sofa::type::Vector2> > _uvO; ///< output: UV coordinates of the component
    Data<type::vector<type::fixed_array <unsigned int,3> > > _indicesComponents; ///< Shape # | number of nodes | number of triangles
    Data<int> _numberShape; ///< Shape number to be loaded (see Outputs tab of STEPLoader for a description of the shapes)
};

}

}

}

#endif
