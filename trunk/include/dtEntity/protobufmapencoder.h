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
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapencoder.h>
#include <iostream>

namespace dtEntity
{
   class MapSystem;

   class DT_ENTITY_EXPORT ProtoBufMapEncoder
         : public MapEncoder
   {
      
   public:

      ProtoBufMapEncoder(EntityManager& em);
      virtual ~ProtoBufMapEncoder();

      virtual bool LoadMapFromFile(const std::string& path);

      // empty destination means overwrite mapPath
      virtual bool SaveMapToFile(const std::string& mapPath, const std::string& destination = "");

      virtual bool LoadSceneFromFile(const std::string& path);

      // empty dest means overwrite original file
      virtual bool SaveSceneToFile(const std::string& path);

      virtual bool AcceptsMapExtension(const std::string& extension) const
      {
         return extension == "bmap";
      }

      virtual bool AcceptsSceneExtension(const std::string& extension) const
      {
         return extension == "bscene";
      }

      static bool EncodeMessage(const Message& m, std::iostream& stream);
      static Message* DecodeMessage(std::istream& stream);

   private:

      EntityManager* mEntityManager;
      MapSystem* mMapSystem;

   };
}

