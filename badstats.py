import numpy as np
from itertools import groupby
import matplotlib.pyplot as plt
import seaborn as sns
# This opens a handle to your file, in 'r' read mode
file_handle = open('/tmp/out.txt', 'r')
# Read in all the lines of your file into a list of lines
lines_list = file_handle.readlines()
my_data = [float(val) for val in lines_list]
data = np.array(my_data)

# sns.distplot(data,bins=np.arange(data.min(), data.max()+1),kde=False,hist_kws={"align" : "left"})
sns.displot(data,bins=np.arange(data.min(), data.max()+1),kde=False,log_scale=(False,False))
# sns.displot(data,bins=np.arange(data.min(), data.max()+1),kde=False,log_scale=(False,True))

plt.show()

