"""Create TASK-052 paper / ink / gold / liquid / portrait UI materials in UE 5.8.

Usage: UnrealEditor-Cmd.exe DreamOfPadma.uproject -run=pythonscript
       -script=Scripts/Editor/AuthorMotionUI.py -unattended -nop4
Use a real RHI, not -nullrhi, to validate Custom HLSL compilation.
New assets only. Repeated runs validate and preserve graphs and MI overrides.
Report: Artifacts/TASK-052/motion-ui.json. No gameplay or CPU tick is authored.
"""

import hashlib
import json
import os
import struct
import unreal


PROJECT = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
SOURCE = os.path.join(PROJECT, "Content", "Padma", "UI", "SourceArt", "T_HandParchment.png")
REPORT_PATH = os.path.join(PROJECT, "Artifacts", "TASK-052", "motion-ui.json")
TEXTURE_PATH = "/Game/Padma/UI/Textures/T_HandParchment"
PORTRAIT_TEXTURE_PATH = "/Game/Padma/UI/Textures/T_CardIllustrations"
MATERIAL_FOLDER = "/Game/Padma/UI/Materials"
GENERATOR_TAG = "Padma.MotionUI.Generator"
GENERATOR_ID = "TASK-052.AuthorMotionUI.v1"
SOURCE_HASH_TAG = "Padma.MotionUI.SourceSHA256"
LIB = unreal.MaterialEditingLibrary
ASSETS = unreal.EditorAssetLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
COMMON = {
    "RevealAmount": 1.0, "HoverAmount": 0.0, "AnimationStrength": 1.0,
    "FreezeAnimation": 0.0, "PreviewTime": 0.0,
}
REPORT = {
    "status": "started", "created": [], "preserved": [], "migrated": [], "warnings": [],
    "source": SOURCE, "texture": TEXTURE_PATH, "materials": {},
    "runtime_contract": {
        "RevealAmount": "0 hidden; 1 fully visible. Optional entry mask, default fully visible.",
        "HoverAmount": "0 resting; 1 hovered. Set on hover transitions, not per-frame phase.",
        "AnimationStrength": "0 removes time-driven movement and scan; 1 full motion.",
        "FreezeAnimation": "1 substitutes PreviewTime for GPU Time, for deterministic capture.",
        "PreviewTime": "Seconds used only when FreezeAnimation is 1.",
        "ScanAmount": "Gold scan intensity, multiplied by HoverAmount and AnimationStrength.",
        "Balance": "War strip: 0 all ruler red, 0.5 even, 1 all player teal.",
        "AspectRatio": "War strip width / height; default 12, set once for its geometry.",
        "WaveStrength": "War strip surface/seam-wave amplitude, 0 flat to 1 standard.",
        "PortraitTexture": "Portrait defaults to the existing card atlas; a per-portrait MID may bind separate art.",
        "PortraitUV": "UVMinX/Y=0, UVMaxX/Y=1 by default. Atlas portrait uses ((col+.01)/3,(row+.01)/3)..((col+.99)/3,(row+.99)/3); no second inset.",
        "PortraitReveal": "RevealAmount drives a slanted alpha wipe and narrow pale-gold edge. No Time-driven portrait motion or color shift.",
    },
}

# All RGB values below are linear UI colors. There is no normal, lighting or bevel.
# UV never displaces the paper sample. Reveal only changes alpha.
PREAMBLE = r"""
float2 uv = saturate(UV);
float t = lerp(Time, PreviewTime, step(0.5, FreezeAnimation));
float motion = saturate(AnimationStrength);
float hover = saturate(HoverAmount);
float grain = frac(sin(dot(floor(uv * float2(1723.0, 1327.0)), float2(12.9898, 78.233))) * 43758.5453) - 0.5;
float reveal = smoothstep(-0.035, 0.015, saturate(RevealAmount) * 1.08 - dot(uv, float2(0.86, 0.14)));
reveal *= step(0.0001, RevealAmount);
"""

PAPER = PREAMBLE + r"""
float3 paper = Texture2DSample(PaperTexture, PaperTextureSampler, uv).rgb;
float3 color = paper * (1.0 + grain * 0.009);
color = lerp(color, float3(0.94, 0.87, 0.70), hover * 0.035);
return float4(saturate(color), reveal);
"""

INK = PREAMBLE + r"""
float2 centered = uv * 2.0 - 1.0;
float vignette = saturate(dot(centered, centered) * 0.28);
float3 color = float3(0.0085, 0.020, 0.032) * (1.0 - vignette * 0.11 + grain * 0.045);
color += hover * float3(0.002, 0.004, 0.006);
float edgeDistance = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
float organic = 0.65 + 0.35 * sin(uv.x * 237.0 + sin(uv.y * 173.0) * 2.0);
float edge = (1.0 - smoothstep(0.0012, 0.0036, edgeDistance)) * organic * 0.28;
color = lerp(color, float3(0.26, 0.18, 0.077), edge);
return float4(saturate(color), reveal);
"""

GOLD = PREAMBLE + r"""
float3 paper = Texture2DSample(PaperTexture, PaperTextureSampler, uv).rgb;
float luminance = dot(paper, float3(0.2126, 0.7152, 0.0722));
float variance = clamp(luminance - 0.86, -0.09, 0.09);
float3 color = float3(0.73, 0.51, 0.205) + variance * float3(0.32, 0.26, 0.14);
color += grain * float3(0.009, 0.007, 0.003);
color += hover * float3(0.028, 0.019, 0.007);
float scanPosition = frac(t * 0.32) * 1.50 - 0.25;
float scanDistance = (uv.x + uv.y * 0.16 - scanPosition) / 0.085;
float scan = exp(-scanDistance * scanDistance) * hover * saturate(ScanAmount) * motion;
color += float3(0.12, 0.095, 0.045) * scan;
// Fine clipped corners only. The frame is drawn by Slate, not baked as a pillow.
float corner = min(min(uv.x + uv.y, 1.0 - uv.x + uv.y),
                   min(uv.x + 1.0 - uv.y, 2.0 - uv.x - uv.y));
float alpha = smoothstep(0.025, 0.030, corner) * reveal;
return float4(saturate(color), alpha);
"""

WAR = PREAMBLE + r"""
float aspect = max(1.0, AspectRatio);
float2 p = float2((uv.x - 0.5) * aspect, uv.y - 0.5);
float2 q = abs(p) - float2(max(0.10, aspect * 0.5 - 0.24), 0.15);
float distanceToStrip = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - 0.23;
float aa = max(fwidth(distanceToStrip), 0.0015);
float shape = 1.0 - smoothstep(-aa, aa, distanceToStrip);
float balance = saturate(Balance);
float ends = smoothstep(0.01, 0.08, balance) * (1.0 - smoothstep(0.92, 0.99, balance));
float wave = (sin(uv.y * 15.0 - t * 1.7) + 0.42 * sin(uv.y * 29.0 + t * 1.1)) * 0.010;
float seamPosition = balance + wave * saturate(WaveStrength) * motion * ends;
float playerSide = 1.0 - smoothstep(seamPosition - 0.004, seamPosition + 0.004, uv.x);
playerSide *= step(0.0001, balance);
playerSide = lerp(playerSide, 1.0, step(0.9999, balance));
float3 color = lerp(float3(0.38, 0.052, 0.036), float3(0.010, 0.205, 0.172), playerSide);
float flowA = sin(uv.x * 28.0 + uv.y * 8.0 - t * 1.5);
float flowB = sin(uv.x * 53.0 - uv.y * 13.0 + t * 0.8);
float ripple = (flowA * 0.035 + flowB * 0.018) * motion;
color *= 1.0 + ripple;
// Visible free surface across both halves, including when the center glyph hides the seam.
// A dark headspace and a subpixel foam edge keep this liquid, not a convex button bevel.
float surfaceWave = 0.038 * sin(uv.x * 22.0 - t * 1.45) + 0.015 * sin(uv.x * 47.0 + t * 0.83);
float surfaceY = 0.32 + surfaceWave * motion * saturate(WaveStrength);
float surfaceAA = max(fwidth(uv.y), 0.0015);
float liquidMask = smoothstep(-surfaceAA * 0.5, surfaceAA * 0.5, uv.y - surfaceY);
float bubbles = 0.0;
[unroll]
for (int i = 0; i < 5; ++i)
{
    float seed = frac(sin((float(i) + 1.0) * 12.731) * 43758.23);
    float cx = 0.07 + seed * 0.86;
    float cy = 0.84 - frac(t * (0.06 + seed * 0.04) + seed) * 0.50;
    float radius = 0.018 + frac(seed * 37.1) * 0.016;
    float2 bp = float2((uv.x - cx) * aspect, uv.y - cy);
    float ringDistance = abs(length(bp) - radius);
    bubbles += (1.0 - smoothstep(0.003, 0.009, ringDistance)) * 0.035;
}
color += bubbles * motion * lerp(float3(0.35, 0.13, 0.09), float3(0.15, 0.40, 0.34), playerSide);
float seam = exp(-abs(uv.x - seamPosition) * 850.0) * ends;
color += float3(0.34, 0.39, 0.35) * seam * 0.28;
color = lerp(float3(0.006, 0.018, 0.027), color, liquidMask);
float foam = 1.0 - smoothstep(0.007, 0.017 + surfaceAA * 0.5, abs(uv.y - surfaceY));
float3 foamColor = lerp(float3(0.58, 0.28, 0.18), float3(0.23, 0.57, 0.45), playerSide);
color = lerp(color, foamColor, foam * 0.40);
float edge = 1.0 - smoothstep(aa, aa + 0.012, abs(distanceToStrip));
color = lerp(color, float3(0.32, 0.35, 0.28), edge * 0.26);
color += hover * float3(0.006, 0.009, 0.008);
return float4(saturate(color), shape * reveal);
"""

PORTRAIT = PREAMBLE + r"""
float2 uvMinimum = saturate(float2(UVMinX, UVMinY));
float2 uvMaximum = max(uvMinimum, saturate(float2(UVMaxX, UVMaxY)));
float2 portraitUV = lerp(uvMinimum, uvMaximum, uv);
float4 portrait = Texture2DSample(PortraitTexture, PortraitTextureSampler, portraitUV);
float frontDistance = saturate(RevealAmount) * 1.08 - dot(uv, float2(0.86, 0.14));
float mask = smoothstep(-0.003, 0.006, frontDistance) * step(0.0001, RevealAmount);
float edge = (1.0 - smoothstep(0.002, 0.014, frontDistance)) * step(0.0, frontDistance);
edge *= step(0.0001, RevealAmount) * (1.0 - step(0.9999, RevealAmount));
float3 color = lerp(portrait.rgb, float3(0.91, 0.81, 0.59), edge * 0.42);
return float4(color, portrait.a * mask);
"""

SPECS = {
    "UIPaper": {"shader": PAPER, "scalars": dict(COMMON), "paper": True},
    "UIInkPanel": {"shader": INK, "scalars": dict(COMMON), "paper": False},
    "UIGoldAction": {"shader": GOLD, "scalars": dict(COMMON, ScanAmount=0.35), "paper": True},
    "UIWarLiquid": {"shader": WAR, "scalars": dict(COMMON, Balance=0.5, AspectRatio=12.0, WaveStrength=1.0), "paper": False},
    "UIPortraitReveal": {"shader": PORTRAIT, "scalars": dict(COMMON, UVMinX=0.0, UVMinY=0.0, UVMaxX=1.0, UVMaxY=1.0), "paper": False, "portrait": True},
}

# Only this exact already-generated TASK-052 shader may be upgraded in place.
# No other material or unknown Custom code is migrated.
PRIOR_SHADER_HASHES = {
    "UIWarLiquid": {"65e603823993b13c0eedf14cfeb4e280003ea64c740ddeab8bbd4e2fe0d3396f"},
}


def texture_parameter(spec):
    return "PortraitTexture" if spec.get("portrait") else "PaperTexture" if spec["paper"] else None


def checked_asset(path, expected):
    if not ASSETS.does_asset_exist(path):
        return None
    asset = unreal.load_asset(path)
    if not isinstance(asset, expected):
        raise RuntimeError("Wrong existing asset class at {}; existing asset preserved.".format(path))
    return asset


def mark(asset):
    ASSETS.set_metadata_tag(asset, GENERATOR_TAG, GENERATOR_ID)


def save(asset):
    if not ASSETS.save_loaded_asset(asset):
        raise RuntimeError("Cannot save " + asset.get_path_name())


def compile_material(material):
    # UE 5.8 returns compiler error strings, not a success boolean.
    errors = LIB.recompile_material(material)
    if errors:
        raise RuntimeError("Shader compilation failed for {}: {}".format(
            material.get_path_name(), " | ".join(str(error) for error in errors)))


def expression(material, cls, x, y):
    node = LIB.create_material_expression(material, cls, x, y)
    if not node:
        raise RuntimeError("Cannot create expression " + cls.__name__)
    return node


def connect(source, destination, pin):
    if not LIB.connect_material_expressions(source, "", destination, pin):
        raise RuntimeError("Cannot connect material input " + pin)


def create_texture(source_hash):
    task = unreal.AssetImportTask()
    task.filename = SOURCE
    task.destination_path = "/Game/Padma/UI/Textures"
    task.destination_name = "T_HandParchment"
    task.automated = True
    task.replace_existing = False
    task.save = False
    TOOLS.import_asset_tasks([task])
    texture = checked_asset(TEXTURE_PATH, unreal.Texture2D)
    if not texture:
        raise RuntimeError("Paper import failed: " + TEXTURE_PATH)
    for name, value in {
        "compression_settings": unreal.TextureCompressionSettings.TC_EDITOR_ICON,
        "lod_group": unreal.TextureGroup.TEXTUREGROUP_UI,
        "srgb": True, "max_texture_size": 0, "lod_bias": 0,
        "mip_gen_settings": unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS,
        "never_stream": True, "virtual_texture_streaming": False,
        "address_x": unreal.TextureAddress.TA_CLAMP, "address_y": unreal.TextureAddress.TA_CLAMP,
    }.items():
        texture.set_editor_property(name, value)
    mark(texture)
    ASSETS.set_metadata_tag(texture, SOURCE_HASH_TAG, source_hash)
    save(texture)
    REPORT["created"].append(TEXTURE_PATH)
    return texture


def validate_material(material, spec, texture):
    if material.get_editor_property("material_domain") != unreal.MaterialDomain.MD_UI:
        raise RuntimeError("Existing material must be UI-domain: " + material.get_path_name())
    if material.get_editor_property("blend_mode") != unreal.BlendMode.BLEND_TRANSLUCENT:
        raise RuntimeError("Existing material must use translucent alpha: " + material.get_path_name())
    names = {str(name) for name in LIB.get_scalar_parameter_names(material)}
    if set(spec["scalars"]) - names:
        raise RuntimeError("Missing scalar parameters in " + material.get_path_name())
    nodes = LIB.get_material_expressions(material)
    if not any(isinstance(node, unreal.MaterialExpressionCustom)
               and node.get_editor_property("code") == spec["shader"] for node in nodes):
        raise RuntimeError("Existing graph differs from this generator; preserved: " + material.get_path_name())
    for prop in (unreal.MaterialProperty.MP_EMISSIVE_COLOR, unreal.MaterialProperty.MP_OPACITY):
        if not LIB.get_material_property_input_node(material, prop):
            raise RuntimeError("Missing UI color or opacity output in " + material.get_path_name())
    parameter = texture_parameter(spec)
    if parameter and LIB.get_material_default_texture_parameter_value(material, parameter) != texture:
        raise RuntimeError("Existing " + parameter + " differs; binding preserved: " + material.get_path_name())


def migrate_known_shader(name, material, spec, texture):
    allowed = PRIOR_SHADER_HASHES.get(name)
    if not allowed:
        return False
    nodes = LIB.get_material_expressions(material)
    custom_nodes = [node for node in nodes if isinstance(node, unreal.MaterialExpressionCustom)]
    if len(custom_nodes) != 1:
        return False
    custom = custom_nodes[0]
    old_code = custom.get_editor_property("code")
    old_hash = hashlib.sha256(old_code.encode("utf-8")).hexdigest()
    if old_hash not in allowed:
        return False
    # A user graph, foreign asset or altered node set is never overwritten.
    expected_counts = {
        "MaterialExpressionCustom": 1, "MaterialExpressionTextureCoordinate": 1,
        "MaterialExpressionTime": 1, "MaterialExpressionScalarParameter": len(spec["scalars"]),
        "MaterialExpressionComponentMask": 2,
    }
    actual_counts = {}
    for node in nodes:
        key = node.get_class().get_name()
        actual_counts[key] = actual_counts.get(key, 0) + 1
    if (ASSETS.get_metadata_tag(material, GENERATOR_TAG) != GENERATOR_ID
            or custom.get_editor_property("description") != GENERATOR_ID + "." + name
            or actual_counts != expected_counts
            or {str(value) for value in LIB.get_scalar_parameter_names(material)} != set(spec["scalars"])):
        raise RuntimeError("Known old shader is inside an altered/foreign graph; preserved: " + material.get_path_name())
    validate_material(material, dict(spec, shader=old_code), texture)
    custom.set_editor_property("code", spec["shader"])
    try:
        validate_material(material, spec, texture)
        compile_material(material)
    except Exception:
        # Restore the prior in-memory code; a failing upgrade is never saved.
        custom.set_editor_property("code", old_code)
        LIB.recompile_material(material)
        raise
    save(material)
    REPORT["migrated"].append({
        "material": MATERIAL_FOLDER + "/M_" + name, "changed": "Custom.code only",
        "from_shader_sha256": old_hash,
        "to_shader_sha256": hashlib.sha256(spec["shader"].encode("utf-8")).hexdigest(),
    })
    return True


def create_material(name, spec, texture):
    material = TOOLS.create_asset("M_" + name, MATERIAL_FOLDER, unreal.Material, unreal.MaterialFactoryNew())
    if not material:
        raise RuntimeError("Cannot create M_" + name)
    material.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
    material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    custom = expression(material, unreal.MaterialExpressionCustom, 0, 0)
    custom.set_editor_property("description", GENERATOR_ID + "." + name)
    custom.set_editor_property("code", spec["shader"])
    custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT4)
    input_names = ["UV", "Time"] + list(spec["scalars"])
    parameter = texture_parameter(spec)
    if parameter:
        input_names.append(parameter)
    inputs = []
    for input_name in input_names:
        item = unreal.CustomInput()
        item.set_editor_property("input_name", input_name)
        inputs.append(item)
    custom.set_editor_property("inputs", inputs)
    uv = expression(material, unreal.MaterialExpressionTextureCoordinate, -500, -180)
    connect(uv, custom, "UV")
    time = expression(material, unreal.MaterialExpressionTime, -500, -60)
    time.set_editor_property("ignore_pause", True)
    connect(time, custom, "Time")
    for index, (parameter_name, default) in enumerate(spec["scalars"].items()):
        node = expression(material, unreal.MaterialExpressionScalarParameter, -500, 90 + index * 120)
        node.set_editor_property("parameter_name", parameter_name)
        node.set_editor_property("default_value", default)
        node.set_editor_property("group", "Motion UI")
        node.set_editor_property("slider_min", 1.0 if parameter_name == "AspectRatio" else 0.0)
        node.set_editor_property("slider_max", 120.0 if parameter_name == "PreviewTime" else 32.0 if parameter_name == "AspectRatio" else 1.0)
        connect(node, custom, parameter_name)
    if parameter:
        node = expression(material, unreal.MaterialExpressionTextureObjectParameter, -500, -330)
        node.set_editor_property("parameter_name", parameter)
        node.set_editor_property("texture", texture)
        node.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
        connect(node, custom, parameter)
    color = expression(material, unreal.MaterialExpressionComponentMask, 320, -60)
    opacity = expression(material, unreal.MaterialExpressionComponentMask, 320, 130)
    for channel in ("r", "g", "b", "a"):
        color.set_editor_property(channel, channel != "a")
        opacity.set_editor_property(channel, channel == "a")
    # ComponentMask's first pin has no exposed name in UE 5.8. The editor
    # library maps an empty ToInputName to GetInput(0), not the C++ field "Input".
    connect(custom, color, "")
    connect(custom, opacity, "")
    if not LIB.connect_material_property(color, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("Cannot bind UI Final Color for " + name)
    if not LIB.connect_material_property(opacity, "", unreal.MaterialProperty.MP_OPACITY):
        raise RuntimeError("Cannot bind UI Opacity for " + name)
    compile_material(material)
    mark(material)
    save(material)
    REPORT["created"].append(MATERIAL_FOLDER + "/M_" + name)
    return material


def get_instance(name, material, spec, texture):
    path = MATERIAL_FOLDER + "/MI_" + name
    parameter = texture_parameter(spec)
    instance = checked_asset(path, unreal.MaterialInstanceConstant)
    if instance:
        if instance.get_editor_property("parent") != material:
            raise RuntimeError("Existing instance has a different parent; preserved: " + path)
        if parameter and LIB.get_material_instance_texture_parameter_value(instance, parameter) != texture:
            raise RuntimeError("Existing instance points to different " + parameter + "; override preserved: " + path)
        REPORT["preserved"].append(path)
        return instance
    instance = TOOLS.create_asset("MI_" + name, MATERIAL_FOLDER,
                                 unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    if not instance:
        raise RuntimeError("Cannot create " + path)
    LIB.set_material_instance_parent(instance, material)
    if instance.get_editor_property("parent") != material:
        raise RuntimeError("Instance parent readback failed: " + path)
    if parameter:
        # UE 5.8 can return false after successful setters: trust readback only.
        LIB.set_material_instance_texture_parameter_value(instance, parameter, texture)
        if LIB.get_material_instance_texture_parameter_value(instance, parameter) != texture:
            raise RuntimeError(parameter + " readback failed: " + path)
    LIB.update_material_instance(instance)
    mark(instance)
    save(instance)
    REPORT["created"].append(path)
    return instance


def run():
    if not os.path.isfile(SOURCE):
        raise RuntimeError("Missing paper source: " + SOURCE)
    with open(SOURCE, "rb") as stream:
        data = stream.read()
    if len(data) < 24 or data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        raise RuntimeError("Paper source has no valid PNG header")
    width, height = struct.unpack(">II", data[16:24])
    if min(width, height) < 512:
        raise RuntimeError("Paper source must be at least 512 pixels on both axes")
    source_hash = hashlib.sha256(data).hexdigest()
    REPORT["source_pixels"] = [width, height]
    REPORT["source_sha256"] = source_hash
    # Preflight every destination before mutating the new owned asset set.
    texture = checked_asset(TEXTURE_PATH, unreal.Texture2D)
    portrait_texture = checked_asset(PORTRAIT_TEXTURE_PATH, unreal.Texture2D)
    if not portrait_texture:
        raise RuntimeError("Missing existing portrait atlas; preserve/import it with the card-art authoring workflow: " + PORTRAIT_TEXTURE_PATH)
    REPORT["portrait_default_texture"] = PORTRAIT_TEXTURE_PATH
    for name in SPECS:
        checked_asset(MATERIAL_FOLDER + "/M_" + name, unreal.Material)
        checked_asset(MATERIAL_FOLDER + "/MI_" + name, unreal.MaterialInstanceConstant)
    for folder in ("/Game/Padma/UI/Textures", MATERIAL_FOLDER):
        if not ASSETS.does_directory_exist(folder) and not ASSETS.make_directory(folder):
            raise RuntimeError("Cannot create content folder " + folder)
    if texture:
        REPORT["preserved"].append(TEXTURE_PATH)
        imported_hash = ASSETS.get_metadata_tag(texture, SOURCE_HASH_TAG)
        if imported_hash != source_hash:
            raise RuntimeError("Existing paper source hash differs or is absent; existing art preserved. Reimport intentionally through UE.")
        if (not texture.get_editor_property("srgb")
                or texture.get_editor_property("lod_group") != unreal.TextureGroup.TEXTUREGROUP_UI
                or texture.get_editor_property("max_texture_size") != 0
                or texture.get_editor_property("lod_bias") != 0):
            raise RuntimeError("Existing paper is not full-resolution sRGB UI texture; preserved.")
    else:
        texture = create_texture(source_hash)
    REPORT["texture_pixels"] = [texture.blueprint_get_size_x(), texture.blueprint_get_size_y()]
    if REPORT["texture_pixels"] != [width, height]:
        raise RuntimeError("Imported paper dimensions differ from source")
    for name, spec in SPECS.items():
        path = MATERIAL_FOLDER + "/M_" + name
        binding_texture = portrait_texture if spec.get("portrait") else texture
        material = checked_asset(path, unreal.Material)
        if material:
            migrated = migrate_known_shader(name, material, spec, binding_texture)
            validate_material(material, spec, binding_texture)
            if not migrated:
                compile_material(material)
                REPORT["preserved"].append(path)
        else:
            material = create_material(name, spec, binding_texture)
            validate_material(material, spec, binding_texture)
        instance = get_instance(name, material, spec, binding_texture)
        REPORT["materials"][name] = {
            "material": path, "instance": MATERIAL_FOLDER + "/MI_" + name,
            "domain": "UI", "blend": "Translucent", "compile_errors": [],
            "shader_sha256": hashlib.sha256(spec["shader"].encode("utf-8")).hexdigest(),
            "scalar_defaults": spec["scalars"],
            "effective_scalar_values": {
                parameter: LIB.get_material_instance_scalar_parameter_value(instance, parameter)
                for parameter in spec["scalars"]
            },
            "texture_parameter": texture_parameter(spec),
            "default_texture": PORTRAIT_TEXTURE_PATH if spec.get("portrait") else TEXTURE_PATH if spec["paper"] else None,
        }
    REPORT["status"] = "ready"
    unreal.log("[TASK-052] Five Motion UI materials compiled and validated.")


try:
    run()
except Exception as error:
    REPORT["status"] = "failed"
    REPORT["error"] = str(error)
    unreal.log_error("[TASK-052] " + str(error))
    raise
finally:
    os.makedirs(os.path.dirname(REPORT_PATH), exist_ok=True)
    with open(REPORT_PATH, "w", encoding="utf-8") as stream:
        json.dump(REPORT, stream, ensure_ascii=False, indent=2)
        stream.write("\n")
