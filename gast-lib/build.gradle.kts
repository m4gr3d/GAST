plugins {
    id ("com.android.library")
    kotlin("android")
    kotlin("android.extensions")
}

android {
    compileSdkVersion(Versions.compileSdk)
    buildToolsVersion = Versions.buildTools
    defaultConfig {
        minSdkVersion(Versions.minSdk)
        targetSdkVersion(Versions.targetSdk)
        versionName = "1.0"
        versionCode = 1
    }
}

dependencies {
    implementation(project(":godot:lib"))
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk7:${Versions.kotlinVersion}")
    implementation("androidx.appcompat:appcompat:1.1.0")
    implementation("androidx.core:core-ktx:1.1.0")
}