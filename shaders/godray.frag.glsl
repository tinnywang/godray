# version 120

uniform vec2 screenLightPos;
uniform sampler2D occlusionMap;
uniform sampler2D sceneRender;

const float exposure = 0.0035;
const float decay = 1;
const float density = 0.84;
uniform float weight = 5.65;
const int NUM_SAMPLES = 100;

void main() {
  vec2 deltaTextCoord = vec2(gl_TexCoord[0].st - screenLightPos.xy);
  vec2 textCoord = gl_TexCoord[0].st;
  deltaTextCoord *= 1.0/float(NUM_SAMPLES)*density;
  float illuminationDecay = 1.0;

  gl_FragColor = vec4(0, 0, 0, 1);
  for (int i = 0; i < NUM_SAMPLES; i++) {
    textCoord -= deltaTextCoord;
    vec4 sample = texture2D(occlusionMap, textCoord);
    sample *= illuminationDecay * weight;
    gl_FragColor += sample;
    illuminationDecay *= decay;
  }
  gl_FragColor *= exposure;
  
  //gl_FragColor += texture2D(sceneRender, gl_TexCoord[0].st);
}
