#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

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

