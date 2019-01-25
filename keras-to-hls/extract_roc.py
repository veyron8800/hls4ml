from __future__ import print_function
from sklearn.metrics import roc_curve, auc
import numpy as np
import json
import yaml
import os, sys
import argparse
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

def parse_config(config_file) :
    print("Loading configuration from", config_file)
    config = open(config_file, 'r')
    return yaml.load(config)

def extract_roc(yamlConfig):
    print("Extracting ROC")

    output_filename = "./{}/{}_prj/solution1/csim/build/tb_output_data.dat".format(yamlConfig['OutputDir'], yamlConfig['ProjectName'])
    truth_filename = yamlConfig['TruthLabels']
    predict_filename = yamlConfig['OutputPredictions']

    ## Check if file exits
    if not os.path.isfile(output_filename):
        print("No output data found")
        return

    truth_df = np.loadtxt(truth_filename)
    predict_df = np.loadtxt(predict_filename)
    output_df = np.loadtxt(output_filename)
    
    if len(truth_df.shape) == 1:
        truth_df = truth_df.reshape((truth_df.shape[0], 1))
        predict_df = predict_df.reshape((predict_df.shape[0], 1))
        output_df = output_df.reshape((output_df.shape[0], 1))
    Noutputs = truth_df.shape[1]

    predict_df = predict_df[:output_df.shape[0],:]
    truth_df = truth_df[:output_df.shape[0],:]

    for i in range(Noutputs):
        plt.clf()

        ## Expected AUC from keras
        efpr, etpr, ethreshold = roc_curve(truth_df[:,i],predict_df[:,i])
        eauc = auc(efpr, etpr)
        plt.plot(etpr,efpr,label='Keras auc = %.1f%%'%(eauc * 100))

        ## Expected AUC from HLS
        dfpr, dtpr, dthreshold = roc_curve(truth_df[:,i],output_df[:,i])
        dauc = auc(dfpr, dtpr)
        plt.plot(dtpr,dfpr,label='HLS auc = %.1f%%'%(dauc * 100))
        
        plt.plot([], [], ' ', label="Ratio = {:.2f}".format(dauc / eauc))

        plt.semilogy()
        plt.legend(loc='upper left')
        roc_filename = "{}_ROC{}.pdf".format(yamlConfig['OutputDir'],str(i))
        plt.savefig(roc_filename)
        print("ROC written to", roc_filename)

    print("Done")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='')
    parser.add_argument("-c", action='store', dest='config',
                        help="Configuration file.")
    args = parser.parse_args()
    if not args.config: parser.error('A configuration file needs to be specified.')
    
    configDir  = os.path.abspath(os.path.dirname(args.config))
    yamlConfig = parse_config(args.config)
    
    extract_roc(yamlConfig)
