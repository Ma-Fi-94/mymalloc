import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import plottingtools as pt

ticks = np.array([6155274, 9539996, 9515677, 9539601, 6141921, 6186224])
allocator = np.array(["first", "best", "best", "best", "first", "first"])

pt.darkmode()
fig, ax = pt.singleplot((5,5))
sns.barplot(x=allocator, y=ticks, ax=ax, order=["best", "first"], ci="sd", capsize=.2, errwidth=1, errcolor="lightgrey")
pt.limits(ax, None, (1,10000000))
pt.despine(ax)
pt.ticklabelsize(ax)
pt.labels(ax, "Allocator", "Ticks [num.]")
pt.ticks_and_labels(ax, "x", [0,1], ["Best-Fit", "First-Fit"])
plt.show()

