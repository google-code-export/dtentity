/* -*-c++-*-
* testDebugDrawManager - testDebugDrawManager(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* Martin Scheffler
*/
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/component.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/windowmanager.h>
#include <dtEntityPhysX/pagedterraincullvisitor.h>
#include <dtEntityPhysX/physxcomponent.h>
#include <dtEntityPhysX/physxpagedterraincomponent.h>
#include <osgViewer/Renderer>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/TrackballManipulator>
#include <osgSim/DOFTransform>

using namespace dtEntity;

////////////////////////////////////////////////////////////////////////////////
class FindNamedNodeVisitor : public osg::NodeVisitor
{
   std::string mName;
   osg::ref_ptr<osg::Node> mNode;

public:
   FindNamedNodeVisitor(const std::string& name)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , mName(name)
   {
   }

    virtual void apply(osg::Node& node)
    {
       if(node.getName() == mName)
       {
          mNode = &node;
       }
       else
       {
          traverse(node);
       }
    }

    osg::Node* getNode()
    {
        return mNode.get();
    }
};

////////////////////////////////////////////////////////////////////////////////

class WheelSystem;
// this component stores speed and force vectos for an entity.
class WheelComponent : public Component
{

   friend class WheelSystem;

private:
   typedef std::list<std::pair<osgSim::DOFTransform*, float> > DOFList;
   DOFList mDOFs;
   Entity* mEntity;
   dtEntity::ArrayProperty mWheels;
   dtEntity::FloatProperty mSpeed;
   bool mInit; 
public:
   static const dtEntity::StringId TYPE;
   static const dtEntity::StringId WheelsId;
   static const dtEntity::StringId RadiusId;
   static const dtEntity::StringId NodeNameId;
   static const dtEntity::StringId SpeedId;
   

   WheelComponent()
      : mEntity(NULL)
      , mInit(false)
   {
      Register(WheelsId, &mWheels);
      Register(SpeedId, &mSpeed);
   }

   virtual dtEntity::ComponentType GetType() const { return TYPE; }

   virtual bool IsInstanceOf(dtEntity::ComponentType id) const
   { 
      return ((id == TYPE)); 
   }

   virtual void OnAddedToEntity(Entity& entity)
   {
      mEntity = &entity;
   }

   virtual void OnRemovedFromEntity(Entity& entity)
   {
      mEntity = NULL;
   }

   void SetSpeed(float speed)
   {
      for(DOFList::iterator i = mDOFs.begin(); i != mDOFs.end(); ++i)
      {
         osgSim::DOFTransform* doft = i->first;
         float radius = i->second;
         doft->setAnimationOn(true);
         float rot = speed / (radius * osg::PI * 2);
         doft->setIncrementHPR(osg::Vec3(0, rot, 0));
      }
   }

   virtual void Init()
   {
      if(mInit)
      {
         return;
      }
      mInit = true;

      assert(mEntity != NULL);

      StaticMeshComponent* smc; 
      if(!mEntity->GetComponent(smc))
      {
         LOG_ERROR("Error setting up wheel component: Please add mesh component first");
         return;
      }
     
      PropertyArray props = mWheels.ArrayValue();
      for(PropertyArray::iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* prop = *i;
         assert(prop->GetType() == DataType::GROUP);
         PropertyGroup pg = prop->GroupValue();
         assert(pg.find(SID("NodeName")) != pg.end());
         assert(pg.find(SID("Radius")) != pg.end());
         std::string nodeName = pg[SID("NodeName")]->StringValue();
         
         float radius = pg[SID("Radius")]->FloatValue();
         
         osg::ref_ptr<FindNamedNodeVisitor> v = new FindNamedNodeVisitor(nodeName);
         smc->GetNode()->accept(*v);
         osgSim::DOFTransform* trans = dynamic_cast<osgSim::DOFTransform*>(v->getNode());

         if(trans != NULL)
         {
            mDOFs.push_back(std::make_pair(trans, radius));
         }
      }
      SetSpeed(mSpeed.FloatValue());
   
   
   }
};

const dtEntity::StringId WheelComponent::TYPE(dtEntity::SID("Wheel"));
const dtEntity::StringId WheelComponent::SpeedId(dtEntity::SID("Speed"));
const dtEntity::StringId WheelComponent::WheelsId(dtEntity::SID("Wheels"));


////////////////////////////////////////////////////////////////////////////////


// iterates over MovementComponents and updates the translation of the entity
// after applying forces
class WheelSystem
   : public dtEntity::DefaultEntitySystem<WheelComponent>
{  

public:
   WheelSystem(dtEntity::EntityManager& em)
      : DefaultEntitySystem<WheelComponent>(em)
   {
      mTickFunctor = dtEntity::MessageFunctor(this, &WheelSystem::Tick);
      em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
   }

   void Tick(const dtEntity::Message& msg)
   {
      assert(msg.GetType() == dtEntity::TickMessage::TYPE);
      float dt = msg.GetFloat(dtEntity::TickMessage::DeltaSimTimeId);
      if(dt > 0.05f) dt = 0.05f; // stabilize

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         WheelComponent* mcomp = i->second;
         mcomp->Init();
         WheelComponent::DOFList::iterator j;
         for(j = mcomp->mDOFs.begin(); j != mcomp->mDOFs.end(); ++j)
         {
            j->first->animate(dt);
         }
      }      
   }

private:
   dtEntity::MessageFunctor mTickFunctor;
};



////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
   osg::ArgumentParser arguments(&argc,argv);   
   osgViewer::Viewer viewer(arguments);
   dtEntity::EntityManager em;
   
   if(!dtEntity::InitOSGViewer(argc, argv, &viewer, &em))
   {
      LOG_ERROR("Error setting up dtEntity!");
      return 0;
   }
   em.AddEntitySystem(*new WheelSystem(em));
   
   dtEntity::MapSystem* mSystem;
   em.GetEntitySystem(dtEntity::MapComponent::TYPE, mSystem);
   mSystem->LoadScene("Scenes/wheels.dtescene");

   return viewer.run();
}
