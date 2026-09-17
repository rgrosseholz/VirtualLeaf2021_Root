import os, string

leaf = 'data/leaves/root3.xml'
executable = 'VirtualLeaf'
plugin = 'libturingplugin.so'

os.system('bin/' + executable + ' -b -l ' + leaf )
