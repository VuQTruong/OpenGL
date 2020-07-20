#version 420

//Vertices coming INTO the vertex shader
in vec4 vPos;         //in vec3 vPos;
in vec4 vNormal;
in vec4 vCol;         //in vec3 vCol;
in vec4 vUVx2;

out vec4 fColour;     //out vec3 fColour;
out vec4 fNormal;
out vec4 fUVx2;
out vec4 fVertWorldPos;

//uniform mat4 MVP;
//uniform vec4 objectColour;

uniform mat4 matModel;  //"model" or "world" matrix
uniform mat4 matView;   //"view" or "camera" or "eye" matrix
uniform mat4 matProj;   //"projection" matrix (ortographic or perspective)

void main()
{
    //gl_Position = MVP * vec4(vPos, 1.0);
	vec3 positionXYZ = vec3(vPos.xyz);

    //We have to multiply in the revert order PVM instead of MVP
    //mat4 MVP = matModel * matView * matProj;
    mat4 MVP = matProj * matView * matModel;

    gl_Position = MVP * vec4(positionXYZ, 1.0);

    fVertWorldPos = matModel *  vec4(positionXYZ, 1.0);
    
    //fNormal = vNormal;
    //We want to remove translation and scaling from the normals
    //and leaving only rotation
    fNormal = inverse(transpose(matModel)) * vec4(vNormal.xyz, 1.0f);

    fUVx2 = vUVx2;

    fColour = vCol;

    //fColour.rgb *= 0.001f;              //translate the colour to black
    //fColour.rgb += objectColour.rgb;
}