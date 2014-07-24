attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec3 inVertexColor;

uniform mat4 WorldViewProjMat;
uniform mat4 WorldViewMat;
uniform float CamFar;
uniform vec3 BaseColor;
uniform float Transform;

varying vec3 Color;

void main()
{
    vec3 Vertex = (WorldViewMat * vec4(inVertexPosition, 1.0)).xyz;
    float depth = length(Vertex) / CamFar;

    float alpha = min(1.0, 8.0 - 10.0*depth);
    Color = inVertexColor * alpha;

    vec4 clipPos = WorldViewProjMat * vec4(inVertexPosition + inVertexNormal * (depth * 0.07 + 0.01), 1.0);
    clipPos.y += (1.0 - cos(clipPos.x / 3.14)) * Transform;
    gl_Position = clipPos;
}

