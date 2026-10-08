import shaders::uniforms::{UniformGlobal, UniformMaterial, UniformObject, UniformGXShading};
@if (GX_SHADING)
import shaders::gx_lighting::calculateRasColor;

@group(0) @binding(0) var<uniform> global : UniformGlobal;

@group(1) @binding(0) var<uniform> material : UniformMaterial;
@group(1) @binding(1) var tex0_t : texture_2d<f32>;
@group(1) @binding(2) var tex0_s : sampler;

@group(2) @binding(0) var<uniform> object : UniformObject;
@if (SKINNED)
@group(2) @binding(1) var<storage, read> pose : array<mat4x4f>;
@if (GX_SHADING)
@group(2) @binding(2) var<uniform> gxShading : UniformGXShading;

struct VsInput {
    @location(0) position: vec3f,
    @location(1) texCoord: vec2f,
    @if (NORMALS)
    @location(2) normal: vec3f,

    @if (SKINNED)
    @location(6) joints: vec4u,
    @if (SKINNED)
    @location(7) weights: vec4f,
}

struct VsOutput {
    @builtin(position) position: vec4f,
    @location(0) texCoord: vec2f,

    @if (GX_SHADING)
    @location(1) rasColor: vec4f,

    @if (NORMALS)
    @location(2) normal: vec3f,
}

@if (SKINNED)
fn calcSkinnedMatrix(joints: vec4u, weights: vec4f) -> mat4x4f {
    return pose[joints.x] * weights.x + pose[joints.y] * weights.y + pose[joints.z] * weights.z + pose[joints.w] * weights.w;
}

fn transformNormals(attr: vec3f, matrix: mat4x4f) -> vec3f {
    let normalMatrix = mat3x3(matrix[0].xyz, matrix[1].xyz, matrix[2].xyz);
    return normalMatrix * attr;
}

@vertex
fn vs_main(input: VsInput) -> VsOutput {
    @if (SKINNED)
    let modelMtx = calcSkinnedMatrix(input.joints, input.weights);
    @else
    let modelMtx = object.model;

    let worldPos = modelMtx * vec4f(input.position, 1.0);
    let pos = global.projView * worldPos;

    @if (NORMALS)
    let normals = transformNormals(input.normal, modelMtx);

    var output: VsOutput;
    output.position = pos;
    output.texCoord = input.texCoord;

    @if (GX_SHADING)
    output.rasColor = calculateRasColor(gxShading.lighting, worldPos.xyz, normalize(normals));

    @if (NORMALS)
    output.normal = normals;

    return output;
}

@fragment
fn fs_main(input: VsOutput) -> @location(0) vec4f {
    let texVal = textureSample(tex0_t, tex0_s, input.texCoord);

    var tev = gxShading.kColor0 * (vec4f(1.0) - input.rasColor) + texVal * input.rasColor;
    tev += gxShading.color1;
    tev *= 4.0;
    tev *= material.color;

    return clamp(tev, vec4f(0.0), vec4f(1.0));

    // return texVal * material.color;
    // return input.rasColor;
    // return vec4f(normalize(input.normal), 1.0);
}
