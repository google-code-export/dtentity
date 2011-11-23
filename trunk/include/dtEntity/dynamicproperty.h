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

#include <dtEntity/export.h>
#include <dtEntity/property.h>
#include <dtEntity/FastDelegate.h>

namespace dtEntity
{
   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicStringProperty : public StringProperty
   {
   public:
      typedef fastdelegate::FastDelegate1< const std::string&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<std::string> GetValueCB;

      DynamicStringProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual const std::string StringValue() const { return mGetValueCallback(); }
      virtual StringId StringIdValue() const { return dtEntity::SID(StringValue()); }
      virtual void SetStringId(dtEntity::StringId v) { Set(GetStringFromSID(v)); }

      const std::string Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new StringProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.StringValue() == Get(); }
      void Set(const std::string& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Set(v); }
      virtual bool SetFrom(const Property& other) { Set(other.StringValue()); }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

}
