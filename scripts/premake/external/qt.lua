function includeQt()
    includedirs "%{BASE_DIR}external/qt/qtbase/include"
end

function linkQt()
    filter { "platforms:Win64"}
        libdirs { 
            "%{BASE_DIR}external/qt/qtbase/lib",
            "%{BASE_DIR}external/qt/qtbase/plugins/platforms",

        }

    filter {}
        postbuildcommands { 
            "{copy} %{wks.location}../external/qt/qtbase/bin/Qt6Core.dll %{cfg.buildtarget.directory}",
            "{copy} %{wks.location}../external/qt/qtbase/bin/Qt6Gui.dll %{cfg.buildtarget.directory}",
            "{copy} %{wks.location}../external/qt/qtbase/bin/Qt6Widgets.dll %{cfg.buildtarget.directory}",
            "{MKDIR} %{cfg.buildtarget.directory}platforms/",
            "{copy} %{wks.location}../external/qt/qtbase/plugins/platforms/qwindows.dll %{cfg.buildtarget.directory}platforms/",
            "{copy} %{wks.location}../external/qt/qtbase/plugins/platforms/qdirect2d.dll %{cfg.buildtarget.directory}platforms/",

        }

    links {
        "Qt6Core.lib",
        "Qt6Gui.lib",
        "Qt6Widgets.lib",
        "qwindows.lib"
        
    }

    filter {}


end