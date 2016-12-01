#version 330
uniform sampler2D Texture;
in vec2 Frag_UV;
in vec4 Frag_Color;
/// @brief our output fragment colour
layout (location =0) out vec4 fragColour;
void main()
{
  fragColour = Frag_Color * texture( Texture, Frag_UV.st);
}
