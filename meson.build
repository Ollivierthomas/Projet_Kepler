project('kepler', 'c', default_options : ['c_std=c23'])

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)
raylib = cc.find_library('raylib', required : true)
GL = cc.find_library('GL', required : true)
pthread = cc.find_library('pthread', required : true)
dl = cc.find_library('dl', required : true)
rt = cc.find_library('rt', required : true)
X11 = cc.find_library('X11', required : true)

sources = files(
  'src/euler.c',  
  'src/energie.c',
  'src/trajectoire.c',
  'src/systeme_solaire.c',
)

executable('kepler', sources, dependencies : [m_dep, raylib, GL, pthread, dl, rt, X11], override_options : ['c_std=c23'])
