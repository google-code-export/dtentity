/*  -*-c++-*- 
 *  Copyright (C) 2008 Cedric Pinson <cedric.pinson@plopbyte.net>
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

in vec4 boneWeight0;
in vec4 boneWeight1;
in vec4 boneWeight2;
in vec4 boneWeight3;

uniform int nbBonesPerVertex;
uniform mat4 matrixPalette[MAX_MATRIX];
vec4 position;
varying vec3 vNormal;
varying vec4 vFrontColor;

// accumulate position and normal in global scope
void computeAcummulatedNormalAndPosition(vec4 boneWeight)
{
    
    int matrixIndex;
    float matrixWeight;
    for (int i = 0; i < 2; i++)
    {
        matrixIndex =  int(boneWeight[0]);
        matrixWeight = boneWeight[1];
        mat4 matrix = matrixPalette[matrixIndex];
        // correct for normal if no scale in bone
        mat3 matrixNormal = mat3(matrix);
        position += matrixWeight * (matrix * gl_Vertex );
        vNormal += matrixWeight * (matrixNormal * gl_Normal );

        boneWeight = boneWeight.zwxy;
    }
}

void main( void )
{
    position =  vec4(0.0,0.0,0.0,0.0);
    vNormal = vec3(0.0,0.0,0.0);

    // there is 2 bone data per attributes
    if (nbBonesPerVertex > 0)
        computeAcummulatedNormalAndPosition(boneWeight0);
    if (nbBonesPerVertex > 2)
        computeAcummulatedNormalAndPosition(boneWeight1);
    if (nbBonesPerVertex > 4)
        computeAcummulatedNormalAndPosition(boneWeight2);
    if (nbBonesPerVertex > 6)
        computeAcummulatedNormalAndPosition(boneWeight3);

    vNormal = gl_NormalMatrix * vNormal;

    gl_FrontColor = gl_Color;

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
    vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
    vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
    gl_FogFragCoord = length(ecPosition3);
    vFrontColor = gl_FrontMaterial.specular;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
