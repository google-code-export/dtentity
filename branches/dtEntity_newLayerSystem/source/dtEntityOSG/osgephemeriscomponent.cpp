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

#include <dtEntityOSG/osgephemeriscomponent.h>

#include <dtEntity/core.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/systemmessages.h>
#include <sstream>
#include <osgEphemeris/EphemerisModel.h>
#include <osg/ref_ptr>
#include <osg/LightModel>
#include <osgGA/GUIEventHandler>
#include <dtEntityOSG/osgsysteminterface.h>
#include <osgViewer/View>
#include <osg/io_utils>

namespace dtEntityOSG
{

   class TimeChangeHandler : public osgGA::GUIEventHandler
   {
   public:
       TimeChangeHandler(osgEphemeris::EphemerisModel *ephem) : m_ephem(ephem) {}

       virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
       {
           if (!ea.getHandled() && ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
           {
               if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_Add)
               {
                   // Increment time
                   // Hopefully the DateTime will wrap around correctly if we get 
                   // to invalid dates / times...
                   osgEphemeris::EphemerisData* data = m_ephem->getEphemerisData();
                   if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)          // Increment by one hour
                       data->dateTime.setHour( data->dateTime.getHour() + 1 );
                   else if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT)       // Increment by one day
                       data->dateTime.setDayOfMonth( data->dateTime.getDayOfMonth() + 1 );
                   else if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)      // Increment by one month
                       data->dateTime.setMonth( data->dateTime.getMonth() + 1 );
                   else                                                                    // Increment by one minute
                       data->dateTime.setMinute( data->dateTime.getMinute() + 1 );

                   return true;
               }

               else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_Subtract)
               {
                   // Decrement time
                   // Hopefully the DateTime will wrap around correctly if we get 
                   // to invalid dates / times...
                   osgEphemeris::EphemerisData* data = m_ephem->getEphemerisData();
                   if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)          // Decrement by one hour
                       data->dateTime.setHour( data->dateTime.getHour() - 1 );
                   else if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT)       // Decrement by one day
                       data->dateTime.setDayOfMonth( data->dateTime.getDayOfMonth() - 1 );
                   else if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)      // Decrement by one month
                       data->dateTime.setMonth( data->dateTime.getMonth() - 1 );
                   else                                                                    // Decrement by one minute
                       data->dateTime.setMinute( data->dateTime.getMinute() - 1 );

                   return true;
               }

           }

           return false;
       }

       virtual void getUsage(osg::ApplicationUsage& usage) const
       {
           usage.addKeyboardMouseBinding("Keypad +",       "Increment time by one minute");
           usage.addKeyboardMouseBinding("Shift Keypad +", "Increment time by one hour"  );
           usage.addKeyboardMouseBinding("Alt Keypad +",   "Increment time by one day"   );
           usage.addKeyboardMouseBinding("Ctrl Keypad +",  "Increment time by one month" );
           usage.addKeyboardMouseBinding("Keypad -",       "Decrement time by one minute");
           usage.addKeyboardMouseBinding("Shift Keypad -", "Decrement time by one hour"  );
           usage.addKeyboardMouseBinding("Alt Keypad -",   "Decrement time by one day"   );
           usage.addKeyboardMouseBinding("Ctrl Keypad -",  "Decrement time by one month" );
       }

       osg::ref_ptr<osgEphemeris::EphemerisModel> m_ephem;
   };

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId OSGEphemerisComponent::TYPE(dtEntity::SID("OSGEphemeris"));
   

   class SetRenderBinsVisitor : public osg::NodeVisitor
   {
      
   public:

      SetRenderBinsVisitor()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      {}

      
      virtual void apply(osg::Node& node)
      {
         if(dynamic_cast<osgEphemeris::SkyDome*>(&node) != NULL)
         {
            node.getOrCreateStateSet()->setRenderBinDetails(-10000,"RenderBin");
         }
         else
         {         
            node.getOrCreateStateSet()->setRenderBinDetails(-10010,"RenderBin");
         }
         traverse(node);
      }
   };
   ////////////////////////////////////////////////////////////////////////////
   OSGEphemerisComponent::OSGEphemerisComponent()
      : mDrawables(new osg::Group())
   {
      mModel = new osgEphemeris::EphemerisModel();
      SetNode(mModel);
      GetNode()->asGroup()->addChild(mDrawables);

      mModel->setAutoDateTime(true);      
      mModel->setSkyDomeRadius(8000);
      mModel->setLatitudeLongitude(53.5f,8.48f);
      mModel->setMembers(osgEphemeris::EphemerisModel::DEFAULT_MEMBERS);
      
      //mModel->setMoveWithEyePoint(true);
      mModel->setAutoDateTime(false);
      osgEphemeris::DateTime dateTime( 2012, 31, 7, 13, 0, 0 );
      
      mModel->setDateTime(dateTime);
      mModel->setSunFudgeScale(4);

      osg::ref_ptr<osg::StateSet> sset = GetNode()->getOrCreateStateSet();
      osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel;
      lightModel->setAmbientIntensity( osg::Vec4( 0.0025, 0.0025,0.0025, 1.0 ));
      sset->setAttributeAndModes( lightModel.get() );
      sset->setRenderBinDetails(-10000,"RenderBin");
      SetRenderBinsVisitor v;
      mModel->accept(v);

      dynamic_cast<dtEntityOSG::OSGSystemInterface*>(dtEntity::GetSystemInterface())->GetPrimaryView()->addEventHandler(new TimeChangeHandler(mModel));
   }


   ////////////////////////////////////////////////////////////////////////////
   OSGEphemerisComponent::~OSGEphemerisComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 OSGEphemerisComponent::GetSunLightPos()
   {
      if(mModel->getSunLightSource())
      {
         return mModel->getSunLightSource()->getLight()->getPosition();
      }
      return osg::Vec4();
   }

   ////////////////////////////////////////////////////////////////////////////
   void OSGEphemerisComponent::Update()
   {
      mModel->update();
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId OSGEphemerisSystem::TYPE(dtEntity::SID("OSGEphemeris"));
   
   ////////////////////////////////////////////////////////////////////////////
   OSGEphemerisSystem::OSGEphemerisSystem(dtEntity::EntityManager& em)
     : dtEntity::DefaultEntitySystem<OSGEphemerisComponent>(em)
   {
      mTickFunctor = dtEntity::MessageFunctor(this, &OSGEphemerisSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_LATE, "OSGEphemerisSystem::Tick");
   }

   ////////////////////////////////////////////////////////////////////////////
   OSGEphemerisSystem::~OSGEphemerisSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void OSGEphemerisSystem::Tick(const dtEntity::Message& msg)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->Update();
      }
   }
}
