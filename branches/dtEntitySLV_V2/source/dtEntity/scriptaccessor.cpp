/*
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

#include <dtEntity/scriptaccessor.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   void ScriptAccessor::AddScriptedMethod(const std::string& methodname, const ScriptMethodFunctor& functor)
   {
      mFunctors[methodname] = functor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* ScriptAccessor::CallScriptedMethod(const std::string& methodname, const PropertyArgs& args) const
   {
      std::map<std::string,ScriptMethodFunctor>::const_iterator i = mFunctors.find(methodname);
      if(i == mFunctors.end())
      {
         return NULL;
      }
      return i->second(args);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ScriptAccessor::GetMethodNames(std::vector<std::string>& names) const
   {
      std::map<std::string,ScriptMethodFunctor>::const_iterator i;
      for(i = mFunctors.begin(); i != mFunctors.end(); ++i)
      {
         names.push_back(i->first);
      }
   }
}

