#!/bin/sh

########################################################################
#
#  This script builds the Doxygen documentation for your project
#  and loads the docset into Xcode.
#
#  This script tries to find the Doxygen binary's path. If it fails to
#  find it, add your doxygen path to the list below.
#
########################################################################

# Set to 1 to build the latex documentation for PDF generation.
GENERATE_PDF=0

if [ "$SOURCE_ROOT"X = "X" ] || [ "$TEMP_DIR"X = "X" ] || [ "$PROJECT_NAME"X = "X" ]; then
    echo "Error: This script must be run from within XCode's build environment."
    exit 1
fi

# Find Doxygen
DOXYGEN_PATH=/Applications/Doxygen.app/Contents/Resources/doxygen
if [ ! -f "$DOXYGEN_PATH" ]; then
    DOXYGEN_PATH=/opt/local/bin/doxygen
fi
if [ ! -f "$DOXYGEN_PATH" ]; then
    DOXYGEN_PATH=~/bin/doxygen
fi
if [ ! -f "$DOXYGEN_PATH" ]; then
    DOXYGEN_PATH=/usr/local/bin/doxygen
fi
if [ ! -f "$DOXYGEN_PATH" ]; then
    DOXYGEN_PATH=/usr/share/bin/doxygen
fi
if [ ! -f "$DOXYGEN_PATH" ]; then
    DOXYGEN_PATH=/usr/bin/doxygen
fi
if [ ! -f "$DOXYGEN_PATH" ]; then
    DOXYGEN_PATH=/bin/doxygen
fi
if [ ! -f "$DOXYGEN_PATH" ]; then
    echo "Error: Doxygen is not installed or I can't find it (examine build_docs.sh to see where I'm looking)."
    exit 1
fi

DOX_SRC="$TEMP_DIR/doxygen.config"
DOX_TEMP="$TEMP_DIR/doxygen.temp"
DOX_DEST="$TEMP_DIR/doxygen.config"
DOX_SET="$TEMP_DIR/DoxygenDocs.docset"
DOX_NAME="org.doxygen.$PROJECT_NAME"

#
# If the config file doesn't exist, run 'doxygen -g <dest>' to create a default file.
#
if ! [ -f "$DOX_SRC" ]; then
    echo doxygen config file does not exist
    $DOXYGEN_PATH -g "$DOX_SRC"
fi

#
#  Make a copy of the default config
#
echo $DOX_SRC
echo $DOX_DEST
cp "$DOX_SRC" "$DOX_DEST"

#
# Customize the settings we want to change
#
echo "PROJECT_NAME       =  \"${PROJECT_NAME}\"" >> $DOX_DEST
echo "INPUT              =  ${SOURCE_ROOT}/../../libs/frameworks/OpenThreads.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../include/cefix \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osg.framework/Headers \\" >> $DOX_DEST
echo "						${SOURCE_ROOT}/../../libs/frameworks/osgDB.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgGA.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgUtil.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgViewer.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgText.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgTerrain.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgSim.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgShadow.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/frameworks/osgVolume.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/cefixNet.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/cefixbt.framework/Headers \\" >> $DOX_DEST
echo "                      ${SOURCE_ROOT}/../../libs/cefixcv.framework/Headers" >> $DOX_DEST
echo "OUTPUT_DIRECTORY    = $DOX_SET" >> $DOX_DEST
echo "DOCSET_BUNDLE_ID    = $DOX_NAME" >> $DOX_DEST
echo "RECURSIVE           = YES" >> $DOX_DEST
echo "GENERATE_DOCSET     = YES" >> $DOX_DEST
echo "GENERATE_XML        = YES" >> $DOX_DEST
echo "QUIET               = YES" >> $DOX_DEST
echo "TAB_SIZE            = 4" >> $DOX_DEST
echo "QT_AUTOBRIEF        = YES" >> $DOX_DEST
echo "JAVADOC_AUTOBRIEF   = YES" >> $DOX_DEST
echo "FULL_PATH_NAMES     = NO" >> $DOX_DEST
echo "EXTRACT_PRIVATE     = NO" >> $DOX_DEST
echo "EXTRACT_STATIC      = NO" >> $DOX_DEST
echo "ALWAYS_DETAILED_SEC = YES" >> $DOX_DEST
echo "FILE_PATTERNS       = * *.* *.c *.cc *.cxx *.cpp *.c++ *.java *.h *.hh *.hxx *.hpp *.h++ *.m *.mm *.dox" >> $DOX_DEST
echo "EXCLUDE_PATTERNS    = */.svn/* *.svn-work *.svn-base" >> $DOX_DEST
echo "WARN_FORMAT         = \"\$file:\$line: warning: \$text\"" >> $DOX_DEST
echo "GENERATE_LATEX      = NO" >> $DOX_DEST
echo "USE_PDFLATEX        = NO" >> $DOX_DEST
echo "PDF_HYPERLINKS      = NO" >> $DOX_DEST
echo "config file: ${DOX_DEST}"
#
#  Run doxygen on the updated config file.
#  Note: doxygen creates a Makefile that does most of the heavy lifting.
#
$DOXYGEN_PATH "$DOX_DEST"

#
#  make will invoke docsetutil. Take a look at the Makefile to see how this is done.
#
make -C "$DOX_SET/html" install

#
#  Load the doc set
#
open /Users/'$USER'/Library/Developer/Shared/Documentation/DocSets/
echo ${INPUT}
#/usr/bin/osascript -e 'tell application "Xcode3" to load documentation set with path "/Users/'$USER'/Library/Developer/Shared/Documentation/DocSets/'$DOX_NAME'.docset"'

exit 0

