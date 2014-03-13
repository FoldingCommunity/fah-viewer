// Author: Maurice Ribble
// 3D Application Research Group
// (C) ATI Research, Inc. 2006 All rights reserved.
//
// Main fragment shader for the poteins.
// This applies base map textures shadowing and lighting.

uniform sampler2D NormalMap;
uniform sampler2D shadowMapFbo;
uniform float toon;

varying vec4 diffuse;
varying vec3 normal, lightDir, halfVector;
varying float dist;
varying vec3 vViewVec;
varying vec2 vTexCoord;
varying vec4 worldCoords;


float SiSampleShadowMap9TapPCF(vec2 shadowUV, float depth) {
  vec2 vInvShadowMapRes = vec2(1.0 / 1024.0, 1.0 / 1024.0);
  float shadowDarkness = 0.6;

  float accum;

  vec2 poisson[8];
  poisson[0] = vec2(0.527837, -0.085868);
  poisson[1] = vec2(-0.040088, 0.536087);
  poisson[2] = vec2(-0.670445, -0.179949);
  poisson[3] = vec2(-0.419418, -0.616039);
  poisson[4] = vec2(0.440453, -0.639399);
  poisson[5] = vec2(-0.757088, 0.349334);
  poisson[6] = vec2(0.574619, 0.685879);
  poisson[7] = vec2(0.03851, -0.939059);

  float mapDepth = texture2D(shadowMapFbo, shadowUV).r;
  accum = (mapDepth > depth) ? 1.0 : shadowDarkness;

  for (int i = 0; i < 7; i++) {
    vec2 sPos = shadowUV + poisson[i] * 3.0 * vInvShadowMapRes;

    mapDepth = texture2D(shadowMapFbo, sPos).r;
    accum += (mapDepth > depth) ? 1.0 : shadowDarkness;
  }

  return accum / 8.0;
}


void main() {
  // Do the lighting
  vec3 finalNormal, halfV, viewV, ldir;
  float NdotL, NdotHV;
  float att;
  vec4 ambient, ambientGlobal;
  vec3 normalNoise = texture2D(NormalMap, vTexCoord).xyz;

  // The ambient terms have been separated since one of them suffers attenuation
  ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
  ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  vec4 color = ambientGlobal;

  // a fragment shader can't write a varying variable, hence we need a new
  // variable to store the normalized interpolated normal
  normalNoise = normalNoise * gl_FrontMaterial.shininess / 150.0
    -gl_FrontMaterial.shininess / 300.0; // Scale and bias the normal Noise
  normalNoise = normal + normalNoise;

  if (toon != 0.0) finalNormal = normal;
  else finalNormal = normalize(normalNoise);

  // compute the dot product between normal and normalized lightdir
  NdotL = dot(finalNormal, normalize(lightDir));
  if (NdotL < 0.0) NdotL = 0.0;

  if (toon != 0.0) {
    if (NdotL > 0.95) color += gl_FrontMaterial.diffuse;
    else if (NdotL > 0.5) color += 0.6 * gl_FrontMaterial.diffuse;
    else if (NdotL > 0.25) color += 0.4 * gl_FrontMaterial.diffuse;
    else color += 0.25 * gl_FrontMaterial.diffuse;
  }

  if (NdotL > 0.0) {
    att = 1.0 / (gl_LightSource[0].constantAttenuation +
                 gl_LightSource[0].linearAttenuation * dist +
                 gl_LightSource[0].quadraticAttenuation * dist * dist);

    if (toon == 0.0) color += att * (diffuse * NdotL + ambient);

    halfV = normalize(halfVector);
    NdotHV = dot(finalNormal, halfV);
    if (NdotHV < 0.0) NdotHV = 0.0;
    color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular *
      pow(NdotHV, gl_FrontMaterial.shininess);
  }

  // We'll use the distance to decide how much blur we want
  // store it in the alpha channel
  color.a = length(vViewVec);

  // Do the Shadowing
  // transform point into shadow map space
  vec4 vShadowPos = gl_TextureMatrix[0] * vec4(worldCoords);
  vShadowPos /= vShadowPos.w;
  float fDepth = vShadowPos.z - 0.005;

  // get UVs for shadow lookup
  vec2 vShadowUV = vShadowPos.xy;

  // Scale and bias already done matrix
  float fShadow = SiSampleShadowMap9TapPCF(vShadowUV, fDepth);

  if (toon != 0.0) {
    if (abs(dot(vViewVec, finalNormal)) < 0.1)
      color.rgb = mix(color.rgb, vec3(0.0), 0.5);
    else if (abs(dot(vViewVec, finalNormal)) < 0.11)
      color.rgb = mix(color.rgb, vec3(0.0), 0.25);
  }

  // Output the final color
  gl_FragColor = vec4(fShadow * color.rgb, color.a);
}
