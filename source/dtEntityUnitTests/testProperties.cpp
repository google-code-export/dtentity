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

#define TOLERANCE 0.001


TEST(ReinterpretCastsV3f)
{
   float values[3] = {3.0f, -234.234f, 0.0f};
   const Vec3f& casted = reinterpret_cast<const Vec3f&>(values);
   CHECK_EQUAL(values[0], casted[0]);
   CHECK_EQUAL(values[1], casted[1]);
   CHECK_EQUAL(values[2], casted[2]);
}

TEST(ReinterpretCastsV4d)
{
   double values[4] = {3.0f, -234.234f, 0.0f, 3254325.235325};
   const Vec4d& casted = reinterpret_cast<const Vec4d&>(values);
   CHECK_EQUAL(values[0], casted[0]);
   CHECK_EQUAL(values[1], casted[1]);
   CHECK_EQUAL(values[2], casted[2]);
   CHECK_EQUAL(values[3], casted[3]);
}

TEST(ReinterpretCastsQuat)
{
   double values[4] = {3.0f, -234.234f, 0.0f, 3254325.235325};
   const Quat& casted = reinterpret_cast<const Quat&>(values);
   CHECK_EQUAL(values[0], casted[0]);
   CHECK_EQUAL(values[1], casted[1]);
   CHECK_EQUAL(values[2], casted[2]);
   CHECK_EQUAL(values[3], casted[3]);
}

TEST(SetValuesVec2)
{
   Vec2Property v2prop(1,2);
   CHECK_EQUAL(v2prop.Vec2Value()[0], 1.0f);
   CHECK_EQUAL(v2prop.Vec2Value()[1], 2.0f);
}

TEST(SetValuesVec3)
{
   Vec3Property v3prop(1,2,3);
   CHECK_EQUAL(v3prop.Vec3Value()[0], 1.0f);
   CHECK_EQUAL(v3prop.Vec3Value()[1], 2.0f);
   CHECK_EQUAL(v3prop.Vec3Value()[2], 3.0f);
} 

TEST(SetValuesVec4)
{
   Vec4Property v4prop(1,2,3,4);
   CHECK_EQUAL(v4prop.Vec4Value()[0], 1.0f);
   CHECK_EQUAL(v4prop.Vec4Value()[1], 2.0f);
   CHECK_EQUAL(v4prop.Vec4Value()[2], 3.0f);
   CHECK_EQUAL(v4prop.Vec4Value()[3], 4.0f);
}


TEST(SetValuesVecRe2)
{
   Vec2Property v2prop(1,2);
   CHECK_EQUAL(v2prop.Get()[0], 1.0f);
   CHECK_EQUAL(v2prop.Get()[1], 2.0f);
}

TEST(SetValuesVecRe3)
{
   Vec3Property v3prop(1,2,3);
   CHECK_EQUAL(v3prop.Get()[0], 1.0f);
   CHECK_EQUAL(v3prop.Get()[1], 2.0f);
   CHECK_EQUAL(v3prop.Get()[2], 3.0f);
}

TEST(SetValuesVecRe4)
{
   Vec4Property v4prop(1,2,3,4);
   CHECK_EQUAL(v4prop.Get()[0], 1.0f);
   CHECK_EQUAL(v4prop.Get()[1], 2.0f);
   CHECK_EQUAL(v4prop.Get()[2], 3.0f);
   CHECK_EQUAL(v4prop.Get()[3], 4.0f);
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
   Matrix mat;
   mat(0, 0) = 3;

   MatrixProperty p(mat);
   CHECK_EQUAL(p.MatrixValue()(0, 0), 3.0);
}

TEST(SetValuesQuat)
{
   Quat q(1,2,3,4);
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
   Vec2Property p(1,2);
   Property* c = p.Clone();
   CHECK_EQUAL(p.Vec2Value()[0], c->Vec2Value()[0]);
   CHECK_EQUAL(p.Vec2Value()[1], c->Vec2Value()[1]);
   delete c;
}

TEST(CloneVec3)
{
   Vec3Property p(1,2, 3);
   Property* c = p.Clone();
   CHECK_EQUAL(p.Vec3Value()[0], c->Vec3Value()[0]);
   CHECK_EQUAL(p.Vec3Value()[1], c->Vec3Value()[1]);
   CHECK_EQUAL(p.Vec3Value()[2], c->Vec3Value()[2]);
   delete c;
}

TEST(CloneVec4)
{
   Vec4Property p(1,2, 3, 4);
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

   Matrix mat;
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

   Quat v (1,2,3,4);
   
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


TEST(FromToStringVec2)
{

   Vec2Property v(6.0f, -7.4f);
   Vec2Property v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
}

TEST(FromToStringVec3)
{

   Vec3Property v(6.0f, -7.4f, 8.0f);
   Vec3Property v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
   CHECK_CLOSE(v.Get()[2], v2.Get()[2], TOLERANCE);
}

TEST(FromToStringVec4)
{

   Vec4Property v(6.0f, -7.0f, 8.0f, 245.23456246f);
   Vec4Property v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
   CHECK_CLOSE(v.Get()[2], v2.Get()[2], TOLERANCE);
   CHECK_CLOSE(v.Get()[3], v2.Get()[3], TOLERANCE);
}

TEST(FromToStringVec2d)
{

   Vec2dProperty v(6.0f, -7.4f);
   Vec2dProperty v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
}

TEST(FromToStringVec3d)
{

   Vec3dProperty v(6, -7.4, 8);
   Vec3dProperty v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
   CHECK_CLOSE(v.Get()[2], v2.Get()[2], TOLERANCE);
}

TEST(FromToStringVec4d)
{

   Vec4dProperty v(6, -7.4, 8, 245.23456246);
   Vec4dProperty v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
   CHECK_CLOSE(v.Get()[2], v2.Get()[2], TOLERANCE);
   CHECK_CLOSE(v.Get()[3], v2.Get()[3], TOLERANCE);
}

TEST(FromToStringBool)
{

   BoolProperty v1(true);
   BoolProperty v2;
   v2.SetString(v1.StringValue());
   CHECK_EQUAL(v2.Get(), true);

   BoolProperty v3(false);
   BoolProperty v4;
   v4.SetString(v3.StringValue());
   CHECK_EQUAL(v4.Get(), false);
}

TEST(FromToStringDouble)
{

   DoubleProperty v1(2351235.1341345);
   DoubleProperty v2;
   v2.SetString(v1.StringValue());
   CHECK_CLOSE(v2.Get(), v1.Get(), TOLERANCE);

}

TEST(FromToStringFloat)
{

   FloatProperty v1(2351235.1341345f);
   FloatProperty v2;
   v2.SetString(v1.StringValue());
   CHECK_CLOSE(v2.Get(), v1.Get(), TOLERANCE);

}

TEST(FromToStringInt)
{

   {
      IntProperty v1(2351235);
      IntProperty v2;
      v2.SetString(v1.StringValue());
      CHECK_EQUAL(v2.Get(), v1.Get());
   }

   {
      IntProperty v1(-2351235);
      IntProperty v2;
      v2.SetString(v1.StringValue());
      CHECK_EQUAL(v2.Get(), v1.Get());
   }

}

TEST(FromToStringMatrix)
{

   Matrix m(0, -1234.1234, 1, 2
                 , 1234124, 21.245, 2, 2
                 , 52, 246, 21346, 2346
                 , 0, 3, -3, 5);

   MatrixProperty v1(m);
   MatrixProperty v2;
   v2.SetString(v1.StringValue());
   CHECK_CLOSE(v2.Get()(0, 0), v1.Get()(0, 0), TOLERANCE);
   CHECK_CLOSE(v2.Get()(0, 1), v1.Get()(0, 1), TOLERANCE);
   CHECK_CLOSE(v2.Get()(0, 2), v1.Get()(0, 2), TOLERANCE);
   CHECK_CLOSE(v2.Get()(0, 3), v1.Get()(0, 3), TOLERANCE);

   CHECK_CLOSE(v2.Get()(1, 0), v1.Get()(1, 0), TOLERANCE);
   CHECK_CLOSE(v2.Get()(1, 1), v1.Get()(1, 1), TOLERANCE);
   CHECK_CLOSE(v2.Get()(1, 2), v1.Get()(1, 2), TOLERANCE);
   CHECK_CLOSE(v2.Get()(1, 3), v1.Get()(1, 3), TOLERANCE);

   CHECK_CLOSE(v2.Get()(2, 0), v1.Get()(2, 0), TOLERANCE);
   CHECK_CLOSE(v2.Get()(2, 1), v1.Get()(2, 1), TOLERANCE);
   CHECK_CLOSE(v2.Get()(2, 2), v1.Get()(2, 2), TOLERANCE);
   CHECK_CLOSE(v2.Get()(2, 3), v1.Get()(2, 3), TOLERANCE);

   CHECK_CLOSE(v2.Get()(3, 0), v1.Get()(3, 0), TOLERANCE);
   CHECK_CLOSE(v2.Get()(3, 1), v1.Get()(3, 1), TOLERANCE);
   CHECK_CLOSE(v2.Get()(3, 2), v1.Get()(3, 2), TOLERANCE);
   CHECK_CLOSE(v2.Get()(3, 3), v1.Get()(3, 3), TOLERANCE);

}

TEST(FromToStringQuat)
{

   QuatProperty v(6, -7.4, 8, 245.23456246);
   QuatProperty v2;
   v2.SetString(v.StringValue());
   CHECK_CLOSE(v.Get()[0], v2.Get()[0], TOLERANCE);
   CHECK_CLOSE(v.Get()[1], v2.Get()[1], TOLERANCE);
   CHECK_CLOSE(v.Get()[2], v2.Get()[2], TOLERANCE);
   CHECK_CLOSE(v.Get()[3], v2.Get()[3], TOLERANCE);
}

TEST(FromToStringString)
{

   StringProperty v("äöü123ß#+*\\/&<>");
   StringProperty v2;
   v2.SetString(v.StringValue());
   CHECK_EQUAL(v.Get(), v2.Get());

}

TEST(FromToStringStringId)
{

   StringIdProperty v(SID("äöü123ß#+*\\/&<>"));
   StringIdProperty v2;
   v2.SetString(v.StringValue());
   CHECK_EQUAL(v.Get(), v2.Get());

}

TEST(FromToStringUInt)
{

   UIntProperty v(123523456);
   UIntProperty v2;
   v2.SetString(v.StringValue());
   CHECK_EQUAL(v.Get(), v2.Get());

}

int main()
{
 int ret = UnitTest::RunAllTests();
 return ret;
}
