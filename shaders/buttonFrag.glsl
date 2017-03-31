#version 410 core

#define BUTTON_TEXT_LENGTH 256
#define DOUBLE_MAX_NOTES 10

// actions
const uint PASSIVE           = 0;
const uint QUIT              = 1;
const uint BUILDHOUSE        = 2;
const uint BUILDSTORE        = 3;
const uint CENTRECAMERA      = 4;
const uint ESCAPE            = 5;
const uint ZOOMIN            = 6;
const uint ZOOMOUT           = 7;
const uint MOVEFORWARD       = 8;
const uint MOVEBACKWARD      = 9;
const uint MOVELEFT          = 10;
const uint MOVERIGHT         = 11;
const uint STOPFORWARD       = 12;
const uint STOPBACKWARD      = 13;
const uint STOPLEFT          = 14;
const uint STOPRIGHT         = 15;
const uint PREFERENCES       = 16;
const uint PASSIVE_CHARACTER = 17;
const uint SETBOOLPREF       = 18;
const uint FORAGE            = 19;
const uint NOTIFY            = 20;

//game states
const uint STATE_MAIN  = 0;
const uint STATE_PAUSE = 1;
const uint STATE_PREFS = 2;

// character exceptions
const int TEXT_CROSS = 32;
const int TEXT_NEWLINE = 10;

// return character intensity of ch at position tp
float character(float ch, vec2 tp);
vec3 text(vec2 pos);
vec3 centerText();
float box(vec2 position, vec2 size, float radius);
vec2 translate(vec2 p, vec2 t);
vec3 getIcon(vec2 pixel_uv);

layout(location = 0) out vec4 outColour;

uniform vec2 fResolution;
uniform int game_state;
uniform sampler2D icons;
uniform sampler2D font;

uniform int notification_ages[DOUBLE_MAX_NOTES];
uniform int max_notification_age;

uniform uint button_text[BUTTON_TEXT_LENGTH];

in vec2 fragPos;
in vec2 fragSize;
in vec2 fragPixelPos;
in vec2 fragPixelSize;
in vec2 fragUV;
in float fragMousedOver;
flat in int fragAction;
flat in int fragId;

const float border_size = 7;
const vec3 border_color = vec3(1,1,0);
const vec3 button_selected = vec3(0.1, 0.2, 0.2);
vec3 button_color = vec3(0.7, 0.1, 0.0);
vec3 button_highlight = vec3(0.9, 0.7, 0.4);

//---------------------------------------------------------
// text functions modified from
// https://www.shadertoy.com/view/MtySzd
//---------------------------------------------------------

//--- common data ---

//--- font data ---
uniform float FONT_SIZE = 20;
uniform float FONT_SPACE = 0.5;

//----- access to the image of ascii code characters ------
//#define S(a) c+=texture(iChannel0,clamp(tp,0.,1.)/16.+fract(floor(vec2(a,15.999-float(a)/16.))/16.)).x; uv.x-=FONT_SPACE;

#define S(a) c+=character(float(a), tp); tp.x-=FONT_SPACE;

#define _play S(5);
#define _pause S(6);
#define _note  S(10);
#define _star  S(28);
#define _smiley S(29);
#define __    S(32);
#define _exc S(33);
#define _add S(43);
#define _comma S(44);
#define _dot S(46);
#define _cross S(215);

#define _0 S(48);
#define _1 S(49);
#define _2 S(50);
#define _3 S(51);
#define _4 S(52);
#define _5 S(53);
#define _6 S(54);
#define _7 S(55);
#define _8 S(56);
#define _9 S(57);

#define _A S(65);
#define _B S(66);
#define _C S(67);
#define _D S(68);
#define _E S(69);
#define _F S(70);
#define _G S(71);
#define _H S(72);
#define _I S(73);
#define _J S(74);
#define _K S(75);
#define _L S(76);
#define _M S(77);
#define _N S(78);
#define _O S(79);
#define _P S(80);
#define _Q S(81);
#define _R S(82);
#define _S S(83);
#define _T S(84);
#define _U S(85);
#define _V S(86);
#define _W S(87);
#define _X S(88);
#define _Y S(89);
#define _Z S(90);

#define _a S(97);
#define _b S(98);
#define _c S(99);
#define _d S(100);
#define _e S(101);
#define _f S(102);
#define _g S(103);
#define _h S(104);
#define _i S(105);
#define _j S(106);
#define _k S(107);
#define _l S(108);
#define _m S(109);
#define _n S(110);
#define _o S(111);
#define _p S(112);
#define _q S(113);
#define _r S(114);
#define _s S(115);
#define _t S(116);
#define _u S(117);
#define _v S(118);
#define _w S(119);
#define _x S(120);
#define _y S(121);
#define _z S(122);

#define _newline tp.y += 1.0; tp.x = tp0.x;

// return character intensity of ch at position tp
float character(float ch, vec2 tp)
{
  vec2 tmp = clamp(tp,0.,1.)/16.+fract(floor(vec2(ch,15.999-float(ch)/16.))/16.);
  vec4 f = texture2D(font,vec2(tmp.x, 1-tmp.y));
  //if (1.0 > 0.0)
  return f.x;   // 2d
  //else
    //return f.x * (f.y+0.3)*(f.z+0.3)*2.0;   // 3d
}

vec3 text(vec2 pos, int start_index, int end_index)
{
  vec2 tp0 = pos / FONT_SIZE;  // original position
  vec2 tp  = tp0;  // dynamic text position

  float c = 0.0;
  //_Q _U _I _T
  int button_id = 0;
  for(int i=start_index; i<end_index; i++)
  {
   if(button_text[i] == TEXT_NEWLINE)
   {
     _newline;
   }
   else
   {
     S(button_text[i]);
   }
  }
  return vec3(max(c, 0.0));
}

vec3 centerText()
{

  int start_index = 0;
  int end_index = 0;
  int line_len = 0;
  int max_len = 0;
  int button_id = 0;
  int num_lines = 0;
  // find string start and end for this button
  for(int i=0; i<BUTTON_TEXT_LENGTH; i++)
  {
    if(button_text[i] == TEXT_NEWLINE)
    {
      line_len = 0;
      num_lines++;
    }
    else
    {
      line_len++;
    }
    max_len = max(max_len, line_len);
    if(button_text[i] == 0)
    {
      if(button_id == fragId)
      {
        end_index = i;
        break;
      }
      else
      {
        button_id++;
        start_index = i+1;
        max_len = 0;
        line_len = 0;
        num_lines = 0;
      }
    }
  }
  //str_len = end_index - start_index;
  if(max_len > 0)
  {
    vec2 pos = gl_FragCoord.xy-vec2(0.0, fResolution.y);// - FONT_SIZE);
    vec2 text_pos = translate(pos, vec2(fragPixelPos.x + (fragPixelSize.x - max_len * FONT_SIZE * FONT_SPACE)/2.0, -(fragPixelPos.y + (fragPixelSize.y - (num_lines - 1) * FONT_SIZE)/2.0)));
    return text(text_pos, start_index, end_index);
  }
  else
  {
    return vec3(0,0,0);
  }
}

// modified functions end

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

vec3 getIcon(vec2 pixel_uv)
{
  vec3 s = vec3(0,1,0);

  s = texture2D(icons, pixel_uv/vec2(1024.0)).xyz;

  return s;
}

void main()
{
  //vec2 buttonUV = (gl_FragCoord.xy - posCoord) / sizeCoord;
  vec2 button_pixel_size = fragSize * fResolution;
  vec2 button_pixel_pos = fragPos * fResolution;
  vec2 button_abs_pixel_pos = button_pixel_pos + vec2(fResolution/2);
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

  s += centerText();
  float a = 1.0;
  if(fragAction == NOTIFY)
  {
    for(int i=1; i < DOUBLE_MAX_NOTES; i+=2)
    {
      if(notification_ages[i-1] == fragId)
      {
        //a = (max_notification_age - notification_ages[1])/float(max_notification_age);
        //a = float(notification_ages[i])/float(max_notification_age);
        //a = smoothstep(1, 0, notification_ages[i] / float(max_notification_age));
      }
    }
    //a = pow(smoothstep(1, 0, notification_ages[1] / float(max_notification_age)), 0.25);
  }
  outColour = vec4(s, a);
}

//return text(translate(pos, vec2(fragPixelPos.x + border_size, -(fragPixelPos.y + FONT_SIZE + border_size))));

