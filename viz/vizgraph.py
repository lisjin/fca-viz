#!/usr/bin/python
# -*- coding: utf-8 -*-

from optparse import OptionParser
import sys, pdb, traceback, re
import networkx as nx

#ignore all matplotlib warnings
import warnings
import pandas
warnings.filterwarnings("ignore")

def x_coord(G, node):
    return nx.get_node_attributes(G,'xy')[node][0]

def y_coord(G, node):
    return nx.get_node_attributes(G,'xy')[node][1]

def isLand(G, node):
    return x_coord(G, node) > 0 or y_coord(G, node) > 0

def isSea(G, node):
    return x_coord(G, node) < 0 and y_coord(G, node) < 0

def parse_args():
    parser = OptionParser(usage="python vizgraph.py --input input1.txt --output output1.txt --mst", add_help_option=False)
    parser.add_option("-h", "--help", action="help",
            help="Show this help message and exit")
    parser.add_option("-i", "--inputf", type="str",
            help="input dataset (.csv filename)")
    parser.add_option("-o", "--outputf", type="str", default = None,
            help="path to folder where results will be saved")
    parser.add_option("-m", "--mst", type="str", help="mst or tsp mode")

    (opts, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    return (opts, args)

def match_or_exit(regex, line):
  m = re.match(regex, line)
  if m:
    return m.groups()
  else:
    m = re.match(r'\s+$', line)
    if m:
      return None
    else:
      sys.exit(1)


def read_input(filename, mst):
    G = nx.Graph()
    f = open(filename)
    num_facilities = int(next(f))
    for i in range(num_facilities):
        groups = match_or_exit(r'(-?[0-9]+)\s+(-?[0-9]+)\s*$', next(f))
        posxy = (int(groups[0]), int(groups[1]))
        G.add_node(i, xy=posxy)  
    for i in G.nodes():
        for j in G.nodes():
            if (mst):
                if isLand(G,i):
                    if isLand(G,j):
                        G.add_edge(i,j) #both land pokemon
                elif isSea(G,i):
                    if isSea(G,j):
                        G.add_edge(i,j) #both sea pokemon
                else:
                    G.add_edge(i,j)
            else:
                G.add_edge(i,j) #i is coast pokemon
    f.close()
    return G


def read_output(filename):
    f = open(filename)

    paths = []
    for line in f:
        # test for a MST output
        m = re.match(r'([0-9]+)\s+([0-9]+)\s*$', line)
        if m:
            paths.append((int(m.groups()[0]), int(m.groups()[1])))
        else:
            break

    if len(paths)==0:
        # test for a TSP output
        tsp_sol = re.findall(r'\b\d+\b', line)
        for i in range(len(tsp_sol)-1):
            paths.append((int(tsp_sol[i]), int(tsp_sol[i+1])))
        paths.append((int(tsp_sol[-1]), int(tsp_sol[0])))

    f.close()
    return paths


def plot_graph(G, titl='', nodesize=300, widthsize=1.5, plotfname='/tmp/tmp.png'):
    import matplotlib as mpl
    mpl.use('Agg', warn=False)
    import matplotlib.pyplot as plt
    from mpltools import style, layout
    style.use('ggplot')

    cm = plt.get_cmap('cool')
    cm.set_under('w')

    f = plt.figure(1)
    ax = f.add_subplot(1,1,1)

    obs = open('obs.txt', 'r').read().split('\n')
    obs = [o.decode('utf-8')[:-1] for o in obs if len(o) > 0]
    txtstr = ''
    # for k, v in enumerate(obs[::-1]):
    #     txtstr += '%d: {%s}' % (len(obs) - k - 1, v) + '\n'


    # titl = '%d nodes, %d edges' % (G.number_of_nodes(), G.number_of_edges())
    f.text(0.02, 0.98, txtstr, transform=ax.transAxes, verticalalignment='top')
    plt.title(titl)

    pos = nx.get_node_attributes(G,'xy')

    nodes = nx.draw_networkx_nodes(G, pos, cmap = cm, node_color='c', labels=None, with_labels=False, ax=ax, node_size=nodesize)
    edges = nx.draw_networkx_edges(G, pos, width=widthsize, ax=ax)

    pos_short = {}
    # for i in range(0, len(obs)):
    #     pos_short[i] = '%d' % i
    for k, v in enumerate(obs):
        pos_short[k] = '{%s}' % v
    labels = nx.draw_networkx_labels(G, pos, labels=pos_short, font_size=8)

    plt.axis('off')
    f.set_facecolor('w')

    plt.savefig(plotfname, dpi=300, bbox_inches='tight')
    plt.clf()
    return 0


def main():
    opts, args = parse_args()
    G = nx.Graph()
    if (opts.mst == "mst"):
        G = read_input(opts.inputf, True)
    else:
        G = read_input(opts.inputf, False)

    if opts.outputf:
        paths = read_output(opts.outputf)
        Gout = nx.Graph()
        Gout.add_nodes_from(G.nodes(data=True))
        for path in paths:
            Gout.add_edge(path[0], path[1])
        plot_graph(Gout, plotfname='output.png')      

    return 0


if  __name__ =='__main__':
    try:
        main()
    except:
        type, value, tb = sys.exc_info()
        traceback.print_exc()
        pdb.post_mortem(tb)
