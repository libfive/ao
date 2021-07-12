#!/bin/bash
set -x -e

EXE=Studio
APP=$EXE.app

VERSION=`git describe --exact-match --tags || echo "($(git rev-parse --abbrev-ref HEAD))"`
VERSION=`echo $VERSION|sed s:/:-:g`

rm -rf build
mkdir build
cd build
cmake -GNinja\
    -DBUILD_GUILE_BINDINGS=OFF \
    -DCMAKE_PREFIX_PATH=/usr/local/opt/qt@5 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.12  ..
rm -rf $APP studio/$APP
ninja clean
ninja studio/all

# Copy to a new location before modifying, so that the built app doesn't
# get modified and future builds don't misbehave due to duplicate frameworks
cp -r studio/$APP $APP

# Pull out framework paths info with otool
MACDEPLOYQT=`otool -L $APP/Contents/MacOS/$EXE | sed -n -e "s:\(.*\)lib/QtCore.*:\1/bin/macdeployqt:gp"`
GUILE_SCM=`otool -L $APP/Contents/MacOS/$EXE | sed -n -e "s:lib/libguile.*:share/guile/3.0/:gp"`
GUILE_CCACHE=`otool -L $APP/Contents/MacOS/$EXE | sed -n -e "s:lib/libguile.*:lib/guile/3.0/ccache/:gp"`
PY3_VERSION=`otool -L studio/Studio.app/Contents/MacOS/Studio | sed -n -e "s:.*Python.framework/Versions/\(3\..\).*:\1:gp"`
PY3_FRAMEWORK=`otool -L studio/Studio.app/Contents/MacOS/Studio | sed -n -e "s:\(.*Python.framework\)/Versions.*:\1:gp"`

$MACDEPLOYQT $APP

# Delete unused Qt plugins
cd $APP/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors bearer

fix_qt () {
    echo "Fixing Qt for $1"
    for LIB in $( otool -L $1 | sed -n -e "s:\(.*Qt.*.framework[^ ]*\).*:\1:gp" )
    do
        RENAMED=`echo $LIB | sed -n -e "s:.*\(Qt.*\)\.framework.*:@executable_path/../Frameworks/\1.framework/Versions/5/\1:gp"`
        install_name_tool -change $LIB $RENAMED $1
    done
}

# Remap platform links
cd platforms
fix_qt libqcocoa.dylib

# Delete unused Qt frameworks (wow, there's a lot of them)
cd ../../Frameworks
rm -rf \
    Qt3DCore.framework \
    Qt3DRender.framework \
    QtDeclarative.framework \
    QtLocation.framework \
    QtMultimedia.framework \
    QtMultimediaWidgets.framework \
    QtNetwork.framework \
    QtPdf.framework \
    QtPositioning.framework \
    QtQml.framework \
    QtQmlModels.framework \
    QtQuick.framework \
    QtScript.framework \
    QtSensors.framework \
    QtSerialBus.framework \
    QtSerialPort.framework \
    QtSql.framework \
    QtSvg.framework \
    QtVirtualKeyboard.framework \
    QtXmlPatterns.framework

# Clean up remaining Qt frameworks
for LIB in $( ls|sed -n -e "s:\(Qt.*\)\.framework:\1:gp" )
do
    fix_qt $LIB.framework/Versions/Current/$LIB
done

# Deploy the Python framework, cleaning out unused info
rm -rf Python.framework
cp -R  $PY3_FRAMEWORK .
file Python.framework/Versions/Current/lib/python$PY3_VERSION/site-packages
rm -rf Python.framework/Versions/Current/lib/python$PY3_VERSION/site-packages
rm -r  Python.framework/Versions/Current/lib/python$PY3_VERSION/test
rm -r  Python.framework/Versions/Current/lib/python$PY3_VERSION/__pycache__
rm -rf Python.framework/Versions/Current/lib/python$PY3_VERSION/*/__pycache__
rm -r  Python.framework/Versions/Current/share/doc

# For some reason, the Python framework links against the Homebrew libintl,
# so bring that along for good measure.
LIBINTL=$(otool -L Python.framework/Python | sed -n -e "s:\(.*libintl.*dylib\).*:\1:gp")
if [ -n "$LIBINTL" ]; then
    cp $LIBINTL .
    LIBINTL_BASE=$(basename $LIBINTL)
    install_name_tool -change $LIBINTL \
        "@executable_path/../Frameworks/$LIBINTL_BASE" Python.framework/Python
fi

# Copy the libfive Python libraries into the site-packages dir
mkdir Python.framework/Versions/Current/lib/python$PY3_VERSION/site-packages
cp -r ../../../../libfive/bind/python/libfive \
      Python.framework/Versions/Current/lib/python$PY3_VERSION/site-packages/

# Rewire the executable to point at the bundled framework
cd ../MacOS
install_name_tool -change \
    $PY3_FRAMEWORK/Versions/$PY3_VERSION/Python \
    @executable_path/../Frameworks/Python.framework/Versions/$PY3_VERSION/Python \
    Studio

# Deploy Guile library (including both bare scm files and precompiled,
# on the assumption that stuff which is useful for this application
# will have been pre-compiled at least one)
cd ../Resources
mkdir -p guile/scm
mkdir -p guile/ccache
# cp -r $GUILE_SCM guile/scm/
# cp -r $GUILE_CCACHE guile/ccache/
# cp -r ../../../../libfive/bind/guile/libfive guile/scm

# In the Resources directory, find any uncompiled scm files and compile them
# now for a faster startup, then delete them to reduce bundle size
cd guile/ccache && find . -name "*.go" | sed "s/\.go//g" | sort > ../../ccache_list
cd ../scm && find . -name "*.scm" | sed "s/\.scm//g" | sort > ../../scm_list
cd ../..
comm -2 -3 scm_list ccache_list | \
     LIBFIVE_FRAMEWORK_DIR=../Frameworks/ xargs -I{} -P8 \
        guild compile -Lguile/scm -o guile/ccache/{}.go guile/scm/{}.scm
rm ccache_list scm_list
rm -r guile/scm

# Update release number in Info.plist
cd ../../..
cp ../studio/deploy/mac/Info.plist $APP/Contents/Info.plist
sed -i "" "s:0\.0\.0:$VERSION:g" $APP/Contents/Info.plist

# Copy icon into bundle
cp ../studio/deploy/icon/studio.icns $APP/Contents/Resources/studio.icns

if [ "$1" == "dmg" ]; then
    # Create the disk image
    rm -rf deploy $EXE.dmg
    mkdir deploy
    cp ../README.md ./deploy/README.txt
    cp -r ../studio/examples ./deploy/examples
    mv $APP ./deploy
    mkdir deploy/.Trash
    hdiutil create $EXE.dmg -volname "$EXE $VERSION" -srcfolder deploy
    rm -rf deploy
    mv $EXE.dmg ..
fi
