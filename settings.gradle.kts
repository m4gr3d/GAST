include(":app")
include(":gast-lib")

include(":godot")
project(":godot").projectDir = File("deps/godot/platform/android/java")

include(":godot:lib")

rootProject.name= "GAST"
