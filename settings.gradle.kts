rootProject.name= "GAST"

include(":gast-core")
project(":gast-core").projectDir = File("core")

include(":gast-video")
project(":gast-video").projectDir = File("video")

include(":gast-webview")
project(":gast-webview").projectDir = File("webview")
