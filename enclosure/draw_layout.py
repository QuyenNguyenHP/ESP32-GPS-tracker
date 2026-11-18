from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, FancyBboxPatch, Circle
out=Path(__file__).resolve().parent
fig,(ax,sec)=plt.subplots(2,1,figsize=(12,8),gridspec_kw={'height_ratios':[2,1]})
fig.suptitle('ESP32 GPS tracker — initial fit prototype (all dimensions in mm)',fontsize=16)
ax.add_patch(FancyBboxPatch((0,0),104,50,boxstyle='round,pad=0,rounding_size=4',facecolor='#edf1f5',edgecolor='#263747',lw=2))
ax.add_patch(Rectangle((2,2),100,46,fill=False,ec='#9aa5b1'))
for x,y in [(5,5),(99,5),(5,45),(99,45)]:
 ax.add_patch(Circle((x,y),3.5,fc='white',ec='#334155'))
 ax.add_patch(Circle((x,y),1.2,fc='#334155'))
for x,y,w,h,c,t in [(4,16,21,18,'#80cbc4','XIAO + GNSS\nstack\n21 × 18'),(32,10,27,30,'#ffcc80','Battery reserve\n27 × 30 × 8\nNOT SELECTED'),(66,9,32,32,'#90caf9','GNSS patch\n32 × 32 × 8\nASSUMED')]:
 ax.add_patch(Rectangle((x,y),w,h,fc=c,ec='#334155',lw=1.5))
 ax.text(x+w/2,y+h/2,t,ha='center',va='center',fontsize=10)
ax.plot([0,4],[25,25],lw=7,color='white'); ax.annotate('USB access\n14 wide × 10 high',xy=(1,25),xytext=(-19,35),arrowprops={'arrowstyle':'->'},fontsize=9)
ax.plot([65,55,43,30,24],[25,44,44,39,33],color='#546e7a',ls='--',lw=2)
ax.text(48,46,'Coax routing allowance',fontsize=9,ha='center')
ax.annotate('',(0,-5),(104,-5),arrowprops={'arrowstyle':'|-|'})
ax.text(52,-8,'104',ha='center');ax.annotate('',(109,0),(109,50),arrowprops={'arrowstyle':'|-|'})
ax.text(112,25,'50',va='center')
ax.set(xlim=(-23,118),ylim=(-12,56),aspect='equal',title='Top view — lid removed; USB at left; ceramic antenna faces lid')
ax.axis('off')
sec.add_patch(Rectangle((0,0),104,2,fc='#78909c'))
for x in [0,102]: sec.add_patch(Rectangle((x,2),2,20,fc='#78909c'))
sec.add_patch(Rectangle((0,22),104,2,fc='#b0bec5'))
for x,z,w,h,c,t in [(4,2,22,2,'#78909c',''),(4,5,21,4,'#80cbc4','ESP32'),(4,10,21,8,'#80cbc4','GNSS reserve'),(32,3,27,8,'#ffcc80','Battery'),(66,4,32,8,'#90caf9','Patch')]:
 sec.add_patch(Rectangle((x,z),w,h,fc=c,ec='#334155'))
 if t: sec.text(x+w/2,z+h/2,t,ha='center',va='center',fontsize=8)
sec.annotate('',(109,0),(109,24),arrowprops={'arrowstyle':'|-|'})
sec.text(112,12,'24',va='center');sec.text(68,17,'No battery/PCB directly below patch',fontsize=9,ha='center')
sec.set(xlim=(-23,118),ylim=(-3,30),aspect='equal',title='Schematic longitudinal section — 2 mm floor, walls and lid')
sec.axis('off')
fig.text(0.5,0.025,'PROVISIONAL: verify antenna backing, board stack, USB height and battery before a final print. No weather seal.',ha='center',fontsize=10,color='#9a3412')
plt.tight_layout(rect=(0,.06,1,.95));fig.savefig(out/'layout.png',dpi=180)
fig.savefig(out/'layout.svg')
