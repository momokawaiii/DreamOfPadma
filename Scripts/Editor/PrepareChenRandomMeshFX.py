"""Prepare source-local Q blade meshes; run with the research Python, not UE.

Unlike the v2 mesh bake, random initial size/rotation remain particle attributes.
Only proven uniform prefab scales are supported by this TRS adapter.
"""
import json
import math
from pathlib import Path
import numpy as np

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'Artifacts/ChenQianyu/Actions/FX'
C = np.array([[-1., 0, 0], [0, 0, 1], [0, 1, 0]])

def prepare():
    data = json.loads((OUT / 'prepared_v2.json').read_text())
    effect = next(e for e in data if e['effect'] == 'P_chen_combo_skill')
    result = {}
    for index in (2, 6, 10):
        p = effect['particles'][index]
        matrix = np.array(p['unity_transform_matrix'])[:3, :3]
        scales = np.linalg.norm(matrix, axis=0)
        assert np.max(scales) - np.min(scales) < 1e-5
        rotation = C @ (matrix / scales) @ C.T
        angle = math.acos(np.clip((np.trace(rotation)-1)/2, -1, 1))
        if angle < 1e-6:
            axis = [0, 0, 1]
        else:
            vals, vectors = np.linalg.eig(rotation)
            axis = np.real(vectors[:, np.argmin(abs(vals-1))])
            skew = np.array([rotation[2,1]-rotation[1,2], rotation[0,2]-rotation[2,0], rotation[1,0]-rotation[0,1]])
            if np.dot(axis, skew) < 0: axis = -axis
            axis = axis.tolist()
        hx = f'{p["mesh"]["PathID"] & ((1 << 64)-1):016X}'
        source = next((OUT / 'objects/Mesh').glob('*_p'+hx+'.json'))
        mesh = json.loads(source.read_text(encoding='utf-8-sig'))
        verts = (C @ np.array(mesh['m_Vertices']).reshape(-1, 3).T).T * 100
        uv = np.array(mesh['m_UV0']).reshape(len(verts), -1)[:, :2]
        indices = np.array(mesh['m_Indices']).reshape(-1, 3)
        name = 'SM_'+p['ue_name']+'_Source'
        lines = ['o SourceLocal'] + ['v '+' '.join(map(str, v)) for v in verts]
        lines += ['vt '+' '.join(map(str, v)) for v in uv]
        lines += ['f '+' '.join(f'{n+1}/{n+1}' for n in face) for face in indices]
        (OUT / 'SourceAssetsV2' / (name+'.obj')).write_text('\n'.join(lines))
        result[p['ue_name']] = dict(mesh=name, prefab_axis=axis,
            prefab_angle=math.degrees(angle), prefab_scale=float(scales[0]), source={k:p['source'][k] for k in ['sourceFile','pathId','rawDataSha256']})
    (OUT / 'mesh-spawn.json').write_text(json.dumps(result, indent=2))
    return result

if __name__ == '__main__': print(list(prepare()))
