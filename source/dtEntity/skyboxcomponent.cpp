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

#include <dtEntity/skyboxcomponent.h>

#include <dtEntity/entitymanager.h>
#include <dtEntity/nodemasks.h>
#include <assert.h>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Transform>
#include <osg/Material>
#include <osg/NodeCallback>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TextureCubeMap>
#include <osg/VertexProgram>
#include <osgUtil/CullVisitor>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
   
namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   class MoveEarthySkyWithEyePointTransform : public osg::Transform
   {
   public:
      /** Get the transformation matrix which moves from local coords to world coords.*/
      virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
      {
         osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
         if (cv)
         {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.preMultTranslate(eyePointLocal);
         }
         return true;
      }

      /** Get the transformation matrix which moves from world coords to local coords.*/
      virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
      {
         osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
         if (cv)
         {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.postMultTranslate(-eyePointLocal);
         }
         return true;
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   struct TexMatCallback : public osg::NodeCallback
   {
   public:

       TexMatCallback(osg::TexMat& tm) :
           _texMat(tm)
       {
       }

       virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
       {
           osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
           if (cv)
           {
               const osg::Matrix& MV = *(cv->getModelViewMatrix());
               const osg::Matrix R = osg::Matrix::rotate( osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f)*
                                     osg::Matrix::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);

               osg::Quat q = MV.getRotate();
               const osg::Matrix C = osg::Matrix::rotate( q.inverse() );

               _texMat.setMatrix( C*R );
           }

           traverse(node,nv);
       }

       osg::TexMat& _texMat;
   };



   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   osg::TextureCubeMap* readCubeMap(SkyBoxComponent* component)
   {
      osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;
      std::string posx = component->GetString(SkyBoxComponent::TextureEastId);
      std::string negx = component->GetString(SkyBoxComponent::TextureWestId);
      std::string posz = component->GetString(SkyBoxComponent::TextureNorthId);
      std::string negz = component->GetString(SkyBoxComponent::TextureSouthId);
      std::string negy = component->GetString(SkyBoxComponent::TextureUpId);
      std::string posy = component->GetString(SkyBoxComponent::TextureDownId);

      if(posx == "" || posy == "" || posz == "" || negx == "" || negy == "" || negz == "")
      {
         return NULL;
      }
      osg::Image* imagePosX = osgDB::readImageFile(posx);
      osg::Image* imageNegX = osgDB::readImageFile(negx);
      osg::Image* imagePosY = osgDB::readImageFile(posy);
      osg::Image* imageNegY = osgDB::readImageFile(negy);
      osg::Image* imagePosZ = osgDB::readImageFile(posz);
      osg::Image* imageNegZ = osgDB::readImageFile(negz);

      if(imagePosX == NULL)
      {
         LOG_ERROR("Cannot read skybox east image from path " + posx);
      }
      if(imagePosY == NULL)
      {
         LOG_ERROR("Cannot read skybox down image from path " + posy);
      }
      if(imagePosZ == NULL)
      {
         LOG_ERROR("Cannot read skybox north image from path " + posz);
      }
      if(imageNegX == NULL)
      {
         LOG_ERROR("Cannot read skybox west image from path " + negx);
      }
      if(imageNegY == NULL)
      {
         LOG_ERROR("Cannot read skybox up image from path " + negy);
      }
      if(imageNegZ == NULL)
      {
         LOG_ERROR("Cannot read skybox south image from path " + negz);
      }

      if (imagePosX && imageNegX && imagePosY && imageNegY && imagePosZ && imageNegZ)
      {
         cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
         cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
         cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
         cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
         cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
         cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

         cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
         cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
         cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

         cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
         cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
      }

      return cubemap;
   }


   ////////////////////////////////////////////////////////////////////////////////
   const StringId SkyBoxComponent::TYPE(dtEntity::SID("SkyBox"));      
   const StringId SkyBoxComponent::TextureUpId(dtEntity::SID("TextureUp"));   
   const StringId SkyBoxComponent::TextureDownId(dtEntity::SID("TextureDown"));   
   const StringId SkyBoxComponent::TextureNorthId(dtEntity::SID("TextureNorth"));   
   const StringId SkyBoxComponent::TextureSouthId(dtEntity::SID("TextureSouth"));   
   const StringId SkyBoxComponent::TextureEastId(dtEntity::SID("TextureEast"));   
   const StringId SkyBoxComponent::TextureWestId(dtEntity::SID("TextureWest"));   

   ////////////////////////////////////////////////////////////////////////////
   SkyBoxComponent::SkyBoxComponent()
      : BaseClass(new osg::ClearNode())
      , mDrawables(new osg::Group())
   {
      Register(TextureUpId, &mTextureUp);
      Register(TextureDownId, &mTextureDown);
      Register(TextureNorthId, &mTextureNorth);
      Register(TextureSouthId, &mTextureSouth);
      Register(TextureEastId, &mTextureEast);
      Register(TextureWestId, &mTextureWest);      
      osg::StateSet* stateset = new osg::StateSet();

      osg::TexEnv* te = new osg::TexEnv;
      te->setMode(osg::TexEnv::REPLACE);
      stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

      osg::TexGen *tg = new osg::TexGen;
      tg->setMode(osg::TexGen::NORMAL_MAP);
      stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

      osg::TexMat *tm = new osg::TexMat;
      stateset->setTextureAttribute(0, tm);

      osg::TextureCubeMap* skymap = readCubeMap(this);
      if(skymap)
      {
         stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);
      }   

      stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
      stateset->setMode(GL_FOG,osg::StateAttribute::OFF);
      stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

      /*osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX);
      osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT);

      const char vpstr[] =
         "varying vec3 vTexCoord;\n"
         "void main(void) {\n"
         "gl_Position = ftransform(); vTexCoord = gl_Vertex.xyz;"
         "}";
      vShader->setShaderSource(vpstr);

      const char fpstr[] =
         "uniform samplerCube skyBox;\n"
         "varying vec3 vTexCoord;\n"
         "void main(void)\n"
         "{\n"
	      "   vec3 tex = vec3(vTexCoord.x, vTexCoord.y, -vTexCoord.z);\n"
	      "   gl_FragColor = textureCube( skyBox, tex.xzy );\n"
	      "   gl_FragColor.a = 0.0;\n"
         "}";
      fShader->setShaderSource(fpstr);

      osg::Program* program = new osg::Program();
      program->setName("SkyboxShader");
      program->addShader(vShader);
      program->addShader(fShader);
*/

      // clear the depth to the far plane.
      osg::Depth* depth = new osg::Depth;
      depth->setFunction(osg::Depth::ALWAYS);
      depth->setRange(1.0,1.0);   
      stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

      stateset->setRenderBinDetails(-1,"RenderBin");

      osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),10000));

      mSkyboxGeode = new osg::Geode;
      mSkyboxGeode->setNodeMask(NodeMasks::VISIBLE);
      mSkyboxGeode->setCullingActive(false);
      mSkyboxGeode->setStateSet( stateset );
      mSkyboxGeode->addDrawable(drawable);

      osg::Transform* transform = new MoveEarthySkyWithEyePointTransform();

      transform->setCullingActive(false);
      transform->addChild(mSkyboxGeode);

     
      osg::ClearNode* clearNode = static_cast<osg::ClearNode*>(GetNode());
      clearNode->setRequiresClear(false);
      clearNode->setCullCallback(new TexMatCallback(*tm));
      clearNode->setNodeMask(0xFFFFFFFF);
      clearNode->addChild(transform);
      clearNode->addChild(mDrawables);
   }
  
   ////////////////////////////////////////////////////////////////////////////
   SkyBoxComponent::~SkyBoxComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void SkyBoxComponent::OnPropertyChanged(StringId propname, Property& prop)
   {
      if(propname == TextureUpId ||
         propname == TextureDownId ||
         propname == TextureNorthId ||
         propname == TextureSouthId ||
         propname == TextureEastId ||
         propname == TextureWestId)
      {

         osg::StateSet* stateset = mSkyboxGeode->getOrCreateStateSet();
         osg::TextureCubeMap* skymap = readCubeMap(this);
         if(skymap)
         {
            stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);
         }      
      }
      else
      {
         BaseClass::OnPropertyChanged(propname, prop);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId SkyBoxSystem::TYPE(dtEntity::SID("SkyBox"));

   ////////////////////////////////////////////////////////////////////////////
   SkyBoxSystem::SkyBoxSystem(EntityManager& em)
      : DefaultEntitySystem<SkyBoxComponent>(em)
   {
   }
}
