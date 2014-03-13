// Author: Maurice Ribble
// 3D Application Research Group
// (C) ATI Research, Inc. 2006 All rights reserved.
//
// Fragment shader that does a post process blur

uniform float sampleDist1;

uniform sampler2D sharpTex;
varying vec2 vTexCoord;

void main(void) {
  vec2 samples[12];
  samples[0]  = vec2(-0.326212, -0.405805);
  samples[1]  = vec2(-0.840144, -0.073580);
  samples[2]  = vec2(-0.695914,  0.457137);
  samples[3]  = vec2(-0.203345,  0.620716);
  samples[4]  = vec2( 0.962340, -0.194983);
  samples[5]  = vec2( 0.473434, -0.480026);
  samples[6]  = vec2( 0.519456,  0.767022);
  samples[7]  = vec2( 0.185461, -0.893124);
  samples[8]  = vec2( 0.507431,  0.064425);
  samples[9]  = vec2( 0.896420,  0.412458);
  samples[10] = vec2(-0.321940, -0.932615);
  samples[11] = vec2(-0.791559, -0.597705);

  vec4 sum = texture2D(sharpTex, vTexCoord);
   
  for (int i = 0; i < 12; i++)
    sum += texture2D(sharpTex, vTexCoord + sampleDist1 * samples[i]);

  sum /= 13.0;

  gl_FragColor = vec4(sum);
}
