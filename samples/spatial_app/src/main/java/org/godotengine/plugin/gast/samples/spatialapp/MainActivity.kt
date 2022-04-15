package org.godotengine.plugin.gast.samples.spatialapp

import android.os.Bundle
import org.godotengine.plugin.gast.xrapp.GastActivity

open class MainActivity : GastActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val testFragment = TestFragment()
        supportFragmentManager
            .beginTransaction()
            .add(R.id.gast_fragment_holder, testFragment)
            .commitAllowingStateLoss()
    }

    override fun isXREnabled() = BuildConfig.FLAVOR == "ovr"
}
