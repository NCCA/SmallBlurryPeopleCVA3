#version 410 core

#define BUTTON_TEXT_LENGTH 512
#define DOUBLE_MAX_NOTES 10

// actions
const uint PASSIVE           =  0;
const uint QUIT              =  1;
const uint BUILDHOUSE        =  2;
const uint BUILDSTORE        =  3;
const uint CENTRECAMERA      =  4;
const uint ESCAPE            =  5;
const uint ZOOMIN            =  6;
const uint ZOOMOUT           =  7;
const uint MOVEFORWARD       =  8;
const uint MOVEBACKWARD      =  9;
const uint MOVELEFT          = 10;
const uint MOVERIGHT         = 11;
const uint STOPFORWARD       = 12;
const uint STOPBACKWARD      = 13;
const uint STOPLEFT          = 14;
const uint STOPRIGHT         = 15;
const uint PREFERENCES       = 16;
const uint PASSIVE_CHARACTER = 17;
const uint SETBOOLPREF       = 18;
const uint CHAR_FORAGE       = 19;
const uint NOTIFY            = 20;
const uint PREFS_VALUE       = 21;
const uint INCR_PREFS        = 22;
const uint DECR_PREFS        = 23;
const uint SAVE_PREFERENCES  = 24;
const uint CHAR_STATE        = 25;
const uint STAMINA_BAR       = 26;
const uint HEALTH_BAR        = 27;
const uint HUNGER_BAR        = 28;
const uint POPULATION        = 29;
const uint CHAR_EAT_BERRIES  = 30;
const uint CHAR_EAT_FISH     = 31;

//game states
const uint STATE_MAIN  = 0;
const uint STATE_PAUSE = 1;
const uint STATE_PREFS = 2;

// character states
const int CHOP_WOOD      =  0;
const int STORE          =  1;
const int FISH           =  2;
const int FORAGE         =  3;
//not needed as done too fast to read message
/*
const int CHECK_WOOD     =  4;
const int CHECK_BERRIES  =  5;
const int CHECK_FISH     =  6;
const int REPEAT         = 17;
*/
const int GET_WOOD       =  7;
const int GET_BERRIES    =  8;
const int GET_FISH       =  9;
const int BUILD          = 10;
const int SLEEP          = 11;
const int EAT_BERRIES    = 12;
const int EAT_FISH       = 13;
const int MOVE           = 14;
const int TRACK          = 15;
const int FIGHT          = 16;
const int IDLE           = 18;

// character exceptions
const int TEXT_CROSS = 32;
const int TEXT_NEWLINE = 10;

///
/// \brief character draw character at the given position
/// \param ch character index
/// \param tp current text position
/// \return greyscale text
///
float character(float ch, vec2 tp);

///
/// \brief charStateText draw character at the given position, checks the current character's state and then writes the corresponding text to the button
/// \param tp position for text
/// \return greyscale text
///
float charStateText(vec2 tp);

///
/// \brief text draws text from the button_text uniform, between given indices
/// \param pos position to draw text
/// \param start_index index of button_text to begin text
/// \param end_index index of button_text to end text
/// \return greyscale text
///
vec3 text(vec2 pos, int start_index, int end_index);

///
/// \brief centerText writes text in center of button, using button_text and fragId
/// \return greyscale text
///
vec3 centerText();

///
/// \brief staminaText write text on top of stamina meter
/// \return greyscale text
///
vec3 staminaText();

///
/// \brief healthText write text on top of health meter
/// \return greyscale text
///
vec3 healthText();

///
/// \brief hungerText write text on top of hunger meter
/// \return greyscale text
///
vec3 hungerText();

///
/// \brief populationText write population text to the population button
/// \return greyscale text
///
vec3 populationText();

///
/// \brief box draw an SDF represented box, with curved corners
/// \param position position of the box
/// \param size size of box
/// \param radius radius of the corner curves
/// \return + for inside the box, - for outside the box, 0 for on the edge
///
float box(vec2 position, vec2 size, float radius);
///
/// \brief translate position for looking up textures, text etc
/// \param p original position
/// \param t amount to translate
/// \return new position
///
vec2 translate(vec2 p, vec2 t);

///
/// \brief getIcon
/// \param pixel_uv uv position of button
/// \param icon_id index of icon needed
/// \param size size of icon in pixels, defaults to 64x64
/// \return colour of texture
///
vec4 getIcon(vec2 pixel_uv, uint icon_id, vec2 size);

///
/// \brief loadingBar draw loading bar
/// \param value value of bar
/// \param uv_x current uv x position
/// return float 1-0 for whether bar at position is full
///
float loadingBar(float value, float uv_x);

///
/// \brief shiftRGB rotates R, G, B values
/// \param c initial colour
/// \param up_down direction to shift colour in
///
vec3 shiftRGB(vec3 c, int up_down);

///
/// \brief buttonRequiresCharacter check whether button requires character to be drawn
/// \return true if button requires character, false otherwise
///
bool buttonRequiresCharacter();

///
/// \brief buttonRequiresCharacter check whether button uses character colour
/// \return true if button is drawn with character colour, false otherwise
///
bool buttonUsesCharacterColor();

layout(location = 0) out vec4 outColour;

uniform vec2 fResolution;
uniform int game_state;
uniform sampler2D icons;
uniform sampler2D font;

uniform int notification_ages[DOUBLE_MAX_NOTES];
uniform int max_notification_age;

uniform uint button_text[BUTTON_TEXT_LENGTH];

uniform bool character_selected;
uniform int character_state;
uniform float character_stamina;
uniform float character_health;
uniform float character_hunger;
uniform vec3 character_color;
uniform int population;
uniform int max_pop;

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
vec3 def_button_color = vec3(0.7, 0.1, 0.0);

//---------------------------------------------------------
// text functions and format inspired by
// https://www.shadertoy.com/view/MtySzd
//---------------------------------------------------------

//--- common data ---

//--- font data ---
uniform float FONT_SIZE = 20;
uniform float FONT_SPACE = 0.5;

// define function for different character indices

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
#define _slash S(47);
#define _cross S(215);
#define _colon S(58);

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
  return f.x;
}

float charStateText(vec2 tp)
{
  float c = 0.0;
  if(character_state == CHOP_WOOD)
  {
    tp = translate(tp, vec2(-8.0/4.0, 0.0));
    _C _h _o _p _p _i _n _g
    //_M
  }
  else if(character_state == STORE)
  {
    tp = translate(tp, vec2(-7.0/4.0, 0.0));
    _S _t _o _r _i _n _g
  }
  else if(character_state == FISH)
  {
    tp = translate(tp, vec2(-7.0/4.0, 0.0));
    _F _i _s _h _i _n _g
  }
  else if(character_state == FORAGE)
  {
    tp = translate(tp, vec2(-8.0/4.0, 0.0));
    _F _o _r _a _g _i _n _g
  }
  else if(character_state == GET_WOOD)
  {
    tp = translate(tp, vec2(-12.0/4.0, 0.0));
    _G _e _t _t _i _n _g __ _w _o _o _d
  }
	else if(character_state == GET_BERRIES)
	{
		tp = translate(tp, vec2(-15.0/4.0, 0.0));
		_G _e _t _t _i _n _g __ _b _e _r _r _i _e _s
	}
	else if(character_state == GET_FISH)
	{
		tp = translate(tp, vec2(-12.0/4.0, 0.0));
		_G _e _t _t _i _n _g __ _f _i _s _h
	}
  else if(character_state == BUILD)
  {
    tp = translate(tp, vec2(-8.0/4.0, 0.0));
    _B _u _i _l _d _i _n _g
  }
  else if(character_state == SLEEP)
  {
    tp = translate(tp, vec2(-8.0/4.0, 0.0));
    _S _l _e _e _p _i _n _g
  }
	else if(character_state == EAT_BERRIES)
	{
    tp = translate(tp, vec2(-14.0/4.0, 0.0));
		_E _a _t _i _n _g __ _b _e _r _r _i _e _s
	}
	else if(character_state == EAT_FISH)
	{
    tp = translate(tp, vec2(-11.0/4.0, 0.0));
		_E _a _t _i _n _g __ _f _i _s _h
	}
	else if(character_state == MOVE || character_state == TRACK)
  {
    tp = translate(tp, vec2(-10.0/4.0, 0.0));
    _T _r _a _v _e _l _l _i _n _g
  }
	else if(character_state == FIGHT)
	{
		tp = translate(tp, vec2(-8.0/4.0, 0.0));
		_F _i _g _h _t _i _n _g
	}
  else if(character_state == IDLE)
  {
    tp = translate(tp, vec2(-4.0/4.0, 0.0));
    _I _d _l _e
  }
  return c;
}

vec3 text(vec2 pos, int start_index, int end_index)
{
  vec2 tp0 = pos / FONT_SIZE;  // original position
  vec2 tp  = tp0;  // dynamic text position

  float c = 0.0;
  //_Q _U _I _T
  if(fragAction == CHAR_STATE)
  {
    c += float(charStateText(tp));
  }
  else
  {
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

vec3 staminaText()
{
  float c = 0;
  vec2 pos = gl_FragCoord.xy-vec2(0.0, fResolution.y);
  vec2 text_pos = translate(pos, vec2(fragPixelPos.x + (fragPixelSize.x - 7.0 * FONT_SIZE * FONT_SPACE)/2.0, -(fragPixelPos.y + (fragPixelSize.y - (0 - 1) * FONT_SIZE)/2.0)));
  vec2 tp = translate(text_pos/FONT_SIZE, vec2(-1.0, 0.0));

  _s _t _a _m _i _n _a
  return vec3(max(c, 0.0));
}

vec3 healthText()
{
  float c = 0;
  vec2 pos = gl_FragCoord.xy-vec2(0.0, fResolution.y);
  vec2 text_pos = translate(pos, vec2(fragPixelPos.x + (fragPixelSize.x - 7.0 * FONT_SIZE * FONT_SPACE)/2.0, -(fragPixelPos.y + (fragPixelSize.y - (0 - 1) * FONT_SIZE)/2.0)));
  vec2 tp = translate(text_pos/FONT_SIZE, vec2(-1.0, 0.0));

  _h _e _a _l _t _h
  return vec3(max(c, 0.0));
}

vec3 hungerText()
{
  float c = 0;
  vec2 pos = gl_FragCoord.xy-vec2(0.0, fResolution.y);
  vec2 text_pos = translate(pos, vec2(fragPixelPos.x + (fragPixelSize.x - 7.0 * FONT_SIZE * FONT_SPACE)/2.0, -(fragPixelPos.y + (fragPixelSize.y - (0 - 1) * FONT_SIZE)/2.0)));
  vec2 tp = translate(text_pos/FONT_SIZE, vec2(-1.0, 0.0));

  _h _u _n _g _e _r
  return vec3(max(c, 0.0));
}

vec3 populationText()
{
  float c = 0;
  vec2 pos = gl_FragCoord.xy-vec2(0.0, fResolution.y);
  vec2 text_pos = translate(pos, vec2(fragPixelPos.x + (fragPixelSize.x - 7.0 * FONT_SIZE * FONT_SPACE)/2.0, -(fragPixelPos.y + (fragPixelSize.y - (0 - 1) * FONT_SIZE)/2.0)));
  vec2 tp = translate(text_pos/FONT_SIZE, vec2(-1.0, 0.0));

  __ __ __

  int pop = population;
  while(pop > 10)
  {
    tp.x-=FONT_SPACE;
    pop/=10;
  }
  vec2 tp0 = tp;
  pop = population;
  do
  {
    S(pop%10 + 48);
    pop /= 10;
    tp.x+=FONT_SPACE*2;
  } while(pop>0);
  tp = tp0;
  __ _slash

  pop = max_pop;
  while(pop > 10)
  {
    tp.x-=FONT_SPACE;
    pop/=10;
  }
  pop = max_pop;
  do
  {
    S(pop%10 + 48);
    pop /= 10;
    tp.x+=FONT_SPACE*2;
  } while(pop>0);

  return vec3(max(c, 0.0));
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

vec4 getIcon(vec2 fragUV, uint icon_id, vec2 size = vec2(64,64))
{
  vec4 s = vec4(0);
  vec2 uv = vec2((fragUV.x + icon_id%16) * size.x/1024, (fragUV.y + icon_id/16) * size.y/1024);
  s = texture2D(icons, uv);

  return s;
}

float loadingBar(float value, float uv_x)
{
  float mult = smoothstep(value+3/fragPixelSize.x, value-3/fragPixelSize.x, uv_x);
  return min(mult + 0.3, 1);
}

vec3 shiftRGB(vec3 c, int up_down)
{
  if(up_down > 0)
  {
    return c.brg;
  }
  else if(up_down < 0)
  {
    return c.gbr;
  }
  else
  {
    return c;
  }
}

bool buttonRequiresCharacter()
{
  if(buttonUsesCharacterColor()
     || fragAction == STAMINA_BAR
     || fragAction == HEALTH_BAR
     || fragAction == HUNGER_BAR)
  {
    return true;
  }
  return false;
}

bool buttonUsesCharacterColor()
{
  if(   fragAction == PASSIVE_CHARACTER
     || fragAction == CHAR_STATE
     || fragAction == CHAR_FORAGE
     || fragAction == BUILDHOUSE
     || fragAction == BUILDSTORE
     || fragAction == CENTRECAMERA
     || fragAction == CHAR_EAT_BERRIES
     || fragAction == CHAR_EAT_FISH)
  {
    return true;
  }
  return false;
}

void main()
{
  vec2 button_pixel_size = fragSize * fResolution;
  vec2 button_pixel_pos = fragPos * fResolution;
  vec2 button_abs_pixel_pos = button_pixel_pos + vec2(fResolution/2);
  vec2 pixel_uv = fragUV * button_pixel_size;

  vec3 s = border_color;
  vec2 button_inner = button_pixel_size/2 - border_size;


  if(box(translate(pixel_uv, button_pixel_size/2), button_pixel_size/2, border_size*3) > 0
     || (buttonRequiresCharacter() && !character_selected))
  {
    discard;
  }

  float edge = box(translate(pixel_uv, button_pixel_size/2), button_inner, border_size*2);
  if(edge <= 0)
  {
    vec3 button_color = def_button_color;
    vec4 icon_color = vec4(0);
    if(buttonUsesCharacterColor())
    {
      button_color = character_color - vec3(0.2, 0.2, 0.2);
    }
    vec3 button_highlight = button_color + vec3(0.4, 0.4, 0.4);
    s = mix(button_highlight, button_color, fragUV.y);
    if(fragAction == STAMINA_BAR)
    {
      s *= loadingBar(character_stamina, fragUV.x);
      s = shiftRGB(s, -1);
    }
    else if(fragAction == HEALTH_BAR)
    {
      s *= loadingBar(character_health, fragUV.x);
      //s = shiftRGB(s, 0);
    }
    else if(fragAction == HUNGER_BAR)
    {
      s *= loadingBar(character_hunger, fragUV.x);
      s = shiftRGB(s, 1);
      s.g -= 0.1;
    }
    else if(fragAction == POPULATION)
    {
      s += populationText();
      icon_color = getIcon(fragUV, 16, vec2(192, 64));
    }
    else if(fragAction == CENTRECAMERA)
    {
      icon_color = getIcon(fragUV, 0);
    }
    else if(fragAction == BUILDHOUSE)
    {
      icon_color = getIcon(fragUV, 1);
    }
    else if(fragAction == BUILDSTORE)
    {
      icon_color = getIcon(fragUV, 2);
    }
    else if(fragAction == CHAR_FORAGE)
    {
      icon_color = getIcon(fragUV, 3);
    }
    else if(fragAction == CHAR_EAT_BERRIES)
    {
      icon_color = getIcon(fragUV, 4);
    }
    else if(fragAction == CHAR_EAT_FISH)
    {
      icon_color = getIcon(fragUV, 5);
    }
    s = mix(s, icon_color.xyz, icon_color.a);

    if(fragMousedOver > 0)
    {
      if(fragAction == STAMINA_BAR)
      {
        s += staminaText();
      }
      else if(fragAction == HEALTH_BAR)
      {
        s += healthText();
      }
      else if(fragAction == HUNGER_BAR)
      {
        s += hungerText();
      }
      else
      {
        s += button_selected;
      }
    }
  }


  s += centerText();

  float a = 1.0;
  if(fragAction == NOTIFY)
  {
    for(int i=1; i < DOUBLE_MAX_NOTES; i+=2)
    {
      if(notification_ages[i-1] == fragId)
      {
        a = smoothstep(1, 0, notification_ages[i] / float(max_notification_age));
      }
    }
  }
  outColour = vec4(s, a);
}

//return text(translate(pos, vec2(fragPixelPos.x + border_size, -(fragPixelPos.y + FONT_SIZE + border_size))));

