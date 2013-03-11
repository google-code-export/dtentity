#include "cal3dcomponent.h"

#include <osgCal/CoreModel>
#include <osgCal/Model>

namespace dtEntityCal3d
{
   ////////////////////////////////////////////////////////////////////////////
   osg::Node* makeModel( osgCal::CoreModel* cm, osgCal::BasicMeshAdder* ma, int animNum = -1 )
   {
      osgCal::Model* model = new osgCal::Model();

      model->load(cm, ma);

      if(animNum != -1)
      {
         model->blendCycle( animNum, 1.0f, 0 );
      }

      return model;
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId Cal3dComponent::TYPE(dtEntity::SID("Cal3d"));
   const dtEntity::StringId Cal3dComponent::PathId(dtEntity::SID("Path"));


   ////////////////////////////////////////////////////////////////////////////
   Cal3dComponent::Cal3dComponent()
      : mEntity(NULL)
   {
      Register(PathId, &mPath);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   Cal3dComponent::~Cal3dComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void Cal3dComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      BaseClass::OnAddedToEntity(e);
      mEntity = &e;
   }

   ////////////////////////////////////////////////////////////////////////////
   void Cal3dComponent::OnRemovedFromEntity(dtEntity::Entity& e)
   {
     BaseClass::OnRemovedFromEntity(e);
   }

   ////////////////////////////////////////////////////////////////////////////
   void Cal3dComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == PathId)
      {
         SetPath(prop.StringValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void Cal3dComponent::SetPath(const std::string& fn)
   {
      mPath.Set(fn);

      osg::ref_ptr<osgCal::CoreModel> coreModel = new osgCal::CoreModel();
      int animNum = -1;

      osg::ref_ptr<osgCal::BasicMeshAdder> meshAdder = new osgCal::DefaultMeshAdder();
      osg::ref_ptr<osgCal::MeshParameters> p = new osgCal::MeshParameters();
            
      p->software = false; // default
         
      try
      {
         coreModel->load(fn, p.get());         
      }
      catch ( std::runtime_error& e )
      {
         LOG_ERROR("runtime error during load:" << e.what());
         return;
      }

      SetNode(makeModel(coreModel.get(), meshAdder.get(), animNum));

      //animationNames = coreModel->getAnimationNames();
   }

   ////////////////////////////////////////////////////////////////////////////
   void Cal3dComponent::Finished()
   {
   }

  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   Cal3dSystem::Cal3dSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {    
   }

   ////////////////////////////////////////////////////////////////////////////
   Cal3dSystem::~Cal3dSystem()
   {
   }
}
