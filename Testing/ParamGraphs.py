import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import csv

def make_surface_plots(x, y, z1, z2, z3, thread_count: int):
    title = f'WebServer Performance with {thread_count} Threads'
    xlabel = 'File Buffer Size'
    ylabel = 'Network Buffer Size'
    subTitles = ('Total Time For 100 Requests', 'Average Time per request', 'Average Throughput')

    fig = plt.figure(figsize=(6, 18))
    fig.suptitle(title)
    plt1 = fig.add_subplot(3, 1, 1, projection='3d')
    plt1.set_title(subTitles[0])
    plt1.set_xlabel(xlabel, fontsize=6)
    plt1.set_ylabel(ylabel, fontsize=6)
    plt1.set_zlabel('Time (seconds)', fontsize=6)

    plt1.scatter(x, y, z1, c=z1, cmap='viridis', depthshade=True)

        # Set tick labels to actual values instead of log2 values
    plt1.view_init(elev=30, azim=45)  # Set elevation and azimuth angle
    

    #Plot 2
    plt2 = fig.add_subplot(3, 1, 2, projection='3d')
    plt2.set_title(subTitles[1])
    plt2.set_xlabel(xlabel, fontsize=6)
    plt2.set_ylabel(ylabel, fontsize=6)
    plt2.set_zlabel('Time (seconds)', fontsize=6)

    plt2.scatter(x, y, z2, c=z2, cmap='viridis', depthshade=True)
        # Set tick labels to actual values instead of log2 values
    plt2.view_init(elev=30, azim=45)  # Set elevation and azimuth angle

    #Plot 3
    plt3 = fig.add_subplot(3, 1, 3, projection='3d')
    plt3.set_title(subTitles[2])
    plt3.set_xlabel(xlabel, fontsize=6)
    plt3.set_ylabel(ylabel, fontsize=6)
    plt3.set_zlabel('Throughput (Bytes/second)', fontsize=6)

    plt3.scatter(x, y, z3, c=z3, cmap='viridis', depthshade=True)
    plt3.view_init(elev=30, azim=45)  # Set elevation and azimuth angle
    
    fig.subplots_adjust(hspace=0.5)


    return fig


if __name__ == '__main__':
    data = []
    with open('./results.csv', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            data.append(row)
    thread_data = {}
    for d in data:
        if d[0] not in thread_data:
            thread_data[d[0]] = [[],[],[],[],[]]
        for i in range(5):
            thread_data[d[0]][i].append(d[i+1])
    # for thread_count, values in thread_data.items():
    #     print(f'Thread Count: {thread_count}')
    #     print(f'X Values: {np.array(values[0])}')
    #     print(f'Y Values: {np.array(values[1])}')
    #     print(f'Z1 Values: {np.array(values[2])}')
    #     print(f'Z2 Values: {np.array(values[3])}')
    #     print(f'Z3 Values: {np.array(values[4])}')
    
    figures = []
    for thread_count, values in thread_data.items():
        x = np.array(values[0], dtype=int)
        y = np.array(values[1], dtype=int)
        z1 = np.array(values[2], dtype=float)
        z2 = np.array(values[3], dtype=float)
        z3 = np.array(values[4], dtype=float)
        fig = make_surface_plots(x, y, z1, z2, z3, thread_count)
        figures.append(fig)
        fig.savefig(f'./plots/{thread_count}.png', dpi=1024)
        fig.show()
        
    

    