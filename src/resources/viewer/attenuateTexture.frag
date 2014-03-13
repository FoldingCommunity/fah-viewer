// Author: Justin Hensley
// (C) AMD 2007

uniform sampler2D inputTexture;

varying vec2 vTexCoord;

void main(void) {
  vec4 val = texture2D(inputTexture, vec2(vTexCoord.x, 1.0 - vTexCoord.y));

  gl_FragColor = vec4(val.rgb, 1.0);
}
