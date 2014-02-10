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

namespace dtEntity
{

   class DebugDrawInterface;
   DT_ENTITY_EXPORT void SetDebugDrawInterface(DebugDrawInterface*);
   DT_ENTITY_EXPORT DebugDrawInterface* GetDebugDrawInterface();

   class InputInterface;
   DT_ENTITY_EXPORT void SetInputInterface(InputInterface*);
   DT_ENTITY_EXPORT InputInterface* GetInputInterface();

   class SystemInterface;
   DT_ENTITY_EXPORT void SetSystemInterface(SystemInterface*);
   DT_ENTITY_EXPORT SystemInterface* GetSystemInterface();

   class WindowInterface;
   DT_ENTITY_EXPORT void SetWindowInterface(WindowInterface*);
   DT_ENTITY_EXPORT WindowInterface* GetWindowInterface();

}
