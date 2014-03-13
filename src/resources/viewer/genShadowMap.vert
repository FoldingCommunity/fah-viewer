//****************************************
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// Vertex shader used to generate the shadow map.
//
//****************************************
// $Id: genShadowMap.vert,v 1.1 2006/09/15 23:49:37 beberg Exp $ 
// 
// Last check-in:  $DateTime:  $ 
// Last edited by: $Author: beberg $
//****************************************
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//****************************************

void main() {	
  gl_Position = ftransform();
} 
