// Author: Maurice Ribble
// 3D Application Research Group
// (C) ATI Research, Inc. 2006 All rights reserved.
//
// Main vertex shader for the poteins.
// This applies base map textures shadowing, and lighting.

uniform float distanceScale;

varying vec4 diffuse;
varying vec3 normal, lightDir, halfVector;
varying float dist;
varying vec3 vViewVec;
varying vec2 vTexCoord;
varying vec4 worldCoords;

void main() {	
  vec4 ecPos;
  vec3 aux;

  vTexCoord = vec2(gl_MultiTexCoord0);
  normal = normalize(gl_NormalMatrix * gl_Normal);

  // Compute the light's direction
  ecPos = gl_ModelViewMatrix * gl_Vertex;
  aux = vec3(gl_LightSource[0].position - ecPos);
  lightDir = normalize(aux);
 
  dist = length(aux);
  halfVector = normalize(gl_LightSource[0].halfVector.xyz);

  // Compute the diffuse, ambient and globalAmbient terms
  diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

  // We multiply with distance scale in the vertex shader
  // instead of the fragment shader to improve performance.
  vViewVec = -vec3(distanceScale * gl_ModelViewMatrix * gl_Vertex);

  worldCoords = (gl_TextureMatrix[1] * gl_Vertex);
  gl_Position = ftransform();
}
