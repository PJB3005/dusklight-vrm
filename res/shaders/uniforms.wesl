import shaders::gx_lighting::GXLightingData;

public struct UniformGlobal {
    projView: mat4x4f
}

public struct UniformMaterial {
    color: vec4f,
}

public struct UniformObject {
    model: mat4x4f,
}

public struct UniformGXShading {
    // Lighting stuff
    lighting: GXLightingData,

    // TEV params used by link.
    kColor0: vec4f,
    color1: vec4f,
}
