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

#include "cloudscomponent.h"

#include "simplexnoise1234.h"
#include <dtEntity/systemmessages.h>
#include <osg/Image>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osgUtil/CullVisitor>
#include <dtEntityOSG/dtentityosg_config.h>

#if OSGEPHEMERIS_FOUND
#include <dtEntityOSG/osgephemeriscomponent.h>
#endif
namespace dtEntityCloud
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

   ////////////////////////////////////////////////////////////////////////////

   // tileable noise map,
   // see http://www.sjeiti.com/creating-tileable-noise-maps/
   osg::Texture2D* createNoiseTex(unsigned int size, unsigned int octave)
   {  
      unsigned char* data = new unsigned char[size * size];
      osg::Image* img = new osg::Image();
      img->setImage(size, size, 1, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE);

      SimplexNoise1234 snoise;

      float step = 4.0f / (float)size;
      float fRds = 1;
      for(unsigned int i = 0; i < size; ++i)
      {
         float u = (float)i * step;
         for(unsigned int j = 0; j < size; ++j)
         {
            float v = (float)j * step;
            float fRdx = u * 2 * osg::PI;
            float fRdy = v * 2 * osg::PI;
		      float a = fRds * sin(fRdx);
		      float b = fRds * cos(fRdx);
		      float c = fRds * sin(fRdy);
		      float d = fRds * cos(fRdy);
		      float noise = snoise.noise(
			             123+a
			            ,231+b
			            ,312+c
			            ,273+d
		            );
            unsigned char nchar = (unsigned char) ((noise / 2 + 0.5f) * 255.0f);
            (*data) = nchar;
            ++data;
         }
      }   

      osg::Texture2D* tex = new osg::Texture2D;
      tex->setImage(img);
      tex->setResizeNonPowerOfTwoHint(false);
      tex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
      tex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
      tex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
      std::ostringstream uniname; uniname << "cloudTexture" << octave;

      return tex;
   }

   

   ////////////////////////////////////////////////////////////////////////////   
   osg::Geode* createScreenQuad(float w, float h, float scale)
   {
      osg::Geometry* geom = osg::createTexturedQuadGeometry(
               osg::Vec3(),
               osg::Vec3(w, 0,0),
               osg::Vec3(0, h, 0),
               0, 0, w * scale, h * scale);
      osg::Geode* quad = new osg::Geode();
      quad->addDrawable(geom);
      int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
      quad->getOrCreateStateSet()->setAttribute(
               new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL),
               values);
      quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
      return quad;
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId CloudsComponent::TYPE(dtEntity::SID("Clouds"));
   const dtEntity::StringId CloudsComponent::WindId(dtEntity::SID("Wind"));
   const dtEntity::StringId CloudsComponent::CloudCoverId(dtEntity::SID("CloudCover"));
   const dtEntity::StringId CloudsComponent::CloudSharpnessId(dtEntity::SID("CloudSharpness"));
   const dtEntity::StringId CloudsComponent::CloudMutationRateId(dtEntity::SID("CloudMutationRate"));
   const dtEntity::StringId CloudsComponent::SunPosId(dtEntity::SID("SunPos"));
   const dtEntity::StringId CloudsComponent::TraceStartId(dtEntity::SID("TraceStart"));
   const dtEntity::StringId CloudsComponent::TraceDistId(dtEntity::SID("TraceDist"));   
   
   ////////////////////////////////////////////////////////////////////////////
   CloudsComponent::CloudsComponent()
      : mSunPos(
         dtEntity::DynamicVec3Property::SetValueCB(this, &CloudsComponent::SetSunPos),
         dtEntity::DynamicVec3Property::GetValueCB(this, &CloudsComponent::GetSunPos)
      )
      , mWind(
         dtEntity::DynamicVec2Property::SetValueCB(this, &CloudsComponent::SetWind),
         dtEntity::DynamicVec2Property::GetValueCB(this, &CloudsComponent::GetWind)
      )
      , mCloudCover(
         dtEntity::DynamicFloatProperty::SetValueCB(this, &CloudsComponent::SetCloudCover),
         dtEntity::DynamicFloatProperty::GetValueCB(this, &CloudsComponent::GetCloudCover)
      )
      , mCloudSharpness(
         dtEntity::DynamicFloatProperty::SetValueCB(this, &CloudsComponent::SetCloudSharpness),
         dtEntity::DynamicFloatProperty::GetValueCB(this, &CloudsComponent::GetCloudSharpness)
      )
      , mCloudMutationRate(
         dtEntity::DynamicFloatProperty::SetValueCB(this, &CloudsComponent::SetCloudMutationRate),
         dtEntity::DynamicFloatProperty::GetValueCB(this, &CloudsComponent::GetCloudMutationRate)
      )
      , mTraceStart(
         dtEntity::DynamicFloatProperty::SetValueCB(this, &CloudsComponent::SetTraceStart),
         dtEntity::DynamicFloatProperty::GetValueCB(this, &CloudsComponent::GetTraceStart)
      )
      , mTraceDist(
         dtEntity::DynamicFloatProperty::SetValueCB(this, &CloudsComponent::SetTraceDist),
         dtEntity::DynamicFloatProperty::GetValueCB(this, &CloudsComponent::GetTraceDist)
      )
      , mDrawables(new osg::Group())
   {
      Register(WindId, &mWind);
      Register(CloudCoverId, &mCloudCover);
      Register(CloudSharpnessId, &mCloudSharpness);
      Register(CloudMutationRateId, &mCloudMutationRate);
      Register(SunPosId, &mSunPos);
      Register(TraceStartId, &mTraceStart);
      Register(TraceDistId, &mTraceDist);

      unsigned int noiseSize = 256;
      unsigned int combinedSize = 256;

      mNoise0 = createNoiseTex(noiseSize, 1);
      mNoise1 = createNoiseTex(noiseSize, 2);
      mNoise2 = createNoiseTex(noiseSize, 3);
      mNoise3 = createNoiseTex(noiseSize, 4);

      mCloudDensity = new osg::Texture2D();
      mCloudDensity->setTextureSize(combinedSize, combinedSize);
      mCloudDensity->setInternalFormat(GL_RGBA);
      mCloudDensity->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
      mCloudDensity->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
      mCloudDensity->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

      mCloudCoverUniform = new osg::Uniform("cloud_cover", 0.75f);
      mCloudSharpnessUniform = new osg::Uniform("clouds_sharpness", 0.95f);
      mCloudMutationRateUniform = new osg::Uniform("cloud_mutation_rate", 0.1f);
      mWindUniform = new osg::Uniform("wind", osg::Vec2(0.0001f, 0.0001f));
      mSunPosUniform = new osg::Uniform("sun_pos", osg::Vec3(0.0f,0.0f,2.0f));
      mTraceStartUniform = new osg::Uniform("trace_start", 1.31000f);
      mTraceDistUniform = new osg::Uniform("trace_dist", 0.01100f);
      osg::Camera* cloudDensityCam = CreateCloudDensityCam(mCloudDensity, combinedSize);
      GetNode()->asGroup()->addChild(cloudDensityCam);

      osg::Geometry* cloudPlane = CreateCloudPlane();
      osg::Geode* cloudGeode = new osg::Geode();
      cloudGeode->addDrawable(cloudPlane);

      osg::Transform* transform = new MoveEarthySkyWithEyePointTransform();
      transform->setCullingActive(false);
      transform->addChild(cloudGeode);
      GetNode()->asGroup()->addChild(transform);
      GetNode()->asGroup()->addChild(mDrawables);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   CloudsComponent::~CloudsComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Camera* CloudsComponent::CreateCloudDensityCam(osg::Texture2D* tex, unsigned int texsize) const
   {
      osg::Camera* camera = new osg::Camera();

      camera->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      camera->setProjectionMatrix(osg::Matrixd::ortho2D(0, 1, 0, 1));
      camera->setViewMatrix(osg::Matrixd::identity());
      camera->setViewport(0,0,texsize,texsize);

      // set the camera to render before the main camera.
      camera->setRenderOrder(osg::Camera::PRE_RENDER);

      // tell the camera to use OpenGL frame buffer object where supported.
      camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

      // attach the texture and use it as the color buffer.
      camera->attach(osg::Camera::COLOR_BUFFER, tex);

      osg::Geode* screenQuad = createScreenQuad(texsize,texsize,1);
      camera->addChild(screenQuad);

      osg::StateSet* ss = screenQuad->getOrCreateStateSet();
      
      ss->setTextureAttributeAndModes(0, mNoise0, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(1, mNoise1, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(2, mNoise2, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(3, mNoise3, osg::StateAttribute::ON);

      
      ss->addUniform(new osg::Uniform("cloudTexture1", 0));
      ss->addUniform(new osg::Uniform("cloudTexture2", 1));
      ss->addUniform(new osg::Uniform("cloudTexture3", 2));
      ss->addUniform(new osg::Uniform("cloudTexture4", 3));
      ss->addUniform(mCloudCoverUniform);
     
      char vertexShaderSource[] =
          "void main(void) \n"
          "{ \n"
          "\n"
          "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
          "    gl_Position = ftransform();\n"
          "}\n";

      char fragmentShaderSource[] =
         "uniform float cloud_cover;\n"
         "uniform float cloud_mutation_rate;\n"
         "uniform sampler2D cloudTexture1;\n"
         "uniform sampler2D cloudTexture2;\n"
         "uniform sampler2D cloudTexture3;\n"
         "uniform sampler2D cloudTexture4;\n"
         "uniform float osg_SimulationTime;\n"
         "vec4 lerp4(vec4 a, vec4 b, float s)\n"
         "{\n"
         "  return vec4(a + (b - a) * s);\n"
         "}\n"
         "void main(void) \n"
         "{ \n"
         "  \n"
         "  vec2 uv = gl_TexCoord[0].st;\n"
         "  float x1 = sin(cloud_mutation_rate * osg_SimulationTime / 16) / 2.0 + 0.5;"
         "  float x2 = sin(cloud_mutation_rate * osg_SimulationTime / 8) / 2.0 + 0.5;"
         "  float x3 = sin(cloud_mutation_rate * osg_SimulationTime / 4) / 2.0 + 0.5;"
         "  float x4 = sin(cloud_mutation_rate * osg_SimulationTime / 2) / 2.0 + 0.5;"
         "  vec4 tex = lerp4(texture2D(cloudTexture1, uv * 1.0), texture2D(cloudTexture1, uv * -1.0), x1) * 1.0;\n"
         "      tex += lerp4(texture2D(cloudTexture2, uv * 2.0), texture2D(cloudTexture2, uv * -2.0), x2) * 0.5;\n"
         "      tex += lerp4(texture2D(cloudTexture3, uv * 4.0), texture2D(cloudTexture3, uv * -4.0), x3) * 0.25;\n"
         "      tex += lerp4(texture2D(cloudTexture4, uv * 8.0), texture2D(cloudTexture4, uv * -8.0), x4) * 0.125;\n"
       
         "  tex = max(tex - 1 + cloud_cover, 0.0);\n"
         "  gl_FragColor = vec4(tex.r, tex.r, tex.r, 1.0);\n"
         "}\n";

      osg::Program* program = new osg::Program;
      ss->setAttribute(program);

      osg::Shader* vertex_shader = new osg::Shader(osg::Shader::VERTEX, vertexShaderSource);
      program->addShader(vertex_shader);

      osg::Shader* fragment_shader = new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource);
      program->addShader(fragment_shader);

      return camera;
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Geometry* CloudsComponent::CreateCloudPlane() const
   {
      // geometry is a disk with center above the viewer position.
      // Having a vert at center of plane makes it possible to use per-vertex
      // fog values.
      // sides of disk are a little lower than center. When disk was a plane,
      // ugly jaggies appeared near the horizon.
      osg::Geometry* cloudPlaneGeometry = new osg::Geometry();
      float height_center = 400;
      float height_sides = 200;
      float width_2 = 8000;

      unsigned int sides = 6;
      osg::Vec3Array* coords = new osg::Vec3Array(sides + 2);
      osg::Vec2Array* tcoords = new osg::Vec2Array(sides + 2);
      cloudPlaneGeometry->setVertexArray(coords);
      cloudPlaneGeometry->setTexCoordArray(0,tcoords);   

      (*coords)[0].set(0, 0, height_center);
      (*tcoords)[0].set(0, 0);
      double angle = osg::PI * 2.0 / (double)sides;
      for(unsigned int i = 0; i < sides + 1; ++i)
      {
         (*coords)[i + 1].set(cosf(i * angle) * width_2, sinf(i * angle) * width_2, height_sides);
         (*tcoords)[i + 1].set(cosf(i * angle) * 0.5f, sinf(i * angle) * 0.5f);
      }
      
      cloudPlaneGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, 0, sides + 2));

      osg::StateSet* ss = cloudPlaneGeometry->getOrCreateStateSet();
      ss->setTextureAttributeAndModes(0, mNoise0, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(1, mNoise1, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(2, mNoise2, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(3, mNoise3, osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(4, mCloudDensity, osg::StateAttribute::ON);
      
      ss->addUniform(new osg::Uniform("cloudTexture1", 0));
      ss->addUniform(new osg::Uniform("cloudTexture2", 1));
      ss->addUniform(new osg::Uniform("cloudTexture3", 2));
      ss->addUniform(new osg::Uniform("cloudTexture4", 3));
      ss->addUniform(new osg::Uniform("densityFieldTexture", 4));
      ss->addUniform(mCloudCoverUniform);
      ss->addUniform(mCloudSharpnessUniform);
      ss->addUniform(mCloudMutationRateUniform);
      ss->addUniform(mWindUniform);
      ss->addUniform(mSunPosUniform);
      ss->addUniform(mTraceStartUniform);
      ss->addUniform(mTraceDistUniform);

      ss->setMode(GL_BLEND,osg::StateAttribute::ON);
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      ss->setRenderBinDetails(-9000,"RenderBin");

      char vertexShaderSource[] =
       "varying float fogFactor;\n"
       "void main(void) \n"
       "{ \n"
       "\n"
       "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
       "    gl_Position = ftransform();\n"
       "    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);\n"
       "    float f = (length(vVertex) - 400) * 0.2;\n"
       "    fogFactor = exp2( -f * f);\n"
       "    fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
       "}\n";

      char fragmentShaderSource[] =
       "vec3 lerp3(vec3 a, vec3 b, float s)\n"
       "{\n"
       "  return vec3(a + (b - a) * s);\n"
       "}\n"
       "vec4 lerp4(vec4 a, vec4 b, float s)\n"
       "{\n"
       "  return vec4(a + (b - a) * s);\n"
       "}\n"
       "varying float fogFactor;\n"
       "uniform float cloud_cover;\n"
       "uniform float clouds_sharpness;\n"
       "uniform float cloud_mutation_rate;\n"
       "uniform vec3 sun_pos;\n"
       "uniform vec2 wind;\n"
       "uniform float trace_start;\n"
       "uniform float trace_dist;\n"
       "uniform sampler2D cloudTexture1;\n"
       "uniform sampler2D cloudTexture2;\n"
       "uniform sampler2D cloudTexture3;\n"
       "uniform sampler2D cloudTexture4;\n"
       "uniform sampler2D densityFieldTexture;\n"   
       "uniform float osg_SimulationTime;\n"
       "uniform mat4 osg_ViewMatrixInverse;\n"
       "void main(void) \n"
       "{ \n"
       "  \n"
       "  vec3 sunColor = vec3(1.1,1.1,1.0);\n"
       "  vec2 eyepos = osg_ViewMatrixInverse[3].xy * 0.00001;\n"
       "  vec2 uv = gl_TexCoord[0].st + wind * osg_SimulationTime + eyepos;\n"
       "  float x1 = sin(cloud_mutation_rate * osg_SimulationTime / 16) / 2.0 + 0.5;"
       "  float x2 = sin(cloud_mutation_rate * osg_SimulationTime / 8) / 2.0 + 0.5;"
       "  float x3 = sin(cloud_mutation_rate * osg_SimulationTime / 4) / 2.0 + 0.5;"
       "  float x4 = sin(cloud_mutation_rate * osg_SimulationTime / 2) / 2.0 + 0.5;"
       "  vec4 tex = lerp4(texture2D(cloudTexture1, uv * 1.0), texture2D(cloudTexture1, uv * -1.0), x1) * 1.0;\n"
       "      tex += lerp4(texture2D(cloudTexture2, uv * 2.0), texture2D(cloudTexture2, uv * -2.0), x2) * 0.5;\n"
       "      tex += lerp4(texture2D(cloudTexture3, uv * 4.0), texture2D(cloudTexture3, uv * -4.0), x3) * 0.25;\n"
       "      tex += lerp4(texture2D(cloudTexture4, uv * 8.0), texture2D(cloudTexture4, uv * -8.0), x4) * 0.125;\n"
       "      tex += lerp4(texture2D(cloudTexture3, uv * 16.0), texture2D(cloudTexture3, uv * -16.0), x4) * 0.05;\n"
       "      tex += lerp4(texture2D(cloudTexture2, uv * 32.0), texture2D(cloudTexture2, uv * -32.0), x4) * 0.03;\n"
       "  tex.r = max(tex.r - 1 + cloud_cover.r, 0.0);\n"       
       "  if(tex.r < 0.004) discard;\n"       
       "  vec3 endTracePos = vec3(uv, -tex.r);\n" 
       "  vec3 traceDir = normalize(endTracePos - sun_pos);\n"
       "  vec3 curTracePos = sun_pos + traceDir * trace_start;\n"
       "  float scattering = 0.0;\n"
       "  for(int i = 0; i < 64; ++i)\n"
       "  {\n"
       "    curTracePos += traceDir * trace_dist;\n"
       "    vec4 tex2 = texture2D(densityFieldTexture, curTracePos.xy);\n"
       "    scattering += step(curTracePos.z, tex2.r);\n"
       "  }\n"
       "  \n"
       "  tex.r = 1.0 - pow(1 - clouds_sharpness, tex.r * 255.0);\n"
#if 1
       "    vec2 sunRay2D = uv - sun_pos.xy;\n"
	    "    float sunDist2D = length(sunRay2D);\n"
       "    scattering = 1 - scattering / 64.0;\n"      
       "    gl_FragColor.rgb = lerp3(sunColor * scattering, vec3(scattering, scattering, scattering), sunDist2D);\n"
	    "    float opacity = 2.0 - sunDist2D;\n"
       "    vec3 shadeColorTweaked = max(gl_FragColor.rgb - 0.75, vec3(0,0,0));\n"
       "    gl_FragColor.rgb += shadeColorTweaked * max(1 - sunDist2D * 16, 0);\n"
       "    gl_FragColor = lerp4(\n"
       "          vec4(gl_FragColor.rgb, tex.r * opacity * 1), // Zenith color\n"
       "          vec4(1.0, 1.0, 1.0, gl_FragColor.r * tex.r * 1), // Horizon color\n"
       "          sunDist2D * 2\n"
       "      );\n"
       "    gl_FragColor.a *= fogFactor;\n"
#else
       "  float light = 1 - scattering / 64.0;\n"
       "  gl_FragColor = vec4(light, light, light, tex.r * fogFactor);\n"  
#endif
       "}\n";

      osg::Program* program = new osg::Program;
      cloudPlaneGeometry->getOrCreateStateSet()->setAttribute(program);

      osg::Shader* vertex_shader = new osg::Shader(osg::Shader::VERTEX, vertexShaderSource);
      program->addShader(vertex_shader);

      osg::Shader* fragment_shader2 = new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource);
      program->addShader(fragment_shader2);
      return cloudPlaneGeometry;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetWind(const osg::Vec2f& w)
   {
      mWindUniform->set(w);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec2f CloudsComponent::GetWind() const
   {
      osg::Vec2f w;
      mWindUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetSunPos(const osg::Vec3f& w)
   {
      osg::Vec3f val = w;
      //val.normalize();
      mSunPosUniform->set(val);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3f CloudsComponent::GetSunPos() const
   {
      osg::Vec3f w;
      mSunPosUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetCloudCover(float w)
   {
      mCloudCoverUniform->set(w);
   }

   ////////////////////////////////////////////////////////////////////////////
   float CloudsComponent::GetCloudCover() const
   {
      float w;
      mCloudCoverUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetCloudSharpness(float w)
   {
      mCloudSharpnessUniform->set(w);
   }

   ////////////////////////////////////////////////////////////////////////////
   float CloudsComponent::GetCloudSharpness() const
   {
      float w;
      mCloudSharpnessUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetCloudMutationRate(float w)
   {
      mCloudMutationRateUniform->set(w);
   }

   ////////////////////////////////////////////////////////////////////////////
   float CloudsComponent::GetCloudMutationRate() const
   {
      float w;
      mCloudMutationRateUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetTraceStart(float w)
   {
      mTraceStartUniform->set(w);
   }

   ////////////////////////////////////////////////////////////////////////////
   float CloudsComponent::GetTraceStart() const
   {
      float w;
      mTraceStartUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsComponent::SetTraceDist(float w)
   {
      mTraceDistUniform->set(w);
   }

   ////////////////////////////////////////////////////////////////////////////
   float CloudsComponent::GetTraceDist() const
   {
      float w;
      mTraceDistUniform->get(w);
      return w;
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId CloudsSystem::TYPE(dtEntity::SID("Clouds"));

   ////////////////////////////////////////////////////////////////////////////
   CloudsSystem::CloudsSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {    
#if OSGEPHEMERIS_FOUND
      mTickFunctor = dtEntity::MessageFunctor(this, &CloudsSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_LATE, "CloudsSystem::Tick");
#endif
   }

   ////////////////////////////////////////////////////////////////////////////
   CloudsSystem::~CloudsSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void CloudsSystem::Tick(const dtEntity::Message& msg)
   {
#if OSGEPHEMERIS_FOUND
      dtEntityOSG::OSGEphemerisSystem* ephemsys;
      if(GetEntityManager().GetES(ephemsys) && ephemsys->begin() != ephemsys->end())
      {
         /*dtEntityOSG::OSGEphemerisComponent* comp = ephemsys->begin()->second;
         osg::Vec4f sunpos = comp->GetSunLightPos();
         for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
         {
            i->second->SetSunPos(osg::Vec3(sunpos[0], sunpos[1], sunpos[2]));
         }*/
      }
#endif
   }
}
