TEMPLATE = lib
CONFIG += plugin relative_qt_rpath c++11
TARGET = qtgeoservices_omim
DISTFILES += \
    omim_plugin.json

QT += location-private positioning-private network concurrent sql core gui opengl

#STATIC_BUILD = false
STATIC_BUILD = true
if ( $$STATIC_BUILD ) {
    CONFIG += static
    CONFIG += qtquickcompiler
    CONFIG += create_prl
    QMAKE_LFLAGS += -static
    QMAKE_AR += -v
    android: CONFIG += no_qtquickcompiler_depend
}

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)

# Use the following, to build the plugin outside Qt
PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryOmim

QMAKE_CXXFLAGS += -w #remove friggin warnings


# Place the lib into a "geoservices" subdir, otherwise addLibraryPath wont work.
DESTDIR = $$PWD/bin/geoservices/

DEPENDENCIES *= \
            #qt_common \
            map \
            drape_frontend \
            openlr \
            routing \
            search \
            storage \
            tracking \
            traffic \
            routing_common \
            ugc \
            indexer \
            drape \
            partners_api \
            local_ads \
            platform \
            editor \
            #bsdiff \
            mwm_diff \
            bsdiff \
            geometry \
            coding \
            base \
            freetype \
            expat \
            gflags \
            jansson \
            protobuf \
            osrm \
            stats_client \
            minizip \
            succinct \
            pugixml \
            oauthcpp \
            stb_image \
            sdf_image \
            icu \
            agg \
            opening_hours

#LIBS += "-L/media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/build-omim-Dev_Debug-Debug/out/debug"
#TODO: update the path below
LIBS += "-L/media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/build-omim-Dev-Debug/out/debug"

# Add libraries' dependencies.
for(project, DEPENDENCIES) {
  LIBS += -l$$project
}


LIBS *= -lz

INCLUDEPATH *= /media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/omim/
INCLUDEPATH *= /media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/omim/3party/glm
INCLUDEPATH *= /media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/omim/3party/agg
INCLUDEPATH *= /media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/omim/3party/opening_hours
#INCLUDEPATH *= /media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/omim/search
#INCLUDEPATH *= /media/paolo/qdata/home/paolo/Qt/Location/playground/omim.git/3rdParty/omim/indexer


CONFIG(release, debug|release) {
  DEFINES *= RELEASE _RELEASE NDEBUG
  CONFIG_NAME = release
} else {
  DEFINES *= DEBUG _DEBUG
  CONFIG_NAME = debug
}
