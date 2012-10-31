#pragma once

#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/property.h>


namespace dtEntityCal3d
{

   class Cal3dComponent : public dtEntity::NodeComponent
   {
      typedef dtEntity::NodeComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId PathId;
     
      Cal3dComponent();
      virtual ~Cal3dComponent();

      void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);
      void OnAddedToEntity(dtEntity::Entity& e);
      void OnRemovedFromEntity(dtEntity::Entity& e);
      void Finished();

      void SetPath(const std::string& path);
      std::string GetPath() const { return mPath.Get(); }

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

   private:

      dtEntity::Entity* mEntity;
      dtEntity::StringProperty mPath;
      
   };


   ////////////////////////////////////////////////////////////////////////////////

   class Cal3dSystem
      : public dtEntity::DefaultEntitySystem<Cal3dComponent>
   {
      typedef dtEntity::DefaultEntitySystem<Cal3dComponent> BaseClass;
      
   public:
     
      Cal3dSystem(dtEntity::EntityManager& em);
      ~Cal3dSystem();

   private:

   };
}
