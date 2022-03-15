package org.godotengine.plugin.gast.samples.spatialapp

import android.os.Bundle
import org.godotengine.plugin.gast.xrapp.GastActivity

class MainActivity : GastActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    override fun isXREnabled() = true
}