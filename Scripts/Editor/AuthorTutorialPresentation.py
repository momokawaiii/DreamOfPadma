"""Create TASK-051 painted-map presentation assets in the UE 5.8 Python commandlet.

Usage: UnrealEditor-Cmd.exe DreamOfPadma.uproject -run=pythonscript
       -script=Scripts/Editor/AuthorTutorialPresentation.py -unattended -nop4
Run with an RHI when validating Custom HLSL compilation; do not use -nullrhi.
Existing assets are validated and preserved, including instance overrides. Reimport
an intentionally changed PNG through UE separately; this script never replaces art.
Report: Artifacts/TASK-051/tutorial-presentation.json.
"""

import hashlib
import json
import os
import struct
import unreal


PROJECT = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
SOURCE = os.path.join(PROJECT, "Content", "Padma", "UI", "SourceArt", "T_TutorialMap.png")
REPORT_PATH = os.path.join(PROJECT, "Artifacts", "TASK-051", "tutorial-presentation.json")
TEXTURE_PATH = "/Game/Padma/UI/Textures/T_TutorialMap"
MATERIAL_PATH = "/Game/Padma/UI/Materials/M_TutorialMap"
INSTANCE_PATH = "/Game/Padma/UI/Materials/MI_TutorialMap"
SCALARS = {"AnimationStrength": 0.7, "CloudStrength": 0.55, "FreezeAnimation": 0.0, "PreviewTime": 0.0}
SOURCE_HASH_TAG = "Padma.Tutorial.SourceSHA256"
GENERATOR_TAG = "Padma.Presentation.Generator"
GENERATOR_ID = "TASK-051.AuthorTutorialPresentation.v1"
CUSTOM_DESCRIPTION = "TASK-051 painted map: fixed UV / localized GPU atmosphere v1"
LIB = unreal.MaterialEditingLibrary
ASSETS = unreal.EditorAssetLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
REPORT = {
    "status": "started", "source": SOURCE, "created": [], "preserved": [], "repaired": [], "warnings": [],
    "texture": TEXTURE_PATH, "material": MATERIAL_PATH, "material_instance": INSTANCE_PATH,
    "scalar_defaults": SCALARS, "texture_parameter": "MapTexture",
    "freeze_recipe": {"FreezeAnimation": 1.0, "PreviewTime": 0.0},
    "reduced_motion_recipe": {"AnimationStrength": 0.0},
}

# The painting is always sampled at its original UV. Only localized lighting and
# haze vary: landmarks, routes, bridges and silhouettes never shift or wobble.
# Custom TextureObject input MapTexture supplies MapTextureSampler automatically.
SHADER = r"""
float2 uv = saturate(UV);
float3 painting = Texture2DSample(MapTexture, MapTextureSampler, uv).rgb;
float t = lerp(Time, PreviewTime, step(0.5, FreezeAnimation));
float strength = saturate(AnimationStrength);
float clouds = saturate(CloudStrength);

// Turquoise pixels in the upper-left lake and diagonal downstream river only.
float waterColor = smoothstep(0.018, 0.12, min(painting.g, painting.b) - painting.r);
waterColor *= 1.0 - smoothstep(0.18, 0.45, abs(painting.g - painting.b));
float lakeArea = (1.0 - smoothstep(0.42, 0.47, uv.x));
lakeArea *= smoothstep(0.27, 0.34, uv.y) * (1.0 - smoothstep(0.57, 0.63, uv.y));
float riverArea = smoothstep(0.25, 0.31, uv.x) * (1.0 - smoothstep(0.77, 0.84, uv.x));
riverArea *= smoothstep(0.51, 0.59, uv.y) * (1.0 - smoothstep(0.93, 0.98, uv.y));
float waterMask = waterColor * max(lakeArea, riverArea);
float waveA = 0.5 + 0.5 * sin(uv.x * 105.0 + uv.y * 170.0 - t * 0.85);
float waveB = 0.5 + 0.5 * sin(uv.x * 207.0 - uv.y * 138.0 + t * 0.47);
float ripples = pow(saturate(waveA), 10.0) * 0.7 + pow(saturate(waveB), 14.0) * 0.3;
float3 color = painting + waterMask * (float3(0.024, 0.073, 0.082) * ripples - float3(0.003, 0.008, 0.008));

// Slowly drifting procedural cloud banks over the painted mountain haze.
float luminance = dot(painting, float3(0.2126, 0.7152, 0.0722));
float chroma = max(painting.r, max(painting.g, painting.b)) - min(painting.r, min(painting.g, painting.b));
float mountainArea = smoothstep(0.04, 0.09, uv.y) * (1.0 - smoothstep(0.30, 0.40, uv.y));
mountainArea *= smoothstep(0.04, 0.12, uv.x) * (1.0 - smoothstep(0.72, 0.80, uv.x));
float paleRock = smoothstep(0.09, 0.24, luminance) * (1.0 - smoothstep(0.13, 0.30, chroma));
float2 p = uv * float2(10.0, 16.0) + float2(-t * 0.025, t * 0.007);
float cloudField = sin(p.x + sin(p.y * 0.7) * 1.6) + 0.5 * sin(p.x * 1.71 - p.y * 1.13) + 0.25 * cos(p.y * 2.17 + p.x * 0.61);
float cloudMask = mountainArea * paleRock * smoothstep(0.15, 1.10, cloudField);
float valleyArea = smoothstep(0.38, 0.46, uv.x) * (1.0 - smoothstep(0.66, 0.75, uv.x));
valleyArea *= smoothstep(0.19, 0.27, uv.y) * (1.0 - smoothstep(0.39, 0.46, uv.y));
float valleyField = 0.5 + 0.5 * sin(uv.x * 29.0 + uv.y * 37.0 - t * 0.12 + sin(uv.y * 18.0));
float hazeMask = valleyArea * smoothstep(0.30, 0.85, valleyField);
color = lerp(color, float3(0.48, 0.62, 0.69), saturate((cloudMask * 0.13 + hazeMask * 0.06) * clouds));

// Warm authored forge pixels, centered on the actual lower-right painting landmark.
float2 forgeUV = (uv - float2(0.765, 0.705)) / float2(0.047, 0.068);
float forgeArea = 1.0 - smoothstep(0.25, 1.0, length(forgeUV));
float forgeColor = smoothstep(0.035, 0.18, painting.r - max(painting.g, painting.b));
float pulse = 0.5 + 0.5 * sin(t * 1.35);
color += float3(0.15, 0.056, 0.009) * forgeArea * forgeColor * pulse;

// Zero strength returns the source painting exactly, independent of GPU time.
return lerp(painting, saturate(color), strength);
"""


def checked_asset(path, expected):
    if not ASSETS.does_asset_exist(path):
        return None
    asset = unreal.load_asset(path)
    if not isinstance(asset, expected):
        actual = asset.get_class().get_name() if asset else "unloadable asset"
        raise RuntimeError("Wrong existing asset class at {}: expected {}, got {}. Asset preserved.".format(path, expected.__name__, actual))
    return asset


def save(asset):
    if not ASSETS.save_loaded_asset(asset):
        raise RuntimeError("Failed to save " + asset.get_path_name())


def mark_generated(asset):
    ASSETS.set_metadata_tag(asset, GENERATOR_TAG, GENERATOR_ID)


def is_generated(asset):
    return ASSETS.get_metadata_tag(asset, GENERATOR_TAG) == GENERATOR_ID


def expression(material, cls, x, y):
    node = LIB.create_material_expression(material, cls, x, y)
    if not node:
        raise RuntimeError("Cannot create material expression " + cls.__name__)
    return node


def connect(source, destination, pin):
    if not LIB.connect_material_expressions(source, "", destination, pin):
        raise RuntimeError("Cannot connect Custom input " + pin)


def compile_material(material):
    # UE 5.8 returns compiler error strings, rather than a success bool.
    errors = LIB.recompile_material(material)
    if errors:
        raise RuntimeError("Tutorial material shader compilation failed: " + " | ".join(str(error) for error in errors))


def create_texture(source_hash):
    task = unreal.AssetImportTask()
    task.filename = SOURCE
    task.destination_path = "/Game/Padma/UI/Textures"
    task.destination_name = "T_TutorialMap"
    task.automated = True
    task.replace_existing = False
    task.save = False
    TOOLS.import_asset_tasks([task])
    texture = checked_asset(TEXTURE_PATH, unreal.Texture2D)
    if not texture:
        raise RuntimeError("PNG import did not create " + TEXTURE_PATH)
    for name, value in {
        "compression_settings": unreal.TextureCompressionSettings.TC_EDITOR_ICON,
        "lod_group": unreal.TextureGroup.TEXTUREGROUP_UI,
        "srgb": True, "max_texture_size": 0, "lod_bias": 0,
        "mip_gen_settings": unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS,
        "never_stream": True, "virtual_texture_streaming": False,
        "address_x": unreal.TextureAddress.TA_CLAMP, "address_y": unreal.TextureAddress.TA_CLAMP,
    }.items():
        texture.set_editor_property(name, value)
    ASSETS.set_metadata_tag(texture, SOURCE_HASH_TAG, source_hash)
    mark_generated(texture)
    save(texture)
    REPORT["created"].append(TEXTURE_PATH)
    return texture


def create_material(texture):
    material = TOOLS.create_asset("M_TutorialMap", "/Game/Padma/UI/Materials", unreal.Material, unreal.MaterialFactoryNew())
    if not material:
        raise RuntimeError("Cannot create " + MATERIAL_PATH)
    material.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
    material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    custom = expression(material, unreal.MaterialExpressionCustom, 0, 0)
    custom.set_editor_property("description", CUSTOM_DESCRIPTION)
    custom.set_editor_property("code", SHADER)
    custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT3)
    inputs = []
    for name in ("MapTexture", "UV", "Time", *SCALARS):
        item = unreal.CustomInput()
        item.set_editor_property("input_name", name)
        inputs.append(item)
    custom.set_editor_property("inputs", inputs)
    painting = expression(material, unreal.MaterialExpressionTextureObjectParameter, -480, -160)
    painting.set_editor_property("parameter_name", "MapTexture")
    painting.set_editor_property("texture", texture)
    painting.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
    connect(painting, custom, "MapTexture")
    uv = expression(material, unreal.MaterialExpressionTextureCoordinate, -480, -40)
    connect(uv, custom, "UV")
    time = expression(material, unreal.MaterialExpressionTime, -480, 80)
    time.set_editor_property("ignore_pause", False)
    connect(time, custom, "Time")
    for index, (name, default) in enumerate(SCALARS.items()):
        parameter = expression(material, unreal.MaterialExpressionScalarParameter, -480, 200 + index * 120)
        parameter.set_editor_property("parameter_name", name)
        parameter.set_editor_property("default_value", default)
        parameter.set_editor_property("group", "Tutorial atmosphere")
        parameter.set_editor_property("slider_min", 0.0)
        parameter.set_editor_property("slider_max", 120.0 if name == "PreviewTime" else 1.0)
        connect(parameter, custom, name)
    opacity = expression(material, unreal.MaterialExpressionConstant, 0, 200)
    opacity.set_editor_property("r", 1.0)
    if not LIB.connect_material_property(custom, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("Cannot connect UI Final Color")
    if not LIB.connect_material_property(opacity, "", unreal.MaterialProperty.MP_OPACITY):
        raise RuntimeError("Cannot connect UI Opacity")
    compile_material(material)
    mark_generated(material)
    save(material)
    REPORT["created"].append(MATERIAL_PATH)
    return material


def validate_material(material, texture):
    if material.get_editor_property("material_domain") != unreal.MaterialDomain.MD_UI:
        raise RuntimeError("Existing tutorial material is not UI-domain; graph preserved.")
    names = {str(name) for name in LIB.get_scalar_parameter_names(material)}
    missing = set(SCALARS) - names
    textures = {str(name) for name in LIB.get_texture_parameter_names(material)}
    if missing or "MapTexture" not in textures:
        raise RuntimeError("Tutorial material is missing required parameters: " + str(sorted(missing | ({"MapTexture"} - textures))))
    if LIB.get_material_default_texture_parameter_value(material, "MapTexture") != texture:
        raise RuntimeError("Tutorial material MapTexture points to other art; existing binding preserved.")
    if not LIB.get_material_property_input_node(material, unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("Tutorial material has no Final Color input; existing graph preserved.")
    if not any(isinstance(node, unreal.MaterialExpressionCustom) for node in LIB.get_material_expressions(material)):
        raise RuntimeError("Tutorial material has no Custom atmosphere node; existing graph preserved.")


def bind_instance_texture(instance, texture):
    # UE 5.8's setter returns false even after applying the value. Readback is the
    # success check (MaterialEditingLibrary.cpp: SetMaterialInstanceTextureParameterValue).
    LIB.set_material_instance_texture_parameter_value(instance, "MapTexture", texture)
    if LIB.get_material_instance_texture_parameter_value(instance, "MapTexture") != texture:
        raise RuntimeError("Cannot bind the tutorial painting to its material instance")


def run():
    if not os.path.isfile(SOURCE):
        raise RuntimeError("Missing tutorial painting source: " + SOURCE)
    with open(SOURCE, "rb") as stream:
        data = stream.read()
    if len(data) < 24 or data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        raise RuntimeError("Tutorial painting source is not a valid PNG header: " + SOURCE)
    width, height = struct.unpack(">II", data[16:24])
    if width <= 0 or height <= 0 or width < height:
        raise RuntimeError("Tutorial painting must have nonzero landscape dimensions.")
    source_hash = hashlib.sha256(data).hexdigest()
    REPORT["source_pixels"] = [width, height]
    REPORT["source_sha256"] = source_hash
    # Validate all existing classes before importing or creating any missing asset.
    texture = checked_asset(TEXTURE_PATH, unreal.Texture2D)
    material = checked_asset(MATERIAL_PATH, unreal.Material)
    instance = checked_asset(INSTANCE_PATH, unreal.MaterialInstanceConstant)
    for folder in ("/Game/Padma/UI/Textures", "/Game/Padma/UI/Materials"):
        if not ASSETS.does_directory_exist(folder) and not ASSETS.make_directory(folder):
            raise RuntimeError("Cannot create content directory " + folder)
    if texture:
        REPORT["preserved"].append(TEXTURE_PATH)
        if (not texture.get_editor_property("srgb") or texture.get_editor_property("lod_group") != unreal.TextureGroup.TEXTUREGROUP_UI
                or texture.get_editor_property("max_texture_size") != 0 or texture.get_editor_property("lod_bias") != 0):
            raise RuntimeError("Existing tutorial texture is not full-resolution sRGB/UserInterface; asset preserved.")
        imported_hash = ASSETS.get_metadata_tag(texture, SOURCE_HASH_TAG)
        if imported_hash and imported_hash != source_hash:
            REPORT["warnings"].append("Source PNG changed; existing art was preserved. Reimport intentionally through Unreal before visual acceptance.")
    else:
        texture = create_texture(source_hash)
    REPORT["texture_pixels"] = [texture.blueprint_get_size_x(), texture.blueprint_get_size_y()]
    if REPORT["texture_pixels"] != [width, height]:
        raise RuntimeError("Tutorial texture dimensions do not match the full-resolution source: " + str(REPORT["texture_pixels"]))
    if material:
        REPORT["preserved"].append(MATERIAL_PATH)
        validate_material(material, texture)
        compile_material(material)
        # The first v1 run saved this exact graph before the instance-setter
        # return-value failure. Recognize it without rebuilding any existing graph.
        if not ASSETS.get_metadata_tag(material, GENERATOR_TAG) and any(
                isinstance(node, unreal.MaterialExpressionCustom)
                and node.get_editor_property("description") == CUSTOM_DESCRIPTION
                and node.get_editor_property("code") == SHADER
                for node in LIB.get_material_expressions(material)):
            mark_generated(material)
            save(material)
    else:
        material = create_material(texture)
        validate_material(material, texture)
    if instance:
        REPORT["preserved"].append(INSTANCE_PATH)
        parent = instance.get_editor_property("parent")
        repaired = False
        if parent is None and is_generated(instance):
            LIB.set_material_instance_parent(instance, material)
            LIB.update_material_instance(instance)
            repaired = True
        elif parent != material:
            raise RuntimeError("Existing tutorial material instance has a different parent; overrides preserved.")
        binding = LIB.get_material_instance_texture_parameter_value(instance, "MapTexture")
        if binding is None and is_generated(instance):
            bind_instance_texture(instance, texture)
            repaired = True
        elif binding != texture:
            raise RuntimeError("Existing tutorial material instance overrides MapTexture with other art; override preserved.")
        if repaired:
            save(instance)
            REPORT["repaired"].append(INSTANCE_PATH)
    else:
        instance = TOOLS.create_asset("MI_TutorialMap", "/Game/Padma/UI/Materials", unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
        if not instance:
            raise RuntimeError("Cannot create " + INSTANCE_PATH)
        mark_generated(instance)
        LIB.set_material_instance_parent(instance, material)
        bind_instance_texture(instance, texture)
        save(instance)
        REPORT["created"].append(INSTANCE_PATH)
    REPORT["instance_scalar_values"] = {name: LIB.get_material_instance_scalar_parameter_value(instance, name) for name in SCALARS}
    REPORT["shader_sha256"] = hashlib.sha256(SHADER.encode("utf-8")).hexdigest()
    REPORT["status"] = "ready"
    for warning in REPORT["warnings"]:
        unreal.log_warning("[TASK-051] " + warning)
    unreal.log("[TASK-051] Painted map ready: " + INSTANCE_PATH)


try:
    run()
except Exception as error:
    REPORT["status"] = "failed"
    REPORT["error"] = str(error)
    unreal.log_error("[TASK-051] " + str(error))
    raise
finally:
    os.makedirs(os.path.dirname(REPORT_PATH), exist_ok=True)
    with open(REPORT_PATH, "w", encoding="utf-8") as stream:
        json.dump(REPORT, stream, ensure_ascii=False, indent=2)
        stream.write("\n")
