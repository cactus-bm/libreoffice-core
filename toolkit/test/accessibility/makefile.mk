# This is the dmake version.

# copied from settings.mk
SOLARBINDIR=$(SOLARVERSION)$/$(INPATH)$/bin$(UPDMINOREXT)

# Please modify the following lines to match your environment:
#   If you use the run: target at the end of the file, then adapt port number.
PORT_NUMBER = 5678

# The following variables probably don't need to be changed.
JAVAC = javac
JAVA = java
#   The JAR_PATH points to the jar files of your local office installation.
JAR_PATH = $(SOLARBINDIR)$/


# The rest of this makefile should not need to be touched.

all : AccessibilityWorkBench dist

JAR_FILES =		\
    unoil.jar	\
    sandbox.jar	\
    ridl.jar	\
    jurt.jar	\
    juh.jar		\
    java_uno.jar

JAVA_FILES = \
    AccTreeNode.java			\
    AccessibilityTree.java		\
    AccessibilityTreeModel.java \
    AccessibilityWorkBench.java	\
    AccessibleActionHandler.java	\
    AccessibleActionNode.java	\
    AccessibleCellHandler.java	\
    AccessibleComponentHandler.java	\
    AccessibleContextHandler.java	\
    AccessibleEditableTextHandler.java	\
    AccessibleExtendedComponentHandler.java	\
    AccessibleHyperlinkHandler.java	\
    AccessibleHypertextHandler.java	\
    AccessibleImageHandler.java	\
    AccessibleObject.java		\
    AccessibleRelationHandler.java	\
    AccessibleSelectionHandler.java	\
    AccessibleTableHandler.java	\
    AccessibleTextHandler.java	\
    AccessibleTreeCellRenderer.java		\
    AccessibleTreeHandler.java	\
    AccessibleTreeNode.java		\
    AccessibleUNOHandler.java		\
    Canvas.java					\
    FrameActionListener.java	\
    InformationWriter.java		\
    MessageInterface.java		\
    NodeHandler.java			\
    OfficeConnection.java		\
    Print.java					\
    SimpleOffice.java			\
    StringNode.java				\
    TopWindowListener.java		\
    VectorNode.java


JAVA_CLASSPATHS := 			\
    .						\
    $(foreach,i,$(JAR_FILES) $(JAR_PATH)$i) \
    $(CLASSPATH)

CLASSPATH !:=$(JAVA_CLASSPATHS:t$(PATH_SEPERATOR))

JFLAGS = -deprecation -classpath $(CLASSPATH)

%.class : %.java
    +$(JAVAC) $(JFLAGS) $<

AccessibilityWorkBench : $(JAVA_FILES:b:+".class")


# Create a jar file of all files neccessary to build and run the work bench.
dist: AccessibilityWorkBench.jar

AccessibilityWorkBench.jar: $(JAVA_FILES:b:+".class") jawb.mf
    +jar -cfm AccessibilityWorkBench.jar jawb.mf *.class

# Example of how to run the work bench.
run:
    +$(JAVA) -classpath $(CLASSPATH) AccessibilityWorkBench -p $(PORT_NUMBER)

