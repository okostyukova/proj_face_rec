TEMPLATE = subdirs

SUBDIRS = \
    cortexclient\
    marker

# build the project sequentially as listed in SUBDIRS !
CONFIG += ordered

OTHER_FILES += README.md
