//****************************************
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// Vertex shader that does a post process blur.
//
//****************************************
// $Id: blur2.vert,v 1.2 2006/09/15 23:49:37 beberg Exp $ 
// 
// Last check-in:  $DateTime:  $ 
// Last edited by: $Author: beberg $
//****************************************
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//****************************************


varying vec2 vTexCoord;

void main(void) {
  // Clean up inaccuracies
  vec2 Pos = sign(gl_Vertex.xy);
  
  gl_Position = vec4(Pos.xy, 0, 1);

  // Image-space
  vTexCoord.x = 0.5 * (1.0 + Pos.x);
  vTexCoord.y = 0.5 * (1.0 + Pos.y);
}

