# This is the dmake version.


# Please modify the following lines to match your environment:
#   If you use the run: target at the end of the file, then adapt port number
#   and file URL.
PORT_NUMBER = 5678
FILE_NAME = "file:///tmp/impress-test-document.sxi"


# The following variables probably don't need to be changed.
JAVAC = javac
JAVA = java
#   The JAR_PATH points to the jar files of your local office installation.
JAR_PATH = $(STAR_RESOURCEPATH)$/



# The rest of this makefile should not need to be touched.

all : AccessibilityWorkBench

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
    AccessibleComponentHandler.java	\
    AccessibleContextHandler.java	\
    AccessibleEditableTextHandler.java	\
    AccessibleExtendedComponentHandler.java	\
    AccessibleHyperlinkHandler.java	\
    AccessibleHypertextHandler.java	\
    AccessibleImageHandler.java	\
    AccessibleObject.java		\
    AccessibleTableHandler.java	\
    AccessibleTextHandler.java	\
    AccessibleTreeCellRenderer.java		\
    AccessibleTreeHandler.java	\
    AccessibleTreeNode.java		\
    Canvas.java					\
    FrameActionListener.java	\
    InformationWriter.java		\
    MessageInterface.java		\
    NodeHandler.java			\
    OfficeConnection.java		\
    Print.java					\
    SimpleOffice.java			\
    StringNode.java				\
    VectorNode.java


JAVA_CLASSPATHS := 			\
    .						\
    $(foreach,i,$(JAR_FILES) $(JAR_PATH)$i) \
    $(CLASSPATH)

CLASSPATH !:=$(JAVA_CLASSPATHS:t$(PATH_SEPERATOR))

JFLAGS = -deprecation -classpath $(CLASSPATH)

%.class : %.java
    $(JAVAC) $(JFLAGS) $<

AccessibilityWorkBench : $(JAVA_FILES:b:+".class")


# Create a jar file of all files neccessary to build and run the work bench.
dist:
    jar -cf AccessibilityWorkBench.jar \
        $(JAVA_FILES)\
        *.class
#		$(JAVA_FILES:b:+".class")

# Example of how to run the work bench.
run:
    $(JAVA) -classpath $(CLASSPATH) AccessibilityWorkBench -p $(PORT_NUMBER) -f $(FILE_NAME)

