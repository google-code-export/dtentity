/*
* Delta3D Open Source Game and Simulation Engine
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

#include <dtEntityPhysX/pagedterraincullvisitor.h>

#include <dtEntityPhysX/physxpagedterraincomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/stringid.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/debugdrawmanager.h>
#include <osg/PagedLOD>
#include <osg/LOD>
#include <osg/ProxyNode>
#include <sstream>
#include <dtEntity/profile.h>

namespace dtEntityPhysX
{
   
   ///////////////////////////////////////////////////////////////////////////
   PagedTerrainCullVisitor::PagedTerrainCullVisitor(dtEntity::EntityManager& em) 
      : osgUtil::CullVisitor()
      , mCurrentlyInTerrain(false)
      , mCameraPosition()
      , mRadius(1250)
      , mPagingDistance(7500)
      , mCheckTerrainAmount(30)
      , mTerrainStep(0)
      , mRunFinalizeTerrain(false)
      , mHitProxyNode(false)
      , mEnablePhysics(true)
      , mInHighDetailLOD(false)
      , mInTopLevel(false)
   {
      PhysXPagedTerrainSystem* s;
      bool success = em.GetEntitySystem(PhysXPagedTerrainComponent::TYPE, s);
      assert(success && "Cannot find physx paged terrain component");
      mTerrainSystem = s;
   }

   ///////////////////////////////////////////////////////////////////////////
   PagedTerrainCullVisitor::~PagedTerrainCullVisitor()
   {

   }

   ///////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::SetEnablePhysics(bool b)
   {
      mEnablePhysics = b;
   }

   ///////////////////////////////////////////////////////////////////////////
   bool PagedTerrainCullVisitor::GetEnablePhysics() const
   {
      return mEnablePhysics;
   }

   ///////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::SetCookingRadius(float radius)
   {
      mRadius = radius;
   }

   ///////////////////////////////////////////////////////////////////////////
   float PagedTerrainCullVisitor::GetCookingRadius() const
   {
      return mRadius;
   }

   ///////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::SetCullRadius(float radius)
   {
      mPagingDistance = radius;
   }

   ///////////////////////////////////////////////////////////////////////////
   float PagedTerrainCullVisitor::GetCullRadius() const
   {
      return mPagingDistance;
   }

   ///////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::SetFrameDelay(int delay)
   {
      mCheckTerrainAmount = delay;
   }

   ///////////////////////////////////////////////////////////////////////////
   int PagedTerrainCullVisitor::GetFrameDelay() const
   {
      return mCheckTerrainAmount;
   }

   ///////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::apply(osg::Transform& node)
   {
      if(mTerrainNode.valid() 
         && ++mTerrainStep > mCheckTerrainAmount)
      {
         if(&node == mTerrainNode.get())
         {
            mTerrainStep = 0;
            
            if(mEnablePhysics)
            {
               mTerrainSystem->ResetTerrainIterator();
            }

            mCurrentlyInTerrain = true;
            mRunFinalizeTerrain = true;
         }
      }
      
      osgUtil::CullVisitor::apply(node);
      
      if(&node == mTerrainNode.get())
      {
         mCurrentlyInTerrain = false;
      }

      if(mEnablePhysics && mCurrentlyInTerrain == false && mRunFinalizeTerrain)
      {
         mRunFinalizeTerrain = mTerrainSystem->FinalizeTerrain(mCheckTerrainAmount - 1);
      }
      
   }

   ///////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::apply(osg::Geode& node)
   {
      // Terrex terrain for example
      if(mEnablePhysics)
      {
         if(mInHighDetailLOD && mCurrentlyInTerrain && node.getBoundingBox().valid())
         {
            osg::Vec3 position = node.getBoundingBox().center() - GetCameraTransform();
            
            osg::Matrix absMatrix;
            if(mHitProxyNode)
            {
               osg::NodePath& nodePath = getNodePath();
               absMatrix.set( osg::computeLocalToWorld(nodePath) );
               position = (node.getBoundingBox().center() + absMatrix.getTrans()) - GetCameraTransform();
            }

            if( position.length() <= mRadius)
            {
               mTerrainSystem->CheckGeode(node, mHitProxyNode, absMatrix);
            }
         }
         // ive terrain for example
         else if(mInHighDetailLOD && mCurrentlyInTerrain && node.getBound().valid())
         {
            osg::Vec3 position = node.getBound().center() - GetCameraTransform();
            osg::Matrix absMatrix;
            if(mHitProxyNode)
            {
               osg::NodePath& nodePath = getNodePath();
               absMatrix.set( osg::computeLocalToWorld(nodePath) );
               position = (node.getBound().center() + absMatrix.getTrans()) - GetCameraTransform();
            }

            if( position.length() <= mRadius)
            {
               mTerrainSystem->CheckGeode(node, mHitProxyNode, absMatrix);
            }
         }
      }
      osgUtil::CullVisitor::apply(node);
   }

   /////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::apply(osg::Group& node)
   {
      bool isTopLevel = false;
      if(!mInTopLevel)
      {
         mInTopLevel = true;
         isTopLevel = true;
         CProfileManager::Start_Profile(dtEntity::SID("PagedTerrainCullVisitor"));
      }
      
      bool hitframeDontCallOtherCull  = false;
      if(mCurrentlyInTerrain)
      {
         osg::ProxyNode* proxyNode = dynamic_cast<osg::ProxyNode*>(&node);
         if(proxyNode != NULL)
         {
            mHitProxyNode = true;
            osgUtil::CullVisitor::apply(node);
            hitframeDontCallOtherCull = true;
            mHitProxyNode = false;
         }
      }

      CProfileManager::Stop_Profile();
      if(hitframeDontCallOtherCull == false)
      { 
         osgUtil::CullVisitor::apply(node);
      }

      if(isTopLevel)
      {
         mInTopLevel = false;
         CProfileManager::Stop_Profile();
      }
   }

   /////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::apply(osg::PagedLOD& plod)
   {
       if (plod.getNumFileNames()>0)
       {
           // Identify the range value for the highest res child
           float targetRangeValue;
           if(plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT)
               targetRangeValue = 1e6; // Init high to find min value
           else
               targetRangeValue = 0; // Init low to find max value
               
           const osg::LOD::RangeList rl = plod.getRangeList();
           osg::LOD::RangeList::const_iterator rit;
           for( rit = rl.begin();
                rit != rl.end();
                ++rit )
           {
               if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
               {
                   if( rit->first < targetRangeValue )
                       targetRangeValue = rit->first;
               }
               else
               {
                   if( rit->first > targetRangeValue )
                       targetRangeValue = rit->first;
               }
           }

           // visit only on children that display at the maximum resolution.
           unsigned int childIndex;
           for( rit = rl.begin(), childIndex = 0;
                rit != rl.end();
                rit++, childIndex++ )
           {
               if( rit->first != targetRangeValue )
                   // This is not one of the highest res children
                   continue;

               osg::ref_ptr<osg::Node> child( NULL );
               if( plod.getNumChildren() > childIndex )
                   child = plod.getChild( childIndex );

               if (child.valid())
               {
                  mInHighDetailLOD = true;   
                  //child->accept(*this);
                  osgUtil::CullVisitor::apply(plod);  
                  mInHighDetailLOD = false;
                  return;
               }         
           }
           mInHighDetailLOD = false;
           osgUtil::CullVisitor::apply(plod);           
       }
   }

   /////////////////////////////////////////////////////////////////////////
   void PagedTerrainCullVisitor::apply(osg::LOD& node)
   {
      
      if(mCurrentlyInTerrain)
      {
         osg::Vec3 position = node.getCenter() - GetCameraTransform();
         
         if(position.length() > mPagingDistance)
         {
            osgUtil::CullVisitor::apply(node);
            return;
         }

         // push the culling mode.
         pushCurrentMask();

         // push the node's state.
         osg::StateSet* node_state = node.getStateSet();
         if (node_state) pushStateSet(node_state);

         handle_cull_callbacks_and_traverse(node);

         // pop the node's state off the render graph stack.    
         if (node_state) popStateSet();

         // pop the culling mode.
         popCurrentMask();
         
      }
      else
      {
         osgUtil::CullVisitor::apply(node);
      }
   }
}
