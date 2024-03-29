attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;

uniform mat4 WorldViewMat;
uniform mat4 ViewProjMat;
uniform mat4 WorldMat, ViewMat;
uniform float CamFar;
uniform vec3 CamPos;
uniform float Transform;

varying vec4 Color;

void main()
{
    vec4 vertex = (WorldViewMat * vec4(inVertexPosition, 1.0));
    float depth = length(vertex.xyz) / CamFar;

    float alpha = max(min(1.0, 8.0 - 10.0*depth), 0.0);
    Color.rgba = inVertexColor.bgra;
    Color.a *= alpha;
    //Color.rgb *= inVertexColor.w * alpha;

    //vertex = WorldMat * vec4(inVertexPosition + inVertexNormal * (depth * 0.08), 1.0);
    vertex = WorldMat * vec4(inVertexPosition, 1.0);

    //vertex.x = ((vertex.x - CamPos.x) / ((vertex.z - CamPos.z)*0.5 + 1.0)) + CamPos.x;

    depth = length((ViewMat * vertex).xyz) / CamFar;
    vertex.xyz += inVertexNormal * (depth * 0.08);

    gl_Position = ViewProjMat * vertex;
}
