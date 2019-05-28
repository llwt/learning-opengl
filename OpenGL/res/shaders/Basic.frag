#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
  // TODO: how can we modulate textColor by u_Color?
  // color = u_Color;

  vec4 texColor = texture(u_Texture, v_TexCoord);

  // Can we just = texture here? 
  color = texColor * u_Color;
};
