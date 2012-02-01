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

#include <UnitTest++.h>
#include <dtEntity/property.h>
#include <osg/Vec2>

using namespace UnitTest;
using namespace dtEntity;


TEST(SetValuesVec2)
{
   Vec2Property v2prop(osg::Vec2(1,2));
   CHECK_EQUAL(v2prop.Vec2Value()[0], 1.0f);
   CHECK_EQUAL(v2prop.Vec2Value()[1], 2.0f);
}

TEST(SetValuesVec3)
{
   Vec3Property v3prop(osg::Vec3(1,2,3));
   CHECK_EQUAL(v3prop.Vec3Value()[0], 1.0f);
   CHECK_EQUAL(v3prop.Vec3Value()[1], 2.0f);
   CHECK_EQUAL(v3prop.Vec3Value()[2], 3.0f);
} 

TEST(SetValuesVec4)
{
   Vec4Property v4prop(osg::Vec4(1,2,3,4));
   CHECK_EQUAL(v4prop.Vec4Value()[0], 1.0f);
   CHECK_EQUAL(v4prop.Vec4Value()[1], 2.0f);
   CHECK_EQUAL(v4prop.Vec4Value()[2], 3.0f);
   CHECK_EQUAL(v4prop.Vec4Value()[3], 4.0f);
}

TEST(SetValuesPropertyArray)
{
   IntProperty iprop(666);
   PropertyArray pa;
   pa.push_back(&iprop);
   ArrayProperty arrprop(pa);
   PropertyArray pb = arrprop.ArrayValue();
   CHECK_EQUAL(pa.front()->IntValue(), pb.front()->IntValue());
}

TEST(SetValuesPropertyGroup)
{
   IntProperty iprop(666);
   PropertyGroup pg;
   pg[SID("BLABLA")] = &iprop;
   GroupProperty grprop(pg);
   PropertyGroup pgv = grprop.GroupValue();
   CHECK_EQUAL(pgv[SID("BLABLA")]->IntValue(), iprop.IntValue());
}


TEST(SetValuesFloat)
{
   FloatProperty fp(0.123f);
   CHECK_EQUAL(fp.FloatValue(), 0.123f);
}

TEST(SetValuesDouble)
{
   DoubleProperty dp(0.123);
   CHECK_EQUAL(dp.DoubleValue(), 0.123);
}

TEST(SetValuesInt)
{
   IntProperty p(666);
   CHECK_EQUAL(p.IntValue(), 666);
}

TEST(SetValuesMat)
{
   osg::Matrix mat;
   mat(0, 0) = 3;

   MatrixProperty p(mat);
   CHECK_EQUAL(p.MatrixValue()(0, 0), 3.0);
}

TEST(SetValuesQuat)
{
   osg::Quat q(1,2,3,4);
   QuatProperty p(q);
   CHECK_EQUAL(p.QuatValue()[0], q[0]);
   CHECK_EQUAL(p.QuatValue()[1], q[1]);
   CHECK_EQUAL(p.QuatValue()[2], q[2]);
   CHECK_EQUAL(p.QuatValue()[3], q[3]);
}


TEST(SetValuesString)
{
   std::string v = "BlaBla";
   StringProperty p(v);
   CHECK_EQUAL(p.StringValue(), v);
}

TEST(SetValuesStringId)
{
   StringId v = SID("TEST");
   StringIdProperty p(v);
   CHECK_EQUAL(p.StringIdValue(), v);
}

TEST(SetValuesBool)
{
   BoolProperty t(true);
   BoolProperty f(false);
   CHECK_EQUAL(t.BoolValue(), true);
   CHECK_EQUAL(f.BoolValue(), false);
}


TEST(CloneVec2)
{
   Vec2Property p(osg::Vec2(1,2));
   Property* c = p.Clone();
   CHECK_EQUAL(p.Vec2Value()[0], c->Vec2Value()[0]);
   CHECK_EQUAL(p.Vec2Value()[1], c->Vec2Value()[1]);
   delete c;
}

TEST(CloneVec3)
{
   Vec3Property p(osg::Vec3(1,2, 3));
   Property* c = p.Clone();
   CHECK_EQUAL(p.Vec3Value()[0], c->Vec3Value()[0]);
   CHECK_EQUAL(p.Vec3Value()[1], c->Vec3Value()[1]);
   CHECK_EQUAL(p.Vec3Value()[2], c->Vec3Value()[2]);
   delete c;
}

TEST(CloneVec4)
{
   Vec4Property p(osg::Vec4(1,2, 3, 4));
   Property* c = p.Clone();
   CHECK_EQUAL(p.Vec4Value()[0], c->Vec4Value()[0]);
   CHECK_EQUAL(p.Vec4Value()[1], c->Vec4Value()[1]);
   CHECK_EQUAL(p.Vec4Value()[2], c->Vec4Value()[2]);
   CHECK_EQUAL(p.Vec4Value()[3], c->Vec4Value()[3]);
   delete c;
}

TEST(CloneArray)
{
   IntProperty iprop(666);
   PropertyArray pa;
   pa.push_back(&iprop);
   ArrayProperty p(pa);
   Property* c = p.Clone();

   PropertyArray pb = c->ArrayValue();
   CHECK_EQUAL(pa.front()->IntValue(), pb.front()->IntValue());
   delete c;
}


TEST(CloneGroup)
{

   IntProperty iprop(666);
   PropertyGroup pg;
   pg[SID("TEST")] = &iprop;
   GroupProperty p(pg);
   Property* c = p.Clone();

   PropertyGroup pgv = c->GroupValue();
   CHECK_EQUAL(pgv[SID("TEST")]->IntValue(), iprop.IntValue());
   delete c;
}

TEST(CloneFloat)
{

   FloatProperty p(0.123f);
   Property* c = p.Clone();

   CHECK_EQUAL(c->FloatValue(), 0.123f);
   delete c;
}

TEST(CloneDouble)
{

   DoubleProperty p(0.123);
   Property* c = p.Clone();

   CHECK_EQUAL(c->DoubleValue(), 0.123);
   delete c;
}


TEST(CloneInt)
{

   IntProperty p(666);
   Property* c = p.Clone();

   CHECK_EQUAL(c->IntValue(), 666);
   delete c;
}

TEST(CloneMatrix)
{

   osg::Matrix mat;
   mat(0, 0) = 666;
   MatrixProperty p(mat);
   Property* c = p.Clone();

   CHECK_EQUAL(c->MatrixValue()(0,0), 666.0);
   delete c;
}



TEST(CloneString)
{

   std::string v = "Blabla";
   StringProperty p(v);
   Property* c = p.Clone();

   CHECK_EQUAL(c->StringValue(), v);
   delete c;
}


TEST(CloneQuat)
{

   osg::Quat v (1,2,3,4);
   
   QuatProperty p(v);
   Property* c = p.Clone();

   CHECK_EQUAL(c->QuatValue()[0], 1.0);
   CHECK_EQUAL(c->QuatValue()[1], 2.0);
   CHECK_EQUAL(c->QuatValue()[2], 3.0);
   CHECK_EQUAL(c->QuatValue()[3], 4.0);
   delete c;
}

TEST(CloneStringId)
{

   StringId v = SID("TEST");
   
   StringIdProperty p(v);
   Property* c = p.Clone();

   CHECK_EQUAL(c->StringIdValue(), v);
   delete c;
}


TEST(CloneBool)
{

   BoolProperty t(true);
   BoolProperty f(false);
   Property* ct = t.Clone();
   Property* cf = f.Clone();

   CHECK_EQUAL(ct->BoolValue(), true);
   CHECK_EQUAL(cf->BoolValue(), false);
   delete ct;
   delete cf;
}




int main()
{
 int ret = UnitTest::RunAllTests();
 return ret;
}
