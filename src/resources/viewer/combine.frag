// Author: Maurice Ribble
// 3D Application Research Group
// (C) ATI Research, Inc. 2006 All rights reserved. 
//
// Fragment shader that combines the blury and sharp images to form a depth 
// of field effect

varying vec2 vTexCoord;

uniform sampler2D sharpTex;
uniform sampler2D blurFbo2;
uniform sampler2D shadowMapFbo;
uniform float range;
uniform float focus;

void main() {
  vec4 sharp = texture2D(sharpTex, vTexCoord);
  vec4 blur  = texture2D(blurFbo2, vTexCoord);
  vec4 shadowMap =  texture2D(shadowMapFbo, vTexCoord);

  gl_FragColor =
    mix(sharp, blur, clamp(range * abs(focus - sharp.a), 0.0, 1.0));
}

