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
    
    compileOptions {
        targetCompatibility = JavaVersion.VERSION_1_8
    }

}

dependencies {
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk7:${Versions.kotlinVersion}")
    implementation("androidx.legacy:legacy-support-v4:1.0.0")

    implementation(project(":gast-core"))
    implementation(fileTree(mapOf("dir" to "libs", "include" to listOf("*.jar"))))
    compileOnly(fileTree(mapOf("dir" to "../libs", "include" to listOf("godot-lib*.aar"))))
}