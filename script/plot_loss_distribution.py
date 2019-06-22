#!/home/zhifei/App/anaconda3/bin/python3
# @Author: Zhifei WANG
# @Date:   21-Jun-2019
# @Last modified by:   Zhifei WANG

import numpy as np
import os
import matplotlib.pyplot as plt
import pandas as pd
# from scipy.interpolate import spline, interp1d
import re
from matplotlib import cm
import seaborn as sns

# ----------------------- plot setting -----------------------
fig_path = '../Graphics/'
fig_format = '.svg'
fig_dvi = 600
LW = 2
FS = 12
FS_ticklabel = 12
FS_label = 12
FS_legend = 12

plot_arg = {"linewidth": LW, "solid_joinstyle": 'round'}
log_plot_arg = {"linewidth": LW, "solid_joinstyle": 'round', 'basex': 4}
LS_list = [':', '-.', '--', '-']
LS_list = LS_list[::-1]
LS_list2 = ['-', '--', ':']
Marker_list = ['o', 'v', 's', 'd']
Marker_Null_list = [None, None, None, None]
GREEN_COLOR = ['#a1d99b', '#74c476', '#41ab5d', '#238b45']
GREEN_COLOR = GREEN_COLOR[::-1]
BLUE_COLOR = ['#d0d1e6', '#a6bddb', '#74a9cf', '#2b8cbe', '#045a8d']
RED_COLOR = ['#fc9272', '#fb6a4a', '#ef3b2c', '#cb181d']
RED_COLOR = RED_COLOR[::-1]
color_matrix = [RED_COLOR, BLUE_COLOR, GREEN_COLOR, ]
if not os.path.exists(fig_path):
    os.makedirs(fig_path)
# ----------------------- beautify the ploting -----------------------
# plt.rc('text', usetex=True)
plt.rc('font', family='serif', size=FS)
kwargs = dict(histtype='stepfilled', alpha=0.75, density=True, bins=40)


# plt.rc('grid', linestyle=":", color='black', linewidth=0.5)
# plt.rc('legend', fancybox=True)
# # plt. rcParams['text.latex.preamble'] = [r'\usepackage{sfmath} \boldmath']


def main():
    data_path = '../Data/'
    # radii = [4, 3, 4, 6]
    # radii = [4, 8, 8, 4]
    # radii = [4, 4, 4, 4, 4]
    radii_list = [
        [4, 8, 8, 4],
        [4, 4, 4, 4, 4]
    ]
    switch_type_list = [
        'Baseline',
        'FODON',
    ]
    stage_switch_type = 'PILOSS'
    for switch_type in switch_type_list:
        for radii in radii_list:
            worker = LossPlotClass(
                radii, switch_type, stage_switch_type, data_path)
            worker.ReadData()
            # worker.PlotLossDistibution()
            worker.PlotLossHeatmap()
    # plt.show()


class LossPlotClass:
    def __init__(self, radii, switch_type, stage_switch_type, data_path):
        self.radii = radii
        self.switch_type = switch_type
        self.stage_switch_type = stage_switch_type
        self.data_path = data_path
        self.port = np.prod(radii)
        self.filename0 = '{}port_{}_{}'.format(
            self.port, switch_type, stage_switch_type)
        for radix in radii:
            self.filename0 = self.filename0 + '_{}'.format(radix)

    def ReadData(self):
        self.loss_matrix = np.genfromtxt(
            self.data_path + self.filename0 + '.csv', delimiter=',')

    def PlotLossDistibution(self):
        loss_list = self.loss_matrix.ravel()
        self.loss_list = loss_list[loss_list != np.array(-1)]
        assert (len(self.loss_list) == self.port * (self.port - 1))
        self.ave_loss = np.mean(self.loss_list)
        self.loss_std = np.std(self.loss_list)
        self.max_loss = np.max(self.loss_list)

        n, bins, patches = plt.hist(self.loss_list, **kwargs)
        plt.xlabel('Loss (dB)')
        plt.ylabel('Probability')
        plt.text(
            16, 0.37, 'ave={:.2f}\n $\sigma$={:.2f}\n max={:.2f}'.format(
                self.ave_loss, self.loss_std, self.max_loss
            ))
        plt.tight_layout()
        plt.savefig(fig_path + self.filename0 + fig_format,
                    dpi=fig_dvi, bbox_inches='tight')

    def PlotLossHeatmap(self):
        # loss_df = pd.read_csv(
        #     self.data_path + self.filename0 + '.csv')
        # ax = sns.heatmap(self.loss_matrix, annot=True)
        plt.figure()
        ax = plt.axes()
        plt.imshow(self.loss_matrix, cmap='viridis', )
        ax.invert_yaxis()
        plt.colorbar()
        plt.savefig(fig_path + self.filename0 + '.png',
                    dpi=fig_dvi * 2, bbox_inches='tight')


if __name__ == "__main__":
    main()
