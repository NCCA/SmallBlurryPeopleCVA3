#version 410 core
float box(vec2 position, vec2 size, float radius);
vec2 translate(vec2 p, vec2 t);

layout(location = 0) out vec4 outColour;

uniform vec2 fResolution;

in vec2 fragPos;
in vec2 fragSize;
in vec2 fragUV;
in float fragMousedOver;

float border_size = 7;
vec3 border_color = vec3(1,1,0);
vec3 button_color = vec3(0.7, 0.1, 0.0);
vec3 button_highlight = vec3(0.9, 0.7, 0.4);

vec3 button_selected = vec3(0.1, 0.2, 0.2);

void main()
{
  //vec2 buttonUV = (gl_FragCoord.xy - posCoord) / sizeCoord;
  vec2 button_pixel_size = fragSize * fResolution;
  vec2 button_pixel_pos = fragPos * fResolution;
  vec2 pixel_uv = fragUV * button_pixel_size;

  vec3 s = border_color;
  vec2 button_inner = button_pixel_size/2 - border_size;


  if(box(translate(pixel_uv, button_pixel_size/2), button_pixel_size/2, border_size*3) > 0)
  {
    discard;
  }

  if(fragMousedOver > 0)
  {
    button_color += button_selected;
    button_highlight += button_selected;
  }
//  vec2 button_inner = 0.5 - (border_size * 2) / (fragSize*fResolution);
//  float radius = border_size;
//  if(box(translate(fragUV, vec2(0.5)), button_inner, 0) <= 0)
  if(box(translate(pixel_uv, button_pixel_size/2), button_inner, border_size*2) <= 0)
  {
    s = mix(button_highlight, button_color, fragUV.y);
  }

  outColour = vec4(s, 1.0);
}

float box(vec2 position, vec2 size, float radius)
{
  //return length(max(abs(position)-size,0.0));
  size -= vec2(radius);
  vec2 d = abs(position) - size;
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;
}

vec2 translate(vec2 p, vec2 t)
{
  p -= t;
  return p;
}
