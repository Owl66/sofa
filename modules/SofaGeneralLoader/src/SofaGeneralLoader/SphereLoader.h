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
#pragma once
#include <SofaGeneralLoader/config.h>

#include <sofa/core/loader/BaseLoader.h>

namespace sofa::component::loader
{

class SphereLoader : public sofa::core::loader::BaseLoader
{
public:
    SOFA_CLASS(SphereLoader,sofa::core::loader::BaseLoader);
protected:
    SphereLoader();

public:
    // Point coordinates in 3D in double.
    Data< type::vector<sofa::type::Vec<3,SReal> > > d_positions; ///< Sphere centers
    Data< type::vector<SReal> > d_radius; ///< Radius of each sphere
    Data< type::Vector3 > d_scale; ///< Scale applied to sphere positions
    Data< type::Vector3 > d_rotation; ///< Rotation of the DOFs
    Data< type::Vector3 > d_translation; ///< Translation applied to sphere positions
    bool load() override;

    void applyTransform();

protected:
    type::vector<sofa::type::Vec<3,SReal> > m_savedPositions;
};

} //namespace sofa::component::loader
