JAVAROOT = $(builddir)
EXTRA_DIST = $(MINDTERM)
CLEANFILES = $(java_DATA)

MINDTERM = $(srcdir)/MindTerm.jar
CLASSPATH_ENV = CLASSPATH=$(MINDTERM)

# Create the shell script that runs the program
if NOT_MINGW
bin_SCRIPTS = scripts/jScope
EXTRA_DIST += jScope.template
CLEANFILES += $(bin_SCRIPTS)

$(bin_SCRIPTS): jScope.template
	$(INSTALL) -d scripts
	$(INSTALL) $< $@
else
bin_SCRIPTS = jScope.bat
endif


# Documentation
dist_docs_DATA = $(DOCS)
docsdir = $(exec_prefix)/java/classes/docs

dist_java_DATA = jScope.properties MindTerm.jar

java_DATA = jScope.jar WaveDisplay.jar
javadir = $(exec_prefix)/java/classes
$(java_DATA): classjava.stamp

EXTRA_DIST += colors1.tbl
jScope.jar: colors1.tbl
	$(MKDIR_P) docs
	cp $(SUBDOCS) ./docs
	$(JAR) c0f $@ $(TOPDOCS) $(JSCOPE_CLASS) docs $<
	rm -Rf docs

WaveDisplay.jar:
	$(JAR) c0f $@ $(WAVEDISPLAY_CLASS)

dist_java_JAVA = $(JSCOPE_SRC) $(WAVEDISPLAY_SRC) $(COMMON_SRC)
COMMON_CLASS = $(COMMON_SRC:.java=*.class)
JSCOPE_CLASS = $(COMMON_CLASS) $(JSCOPE_SRC:.java=*.class) $(EXTRA_CLASS)
WAVEDISPLAY_CLASS = $(COMMON_CLASS) $(WAVEDISPLAY_SRC:.java=*.class)
CLEANFILES += $(JSCOPE_CLASS) $(WAVEDISPLAY_CLASS) $(EXTRA_CLASS)

include Makefile.common
