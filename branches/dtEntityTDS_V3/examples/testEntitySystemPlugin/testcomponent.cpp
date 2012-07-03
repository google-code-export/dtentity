#include "testcomponent.h"

#include <dtEntity/entity.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/stringid.h>
#include <dtEntity/debugdrawmanager.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/positionattitudetransformcomponent.h>
#include <sstream>


/*
  Define the component type. dtEntity::SID takes a string and
  hashes it to an unsigned int.
*/
const dtEntity::StringId TestComponent::TYPE(dtEntity::SID("Test"));

/*
  Define the property names
*/
const dtEntity::StringId TestComponent::ArrayTestId(dtEntity::SID("ArrayTest"));
const dtEntity::StringId TestComponent::DateTimeId(dtEntity::SID("DateTime"));
const dtEntity::StringId TestComponent::EnumId(dtEntity::SID("Enum"));
const dtEntity::StringId TestComponent::PathId(dtEntity::SID("Path"));
const dtEntity::StringId TestComponent::ColorId(dtEntity::SID("Color"));

////////////////////////////////////////////////////////////////////////////
TestComponent::TestComponent()
{
   /*
      Add the properties to the component. Each call to Register
      adds an entry to a map mapping from StringId to property.
      All properties registered here are shown in the editor
      and will be saved to a map.
    */
   Register(ArrayTestId, &mArrayTest);
   Register(DateTimeId, &mDateTime);
   Register(EnumId, &mEnum);
   Register(PathId, &mPath);
   Register(ColorId, &mColor);

   /*
     Set initial values for properties. The color property defines
     the color of the text. We have to set the alpha value of the color to non-null,
     otherwise the text would be invisible.
   */
   mColor.Set(osg::Vec4(0, 0, 0, 1));

   /*
     Also define an initial text to show.
   */
   mEnum.Set("Initial value");
}

////////////////////////////////////////////////////////////////////////////
TestComponent::~TestComponent()
{
   // nothing to do here
}

////////////////////////////////////////////////////////////////////////////
void TestComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
{
   // Here it is possible to react to property changes. We don't have to, so we won't
   if(propname == ArrayTestId)
   {
      LOG_ALWAYS("Array property was set");
   }
}

////////////////////////////////////////////////////////////////////////////
void TestComponent::OnAddedToEntity(dtEntity::Entity& entity)
{
   // nothing to do...
}

////////////////////////////////////////////////////////////////////////////
void TestComponent::OnRemovedFromEntity(dtEntity::Entity& entity)
{
   // nothing to do...
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*
  The constructor initializer list passes the entity manager to the
  base class DefaultEntitySystem. Also an entity manager is created,
  which uses its access to the entity manager to immediately add
  itself to the scene.
*/
TestSystem::TestSystem(dtEntity::EntityManager& em)
   : BaseClass(em)
   , mDebugDrawManager(new dtEntity::DebugDrawManager(em))
{
   /*
     Register the method TestEntitySystem::Tick to be called
     when a message of type TickMessage::TYPE is posted to the entity manager
     This happens every simulation step, so the tick method can be used
     to do stuff in real time.
   */
   mTickFunctor = dtEntity::MessageFunctor(this, &TestSystem::Tick);
   em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);

   /*
     The debug draw manager is not automatically enabled, so do this here
  */
   mDebugDrawManager->SetEnabled(true);
}

////////////////////////////////////////////////////////////////////////////
TestSystem::~TestSystem()
{
   // Clean up. We have to deregister from tick messages, otherwise a crash occurs
   GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
}

////////////////////////////////////////////////////////////////////////////
void TestSystem::Tick(const dtEntity::Message& msg)
{
   /*
     We can be sure that only messages of type TickMessage are received here, so
     a static cast does just fine
   */
   //const dtEntity::TickMessage& tickMessage = static_cast<const dtEntity::TickMessage&>(msg);

   /*
     Get value of message property DeltaSimTime
   */
   //float dt = tickMessage.GetDeltaSimTime();

   /*
     Loop through all components of type TestEntityComponent
   */
   for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
   {
      // The id of the entity the component is assigned to
      dtEntity::EntityId id = i->first;

      // The actual component
      TestComponent* component = i->second;

      /*
        Get access to a component of type PositionAttitudeTransform on the same entity.
        If it does not exist then ignore component
      */
      dtEntity::PositionAttitudeTransformComponent* pcomp;
      if(GetEntityManager().GetComponent(id, pcomp))
      {
         // Get position property from transform component
         osg::Vec3 position = pcomp->GetPosition();

         // draw text a little higher
         position[2] += 1;

         // get color and text values from component
         osg::Vec4 color = component->GetColor();
         std::string label = component->GetEnum();

         // Draw a string at the position of the entity.
         // A duration value of 0 means the text is only rendered once.
         // Because the AddString method is called each frame this is
         // perfect.
         mDebugDrawManager->AddString(position, label, color, 0, true);
      }
   }
}
