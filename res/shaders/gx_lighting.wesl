// I ripped this code from Aurora.

public struct GXLight {
    pos: vec3f,
    dir: vec3f,
    color: vec4f,
    cos_att: vec3f,
    dist_att: vec3f,
}

public const GXMaxLights: u32 = 8;

public struct GXLightingData {
    ambientColor: vec4f,
    lights: array<GXLight, GXMaxLights>,
    activeLights: u32,
}

fn calculateLighting(data: GXLightingData, pos: vec3f, normal: vec3f) -> vec4f {
    var lighting = data.ambientColor;
    for (var i = 0u; i < GXMaxLights; i++) {
        if ((data.activeLights & (1u << i)) == 0u) {
            continue;
        }

        var light = data.lights[i];
        var ldir = light.pos - pos;
        var dist2 = dot(ldir, ldir);
        var dist = sqrt(dist2);
        ldir = ldir / dist;

        // Calc spotlight attenuation
        var cosine = max(0.0, dot(ldir, light.dir));
        var cos_attn = dot(light.cos_att, vec3f(1.0, cosine, cosine * cosine));
        var dist_attn = dot(light.dist_att, vec3f(1.0, dist, dist2));
        var attn = max(0.0, cos_attn / dist_attn);

        var diff = max(0.0, dot(ldir, normal));
        lighting = lighting + (attn * diff * light.color);
    }
    return clamp(lighting, vec4f(0.0), vec4f(1.0));
}

public fn calculateRasColor(data: GXLightingData, pos: vec3f, normal: vec3f) -> vec4f {
    return calculateLighting(data, pos, normal);
}
