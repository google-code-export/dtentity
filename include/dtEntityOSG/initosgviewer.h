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

#include <dtEntityOSG/export.h>
#include <dtEntity/logmanager.h>

namespace osg
{
   class Group;
}

namespace osgViewer
{
   class ViewerBase;
}

namespace dtEntity
{
   class EntityManager;
}

namespace dtEntityOSG
{

   /**
    * Convenience setup function for dtEntity.
    * If you want to do fancy stuff in your setup then maybe you should not call this, instead write your own!
    *
    * Installs a log handler, calls SetupDataPaths(), calls InitDtEntity(), calls SetupViewer(), then calls DoScreenSetup()
    * @param argc number of command line args
    * @param standard c command line args
    * @param viewer An instance of either osgViewer::Viewer or osgViewer::CompositeViewer
    * @param em a valid entity manager
    * @param addStatsHandler Should osg stats handler be installed (activated with key insert)
    * @param checkPaths Should SetupDataPaths complain about missing paths
    * @param addConsoleLog Should log messages be output to console
    * @param pSceneNode An osg group that is used as scene graph root node. This is added as scene node to the main scene, also
    *        the main update callback that drives dtEntity is installed to this node. If this is NULL, a new osg::Group is created
    */
   bool DTENTITY_OSG_EXPORT InitOSGViewer(int argc, char** argv,
      osgViewer::ViewerBase& viewer, dtEntity::EntityManager& em,
      bool addStatsHandler = true, bool checkPaths = true, bool addConsoleLog = true, osg::Group* pSceneNode = 0);


   /**
    * Sets scene graph root node and adds callbacks
    * @param viewer An instance of either osgViewer::Viewer or osgViewer::CompositeViewer
    * @param pSceneNode An osg group that is used as scene graph root node. This is added as scene node to the main scene, also
    *        the main update callback that drives dtEntity is installed to this node. 
    */
   void DTENTITY_OSG_EXPORT SetupSceneGraph(osgViewer::ViewerBase& viewer, dtEntity::EntityManager& em, osg::Group* pSceneNode);


   /** 
     * analyze command line and open OSG window with correct traits
     * @return true if success
     */
   bool DTENTITY_OSG_EXPORT DoScreenSetup(int argc, char** argv, osgViewer::ViewerBase& viewer, dtEntity::EntityManager& em);
}
