# version 120

const int numLights = 10;
uniform int numused;
uniform vec2 lightscreen[numLights];
uniform sampler2D occlusionMap;
uniform sampler2D sceneRender;

const float exposure = 0.0035;
const float decay = 1;
const float density = 0.84;
uniform float weight = 5.65;
const int NUM_SAMPLES = 100;

void main() {
  gl_FragColor = vec4(0, 0, 0, 1);
  for (int i = 0; i < numused; i++) {
    vec2 lightScreenPos = lightscreen[i];
    vec2 deltaTextCoord = vec2(gl_TexCoord[0].st - lightScreenPos.xy);
    vec2 textCoord = gl_TexCoord[0].st;
    deltaTextCoord *= 1.0/float(NUM_SAMPLES)*density;
    float illuminationDecay = 1.0;

    vec4 color = vec4(0, 0, 0, 1);
    for (int j = 0; j < NUM_SAMPLES; j++) {
      textCoord -= deltaTextCoord;
      vec4 sample = texture2D(occlusionMap, textCoord);
      sample *= illuminationDecay * weight;
      color += sample;
      illuminationDecay *= decay;
    }
    color *= exposure;
    gl_FragColor += color;
  } 
}
