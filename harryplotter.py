import csv 
import re
import pandas as pd
import seaborn as sb
import matplotlib.pyplot as plt

# Create CSV file and writer
f = open('data.csv', 'w', newline='')
writer = csv.writer(f)

# Regex matches for temp and time
regex = re.compile(": (\d*\.\d*),.*: (\d*)")

with open("data.txt") as data: 
    # Go through each line of data.txt
    for line in data: 
        # See if it matches with regex
        for match in regex.finditer(line): 
            # Write a new row with regex matches
            writer.writerow([match.group(1),match.group(2)]) 

#Close writer and CSV file, data.csv
f.close()

# Read data.csv, defining column names and setting index column set to 'ms'.
data = pd.read_csv('data.csv', names=['temp', 'ms', 'count'], index_col='ms')

# Rewrite time column to panda timedelta milliseconds
data.index = pd.to_timedelta(data.index, unit='ms')

# Resample to calculate mean temperature and number of events every 15 s
resamp = data.resample('15s').agg({'temp': ['mean', 'count']})

# Flattens multiindex columns and fixes names 
resamp.columns = resamp.columns.map('_'.join)

print(resamp)

# Save the resampled data as CSV
resamp.to_csv('result.csv')

# Create plots
sb.set_style("darkgrid")

fig, ax =plt.subplots(1,3)

res = sb.lineplot(x='ms', y='temp_mean', data=resamp, ax=ax[0], ci='sd')

res.set(xlabel='Time [ms]', ylabel='Mean temperature [°C]')

res2 = sb.lineplot(x='ms', y='temp_count', data=resamp, ax=ax[1], ci='sd')

res2.set(xlabel='Time [ms]', ylabel='# events pr 30 minutes')

res3 = sb.scatterplot(x='temp_count', y='temp_mean', data=resamp, ax=ax[2])

res3.set(xlabel='# events pr 30 minutes', ylabel='Mean temperature [°C]')

plt.show()

fig.savefig("result.pdf", bbox_inches='tight')
