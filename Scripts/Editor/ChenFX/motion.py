"""Source ClampVelocityModule adaptation; dampening is a documented 60 Hz envelope.

Unity native integration has not been recovered. Preserve the exported limit curve
and dampen value, integrate excess-speed relaxation at 60 Hz, and sample by particle
age in Niagara. Random lifetime/speed use a representative mean for this envelope.
"""
import math

def source_curve(v, t):
    mode = v.get('minMaxState', 0)
    scale = v.get('scalar', 1.)
    if mode == 0: return scale
    if mode == 3: return (scale + v.get('minScalar', scale)) * .5
    keys = v.get('maxCurve', {}).get('m_Curve', [])
    if not keys: return 0.
    if t <= keys[0]['time']: return keys[0]['value'] * scale
    if t >= keys[-1]['time']: return keys[-1]['value'] * scale
    for a, b in zip(keys, keys[1:]):
        if a['time'] <= t <= b['time']:
            dt = b['time'] - a['time']; x = (t-a['time']) / dt
            return scale * ((2*x**3-3*x*x+1)*a['value'] + (x**3-2*x*x+x)*dt*a['outSlope'] + (-2*x**3+3*x*x)*b['value'] + (x**3-x*x)*dt*b['inSlope'])

def motion_envelope(p):
    q = p['particle']; clamp = q['ClampVelocityModule']
    life = max(.01, source_curve(q['InitialModule']['startLifetime'], 0))
    initial = abs(source_curve(q['InitialModule']['startSpeed'], 0))
    if not clamp['enabled'] or initial <= 1e-6: return [(0., 1.), (1., 1.)]
    assert not clamp['separateAxis'], p['ue_name']
    assert source_curve(clamp['drag'], 0) == 0, p['ue_name']
    n = max(2, math.ceil(life * 60)); step = life/n
    damp = 1-(1-max(0., min(1., clamp['dampen'])))**(step*60)
    speed = initial; result = [(0., 1.)]
    for i in range(1, n+1):
        age = i/n; limit = max(0., source_curve(clamp['magnitude'], age))
        speed -= max(0., speed-limit) * damp
        result.append((age, speed/initial))
    return result

def envelope_value(points, age):
    for a,b in zip(points, points[1:]):
        if a[0] <= age <= b[0]: return a[1]+(b[1]-a[1])*(age-a[0])/(b[0]-a[0])
    return points[-1][1]
