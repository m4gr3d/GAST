plugins {
    id ("com.android.library")
    kotlin("android")
    kotlin("android.extensions")
}

val kotlinVersion: String by rootProject.extra
val compileSdk: Int by rootProject.extra
val buildTools: String by rootProject.extra
val minSdk: Int by rootProject.extra
val targetSdk: Int by rootProject.extra

android {
    compileSdkVersion(compileSdk)
    buildToolsVersion = buildTools
    defaultConfig {
        minSdkVersion(minSdk)
        targetSdkVersion(targetSdk)
        versionName = "0.2.0"
        versionCode = 2
        setProperty("archivesBaseName", "gast-video.${versionName}")
    }

    compileOptions {
        targetCompatibility = JavaVersion.VERSION_1_8
        sourceCompatibility = JavaVersion.VERSION_1_8
    }

}

dependencies {
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk7:$kotlinVersion")
    implementation("com.google.android.exoplayer:exoplayer:2.11.3")
    implementation("androidx.legacy:legacy-support-v4:1.0.0")

    implementation(project(":gast-core"))
    compileOnly(fileTree(mapOf("dir" to "../libs", "include" to listOf("godot-lib*.aar"))))
}
