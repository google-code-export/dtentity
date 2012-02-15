#pragma once

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

#include <dtEntityWrappers/export.h>
#include <CEGUI/CEGUIEvent.h>
#include <v8.h>
#include <map>

namespace CEGUI
{
   class Window;
   class EventArgs;
}


namespace dtEntityWrappers
{
   class ScriptSystem;
   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Object> WrapGuiWidget(ScriptSystem* ss, CEGUI::Window* v);
   DTENTITY_WRAPPERS_EXPORT CEGUI::Window*  UnwrapGuiWidget(v8::Handle<v8::Value>);
   DTENTITY_WRAPPERS_EXPORT bool IsGuiWidget(v8::Handle<v8::Value>);

}