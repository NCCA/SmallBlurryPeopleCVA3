#version 410 core

//This shader was adapted from the source code available here:
//  http://devlog-martinsh.blogspot.co.uk/2011/11/glsl-depth-of-field-with-bokeh-v21.html
//This shader is not developed wholly by me, and should not be marked as such.
//Ben Hawkyard

uniform sampler2D bgl_RenderedTexture;
uniform sampler2D bgl_DepthTexture;
uniform vec2 bgl_dim;

#define PI  3.14159265

layout( location = 0 ) out vec4 fragColour;

float width = bgl_dim.x; //texture width
float height = bgl_dim.y; //texture height

vec2 texel = vec2(1.0/width,1.0/height);

uniform float focalDepth;  //external focal point value, but you may use autofocus option below

//------------------------------------------
//user variables

int samples = 4; //samples on the first ring
int rings = 3; //ring count

bool autofocus = false; //use autofocus in shader? disable if you use external focalDepth value
vec2 focus = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)
float range = 16.0; //focal range
float maxblur = 4.0; //clamp value of max blur

float threshold = 0.4; //highlight threshold;
float gain = 1.0; //highlight gain;

float bias = 0.4; //bokeh edge bias
float fringe = 0.25; //bokeh chromatic aberration/fringing

bool noise = true; //use noise instead of pattern for sample dithering
float namount = 0.0001; //dither amount

bool depthblur = true; //blur the depth buffer?
float dbsize = 8.0; //depthblursize

/*
next part is experimental
not looking good with small sample and ring count
looks okay starting from samples = 4, rings = 4
*/

bool pentagon = true; //use pentagon as bokeh shape?
float feather = 0.4; //pentagon shape feather

//------------------------------------------


float penta(vec2 coords) //pentagonal shape
{
    float scale = float(rings) - 1.3;
    vec4  HS0 = vec4( 1.0,         0.0,         0.0,  1.0);
    vec4  HS1 = vec4( 0.309016994, 0.951056516, 0.0,  1.0);
    vec4  HS2 = vec4(-0.809016994, 0.587785252, 0.0,  1.0);
    vec4  HS3 = vec4(-0.809016994,-0.587785252, 0.0,  1.0);
    vec4  HS4 = vec4( 0.309016994,-0.951056516, 0.0,  1.0);
    vec4  HS5 = vec4( 0.0        ,0.0         , 1.0,  1.0);

    vec4  one = vec4( 1.0 );

    vec4 P = vec4((coords),vec2(scale, scale));

    vec4 dist = vec4(0.0);
    float inorout = -4.0;

    dist.x = dot( P, HS0 );
    dist.y = dot( P, HS1 );
    dist.z = dot( P, HS2 );
    dist.w = dot( P, HS3 );

    dist = smoothstep( -feather, feather, dist );

    inorout += dot( dist, one );

    dist.x = dot( P, HS4 );
    dist.y = HS5.w - abs( P.z );

    dist = smoothstep( -feather, feather, dist );
    inorout += dist.x;

    return clamp( inorout, 0.0, 1.0 );
}

float bdepth(vec2 coords) //blurring depth
{
    float d = 0.0;
    float kernel[9];
    vec2 offset[9];

    vec2 wh = vec2(texel.x, texel.y) * dbsize;

    offset[0] = vec2(-wh.x,-wh.y);
    offset[1] = vec2( 0.0, -wh.y);
    offset[2] = vec2( wh.x -wh.y);

    offset[3] = vec2(-wh.x,  0.0);
    offset[4] = vec2( 0.0,   0.0);
    offset[5] = vec2( wh.x,  0.0);

    offset[6] = vec2(-wh.x, wh.y);
    offset[7] = vec2( 0.0,  wh.y);
    offset[8] = vec2( wh.x, wh.y);

    kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
    kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
    kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;


    for( int i=0; i<9; i++ )
    {
        float tmp = texture(bgl_DepthTexture, coords + offset[i]).r;
        d += tmp * kernel[i];
    }

    return d;
}


vec4 color(vec2 coords,float blur) //processing the sample
{
    vec4 col = vec4(0.0);

    col.r = texture(bgl_RenderedTexture,coords + vec2(0.0,1.0)*texel*fringe*blur).r;
    col.g = texture(bgl_RenderedTexture,coords + vec2(-0.866,-0.5)*texel*fringe*blur).g;
    col.b = texture(bgl_RenderedTexture,coords + vec2(0.866,-0.5)*texel*fringe*blur).b;
    col.a = texture(bgl_RenderedTexture,coords + vec2(0.866,-0.5)*texel*fringe*blur).a;

    vec3 lumcoeff = vec3(0.299,0.587,0.114);
    float lum = dot(col.rgb, lumcoeff);
    float thresh = max((lum-threshold)*gain, 0.0);
    return col+mix(vec4(0.0),col,thresh*blur);
}

vec2 rand(in vec2 coord) //generating noise/pattern texture for dithering
{
    float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
    float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;

    if (noise)
    {
        noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
        noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
    }
    return vec2(noiseX,noiseY);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / bgl_dim;

    float depth = texture(bgl_DepthTexture,uv).r;

    float blur = 0.0;

    if (depthblur)
    {
        depth = bdepth(uv);
    }

    /*if(depth > 200)
    {
        fragColour = vec4(0.0, 0.0, 1.0, 1.0);
        return;
    }*/
    blur = clamp((abs(depth - focalDepth) / range), 0.1, maxblur);

    if (autofocus)
    {
        float fDepth = texture(bgl_DepthTexture,focus).r;
        blur = clamp((abs(depth - fDepth)/range) * 100.0,-maxblur,maxblur);
    }

    blur = abs(uv.y - 0.5) * 2.0 * maxblur;

    vec2 noise = rand(uv)*namount*blur;

    float w = (1.0/width)*blur+noise.x;
    float h = (1.0/height)*blur+noise.y;

    vec4 col = texture(bgl_RenderedTexture, uv);
    float s = 1.0;

    int ringsamples;

    for (int i = 1; i <= rings; i += 1)
    {
        ringsamples = i * samples;

        for (int j = 0 ; j < ringsamples ; j += 1)
        {
            float step = PI*2.0 / float(ringsamples);
            float pw = (cos(float(j)*step)*float(i));
            float ph = (sin(float(j)*step)*float(i));
            float p = 1.0;
            if (pentagon)
            {
                p = penta(vec2(pw,ph));
            }
            col += color(uv + vec2(pw*w,ph*h),blur)*mix(1.0,(float(i))/(float(rings)),bias)*p;
            s += 1.0*mix(1.0,(float(i))/(float(rings)),bias)*p;
        }
    }


    col /= s;

    fragColour = col;

    /*if(depth > 500 && depth < 750)
    {
        fragColour.rgb = vec3(1.0, 0.0, 0.0);
    }*/
}
