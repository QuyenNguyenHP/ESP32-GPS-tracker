from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import trimesh
out=Path(__file__).resolve().parent
fig=plt.figure(figsize=(12,7)); ax=fig.add_subplot(111,projection='3d')
for name,delta,color in [('base',(0,0,0),'#6a91a4'),('lid',(0,62,0),'#becdd4')]:
 m=trimesh.load_mesh(out/f'{name}.stl');m.apply_translation(delta)
 ax.add_collection3d(Poly3DCollection(m.triangles,facecolor=color,edgecolor='#405766',linewidth=.08,alpha=1))
ax.set(xlim=(0,110),ylim=(0,115),zlim=(0,30),xlabel='X (mm)',ylabel='Y (mm)',zlabel='Z (mm)')
ax.set_box_aspect((110,115,40));ax.view_init(elev=48,azim=-58)
ax.set_title('Actual STL geometry — base and lid in print orientation\n104 × 50 × 24 mm assembled | initial fit prototype')
fig.tight_layout();fig.savefig(out/'stl_preview.png',dpi=180)
