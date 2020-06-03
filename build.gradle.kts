apply(from = "config.gradle.kts")

buildscript {
    apply(from = "config.gradle.kts")

    val kotlinVersion: String by extra
    val gradlePluginVersion: String by extra

    repositories {
        google()
        jcenter()

    }
    dependencies {
        classpath("com.android.tools.build:gradle:$gradlePluginVersion")
        classpath("org.jetbrains.kotlin:kotlin-gradle-plugin:$kotlinVersion")
    }
}

allprojects {
    repositories {
        google()
        jcenter()

    }
}

tasks.register("clean", Delete::class) {
    delete(rootProject.buildDir)
}
