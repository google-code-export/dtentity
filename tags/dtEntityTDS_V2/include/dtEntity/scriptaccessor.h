#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <osg/Referenced>
#include <dtEntity/export.h>
#include <dtEntity/property.h>
#include <dtEntity/FastDelegate.h>
#include <dtEntity/FastDelegateBind.h>
#include <vector>

namespace dtEntity
{


   typedef std::vector<const Property*> PropertyArgs;
   typedef fastdelegate::FastDelegate1<const PropertyArgs&, Property*> ScriptMethodFunctor;

   class DT_ENTITY_EXPORT ScriptAccessor
   {
   public:

      virtual ~ScriptAccessor() { }

      void AddScriptedMethod(const std::string& methodname, const ScriptMethodFunctor& functor);

      // Caller is responsible for deleting the returned property!
      Property* CallScriptedMethod(const std::string& methodname, const PropertyArgs& args) const;

      void GetMethodNames(std::vector<std::string>& names) const;

   private:
      std::map<std::string,ScriptMethodFunctor> mFunctors;
   };
}

