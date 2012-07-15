#pragma once

#include <dtEntity/component.h>
#include <dtEntity/debugdrawmanager.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/property.h>
#include <osg/ref_ptr>

class TestComponent : public dtEntity::Component
{

public:

   /*
     Each component is associated with a unique component type.
     ComponentType is a StringId, a string hashed to an unsigned int.
   */
   static const dtEntity::ComponentType TYPE;

   /*
     Each property of the component has a name. These names are
     declared here and defined in the cpp. They are associated with
     their respective properties in the constructor of the component.
   */
   static const dtEntity::StringId ArrayTestId;
   static const dtEntity::StringId DateTimeId;
   static const dtEntity::StringId EnumId;
   static const dtEntity::StringId PathId;
   static const dtEntity::StringId ColorId;

   // CTor
   TestComponent();

   // DTor
   virtual ~TestComponent();

   /*
      Each component must override the GetType() method and return the
      component type.
   */
   virtual dtEntity::ComponentType GetType() const { return TYPE; }

   /*
     Each component must override the IsInstanceOf method.
     If the component is derived from another component then
     it may be wise to also call the IsInstaceOf method of the superclass.
     Check out dtEntity::TransformComponent and its child class
     dtEntity::MatrixTransformComponent for an example.
   */
   virtual bool IsInstanceOf(dtEntity::ComponentType id) const
   {
      return (id == TYPE);
   }

   /*
     When the editor or a map loader changes a property on a component,
     the OnPropertyChanged method is called. It can be overridden to
     react to property changes. It is good practice to call this method
     whenever you change a component property from outside
   */
   virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);

   /*
     This method is called when the component is assigned to an entity.
     It receives a reference to an entity object, which is basically
     a wrapper for the entity id and contains a reference to the entity manager.
     So if the component needs access to the outside world it is possible to
     store a reference to this entity object or to the entity manager.
   */
   virtual void OnAddedToEntity(dtEntity::Entity& entity);

   /*
     Is called before the component is deleted.
   */
   virtual void OnRemovedFromEntity(dtEntity::Entity& entity);

   /*
    * Is called when all properties were set.
    */
   virtual void Finished() {}

   /* While it is possible to access the properties of the component
      using the GetProperty method inherited from PropertyContainer,
      it is better to provide accessors that don't have to do a map lookup.
      You can also provide setters here, but make sure to sync with the
      OnPropertyChanged method.
    */
   osg::Vec4 GetColor() const { return mColor.Get(); }
   std::string GetEnum() const { return mEnum.Get(); }

private:

   /*
      The properties of the component. Each property holds a single
      primitive value. All property classes also provide generic methods
      like GetType, ToString and FromString for serialization.
   */
   dtEntity::ArrayProperty mArrayTest;
   dtEntity::UIntProperty mDateTime;
   dtEntity::StringProperty mEnum;
   dtEntity::StringProperty mPath;
   dtEntity::Vec4Property mColor;
};


////////////////////////////////////////////////////////////////////////////////

/*
   The entity system that holds the TestEntityComponents.
   It derives from a template class DefaultEntitySystem which
   provides a standard implementation for methods like
   CreateComponent, GetComponent and so on.
   This is a good place to register for messages that
   should influence components.
*/
class TestSystem
   : public dtEntity::DefaultEntitySystem<TestComponent>
{
   // Have to call constructor of base class, so remember base class type
   typedef dtEntity::DefaultEntitySystem<TestComponent> BaseClass;

public:

   /*
     The constructor receives access to the entity manager.
     The entity manager can be used to register for messages or
     access other entity systems and components.
   */
   TestSystem(dtEntity::EntityManager& em);

   // Destructor
   ~TestSystem();

   /*
     This method will be called by the EntityManager when we
     register for tick messages
     */
   void Tick(const dtEntity::Message& msg);

private:

   // The message functor is used to register and deregister for a specific message
   dtEntity::MessageFunctor mTickFunctor;

   // A debug draw manager is a simple way to draw primitive shapes to the scene
   dtEntity::DebugDrawManager mDebugDrawManager;
};
