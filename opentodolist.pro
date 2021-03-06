TEMPLATE = subdirs
SUBDIRS = plugins OpenTodoList

qrc.depends =
qrc.commands = perl $$PWD/bin/mk-qrc.pl \
               --dir $$PWD/OpenTodoList/qml/ \
               --base $$PWD/OpenTodoList/ \
               --out $$PWD/OpenTodoList/OpenTodoList.qrc
QMAKE_EXTRA_TARGETS += qrc

OTHER_FILES += \
  README.md \
  COPYING \
  Doxyfile \
  doc/installers-howto.md
