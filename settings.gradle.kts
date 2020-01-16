include(":app")
include(":gast-lib")

include(":godot")
project(":godot").projectDir = File("deps/godot/platform/android/java")

include(":godot:lib")

include(":demo:android:build")

rootProject.name= "GAST"
