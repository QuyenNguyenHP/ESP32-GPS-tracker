"""Parametric fit-prototype, millimetres. Requires cadquery and trimesh.
Run from any directory: python build_enclosure.py
Component envelopes are provisional; see ../README.md.
"""
from pathlib import Path
import json
import cadquery as cq
import trimesh
OUT = Path(__file__).resolve().parent
L, W, H = 104.0, 50.0, 22.0
WALL, FLOOR, LID = 2.0, 2.0, 2.0
SCREWS = [(5,5),(99,5),(5,45),(99,45)]
# Bounds x,y,z, length,width,height; USB exits the x=0 end.
ENVELOPES = {
    'ESP32S3': (4,16.1,5,21,17.8,4),
    'GNSS_stack_allowance': (4,16,10,21,18,8),
    'battery_reserved_not_selected': (32,10,3,27,30,8),
    'GNSS_antenna_assumed': (66,9,4,32,32,8),
}
def box(x,y,z,dx,dy,dz):
    return cq.Workplane('XY').box(dx,dy,dz,centered=False).translate((x,y,z))
def rounded(x,y,z,dx,dy,dz,r):
    return box(x,y,z,dx,dy,dz).edges('|Z').fillet(r)
def cylinder(x,y,z,r,h):
    return cq.Workplane('XY').circle(r).extrude(h).translate((x,y,z))
base = rounded(0,0,0,L,W,H,4).cut(rounded(WALL,WALL,FLOOR,L-2*WALL,W-2*WALL,H,2))
for x,y in SCREWS:
    base = base.union(cylinder(x,y,0,3.5,H)).cut(cylinder(x,y,4,1.2,H))
# Flat insulating-tape platform avoids guessing PCB mounting-hole positions.
base = base.union(box(4,16,2,22,18,2))
# Low antenna locators, with cable side left open. Antenna attaches via foam tape.
for bounds in [(65,8,2,34,1,3),(65,41,2,34,1,3),(98,8,2,1,34,3)]:
    base = base.union(box(*bounds))
# Battery locator strips; no pressure clips on the pouch.
for bounds in [(31,9,2,29,1,2),(31,40,2,29,1,2)]:
    base = base.union(box(*bounds))
# Oversized rectangular USB access opening; roof bridges 14 mm.
base = base.cut(box(-1,18,4.5,5,14,10))
# Lid exports exterior-down at z=0 for printing. Four M2.5 clearance holes.
lid = rounded(0,0,0,L,W,LID,4)
for x,y in SCREWS:
    lid = lid.cut(cylinder(x,y,-1,1.4,4))
# Four loose alignment tabs: 0.4 mm gap to the inside of the base.
for bounds in [(15,2.4,2,12,1.2,2),(15,46.4,2,12,1.2,2),
               (78,2.4,2,12,1.2,2),(78,46.4,2,12,1.2,2)]:
    lid = lid.union(box(*bounds))
report = {}
for name, part in [('base',base),('lid',lid)]:
    assert part.val().isValid() and len(part.solids().vals()) == 1
    cq.exporters.export(part,str(OUT/f'{name}.stl'),tolerance=0.05,angularTolerance=0.1)
    cq.exporters.export(part,str(OUT/f'{name}.step'))
    mesh=trimesh.load_mesh(OUT/f'{name}.stl')
    assert mesh.is_watertight and mesh.is_winding_consistent and mesh.volume>0
    report[name]={'watertight':bool(mesh.is_watertight),'solids':len(part.solids().vals()),
                  'bounds_mm':mesh.bounds.tolist(),'volume_mm3':round(mesh.volume,2),
                  'triangles':len(mesh.faces)}
assembly=cq.Assembly(name='GPS_tracker_fit_prototype')
assembly.add(base,name='base',color=cq.Color(0.3,0.4,0.5,0.4))
# Flip printed lid to its assembly orientation: tabs point down.
assembled_lid=lid.rotate((0,W/2,0),(1,W/2,0),180).translate((0,0,H+LID))
assembly.add(assembled_lid,name='lid',color=cq.Color(0.5,0.6,0.7,0.3))
assert base.intersect(assembled_lid).val().Volume()<0.001
for name,bounds in ENVELOPES.items():
    env=box(*bounds)
    # Physical envelope checks exclude intentional foam/platform contact.
    interference=base.intersect(env).val().Volume()
    assert interference < 0.001,(name,interference)
    assert assembled_lid.intersect(env).val().Volume()<0.001
    assembly.add(env,name=name,color=cq.Color(0.2,0.7,0.4))
assembly.export(str(OUT/'assembly.step'))
report['envelope_collision_checks']='Passed against base and closed lid; nominal assumed envelopes only'
report['note']='Geometric checks only. Physical components have not been measured or fit-tested.'
(OUT/'validation.json').write_text(json.dumps(report,indent=2)+'\n')
(OUT/'component_envelopes.json').write_text(json.dumps(ENVELOPES,indent=2)+'\n')
print(json.dumps(report,indent=2))
