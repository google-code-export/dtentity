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
#include <dtEntity/stringid.h>
#include <vector>
#include <map>
#include <dtEntity/dtentity_config.h>

namespace dtEntity
{
   class Property;

   /**
    * Holds a number of properties. This container does NOT take ownership
    * of properties, they are not deleted in the constructor.
    */
   class DT_ENTITY_EXPORT PropertyContainer : public GroupProperty
   {
   public:

      PropertyContainer() {}

      virtual ~PropertyContainer() 
      {
         // don't delete properties, they are not on heap
         mValue.clear();
      }
      
#if CALL_ONPROPERTYCHANGED_METHOD == 0
      // hide when compiling with new property system
   private:
#endif
      /**
       * Can be overridden to react to changes of properties.
       * Should be called by user when changing a property on the
       * PropertyContainer.
       */
      virtual void OnPropertyChanged(StringId propname, Property& prop) {}

   public:

      /**
       * should be called when a set of interdependent
       * properties were changed.
       * The component can overwrite this method to react to these
       * changes.
       */
      virtual void Finished() {}

      /**
       * template helper for creating Clone() methods
       */
      template <class T>
      T* CloneContainer() const;

      /**
       * Copy property values from other to this
       */
      void InitFrom(const PropertyContainer& other);

      /**
       * Set value of property registered with given string id.
       * In debug mode this throws an assertion when a component
       * is set to a wrong type.
       */
      void SetArray(StringId name, const std::vector<Property*>& val);
      void SetBool(StringId name, bool val);
      void SetDouble(StringId name, double val);
      void SetFloat(StringId name, float val);
      void SetGroup(StringId name, const std::map<StringId, Property*>& val);
      void SetInt(StringId name, int val);
      void SetMatrix(StringId name, const Matrix& val);
      void SetQuat(StringId name, const Quat& val);
      void SetString(StringId name, const std::string& val);
      void SetStringId(StringId name, StringId val);
      void SetUInt(StringId name, unsigned int val);
      void SetVec2(StringId name, const Vec2f& val);
      void SetVec3(StringId name, const Vec3f& val);
      void SetVec4(StringId name, const Vec4f& val); 
      void SetVec2d(StringId name, const Vec2d& val);
      void SetVec3d(StringId name, const Vec3d& val);
      void SetVec4d(StringId name, const Vec4d& val); 

      /**
       * Return value of property registered with given string id
       */
      std::vector<Property*> GetArray(StringId name) const;
      bool GetBool(StringId name) const;
      double GetDouble(StringId name) const;
      float GetFloat(StringId name) const;
      std::map<StringId, Property*> GetGroup(StringId name) const;
      int GetInt(StringId name) const;
      Matrix GetMatrix(StringId name) const;
      Quat GetQuat(StringId name) const;
      std::string GetString(StringId name) const;
      StringId GetStringId(StringId name) const;
      unsigned int GetUInt(StringId name) const;
      Vec2f GetVec2(StringId name) const;
      Vec3f GetVec3(StringId name) const;
      Vec4f GetVec4(StringId name) const;      
      Vec2d GetVec2d(StringId name) const;
      Vec3d GetVec3d(StringId name) const;
      Vec4d GetVec4d(StringId name) const; 

   protected:
      
      /**
       * Register a property under the given string id
       */
      void Register(StringId name, Property* prop);
      
   private:

      // no copy constructor
      PropertyContainer(const PropertyContainer& other);
   };

   template <class T>
   T* PropertyContainer::CloneContainer() const
   {
      T* ret = new T();
      ret->InitFrom(*this);
      return ret;
   }

}
