"""Author the TASK-050 native landscape sample meshes/materials and activate its visual theme.

Run after the Editor build with -run=pythonscript -script=<this file> -unattended.
Only generated ArtSample assets and the existing playable theme's cosmetic references change.
The map template, generated graph, node positions, catalog gameplay data and levels stay intact.
"""
import json
import math
import os
import random
import unreal

ROOT = "/Game/Padma/World/ArtSample"
THEME_PATH = "/Game/Padma/World/Maps/Presentation/DA_FirstRegion_Theme"
PROJECT = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
OUTPUT = os.path.join(PROJECT, "Artifacts", "TASK-050", "MeshSources")
os.makedirs(OUTPUT, exist_ok=True)
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
REPORT = {"meshes": [], "materials": [], "node_models": [], "theme": THEME_PATH}


def save(asset):
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset):
        raise RuntimeError("Cannot save sample asset: " + asset.get_path_name())


def material(name, color=None, water=False):
    path = ROOT + "/Materials/" + name
    result = unreal.load_asset(path)
    if result and not isinstance(result, unreal.Material):
        raise RuntimeError("Sample material path occupied by another asset: " + path)
    if not result:
        result = TOOLS.create_asset(name, ROOT + "/Materials", unreal.Material, unreal.MaterialFactoryNew())
    if not result:
        raise RuntimeError("Cannot create sample material: " + path)
    unreal.MaterialEditingLibrary.delete_all_material_expressions(result)
    result.set_editor_property("used_with_instanced_static_meshes", True)
    if color is None:
        expression = unreal.MaterialEditingLibrary.create_material_expression(result, unreal.MaterialExpressionVertexColor)
    else:
        expression = unreal.MaterialEditingLibrary.create_material_expression(result, unreal.MaterialExpressionConstant3Vector)
        expression.set_editor_property("constant", unreal.LinearColor(*color, 1))
    if not unreal.MaterialEditingLibrary.connect_material_property(expression, "", unreal.MaterialProperty.MP_BASE_COLOR):
        raise RuntimeError("Cannot connect sample base color: " + path)
    roughness = unreal.MaterialEditingLibrary.create_material_expression(result, unreal.MaterialExpressionConstant)
    roughness.set_editor_property("r", .25 if water else .86)
    unreal.MaterialEditingLibrary.connect_material_property(roughness, "", unreal.MaterialProperty.MP_ROUGHNESS)
    # A little ambient fill retains shaded detail without washing out directional terrain relief.
    fill = unreal.MaterialEditingLibrary.create_material_expression(result, unreal.MaterialExpressionMultiply)
    fill.set_editor_property("const_b", .035 if water else .065)
    unreal.MaterialEditingLibrary.connect_material_expressions(expression, "", fill, "A")
    unreal.MaterialEditingLibrary.connect_material_property(fill, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    unreal.MaterialEditingLibrary.recompile_material(result)
    save(result)
    REPORT["materials"].append(path)
    return result


PALETTE = {
    "bark": (.18, .095, .035), "bark_light": (.3, .18, .065),
    "leaf_deep": (.038, .11, .028), "leaf": (.075, .19, .04),
    "leaf_sun": (.16, .28, .055), "pine": (.03, .115, .075),
    "pine_sun": (.085, .21, .105), "rock": (.18, .215, .22),
    "rock_light": (.32, .365, .35), "stone": (.62, .53, .34),
    "plaster": (.79, .7, .46), "roof": (.12, .255, .22),
    "roof_light": (.21, .36, .26), "gold": (.69, .43, .12),
    "door": (.085, .055, .033), "cloth": (.62, .24, .075),
}
MATERIALS = {key: material("M_Sample_" + key, color) for key, color in PALETTE.items()}
GROUND = material("M_Sample_Landscape")
WATER = material("M_Sample_River", water=True)


class Mesh:
    """Small authored mesh builder. OBJ source is generated locally, never downloaded."""
    def __init__(self):
        self.vertices = []
        self.faces = []

    def vertex(self, point):
        self.vertices.append(point)
        return len(self.vertices)

    def face(self, points, surface):
        ids = [self.vertex(point) for point in points]
        for index in range(1, len(ids) - 1):
            self.faces.append((surface, (ids[0], ids[index], ids[index + 1])))

    def box(self, center, half, surface):
        x, y, z = center
        a, b, c = half
        p = [(x-a,y-b,z-c),(x+a,y-b,z-c),(x+a,y+b,z-c),(x-a,y+b,z-c),
             (x-a,y-b,z+c),(x+a,y-b,z+c),(x+a,y+b,z+c),(x-a,y+b,z+c)]
        for indices in ((0,3,2,1),(4,5,6,7),(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)):
            self.face([p[i] for i in indices], surface)

    def branch(self, start, end, radius, top_radius, surface, sides=9):
        dx, dy, dz = (end[i]-start[i] for i in range(3))
        length = math.sqrt(dx*dx+dy*dy+dz*dz)
        direction = (dx/length, dy/length, dz/length)
        tangent = (direction[2], 0, -direction[0])
        tl = math.sqrt(sum(v*v for v in tangent))
        tangent = tuple(v/tl for v in tangent)
        bitangent = (direction[1]*tangent[2]-direction[2]*tangent[1],
                     direction[2]*tangent[0]-direction[0]*tangent[2],
                     direction[0]*tangent[1]-direction[1]*tangent[0])
        rings = []
        for center, r in ((start, radius), (end, top_radius)):
            rings.append([tuple(center[j]+r*(math.cos(i*math.tau/sides)*tangent[j]+math.sin(i*math.tau/sides)*bitangent[j])
                                for j in range(3)) for i in range(sides)])
        for i in range(sides):
            j = (i+1) % sides
            self.face([rings[0][i], rings[0][j], rings[1][j], rings[1][i]], surface)
        self.face(rings[1], surface)
        self.face(list(reversed(rings[0])), surface)

    def crown(self, center, scale, seed, surfaces, rings=6, sides=11):
        rng = random.Random(seed)
        points = []
        for row in range(rings+1):
            latitude = -math.pi/2 + math.pi*row/rings
            ring = []
            for column in range(sides):
                longitude = math.tau*column/sides + (row % 2)*.13
                bump = rng.uniform(.88, 1.12)
                ring.append((center[0]+math.cos(latitude)*math.cos(longitude)*scale[0]*bump,
                             center[1]+math.cos(latitude)*math.sin(longitude)*scale[1]*bump,
                             center[2]+math.sin(latitude)*scale[2]*bump))
            points.append(ring)
        for row in range(rings):
            for column in range(sides):
                next_column = (column+1) % sides
                surface = surfaces[min(len(surfaces)-1, (row*len(surfaces))//rings)]
                if row == 0:
                    self.face([(center[0],center[1],center[2]-scale[2]),points[1][next_column],points[1][column]],surface)
                elif row == rings-1:
                    self.face([points[row][column],points[row][next_column],(center[0],center[1],center[2]+scale[2])],surface)
                else:
                    self.face([points[row][column],points[row][next_column],points[row+1][next_column],points[row+1][column]],surface)

    def roof(self, center, radius, height, surface="roof", sides=4):
        # Three pitched tiers and outward lifted eaves form a recognisable pagoda silhouette.
        x, y, z = center
        profiles = [(radius, z+4), (radius*.84, z), (radius*.28, z+height*.78), (radius*.06,z+height)]
        rings = [[(x+math.cos(math.tau*i/sides+math.pi/4)*r,
                   y+math.sin(math.tau*i/sides+math.pi/4)*r,h) for i in range(sides)] for r,h in profiles]
        for row in range(len(rings)-1):
            for i in range(sides):
                j = (i+1) % sides
                self.face([rings[row][i],rings[row][j],rings[row+1][j],rings[row+1][i]],
                          "roof_light" if row == 0 else surface)
        self.face(list(reversed(rings[0])), "bark")

    def write_source(self, name):
        source = os.path.join(OUTPUT, name + ".obj")
        with open(os.path.join(OUTPUT, name + ".mtl"), "w", encoding="utf-8") as stream:
            for key, color in PALETTE.items():
                stream.write("newmtl %s\nKd %.6f %.6f %.6f\n\n" % (key, *color))
        with open(source, "w", encoding="utf-8") as stream:
            stream.write("# Dream of Padma project-authored landscape sample; centimeters, Z up\n")
            stream.write("mtllib " + name + ".mtl\n")
            stream.write("o " + name + "\n")
            for point in self.vertices:
                stream.write("v %.6f %.6f %.6f\n" % point)
            # UE 5.8 may use Interchange for reimport even when FbxFactory is requested.
            # Its OBJ translator expects a valid UV index on every face corner.
            # Project each triangle onto its dominant plane: a single XY projection
            # collapses vertical building walls and produces degenerate tangents.
            for surface, indices in self.faces:
                points = [self.vertices[index - 1] for index in indices]
                a = [points[1][axis] - points[0][axis] for axis in range(3)]
                b = [points[2][axis] - points[0][axis] for axis in range(3)]
                normal = (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])
                if max(abs(value) for value in normal) < 1.e-8:
                    raise RuntimeError("Degenerate sample triangle: " + name)
                dominant = max(range(3), key=lambda axis: abs(normal[axis]))
                u, v = ((1, 2), (2, 0), (0, 1))[dominant]
                for point in points:
                    stream.write("vt %.9f %.9f\n" % (point[u]/256., point[v]/256.))
            previous = None
            for face_index, (surface, indices) in enumerate(self.faces):
                if surface != previous:
                    stream.write("usemtl " + surface + "\n")
                    previous = surface
                stream.write("f " + " ".join("%d/%d" % (index, face_index*3+corner+1)
                             for corner, index in enumerate(indices)) + "\n")
        return source

    def asset(self, name):
        source = self.write_source(name)
        options = unreal.FbxImportUI()
        options.set_editor_property("import_mesh", True)
        options.set_editor_property("import_materials", False)
        options.set_editor_property("import_textures", False)
        options.set_editor_property("import_as_skeletal", False)
        options.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_STATIC_MESH)
        options.set_editor_property("automated_import_should_detect_type", False)
        mesh_options = options.get_editor_property("static_mesh_import_data")
        mesh_options.set_editor_property("combine_meshes", True)
        mesh_options.set_editor_property("auto_generate_collision", False)
        mesh_options.set_editor_property("convert_scene", False)
        mesh_options.set_editor_property("convert_scene_unit", False)
        mesh_options.set_editor_property("normal_import_method", unreal.FBXNormalImportMethod.FBXNIM_COMPUTE_NORMALS)
        task = unreal.AssetImportTask()
        task.set_editor_property("filename", source)
        task.set_editor_property("destination_path", ROOT + "/Meshes")
        task.set_editor_property("destination_name", name)
        task.set_editor_property("automated", True)
        task.set_editor_property("replace_existing", True)
        task.set_editor_property("replace_existing_settings", True)
        task.set_editor_property("save", False)
        task.set_editor_property("options", options)
        task.set_editor_property("factory", unreal.FbxFactory())
        TOOLS.import_asset_tasks([task])
        result = unreal.load_asset(ROOT + "/Meshes/" + name)
        if not isinstance(result, unreal.StaticMesh):
            raise RuntimeError("Sample OBJ import failed: " + source)
        slots = result.get_editor_property("static_materials")
        for index, slot in enumerate(slots):
            key = str(slot.get_editor_property("imported_material_slot_name"))
            if key not in MATERIALS:
                key = str(slot.get_editor_property("material_slot_name"))
            if key not in MATERIALS:
                raise RuntimeError("Unknown OBJ material slot %s on %s" % (key, name))
            result.set_material(index, MATERIALS[key])
        save(result)
        bounds = result.get_bounds()
        expected = [max(p[i] for p in self.vertices)-min(p[i] for p in self.vertices) for i in range(3)]
        actual = [bounds.box_extent.x*2, bounds.box_extent.y*2, bounds.box_extent.z*2]
        if any(abs(actual[i]-expected[i]) > 1. for i in range(3)):
            raise RuntimeError("OBJ axes/units mismatch for %s: expected %s actual %s" % (name, expected, actual))
        REPORT["meshes"].append({"asset": result.get_path_name(), "triangles":len(self.faces),
                                  "material_slots":len(slots), "bounds":actual})
        return result


def broadleaf(seed, autumn=False):
    mesh = Mesh()
    mesh.branch((0,0,0),(3,-2,73),5.5,2.7,"bark")
    rng = random.Random(seed)
    for index in range(7):
        angle = index*math.tau/7 + .2
        r = 24 if index < 6 else 0
        center = (math.cos(angle)*r, math.sin(angle)*r, 82+rng.uniform(-10,18))
        mesh.branch((1,0,42),center,2.4,1,"bark_light")
        mesh.crown(center,(23,22,27),seed*31+index,("leaf_deep","leaf","leaf_sun"))
    return mesh


def pine(seed):
    mesh = Mesh()
    mesh.branch((0,0,0),(1,0,138),4.3,1,"bark")
    rng = random.Random(seed)
    for level in range(7):
        z = 36 + level*14
        radius = 32-level*3.5
        for side in range(5):
            angle = side*math.tau/5+level*.63
            end = (math.cos(angle)*radius*.7,math.sin(angle)*radius*.7,z+4)
            mesh.branch((0,0,z-7),end,1.2,.4,"bark")
            mesh.crown(end,(radius*.48,radius*.48,15-level*.6),rng.randrange(99999),("pine","pine","pine_sun"),4,7)
    return mesh


def boulders():
    mesh = Mesh()
    for index, (center, scale) in enumerate((((0,0,17),(27,21,23)),((22,12,9),(16,18,13)),((-18,15,5),(13,12,8)))):
        mesh.crown(center,scale,101+index,("rock","rock","rock_light"),4,7)
    # All sample meshes share a ground-level pivot; PCG does not need per-mesh offsets.
    minimum = min(p[2] for p in mesh.vertices)
    mesh.vertices = [(x,y,z-minimum) for x,y,z in mesh.vertices]
    return mesh


def pavilion(fortress=False):
    mesh = Mesh()
    mesh.box((0,0,5),(49,42,5),"stone")
    mesh.box((0,0,12),(42,35,3),"gold")
    for x in (-27,27):
        for y in (-22,22):
            mesh.branch((x,y,15),(x,y,56),3,3,"bark_light")
            mesh.box((x,y,17),(5,5,3),"stone")
    mesh.box((0,0,34),(22,17,19),"plaster")
    mesh.box((-22.6,0,28),(1,8,12),"door")
    for y in (-11,11):
        mesh.box((0,y,57),(34,3,3),"gold")
    mesh.roof((0,0,57),64,28)
    mesh.box((0,0,87),(14,12,10),"plaster")
    mesh.roof((0,0,98),35,21)
    mesh.branch((0,0,116),(0,0,140),2.5,.5,"gold")
    if fortress:
        for y in (-56,56):
            mesh.box((13,y,21),(17,14,21),"stone")
            mesh.roof((13,y,44),29,19)
            mesh.box((35,y*.5,12),(5,27,12),"stone")
    else:
        for x in (-40,-30,-20):
            mesh.box((x,-42,3),(5,7,3),"stone")
    return mesh


def village():
    mesh = Mesh()
    for x,y,r in ((-20,-17,22),(24,18,25),(-19,28,17)):
        mesh.box((x,y,14),(r*.65,r*.6,14),"plaster")
        mesh.roof((x,y,29),r*1.4,19)
        mesh.box((x-r*.65-.5,y,9),(1,4,9),"door")
    mesh.branch((35,-30,0),(35,-30,42),2,2,"bark")
    mesh.box((24,-30,38),(12,1,6),"cloth")
    return mesh


TREES = [broadleaf(17).asset("SM_Sample_Broadleaf_A"),broadleaf(43).asset("SM_Sample_Broadleaf_B"),
         pine(67).asset("SM_Sample_Cedar"),boulders().asset("SM_Sample_Boulders")]
BUILDINGS = {"home":pavilion(True).asset("SM_Sample_Citadel"),
             "story":pavilion().asset("SM_Sample_Shrine"),
             "village":village().asset("SM_Sample_Village")}


def model(name, mesh):
    folder = ROOT + "/Models"
    result = unreal.load_asset(folder + "/" + name)
    if not result:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.PadmaModelDefinition)
        result = TOOLS.create_asset(name, folder, unreal.PadmaModelDefinition, factory)
    if not isinstance(result, unreal.PadmaModelDefinition):
        raise RuntimeError("Sample model path has wrong class: " + name)
    result.set_editor_property("visual_definition_id", "landscape.sample." + name)
    result.set_editor_property("display_name", "山河样例 · " + name)
    result.set_editor_property("static_model", mesh)
    result.set_editor_property("model_transform", unreal.Transform(scale=unreal.Vector(1,1,1)))
    save(result)
    REPORT["node_models"].append(result.get_path_name())
    return result


MODELS = {key:model("DA_Sample_"+key,mesh) for key,mesh in BUILDINGS.items()}
theme = unreal.load_asset(THEME_PATH)
if not isinstance(theme, unreal.PadmaMapVisualTheme):
    raise RuntimeError("Run AuthorWorldMaps.py first; missing existing first-region visual theme")
theme.set_editor_property("vertex_color_material", GROUND)
theme.set_editor_property("water_material", WATER)
theme.set_editor_property("field_margin", 520.)
theme.set_editor_property("surface_grid_size", 28.)
theme.set_editor_property("hill_relief", 82.)
theme.set_editor_property("mountain_relief", 330.)
theme.set_editor_property("show_river", True)
theme.set_editor_property("decoration_meshes", TREES)
theme.set_editor_property("decoration_material", None)
theme.set_editor_property("decoration_count", 500)
theme.set_editor_property("decoration_scale_min", .55)
theme.set_editor_property("decoration_scale_max", .85)
theme.set_editor_property("decoration_max_slope_degrees", 30.)
terrains = list(theme.get_editor_property("terrain_visuals"))
colors = {"城镇":(.24,.31,.12),"平原":(.18,.30,.075),"林地":(.105,.235,.055),
          "丘陵":(.245,.30,.105),"山地":(.25,.29,.19)}
for terrain in terrains:
    identity = str(terrain.get_editor_property("terrain_id"))
    if identity in colors:
        terrain.set_editor_property("color", unreal.LinearColor(*colors[identity],1))
theme.set_editor_property("terrain_visuals", terrains)
bindings = list(theme.get_editor_property("node_bindings"))
for binding in bindings:
    identity = str(binding.get_editor_property("node_id"))
    if identity in ("home","boss","story","forge"):
        binding.set_editor_property("building_model", MODELS["home" if identity in ("home","boss") else "story" if identity=="story" else "village"])
theme.set_editor_property("node_bindings", bindings)
save(theme)
with open(os.path.join(PROJECT,"Artifacts","TASK-050","landscape-assets.json"),"w",encoding="utf-8") as stream:
    json.dump(REPORT,stream,ensure_ascii=False,indent=2)
unreal.log("[TASK-050] Landscape sample authored: %d meshes, %d materials, %d landmark models; theme=%s" %
           (len(REPORT["meshes"]),len(REPORT["materials"]),len(REPORT["node_models"]),THEME_PATH))
