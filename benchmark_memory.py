import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import plottingtools as pt
import pandas as pd

dat = pd.read_csv("memory.csv")
dat2 = dat[["merge", "allocate_first", "seed", "avg_free_size"]]

#sd_jitter = 0.05
#jitter = np.random.normal(loc=0, scale=sd_jitter, size=np.shape(dat2)[0])

pt.darkmode()
fig, axes = pt.multiplot(1, 2, (12,6), wspace=.5)

ax = axes.flat[0]
x1 = 10*[1]
x2 = 10*[2]
y1 = dat2[(dat2["merge"] == 0) & (dat2.allocate_first == 0)].avg_free_size
y2 = dat2[(dat2["merge"] == 0) & (dat2.allocate_first == 1)].avg_free_size
ax.plot(x1, y1, 'o', alpha=.40, zorder=1, ms=8, mew=1, c="C0")
ax.plot(x2, y2, 'o', alpha=.40, zorder=1, ms=8, mew=1, c="C0")
for x,y,xx,yy in zip(x1,y1,x2,y2):
    sns.lineplot([x,xx],[y,yy], color="0.85", linestyle=":", ax=ax)

pt.title(ax, "No Merging")
#ax.set_yscale("log")
pt.ticks_and_labels(ax, "x", [1,2], ["Best-Fit", "First-Fit"])
pt.ticklabelsize(ax)
pt.limits(ax, None, (0,500))
pt.labels(ax, "Allocator", "Mean Free Block Size")
pt.despine(ax)




ax = axes.flat[1]
x1 = 10*[1]
x2 = 10*[2]
y1 = dat2[(dat2["merge"] == 1) & (dat2.allocate_first == 0)].avg_free_size
y2 = dat2[(dat2["merge"] == 1) & (dat2.allocate_first == 1)].avg_free_size
ax.plot(x1, y1, 'o', alpha=.40, zorder=1, ms=8, mew=1, c="C1")
ax.plot(x2, y2, 'o', alpha=.40, zorder=1, ms=8, mew=1, c="C1")
for x,y,xx,yy in zip(x1,y1,x2,y2):
    sns.lineplot([x,xx],[y,yy], color="0.85", linestyle=":")

pt.title(ax, "Merging")
ax.set_yscale("log")
pt.ticks_and_labels(ax, "x", [1,2], ["Best-Fit", "First-Fit"])
pt.ticklabelsize(ax)
pt.limits(ax, None, (100,1e6))
pt.despine(ax)
pt.labels(ax, "Allocator", "Mean Free Block Size")
plt.show()


