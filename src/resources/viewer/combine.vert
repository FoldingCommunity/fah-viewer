// Author: Maurice Ribble
// 3D Application Research Group
// (C) ATI Research, Inc. 2006 All rights reserved. 
//
// Vertex shader that combines the blury and sharp images to form a depth
// of field effect

varying vec2 vTexCoord;

void main() {
  // Clean up inaccuracies
  vec2 Pos = sign(gl_Vertex.xy);
  
  gl_Position = vec4(Pos.xy, 0, 1);

  // Image-space
  vTexCoord.x = 0.5 * (1.0 + Pos.x);
  vTexCoord.y = 0.5 * (1.0 + Pos.y);
}

