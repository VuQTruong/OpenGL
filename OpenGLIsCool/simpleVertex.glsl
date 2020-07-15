#version 420

uniform mat4 MVP;
//uniform vec4 objectColour;

//Vertices coming INTO the vertex shader
in vec3 vCol;
in vec3 vPos;
//in vec4 vNormal;

out vec3 color;
//out vec4 fNormal;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    color = vCol;
    //fNormal = vNormal;

    //color.rgb *= 0.001f; //translate the colour to black
    //color.rgb += objectColour.rgb;
}