DOC_FOLDERS = doc/html

# Folder(s) must exist to be included in Makefile.
for( folder, DOC_FOLDERS ) {
	system( mkdir -p $$(PWD)/$${folder} )
}

# Documentation build target
doctarget.target = docs

contains(NO_DOCS,yes): {
doctarget.commands = echo \"*** skipped building doxygen docs ***\"
} else {
doctarget.commands = INPUT=\"$${_PRO_FILE_PWD_}/system\" \
                     STRIP_FROM_PATH=\"$${_PRO_FILE_PWD_}\" \
                     doxygen $$system(pwd)/Doxyfile
}

QMAKE_EXTRA_TARGETS += doctarget

# Add documentation cleaning to clean target
doccleantarget.target = clean
for( folder, DOC_FOLDERS ) {
	doccleantarget.commands += rm -rf $${folder}
}
doccleantarget.commands += rm -rf doc/latex

QMAKE_EXTRA_TARGETS += doccleantarget


# Documentation install target
documentation.path = /usr/share/doc/qmsystem2
documentation.files = doc/html
documentation.depends = docs

INSTALLS += documentation
