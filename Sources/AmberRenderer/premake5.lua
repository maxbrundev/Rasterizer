workspace "AmberRenderer"
    configurations { "Debug", "Release" }
    platforms { "Win32", "x64" }
    startproject "AmberEditor"

outputdir = "%{wks.location}/../../Bin/"
objoutdir = "%{wks.location}/../../Bin-Int/"
dependdir = "%{wks.location}/../../Dependencies/"
builddir = "%{wks.location}/../../Build/"
resdir = "%{wks.location}/../../Resources/"

include "AmberGL"
include "AmberEditor"

include "../../Resources"